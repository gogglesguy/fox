/********************************************************************************
*                                                                               *
*                 M u l i t h r e a d i n g   S u p p o r t                     *
*                                                                               *
*********************************************************************************
* Copyright (C) 2004,2010 by Jeroen van der Zijp.   All Rights Reserved.        *
*********************************************************************************
* This library is free software; you can redistribute it and/or modify          *
* it under the terms of the GNU Lesser General Public License as published by   *
* the Free Software Foundation; either version 3 of the License, or             *
* (at your option) any later version.                                           *
*                                                                               *
* This library is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                 *
* GNU Lesser General Public License for more details.                           *
*                                                                               *
* You should have received a copy of the GNU Lesser General Public License      *
* along with this program.  If not, see <http://www.gnu.org/licenses/>          *
********************************************************************************/
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "FXThread.h"

/*
  Notes:

  - We have a amorphous blob of memory reserved for the mutex implementation.
    Since we're trying to avoid having to include platform-specific headers
    in application code, we can't easily know how much to allocate for
    pthread_mutex_t [or CRITICAL_SECTION].

  - We don't want to allocate dynamically because of the performance
    issues, and also because obviously, since heap memory is shared between
    threads, a malloc itself involves locking another mutex, leaving a
    potential for an unexpected deadlock.

  - So we just reserve some memory which we will hope to be enough.  If it
    ever turns out its not, the assert should trigger and we'll just have
    to change the source a bit.

  - If you run into this, try to figure out sizeof(pthread_mutex_t) and
    let me know about it (jeroen@fox-toolkit.com).

  - I do recommend running this in debug mode first time around on a
    new platform.

  - Picked unsigned long so as to ensure alignment issues are taken
    care off.

  - Note that the FXThreadID is only valid when busy==true, except insofar
    as when its used to harvest thread exit status like e.g. join!

  - The busy flag is set BEFORE actually spawning the thread, and reset
    if we were unable to spawn the thread.
    This is because we don't know how the thread creation is implemented:-
    its possible that the new thread may already be running for some time
    before pthread_create() returns successfully.
    We want to be sure that the flag reflects running state from within
    the newly spawned thread [we need this in several API's].

  - Note that cancel() also resets busy since it kills the thread
    right away; however join() doesn't because then we wait for the
    thread to finish normally.

  - About thread suspend/resume.  This does not work on Linux since there is
    no pthread equivalent for SuspendThread() and ResumeThread().  There is,
    however, an exceptionally inelegant solution in Boehm's GC code (file
    linux_threads.c).  But its so ugly we'd rather live without until a real
    suspend/resume facility is implemented in the linux kernel.

  - To find all preprocessor defines in GCC:

      echo | gcc -E -dM -

*/

using namespace FX;


namespace FX {


/*******************************************************************************/


// Initialize mutex
FXMutex::FXMutex(FXbool recursive){
#if defined(WIN32)
  // If this fails on your machine, determine what value
  // of sizeof(CRITICAL_SECTION) is supposed to be on your
  // machine and mail it to: jeroen@fox-toolkit.com!!
  //FXTRACE((150,"sizeof(CRITICAL_SECTION)=%d\n",sizeof(CRITICAL_SECTION)));
  FXASSERT(sizeof(data)>=sizeof(CRITICAL_SECTION));
  InitializeCriticalSection((CRITICAL_SECTION*)data);
#else
  // If this fails on your machine, determine what value
  // of sizeof(pthread_mutex_t) is supposed to be on your
  // machine and mail it to: jeroen@fox-toolkit.com!!
  //FXTRACE((150,"sizeof(pthread_mutex_t)=%d\n",sizeof(pthread_mutex_t)));
  FXASSERT(sizeof(data)>=sizeof(pthread_mutex_t));
  pthread_mutexattr_t mutexatt;
  pthread_mutexattr_init(&mutexatt);
  pthread_mutexattr_settype(&mutexatt,recursive?PTHREAD_MUTEX_RECURSIVE:PTHREAD_MUTEX_DEFAULT);
  pthread_mutex_init((pthread_mutex_t*)data,&mutexatt);
  pthread_mutexattr_destroy(&mutexatt);
#endif
  }


// Lock the mutex
void FXMutex::lock(){
#if defined(WIN32)
  EnterCriticalSection((CRITICAL_SECTION*)data);
#else
  pthread_mutex_lock((pthread_mutex_t*)data);
#endif
  }


// Try lock the mutex
FXbool FXMutex::trylock(){
#if defined(WIN32) && (_WIN32_WINNT >= 0x0400)
  return TryEnterCriticalSection((CRITICAL_SECTION*)data)!=0;
#elif defined(WIN32)
  return false;
#else
  return pthread_mutex_trylock((pthread_mutex_t*)data)==0;
#endif
  }


// Unlock mutex
void FXMutex::unlock(){
#if defined(WIN32)
  LeaveCriticalSection((CRITICAL_SECTION*)data);
#else
  pthread_mutex_unlock((pthread_mutex_t*)data);
#endif
  }


// Test if locked
FXbool FXMutex::locked(){
  if(trylock()){
    unlock();
    return false;
    }
  return true;
  }


// Delete mutex
FXMutex::~FXMutex(){
#if defined(WIN32)
  DeleteCriticalSection((CRITICAL_SECTION*)data);
#else
  pthread_mutex_destroy((pthread_mutex_t*)data);
#endif
  }


/*******************************************************************************/


// Initialize spinlock
FXSpinLock::FXSpinLock(){
#if defined(WIN32)
  data[0]=data[1]=data[2]=data[3]=0;
#elif (defined(__GNUC__) || defined(__INTEL_COMPILER)) && (defined(__i386__) || defined(__x86_64__))
  data[0]=data[1]=data[2]=data[3]=0;
#elif defined(__APPLE__)
  // If this fails on your machine, determine what value
  // of sizeof(pthread_mutex_t) is supposed to be on your
  // machine and mail it to: jeroen@fox-toolkit.com!!
  //FXTRACE((150,"sizeof(OSSpinLock)=%d\n",sizeof(OSSpinLock)));
  FXASSERT(sizeof(data)>=sizeof(OSSpinLock));
  *((OSSpinLock*)data)=OS_SPINLOCK_INIT;
#else
  // If this fails on your machine, determine what value
  // of sizeof(pthread_spinlock_t) is supposed to be on your
  // machine and mail it to: jeroen@fox-toolkit.com!!
  //FXTRACE((150,"sizeof(pthread_spinlock_t)=%d\n",sizeof(pthread_spinlock_t)));
  FXASSERT(sizeof(data)>=sizeof(pthread_spinlock_t));
  pthread_spin_init((pthread_spinlock_t*)(void*)data,PTHREAD_PROCESS_PRIVATE);
#endif
  }


// Lock the spinlock
void FXSpinLock::lock(){
#if defined(WIN32)
  while(InterlockedExchange((LONG*)data,1L)){
    while(data[0]){ }
    }
#elif (defined(__GNUC__) || defined(__INTEL_COMPILER)) && (defined(__i386__) || defined(__x86_64__))
  __asm__ __volatile__ ("movw $0x0100, %%ax \n\t"
                        "lock xaddw %%ax, %0 \n\t"
                        "1: \n\t"
                        "cmpb %%ah, %%al\n\t"
                        "je 2f\n\t"
                        "rep; nop\n\t"
                        "movb %0, %%al\n\t"
                        "jmp 1b\n\t"
                        "2:"  : "+m" (data) : : "memory", "ax", "cc");
#elif defined(__APPLE__)
  OSSpinLockLock((OSSpinLock*)data);
#else
  pthread_spin_lock((pthread_spinlock_t*)(void*)data);
#endif
  }


// Try lock the spinlock
FXbool FXSpinLock::trylock(){
#if defined(WIN32)
  return !InterlockedExchange((LONG*)data,1L);
#elif (defined(__GNUC__) || defined(__INTEL_COMPILER)) && (defined(__i386__) || defined(__x86_64__))
  FXbool ret;
  __asm__ __volatile__ ("movw %1,%%ax\n\t"
                        "cmpb %%ah, %%al\n\t"
                        "jne 1f\n\t"
                        "movw %%ax,%%cx\n\t"
                        "addw $0x0100,%%cx\n\t"
                        "lock cmpxchgw %%cx,%1\n\t"
                        "1:\n\t"
                        "sete %b0\n\t" :"=a" (ret), "+m" (data) : : "ecx", "memory", "cc");
  return ret;
#elif defined(__APPLE__)
  return OSSpinLockTry((OSSpinLock*)data);
#else
  return pthread_spin_trylock((pthread_spinlock_t*)(void*)data)==0;
#endif
  }


// Unlock spinlock
void FXSpinLock::unlock(){
#if defined(WIN32)
  InterlockedExchange((LONG*)data,0L);
#elif (defined(__GNUC__) || defined(__INTEL_COMPILER)) && (defined(__i386__) || defined(__x86_64__))
  __asm__ __volatile__ ("lock incb %0\n\t" : "+m" (data) : : "memory", "cc");
#elif defined(__APPLE__)
  OSSpinLockUnlock((OSSpinLock*)data);
#else
  pthread_spin_unlock((pthread_spinlock_t*)(void*)data);
#endif
  }


// Test if locked
FXbool FXSpinLock::locked(){
#if defined(WIN32)
  return (data[0]!=0);
#elif (defined(__GNUC__) || defined(__INTEL_COMPILER)) && (defined(__i386__) || defined(__x86_64__))
  FXbool ret;
  __asm__ __volatile__ ("movw %1,%%ax\n\t"
                        "cmpb %%ah, %%al\n\t"
                        "setne %%al\n\t" :"=a" (ret), "+m" (data) : : "memory", "cc");
  return ret;
#elif defined(__APPLE__)
  if(OSSpinLockTry((OSSpinLock*)data)){
    OSSpinLockUnlock((OSSpinLock*)data);
    return false;
    }
  return true;
#else
  if(pthread_spin_trylock((pthread_spinlock_t*)(void*)data)==0){
    pthread_spin_unlock((pthread_spinlock_t*)(void*)data);
    return false;
    }
  return true;
#endif
  }


// Delete spinlock
FXSpinLock::~FXSpinLock(){
#if defined(WIN32)
  // NOP //
#elif (defined(__GNUC__) || defined(__INTEL_COMPILER)) && (defined(__i386__) || defined(__x86_64__))
  // NOP //
#elif defined(__APPLE__)
  // NOP //
#else
  pthread_spin_destroy((pthread_spinlock_t*)(void*)data);
#endif
  }


/*******************************************************************************/


#if defined(WIN32)

#ifndef SEMAQUERYINFOCLASS
#define SEMAQUERYINFOCLASS 0
#endif

typedef long NTSTATUS;

typedef struct _SEMAINFO {
  UINT Count;		// current semaphore count
  UINT Limit;		// max semaphore count
  } SEMAINFO;

// Stub function
static NTSTATUS WINAPI myQuerySemaphore(HANDLE Handle,UINT InfoClass,SEMAINFO* SemaInfo,UINT InfoSize,UINT* RetLen);

// Function variable decl
typedef NTSTATUS (WINAPI *PFNQUERYSEMAPHORE)(HANDLE Handle,UINT InfoClass,SEMAINFO* SemaInfo,UINT InfoSize,UINT* RetLen);

// Function pointer into ntdll.dll
static PFNQUERYSEMAPHORE fxQuerySemaphore=myQuerySemaphore;

// When called, grab the true API from the DLL if we can
static NTSTATUS WINAPI myQuerySemaphore(HANDLE Handle,UINT InfoClass,SEMAINFO* SemaInfo,UINT InfoSize,UINT* RetLen){
  HINSTANCE HNTDLL;
  PFNQUERYSEMAPHORE pqs;
  if((HNTDLL=GetModuleHandleA("NTDLL")) && (pqs=(PFNQUERYSEMAPHORE)GetProcAddress(HNTDLL,"NtQuerySemaphore"))){
    fxQuerySemaphore=pqs;
    return fxQuerySemaphore(Handle,InfoClass,SemaInfo,InfoSize,RetLen);
    }
  return -1;
  }

#endif


// Initialize semaphore
FXSemaphore::FXSemaphore(FXint initial){
#if defined(WIN32)
  data[0]=(FXuval)CreateSemaphore(NULL,initial,0x7fffffff,NULL);
#else
  // If this fails on your machine, determine what value
  // of sizeof(sem_t) is supposed to be on your
  // machine and mail it to: jeroen@fox-toolkit.com!!
  //FXTRACE((150,"sizeof(sem_t)=%d\n",sizeof(sem_t)));
  FXASSERT(sizeof(data)>=sizeof(sem_t));
  sem_init((sem_t*)data,0,(unsigned int)initial);
#endif
  }


// Get semaphore value
FXint FXSemaphore::value() const {
#if defined(WIN32)
  SEMAINFO SemInfo;
  UINT RetLen;
  if(fxQuerySemaphore((HANDLE)data[0],SEMAQUERYINFOCLASS,&SemInfo,sizeof(SemInfo),&RetLen)>=0){
    return SemInfo.Count;
    }
  return -1;
#else
  int result=-1;
  sem_getvalue((sem_t*)data,&result);
  return result;
#endif
  }


// Decrement semaphore, waiting if count is zero
void FXSemaphore::wait(){
#if defined(WIN32)
  WaitForSingleObject((HANDLE)data[0],INFINITE);
#else
  sem_wait((sem_t*)data);
#endif
  }


// Decrement semaphore; returning false if count is zero
FXbool FXSemaphore::trywait(){
#if defined(WIN32)
  return WaitForSingleObject((HANDLE)data[0],0)==WAIT_OBJECT_0;
#else
  return sem_trywait((sem_t*)data)==0;
#endif
  }


// Increment semaphore
void FXSemaphore::post(){
#if defined(WIN32)
  ReleaseSemaphore((HANDLE)data[0],1,NULL);
#else
  sem_post((sem_t*)data);
#endif
  }


// Delete semaphore
FXSemaphore::~FXSemaphore(){
#if defined(WIN32)
  CloseHandle((HANDLE)data[0]);
#else
  sem_destroy((sem_t*)data);
#endif
  }


/*******************************************************************************/


// Initialize condition
FXCondition::FXCondition(){
#if defined(WIN32) && (_WIN32_WINNT >= 0x0600)    // Vista or newer
  // If this fails on your machine, determine what value
  // of sizeof(pthread_cond_t) is supposed to be on your
  // machine and mail it to: jeroen@fox-toolkit.com!!
  FXASSERT(sizeof(data)>=sizeof(CONDITION_VARIABLE));
  InitializeConditionVariable((CONDITION_VARIABLE*)data);
#elif defined(WIN32)
  // If this fails on your machine, determine what value
  // of sizeof(pthread_cond_t) is supposed to be on your
  // machine and mail it to: jeroen@fox-toolkit.com!!
  //FXTRACE((150,"sizeof(CRITICAL_SECTION)+sizeof(HANDLE)+sizeof(HANDLE)+sizeof(FXuval)=%d\n",sizeof(CRITICAL_SECTION)+sizeof(HANDLE)+sizeof(HANDLE)+sizeof(FXuval)));
  FXASSERT(sizeof(data)>=sizeof(CRITICAL_SECTION)+sizeof(HANDLE)+sizeof(HANDLE)+sizeof(FXuval));
  data[0]=(FXuval)CreateEvent(NULL,0,0,NULL);                   // Wakes one, autoreset
  data[1]=(FXuval)CreateEvent(NULL,1,0,NULL);                   // Wakes all, manual reset
  data[2]=0;                                                    // Blocked count
  InitializeCriticalSection((CRITICAL_SECTION*)&data[3]);       // Critical section
#else
  // If this fails on your machine, determine what value
  // of sizeof(pthread_cond_t) is supposed to be on your
  // machine and mail it to: jeroen@fox-toolkit.com!!
  //FXTRACE((150,"sizeof(pthread_cond_t)=%d\n",sizeof(pthread_cond_t)));
  FXASSERT(sizeof(data)>=sizeof(pthread_cond_t));
  pthread_cond_init((pthread_cond_t*)data,NULL);
#endif
  }


// Wake up one single waiting thread
void FXCondition::signal(){
#if defined(WIN32) && (_WIN32_WINNT >= 0x0600)    // Vista or newer
  WakeConditionVariable((CONDITION_VARIABLE*)data);
#elif defined(WIN32)
  EnterCriticalSection((CRITICAL_SECTION*)&data[3]);
  int blocked=(data[2]>0);
  LeaveCriticalSection((CRITICAL_SECTION*)&data[3]);
  if(blocked) SetEvent((HANDLE)data[0]);
#else
  pthread_cond_signal((pthread_cond_t*)data);
#endif
  }


// Wake up all waiting threads
void FXCondition::broadcast(){
#if defined(WIN32) && (_WIN32_WINNT >= 0x0600)    // Vista or newer
  WakeAllConditionVariable((CONDITION_VARIABLE*)data);
#elif defined(WIN32)
  EnterCriticalSection((CRITICAL_SECTION*)&data[3]);
  int blocked=(data[2]>0);
  LeaveCriticalSection((CRITICAL_SECTION*)&data[3]);
  if(blocked) SetEvent((HANDLE)data[1]);
#else
  pthread_cond_broadcast((pthread_cond_t*)data);
#endif
  }


// Wait
FXbool FXCondition::wait(FXMutex& mtx){
#if defined(WIN32) && (_WIN32_WINNT >= 0x0600)    // Vista or newer
  return SleepConditionVariableCS((CONDITION_VARIABLE*)data,(CRITICAL_SECTION*)mtx.data,INFINITE)!=0;
#elif defined(WIN32)
  EnterCriticalSection((CRITICAL_SECTION*)&data[3]);
  data[2]++;
  LeaveCriticalSection((CRITICAL_SECTION*)&data[3]);
  mtx.unlock();
  DWORD result=WaitForMultipleObjects(2,(HANDLE*)data,0,INFINITE);
  EnterCriticalSection((CRITICAL_SECTION*)&data[3]);
  data[2]--;
  int last_waiter=(result==WAIT_OBJECT_0+1)&&(data[2]==0);      // Unblocked by broadcast & no other blocked threads
  LeaveCriticalSection((CRITICAL_SECTION*)&data[3]);
  if(last_waiter) ResetEvent((HANDLE)data[1]);                  // Reset signal
  mtx.lock();
  return (WAIT_OBJECT_0+0==result)||(result==WAIT_OBJECT_0+1);
#else
  return pthread_cond_wait((pthread_cond_t*)data,(pthread_mutex_t*)mtx.data)==0;
#endif
  }


// Wait using single global mutex
FXbool FXCondition::wait(FXMutex& mtx,FXTime nsec){
#if defined(WIN32) && (_WIN32_WINNT >= 0x0600)    // Vista or newer
  if(0<nsec){
    DWORD delay=INFINITE;
    if(nsec<forever) delay=nsec/1000000;
    return SleepConditionVariableCS((CONDITION_VARIABLE*)data,(CRITICAL_SECTION*)mtx.data,delay)!=0;
    }
  return false;
#elif defined(WIN32)
  if(0<nsec){
    DWORD delay=INFINITE;
    if(nsec<forever) delay=nsec/1000000;
    EnterCriticalSection((CRITICAL_SECTION*)&data[3]);
    data[2]++;
    LeaveCriticalSection((CRITICAL_SECTION*)&data[3]);
    mtx.unlock();
    DWORD result=WaitForMultipleObjects(2,(HANDLE*)data,0,delay);
    EnterCriticalSection((CRITICAL_SECTION*)&data[3]);
    data[2]--;
    int last_waiter=(result==WAIT_OBJECT_0+1)&&(data[2]==0);    // Unblocked by broadcast & no other blocked threads
    LeaveCriticalSection((CRITICAL_SECTION*)&data[3]);
    if(last_waiter) ResetEvent((HANDLE)data[1]);                // Reset signal
    mtx.lock();
    return (WAIT_OBJECT_0+0==result)||(result==WAIT_OBJECT_0+1);
    }
  return false;
#else
  if(0<nsec){
    if(nsec<forever){
#if (_POSIX_C_SOURCE >= 199309L)
      struct timespec ts;
      clock_gettime(CLOCK_REALTIME,&ts);
      ts.tv_sec=ts.tv_sec+(ts.tv_nsec+nsec)/1000000000;
      ts.tv_nsec=(ts.tv_nsec+nsec)%1000000000;
      return pthread_cond_timedwait((pthread_cond_t*)data,(pthread_mutex_t*)mtx.data,&ts)==0;
#else
      struct timespec ts;
      struct timeval tv;
      gettimeofday(&tv,NULL);
      tv.tv_usec*=1000;
      ts.tv_sec=tv.tv_sec+(tv.tv_usec+nsec)/1000000000;
      ts.tv_nsec=(tv.tv_usec+nsec)%1000000000;
      return pthread_cond_timedwait((pthread_cond_t*)data,(pthread_mutex_t*)mtx.data,&ts)==0;
#endif
      }
    return pthread_cond_wait((pthread_cond_t*)data,(pthread_mutex_t*)mtx.data)==0;
    }
  return false;
#endif
  }


// Delete condition
FXCondition::~FXCondition(){
#if defined(WIN32) && (_WIN32_WINNT >= 0x0600)    // Vista or newer
  // NOP //
#elif defined(WIN32)
  CloseHandle((HANDLE)data[0]);
  CloseHandle((HANDLE)data[1]);
  DeleteCriticalSection((CRITICAL_SECTION*)&data[3]);
#else
  pthread_cond_destroy((pthread_cond_t*)data);
#endif
  }


/*******************************************************************************/


#if defined(WIN32)
struct RWLOCK {
  CRITICAL_SECTION mutex[1];
  CRITICAL_SECTION access[1];
  DWORD            readers;
  };
#endif


// Initialize read/write lock
FXReadWriteLock::FXReadWriteLock(){
#if defined(WIN32) && (_WIN32_WINNT >= 0x0600)    // Vista or newer
  // If this fails on your machine, determine what value
  // of sizeof(RWLOCK) is supposed to be on your
  // machine and mail it to: jeroen@fox-toolkit.com!!
  //FXTRACE((150,"sizeof(SRWLOCK)=%d\n",sizeof(SRWLOCK)));
  FXASSERT(sizeof(data)>=sizeof(SRWLOCK));
  InitializeSRWLock((SRWLOCK*)data);
#elif defined(WIN32)
  // If this fails on your machine, determine what value
  // of sizeof(RWLOCK) is supposed to be on your
  // machine and mail it to: jeroen@fox-toolkit.com!!
  //FXTRACE((150,"sizeof(RWLOCK)=%d\n",sizeof(RWLOCK)));
  FXASSERT(sizeof(data)>=sizeof(RWLOCK));
  InitializeCriticalSection(((RWLOCK*)data)->mutex);
  InitializeCriticalSection(((RWLOCK*)data)->access);
  ((RWLOCK*)data)->readers=0;
#elif defined(__APPLE__)
  // If this fails on your machine, determine what value
  // of sizeof(pthread_rwlock_t) is supposed to be on your
  // machine and mail it to: jeroen@fox-toolkit.com!!
  //FXTRACE((150,"sizeof(pthread_rwlock_t)=%d\n",sizeof(pthread_rwlock_t)));
  FXASSERT(sizeof(data)>=sizeof(pthread_rwlock_t));
  pthread_rwlock_init((pthread_rwlock_t*)data,NULL);
#else
  // If this fails on your machine, determine what value
  // of sizeof(pthread_rwlock_t) is supposed to be on your
  // machine and mail it to: jeroen@fox-toolkit.com!!
  //FXTRACE((150,"sizeof(pthread_rwlock_t)=%d\n",sizeof(pthread_rwlock_t)));
  FXASSERT(sizeof(data)>=sizeof(pthread_rwlock_t));
  pthread_rwlockattr_t rwlockatt;
  pthread_rwlockattr_init(&rwlockatt);
  pthread_rwlockattr_setkind_np(&rwlockatt,PTHREAD_RWLOCK_PREFER_WRITER_NP);
  pthread_rwlock_init((pthread_rwlock_t*)data,&rwlockatt);
  pthread_rwlockattr_destroy(&rwlockatt);
#endif
  }


// Acquire read lock for read/write lock
void FXReadWriteLock::readLock(){
#if defined(WIN32) && (_WIN32_WINNT >= 0x0600)    // Vista or newer
  AcquireSRWLockShared((SRWLOCK*)data);
#elif defined(WIN32)
  EnterCriticalSection(((RWLOCK*)data)->mutex);
  if(++((RWLOCK*)data)->readers==1){
    EnterCriticalSection(((RWLOCK*)data)->access);
    }
  LeaveCriticalSection(((RWLOCK*)data)->mutex);
#else
  pthread_rwlock_rdlock((pthread_rwlock_t*)data);
#endif
  }


// Try to acquire read lock for read/write lock
FXbool FXReadWriteLock::tryReadLock(){
#if defined(WIN32) && (_WIN32_WINNT >= 0x0600)    // Vista or newer
  return TryAcquireSRWLockShared((SRWLOCK*)data)!=0;
#elif defined(WIN32) && (_WIN32_WINNT >= 0x0400)
  if(TryEnterCriticalSection(((RWLOCK*)data)->mutex)){
    if(++((RWLOCK*)data)->readers==1 && !TryEnterCriticalSection(((RWLOCK*)data)->access)){
      --((RWLOCK*)data)->readers;
      LeaveCriticalSection(((RWLOCK*)data)->mutex);
      return false;
      }
    LeaveCriticalSection(((RWLOCK*)data)->mutex);
    return true;
    }
  return false;
#elif defined(WIN32)
  return false;
#else
  return pthread_rwlock_tryrdlock((pthread_rwlock_t*)data)==0;
#endif
  }


// Unlock read lock
void FXReadWriteLock::readUnlock(){
#if defined(WIN32) && (_WIN32_WINNT >= 0x0600)    // Vista or newer
  ReleaseSRWLockShared((SRWLOCK*)data);
#elif defined(WIN32)
  EnterCriticalSection(((RWLOCK*)data)->mutex);
  if(--((RWLOCK*)data)->readers==0){
    LeaveCriticalSection(((RWLOCK*)data)->access);
    }
  LeaveCriticalSection(((RWLOCK*)data)->mutex);
#else
  pthread_rwlock_unlock((pthread_rwlock_t*)data);
#endif
  }


// Acquire write lock for read/write lock
void FXReadWriteLock::writeLock(){
#if defined(WIN32) && (_WIN32_WINNT >= 0x0600)    // Vista or newer
  AcquireSRWLockExclusive((SRWLOCK*)data);
#elif defined(WIN32)
  EnterCriticalSection(((RWLOCK*)data)->access);
#else
  pthread_rwlock_wrlock((pthread_rwlock_t*)data);
#endif
  }


// Try to acquire write lock for read/write lock
FXbool FXReadWriteLock::tryWriteLock(){
#if defined(WIN32) && (_WIN32_WINNT >= 0x0600)    // Vista or newer
  return TryAcquireSRWLockExclusive((SRWLOCK*)data)!=0;
#elif defined(WIN32) && (_WIN32_WINNT >= 0x0400)
  return TryEnterCriticalSection(((RWLOCK*)data)->access)!=0;
#elif defined(WIN32)
  return false;
#else
  return pthread_rwlock_trywrlock((pthread_rwlock_t*)data)==0;
#endif
  }


// Unlock write lock
void FXReadWriteLock::writeUnlock(){
#if defined(WIN32) && (_WIN32_WINNT >= 0x0600)    // Vista or newer
  ReleaseSRWLockExclusive((SRWLOCK*)data);
#elif defined(WIN32)
  LeaveCriticalSection(((RWLOCK*)data)->access);
#else
  pthread_rwlock_unlock((pthread_rwlock_t*)data);
#endif
  }


// Delete read/write lock
FXReadWriteLock::~FXReadWriteLock(){
#if defined(WIN32) && (_WIN32_WINNT >= 0x0600)    // Vista or newer
  // NOP //
#elif defined(WIN32)
  DeleteCriticalSection(((RWLOCK*)data)->mutex);
  DeleteCriticalSection(((RWLOCK*)data)->access);
#else
  pthread_rwlock_destroy((pthread_rwlock_t*)data);
#endif
  }


/*******************************************************************************/


// Initialize the barrier
FXBarrier::FXBarrier(FXuint count):generation(0),threshold(count),counter(count){
  if(count<1){ fxerror("FXBarrier::FXBarrier: bad count argument.\n"); }
  }


// Wait for all threads to hit the barrier
FXbool FXBarrier::wait(){
  FXMutexLock locker(mutex);
  FXuint gen=generation;
  if(--counter==0){
    counter=threshold;
    generation++;
    condition.broadcast();
    return true;
    }
  while(gen==generation){
    condition.wait(mutex);
    }
  return false;
  }


// Delete the barrier
FXBarrier::~FXBarrier(){
  }


/*******************************************************************************/


// Automatically acquire a thread-local storage key
FXAutoThreadStorageKey::FXAutoThreadStorageKey(){
#if defined(WIN32)
  FXASSERT(sizeof(FXThreadStorageKey)==sizeof(DWORD));
  value=(FXThreadStorageKey)TlsAlloc();
#else
  FXASSERT(sizeof(FXThreadStorageKey)==sizeof(pthread_key_t));
  pthread_key_create((pthread_key_t*)&value,NULL);
#endif
  }


// Set thread local storage associated with this key
void FXAutoThreadStorageKey::set(void* ptr) const {
#if defined(WIN32)
  TlsSetValue((DWORD)value,ptr);
#else
  pthread_setspecific((pthread_key_t)value,ptr);
#endif
  }


// Get thread local storage associated with this key
void* FXAutoThreadStorageKey::get() const {
#if defined(WIN32)
  return TlsGetValue((DWORD)value);
#else
  return pthread_getspecific((pthread_key_t)value);
#endif
  }


// Automatically release a thread-local storage key
FXAutoThreadStorageKey::~FXAutoThreadStorageKey(){
#if defined(WIN32)
  TlsFree((DWORD)value);
#else
  pthread_key_delete((pthread_key_t)value);
#endif
  }


/*******************************************************************************/


// Generate one for the thread itself
FXAutoThreadStorageKey FXThread::selfKey;


// Initialize thread
FXThread::FXThread():tid(0),busy(false){
  }


// Return thread id of this thread object.
// Purposefully NOT inlined, the tid may be changed by another
// thread and therefore we must force the compiler to fetch
// this value fresh each time it is needed!
FXThreadID FXThread::id() const {
  return tid;
  }


// Return true if this thread is running
FXbool FXThread::running() const {
  return busy;
  }


// Change pointer to thread
void FXThread::self(FXThread* t){
  FXThread::selfKey.set(t);
  }


// Return pointer to calling thread
FXThread* FXThread::self(){
  return (FXThread*)FXThread::selfKey.get();
  }


// Start the thread; we associate the FXThread instance with
// this thread using thread-local storage accessed with self_key.
// Also, we catch any errors thrown by the thread code here.
// If FXThread is still around after run() returns, reset busy to false.
#if defined(WIN32)
unsigned int CALLBACK FXThread::function(void* ptr){
  register FXThread *thread=(FXThread*)ptr;
  register FXint code=-1;
  self(thread);
  try{ code=thread->run(); } catch(...){ }
  if(self()){ self()->busy=false; }
  return code;
  }
#else
void* FXThread::function(void* ptr){
  register FXThread *thread=(FXThread*)ptr;
  register FXint code=-1;
  self(thread);
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
#if defined(__USE_POSIX199506) || defined(__USE_UNIX98)
  sigset_t sigset;
  sigfillset(&sigset);
  pthread_sigmask(SIG_BLOCK,&sigset,0);         // No signals except to main thread
#endif
  try{ code=thread->run(); } catch(...){ }
  if(self()){ self()->busy=false; }
  return (void*)(FXival)code;
  }
#endif


// Start thread
FXbool FXThread::start(unsigned long stacksize){
#if defined(WIN32)
  DWORD thd;
  if(busy){ fxerror("FXThread::start: thread already running.\n"); }
  if(tid){ fxerror("FXThread::start: thread still attached.\n"); }
  busy=true;
  if((tid=(FXThreadID)CreateThread(NULL,stacksize,(LPTHREAD_START_ROUTINE)FXThread::function,this,0,&thd))==NULL) busy=false;
#else
  pthread_attr_t attr;
  if(busy){ fxerror("FXThread::start: thread already running.\n"); }
  if(tid){ fxerror("FXThread::start: thread still attached.\n"); }
  pthread_attr_init(&attr);
  pthread_attr_setinheritsched(&attr,PTHREAD_INHERIT_SCHED);
  if(stacksize){ pthread_attr_setstacksize(&attr,stacksize); }
  busy=true;
  if(pthread_create((pthread_t*)&tid,&attr,FXThread::function,(void*)this)!=0) busy=false;
  pthread_attr_destroy(&attr);
#endif
  return busy;
  }


// Suspend calling thread until thread is done
FXbool FXThread::join(FXint& code){
#if defined(WIN32)
  if(tid && WaitForSingleObject((HANDLE)tid,INFINITE)==WAIT_OBJECT_0){
    GetExitCodeThread((HANDLE)tid,(DWORD*)&code);
    CloseHandle((HANDLE)tid);
    tid=0;
    return true;
    }
#else
  void *trc=NULL;
  if(tid && pthread_join((pthread_t)tid,&trc)==0){
    code=(FXint)(FXival)trc;
    tid=0;
    return true;
    }
#endif
  return false;
  }


// Suspend calling thread until thread is done
FXbool FXThread::join(){
#if defined(WIN32)
  if(tid && WaitForSingleObject((HANDLE)tid,INFINITE)==WAIT_OBJECT_0){
    CloseHandle((HANDLE)tid);
    tid=0;
    return true;
    }
#else
  if(tid && pthread_join((pthread_t)tid,NULL)==0){
    tid=0;
    return true;
    }
#endif
  return false;
  }


// Cancel the thread
FXbool FXThread::cancel(){
#if defined(WIN32)
  if(tid){
    if(busy && TerminateThread((HANDLE)tid,0)) busy=false;
    if(CloseHandle((HANDLE)tid)){
      tid=0;
      return true;
      }
    }
#else
  if(tid){
    if(busy && pthread_cancel((pthread_t)tid)==0) busy=false;
    if(pthread_join((pthread_t)tid,NULL)==0){
      tid=0;
      return true;
      }
    }
#endif
  return false;
  }


// Detach thread
FXbool FXThread::detach(){
#if defined(WIN32)
  if(tid && CloseHandle((HANDLE)tid)){
    tid=0;
    return true;
    }
#else
  if(tid && pthread_detach((pthread_t)tid)==0){
    tid=0;
    return true;
    }
#endif
  return false;
  }


// Exit calling thread
void FXThread::exit(FXint code){
#if defined(WIN32)
  if(self()){ self()->busy=false; }
  ExitThread(code);
#else
  if(self()){ self()->busy=false; }
  pthread_exit((void*)(FXival)code);
#endif
  }


// Yield the thread
void FXThread::yield(){
#if defined(WIN32)
  Sleep(0);
#else
  sched_yield();                // More portable than pthread_yield()
#endif
  }


// Get time in nanoseconds since Epoch
FXTime FXThread::time(){
#if defined(WIN32)
  FXTime now;
  GetSystemTimeAsFileTime((FILETIME*)&now);
#if defined(__CYGWIN__) || defined(__MINGW32__) || defined(__SC__)
  return (now-116444736000000000LL)*100LL;
#else
  return (now-116444736000000000L)*100L;
#endif
#else
#if (_POSIX_C_SOURCE >= 199309L)
  const FXTime seconds=1000000000;
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME,&ts);
  return ts.tv_sec*seconds+ts.tv_nsec;
#else
  const FXTime seconds=1000000000;
  const FXTime microseconds=1000;
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return tv.tv_sec*seconds+tv.tv_usec*microseconds;
#endif
#endif
  }


// Make the calling thread sleep for a number of nanoseconds
void FXThread::sleep(FXTime nsec){
#if defined(WIN32)
  Sleep((DWORD)(nsec/1000000));
#else
#if (_POSIX_C_SOURCE >= 199309L)
  const FXTime seconds=1000000000;
  struct timespec value;
  value.tv_sec=nsec/seconds;
  value.tv_nsec=nsec%seconds;
  nanosleep(&value,NULL);
#else
  const FXTime seconds=1000000000;
  const FXTime microseconds=1000;
  const FXTime milliseconds=1000000;
  struct timeval value;
  value.tv_usec=(nsec/microseconds)%milliseconds;
  value.tv_sec=nsec/seconds;
  select(1,0,0,0,&value);
#endif
#endif
  }


// Wake at appointed time
void FXThread::wakeat(FXTime nsec){
#if defined(WIN32)
  nsec-=FXThread::time();
  if(nsec<0) nsec=0;
  Sleep((DWORD)(nsec/1000000));
#else
#if (_POSIX_C_SOURCE >= 199309L)
  const FXTime seconds=1000000000;
  struct timespec value;
#if (_XOPEN_SOURCE >= 600)
  value.tv_sec=nsec/seconds;
  value.tv_nsec=nsec%seconds;
  clock_nanosleep(CLOCK_REALTIME,TIMER_ABSTIME,&value,NULL);
#else
  nsec-=FXThread::time();
  if(nsec<0) nsec=0;
  value.tv_sec=nsec/seconds;
  value.tv_nsec=nsec%seconds;
  nanosleep(&value,NULL);
#endif
#else
  const FXTime seconds=1000000000;
  const FXTime microseconds=1000;
  const FXTime milliseconds=1000000;
  struct timeval value;
  if(nsec<0) nsec=0;
  value.tv_usec=(nsec/microseconds)%milliseconds;
  value.tv_sec=nsec/seconds;
  select(1,0,0,0,&value);
#endif
#endif
  }


// Return thread id of caller
FXThreadID FXThread::current(){
#if defined(WIN32)
  return (FXThreadID)GetCurrentThreadId();
#else
  return (FXThreadID)pthread_self();
#endif
  }


// Return number of processors
FXint FXThread::processors(){
#if defined(WIN32)
  SYSTEM_INFO info;
  GetSystemInfo(&info);
  return info.dwNumberOfProcessors;
#else
#if defined(_SC_NPROCESSORS_ONLN)
  int result;
  if((result=sysconf(_SC_NPROCESSORS_ONLN))>0){
    return result;
    }
#elif defined(__IRIX__) && defined(_SC_NPROC_ONLN)
  int result;
  if((result=sysconf(_SC_NPROC_ONLN))>0){
    return result;
    }
#elif defined(__APPLE__)
  int result=1;
  size_t len=sizeof(result);
  if(sysctlbyname("hw.activecpu",&result,&len,NULL,0)!=-1){
    return result;
    }
#elif defined(HW_NCPU)
  int result=1;
  int mib[2]={CTL_HW,HW_NCPU};
  size_t len=sizeof(result);
  if(sysctl(mib,2,&result,&len,NULL,0)!=-1){
    return result;
    }
#elif defined(hpux) || defined(__hpux) || defined(_hpux)
  struct pst_dynamic psd;
  if(!pstat_getdynamic(&psd,sizeof(psd),(size_t)1,0)){
    return (int)psd.psd_proc_cnt;
    }
#endif
  return 1;
#endif
  }


// Generate new thread local storage key
FXThreadStorageKey FXThread::createStorageKey(){
#if defined(WIN32)
  return (FXThreadStorageKey)TlsAlloc();
#else
  pthread_key_t key;
  return pthread_key_create(&key,NULL)==0 ? (FXThreadStorageKey)key : ~0;
#endif
  }


// Dispose of thread local storage key
void FXThread::deleteStorageKey(FXThreadStorageKey key){
#if defined(WIN32)
  TlsFree((DWORD)key);
#else
  pthread_key_delete((pthread_key_t)key);
#endif
  }


// Get thread local storage pointer using key
void* FXThread::getStorage(FXThreadStorageKey key){
#if defined(WIN32)
  return TlsGetValue((DWORD)key);
#else
  return pthread_getspecific((pthread_key_t)key);
#endif
  }


// Set thread local storage pointer using key
void FXThread::setStorage(FXThreadStorageKey key,void* ptr){
#if defined(WIN32)
  TlsSetValue((DWORD)key,ptr);
#else
  pthread_setspecific((pthread_key_t)key,ptr);
#endif
  }


#if 0

#if defined(WIN32)
#define MS_VC_EXCEPTION 0x406D1388

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO {
  DWORD dwType;         // Must be 0x1000.
  LPCSTR szName;        // Pointer to name (in user addr space).
  DWORD dwThreadID;     // Thread ID (-1=caller thread).
  DWORD dwFlags;        // Reserved for future use, must be zero.
  } THREADNAME_INFO;
#pragma pack(pop)
#endif


// Change thread name
FXbool FXThread::setName(const FXString& nm){
#if defined(WIN32)
  if(tid){
    THREADNAME_INFO info;
    FXASSERT(nm.length()<32);
    info.dwType=0x1000;
    info.szName=nm.text();
    info.dwThreadID=tid;
    info.dwFlags=0;
    __try {
       RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
      }
    __except(EXCEPTION_EXECUTE_HANDLER){
      }
    return true;
    }
  return false;
#else
  if(tid && nm.length()<32){
    return pthread_setname_np(tid,nm.text())==0;
    }
  return false;
#endif
  }


// Return thread name
FXString FXThread::getName() const {
#if defined(WIN32)
  return FXString::null;
#else
  if(tid){
    FXchar name[32];
    if(pthread_getname_np(tid,name,sizeof(name)==0)){
      return name;
      }
    }
  return FXString::null;
#endif
  }

#endif


// Set thread priority
FXbool FXThread::priority(FXThread::Priority prio){
#if defined(WIN32)
  if(tid){
    int pri;
    switch(prio){
      case PriorityMinimum:
        pri=THREAD_PRIORITY_LOWEST;
        break;
      case PriorityLower:
        pri=THREAD_PRIORITY_BELOW_NORMAL;
        break;
      case PriorityMedium:
        pri=THREAD_PRIORITY_NORMAL;
        break;
      case PriorityHigher:
        pri=THREAD_PRIORITY_ABOVE_NORMAL;
        break;
      case PriorityMaximum:
        pri=THREAD_PRIORITY_HIGHEST;
        break;
      default:
        pri=THREAD_PRIORITY_NORMAL;
        break;
      }
    return SetThreadPriority((HANDLE)tid,pri)!=0;
    }
  return false;
#elif defined(__APPLE__)
  return false;
#else
  if(tid){
    sched_param sched={0};
    int plcy=0;
    if(pthread_getschedparam((pthread_t)tid,&plcy,&sched)==0){
#if defined(_POSIX_PRIORITY_SCHEDULING)
      int priomax=sched_get_priority_max(plcy);         // Note that range may depend on scheduling policy!
      int priomin=sched_get_priority_min(plcy);
#elif defined(PTHREAD_MINPRIORITY) && defined(PTHREAD_MAX_PRIORITY)
      int priomin=PTHREAD_MIN_PRIORITY;
      int priomax=PTHREAD_MAX_PRIORITY;
#else
      int priomin=0;
      int priomax=20;
#endif
      if(priomax!=-1 && priomin!=-1){
        int priomed=(priomax+priomin)/2;
        switch(prio){
          case PriorityMinimum:
            sched.sched_priority=priomin;
            break;
          case PriorityLower:
            sched.sched_priority=(priomin+priomed)/2;
            break;
          case PriorityMedium:
            sched.sched_priority=priomed;
            break;
          case PriorityHigher:
            sched.sched_priority=(priomax+priomed)/2;
            break;
          case PriorityMaximum:
            sched.sched_priority=priomax;
            break;
          default:
            sched.sched_priority=priomed;
            break;
          }
        return pthread_setschedparam((pthread_t)tid,plcy,&sched)==0;
        }
      }
    }
  return false;
#endif
  }


// Return thread priority
FXThread::Priority FXThread::priority() const {
#if defined(WIN32)
  Priority result=PriorityError;
  if(tid){
    int pri=GetThreadPriority((HANDLE)tid);
    if(pri!=THREAD_PRIORITY_ERROR_RETURN){
      switch(pri){
        case THREAD_PRIORITY_IDLE:
          result=PriorityMinimum;
          break;
        case THREAD_PRIORITY_BELOW_NORMAL:
          result=PriorityLower;
          break;
        case THREAD_PRIORITY_NORMAL:
          result=PriorityMedium;
          break;
        case THREAD_PRIORITY_ABOVE_NORMAL:
          result=PriorityHigher;
          break;
        case THREAD_PRIORITY_HIGHEST:
          result=PriorityMaximum;
          break;
        default:
          result=PriorityDefault;
          break;
        }
      }
    }
  return result;
#elif defined(__APPLE__)
  return PriorityError;
#else
  Priority result=PriorityError;
  if(tid){
    sched_param sched={0};
    int plcy=0;
    if(pthread_getschedparam((pthread_t)tid,&plcy,&sched)==0){
#if defined(_POSIX_PRIORITY_SCHEDULING)
      int priomax=sched_get_priority_max(plcy);         // Note that range may depend on scheduling policy!
      int priomin=sched_get_priority_min(plcy);
#elif defined(PTHREAD_MINPRIORITY) && defined(PTHREAD_MAX_PRIORITY)
      int priomin=PTHREAD_MIN_PRIORITY;
      int priomax=PTHREAD_MAX_PRIORITY;
#else
      int priomin=0;
      int priomax=32;
#endif
      if(priomax!=-1 && priomin!=-1){
        int priomed=(priomax+priomin)/2;
        if(sched.sched_priority<priomed){
          if(sched.sched_priority<=priomin){
            result=PriorityMinimum;
            }
          else{
            result=PriorityLower;
            }
          }
        else if(sched.sched_priority<priomed){
          if(sched.sched_priority>=priomax){
            result=PriorityMaximum;
            }
          else{
            result=PriorityHigher;
            }
          }
        else{
          result=PriorityMedium;
          }
        }
      return result;
      }
    }
  return result;
#endif
  }


// Set thread scheduling policy
FXbool FXThread::policy(FXThread::Policy plcy){
#if defined(WIN32)
  return false;
#elif defined(__APPLE__)
  return false;
#else
  if(tid){
    sched_param sched={0};
    int oldplcy=0;
    int newplcy=0;
    if(pthread_getschedparam((pthread_t)tid,&oldplcy,&sched)==0){
      switch(plcy){
        case PolicyFifo:
          newplcy=SCHED_FIFO;
          break;
        case PolicyRoundRobin:
          newplcy=SCHED_RR;
          break;
        default:
          newplcy=SCHED_OTHER;
          break;
        }
#if defined(_POSIX_PRIORITY_SCHEDULING)
      sched.sched_priority=sched_get_priority_min(newplcy);
#endif
      return pthread_setschedparam((pthread_t)tid,newplcy,&sched)==0;
      }
    }
  return false;
#endif
  }


// Get thread scheduling policy
FXThread::Policy FXThread::policy() const {
#if defined(WIN32)
  return PolicyError;
#elif defined(__APPLE__)
  return PolicyError;
#else
  Policy result=PolicyError;
  if(tid){
    sched_param sched={0};
    int plcy=0;
    if(pthread_getschedparam((pthread_t)tid,&plcy,&sched)==0){
      switch(plcy){
        case SCHED_FIFO:
          result=PolicyFifo;
          break;
        case SCHED_RR:
          result=PolicyRoundRobin;
          break;
        default:
          result=PolicyDefault;
          break;
        }
      }
    }
  return result;
#endif
  }


// Suspend thread
FXbool FXThread::suspend(){
#if defined(WIN32)
  return tid && (SuspendThread((HANDLE)tid)!=(DWORD)-1L);
#elif defined(_HPUX_SOURCE)
  return tid && (pthread_suspend((pthread_t)tid)==0);
#elif defined(SUNOS)
  return tid && (thr_suspend((pthread_t)tid)==0);
#else
  // return tid && (pthread_kill((pthread_t)tid,SIGSTOP)==0);   // FIXME this does not work.
  return false;
#endif
  }


// Resume thread
FXbool FXThread::resume(){
#if defined(WIN32)
  return tid && (ResumeThread((HANDLE)tid)!=(DWORD)-1L);
#elif defined(_HPUX_SOURCE)
  return tid && (pthread_resume_np((pthread_t)tid,PTHREAD_COUNT_RESUME_NP)==0);
#elif defined(SUNOS)
  return tid && (thr_continue((pthread_t)tid)==0);
#else
  // return tid && (pthread_kill((pthread_t)tid,SIGCONT)==0);   // FIXME this does not work.
  return false;
#endif
  }


// Destroy
FXThread::~FXThread(){
  if(self()==this){
    self(NULL);
    detach();
    }
  else{
    cancel();
    }
  }

}
