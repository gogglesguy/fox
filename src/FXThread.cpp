/********************************************************************************
*                                                                               *
*                 M u l i t h r e a d i n g   S u p p o r t                     *
*                                                                               *
*********************************************************************************
* Copyright (C) 2004,2007 by Jeroen van der Zijp.   All Rights Reserved.        *
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
*********************************************************************************
* $Id: FXThread.cpp,v 1.119 2007/11/01 04:55:10 fox Exp $                       *
********************************************************************************/
#ifdef WIN32
#if _WIN32_WINNT < 0x0400
#define _WIN32_WINNT 0x0400
#endif
#endif
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "FXThread.h"
#ifndef WIN32
#ifdef __APPLE__
#ifdef Status
#undef Status
#endif
#include <CoreServices/CoreServices.h>
#include <pthread.h>
#else
#include <pthread.h>
#include <semaphore.h>
#endif
#else
#include <process.h>
#endif

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
    let me know about it (jeroen@fox-toolkit.org).

  - I do recommend running this in debug mode first time around on a
    new platform.

  - Picked unsigned long so as to ensure alignment issues are taken
    care off.

  - The ReadWriteLock currently favors readers; we may implement a fair
    algorithm at some future time!

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
*/

using namespace FX;


namespace FX {

/*******************************************************************************/

// Windows implementation

#ifdef WIN32

// Initialize mutex
FXMutex::FXMutex(FXbool){
  // If this fails on your machine, determine what value
  // of sizeof(CRITICAL_SECTION) is supposed to be on your
  // machine and mail it to: jeroen@fox-toolkit.org!!
  //FXTRACE((150,"sizeof(CRITICAL_SECTION)=%d\n",sizeof(CRITICAL_SECTION)));
  FXASSERT(sizeof(data)>=sizeof(CRITICAL_SECTION));
  InitializeCriticalSection((CRITICAL_SECTION*)data);
  }


// Lock the mutex
void FXMutex::lock(){
  EnterCriticalSection((CRITICAL_SECTION*)data);
  }



// Try lock the mutex
FXbool FXMutex::trylock(){
#if(_WIN32_WINNT >= 0x0400)
  return TryEnterCriticalSection((CRITICAL_SECTION*)data)!=0;
#else
  return false;
#endif
  }


// Unlock mutex
void FXMutex::unlock(){
  LeaveCriticalSection((CRITICAL_SECTION*)data);
  }


// Test if locked
FXbool FXMutex::locked(){
#if(_WIN32_WINNT >= 0x0400)
  if(TryEnterCriticalSection((CRITICAL_SECTION*)data)!=0){
    LeaveCriticalSection((CRITICAL_SECTION*)data);
    return false;
    }
#endif
  return true;
  }


// Delete mutex
FXMutex::~FXMutex(){
  DeleteCriticalSection((CRITICAL_SECTION*)data);
  }


/*******************************************************************************/

// Initialize spinlock
FXSpinLock::FXSpinLock(){
  FXASSERT(0);
  }


// Lock the spinlock
void FXSpinLock::lock(){
  FXASSERT(0);
  }


// Try lock the spinlock
FXbool FXSpinLock::trylock(){
  FXASSERT(0);
  return false;
  }


// Unlock spinlock
void FXSpinLock::unlock(){
  FXASSERT(0);
  }


// Test if locked
FXbool FXSpinLock::locked(){
  FXASSERT(0);
  return true;
  }


// Delete spinlock
FXSpinLock::~FXSpinLock(){
  FXASSERT(0);
  }


/*******************************************************************************/


struct RWLOCK {
  CRITICAL_SECTION mutex[1];
  CRITICAL_SECTION access[1];
  DWORD            readers;
  };


// Initialize read/write lock
FXReadWriteLock::FXReadWriteLock(){
  // If this fails on your machine, determine what value
  // of sizeof(RWLOCK) is supposed to be on your
  // machine and mail it to: jeroen@fox-toolkit.org!!
  //FXTRACE((150,"sizeof(RWLOCK)=%d\n",sizeof(RWLOCK)));
  FXASSERT(sizeof(data)>=sizeof(RWLOCK));
  InitializeCriticalSection(((RWLOCK*)data)->mutex);
  InitializeCriticalSection(((RWLOCK*)data)->access);
  ((RWLOCK*)data)->readers=0;
  }


// Acquire read lock for read/write lock
void FXReadWriteLock::readLock(){
  EnterCriticalSection(((RWLOCK*)data)->mutex);
  if(++((RWLOCK*)data)->readers==1){
    EnterCriticalSection(((RWLOCK*)data)->access);
    }
  LeaveCriticalSection(((RWLOCK*)data)->mutex);
  }


// Try to acquire read lock for read/write lock
bool FXReadWriteLock::tryReadLock(){
#if(_WIN32_WINNT >= 0x0400)
  if(TryEnterCriticalSection(((RWLOCK*)data)->mutex)){
    if(++((RWLOCK*)data)->readers==1 && !TryEnterCriticalSection(((RWLOCK*)data)->access)){
      --((RWLOCK*)data)->readers;
      LeaveCriticalSection(((RWLOCK*)data)->mutex);
      return false;
      }
    LeaveCriticalSection(((RWLOCK*)data)->mutex);
    return true;
    }
#endif
  return false;
  }


// Unlock read lock
void FXReadWriteLock::readUnlock(){
  EnterCriticalSection(((RWLOCK*)data)->mutex);
  if(--((RWLOCK*)data)->readers==0){
    LeaveCriticalSection(((RWLOCK*)data)->access);
    }
  LeaveCriticalSection(((RWLOCK*)data)->mutex);
  }


// Acquire write lock for read/write lock
void FXReadWriteLock::writeLock(){
  EnterCriticalSection(((RWLOCK*)data)->access);
  }


// Try to acquire write lock for read/write lock
bool FXReadWriteLock::tryWriteLock(){
#if(_WIN32_WINNT >= 0x0400)
  return TryEnterCriticalSection(((RWLOCK*)data)->access)!=0;
#else
  return false;
#endif
  }


// Unlock write lock
void FXReadWriteLock::writeUnlock(){
  LeaveCriticalSection(((RWLOCK*)data)->access);
  }


// Delete read/write lock
FXReadWriteLock::~FXReadWriteLock(){
  DeleteCriticalSection(((RWLOCK*)data)->mutex);
  DeleteCriticalSection(((RWLOCK*)data)->access);
  }

/*******************************************************************************/


// Initialize semaphore
FXSemaphore::FXSemaphore(FXint initial){
  data[0]=(FXuval)CreateSemaphore(NULL,initial,0x7fffffff,NULL);
  }


// Decrement semaphore
void FXSemaphore::wait(){
  WaitForSingleObject((HANDLE)data[0],INFINITE);
  }


// Non-blocking semaphore decrement
FXbool FXSemaphore::trywait(){
  return WaitForSingleObject((HANDLE)data[0],0)==WAIT_OBJECT_0;
  }


// Increment semaphore
void FXSemaphore::post(){
  ReleaseSemaphore((HANDLE)data[0],1,NULL);
  }


// Delete semaphore
FXSemaphore::~FXSemaphore(){
  CloseHandle((HANDLE)data[0]);
  }


/*******************************************************************************/


// This is the solution according to Schmidt, the win32-threads
// implementation thereof which is found inside GCC somewhere.
// See: (http://www.cs.wustl.edu/~schmidt/win32-cv-1.html).
//
// Our implementation however initializes the Event objects in
// the constructor, under the assumption that you wouldn't be creating
// a condition object if you weren't planning to use them somewhere.


// Initialize condition
FXCondition::FXCondition(){
  // If this fails on your machine, notify jeroen@fox-toolkit.org!
  FXASSERT(sizeof(data)>=sizeof(CRITICAL_SECTION)+sizeof(HANDLE)+sizeof(HANDLE)+sizeof(FXuval));
  data[0]=(FXuval)CreateEvent(NULL,0,0,NULL);                   // Wakes one, autoreset
  data[1]=(FXuval)CreateEvent(NULL,1,0,NULL);                   // Wakes all, manual reset
  data[2]=0;                                                    // Blocked count
  InitializeCriticalSection((CRITICAL_SECTION*)&data[3]);       // Critical section
  }


// Wake up one single waiting thread
void FXCondition::signal(){
  EnterCriticalSection((CRITICAL_SECTION*)&data[3]);
  int blocked=(data[2]>0);
  LeaveCriticalSection((CRITICAL_SECTION*)&data[3]);
  if(blocked) SetEvent((HANDLE)data[0]);
  }


// Wake up all waiting threads
void FXCondition::broadcast(){
  EnterCriticalSection((CRITICAL_SECTION*)&data[3]);
  int blocked=(data[2]>0);
  LeaveCriticalSection((CRITICAL_SECTION*)&data[3]);
  if(blocked) SetEvent((HANDLE)data[1]);
  }


// Wait
FXbool FXCondition::wait(FXMutex& mtx){
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
  }


// Wait using single global mutex
FXbool FXCondition::wait(FXMutex& mtx,FXTime nsec){
  EnterCriticalSection((CRITICAL_SECTION*)&data[3]);
  data[2]++;
  LeaveCriticalSection((CRITICAL_SECTION*)&data[3]);
  mtx.unlock();
  nsec-=FXThread::time();
  DWORD result=WaitForMultipleObjects(2,(HANDLE*)data,0,(DWORD)(nsec/1000000));
  EnterCriticalSection((CRITICAL_SECTION*)&data[3]);
  data[2]--;
  int last_waiter=(result==WAIT_OBJECT_0+1)&&(data[2]==0);      // Unblocked by broadcast & no other blocked threads
  LeaveCriticalSection((CRITICAL_SECTION*)&data[3]);
  if(last_waiter) ResetEvent((HANDLE)data[1]);                  // Reset signal
  mtx.lock();
  return (WAIT_OBJECT_0+0==result)||(result==WAIT_OBJECT_0+1);
  }


// Delete condition
FXCondition::~FXCondition(){
  CloseHandle((HANDLE)data[0]);
  CloseHandle((HANDLE)data[1]);
  DeleteCriticalSection((CRITICAL_SECTION*)&data[3]);
  }


/*******************************************************************************/

// Automatically acquire a thread-local storage key
FXAutoThreadStorageKey::FXAutoThreadStorageKey(){
  FXASSERT(sizeof(FXThreadStorageKey)==sizeof(DWORD));
  value=(FXThreadStorageKey)TlsAlloc();
  }


// Automatically release a thread-local storage key
FXAutoThreadStorageKey::~FXAutoThreadStorageKey(){
  TlsFree((DWORD)value);
  }


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
  TlsSetValue((DWORD)FXThread::selfKey,t);
  }


// Return pointer to calling thread
FXThread* FXThread::self(){
  return (FXThread*)TlsGetValue((DWORD)FXThread::selfKey);
  }


// Start the thread; we associate the FXThread instance with
// this thread using thread-local storage accessed with self_key.
// Also, we catch any errors thrown by the thread code here.
// If FXThread is still around after run() returns, reset busy to false.
unsigned int CALLBACK FXThread::function(void* thread){
  register FXint code=-1;
  TlsSetValue((DWORD)FXThread::selfKey,thread);
  try{ code=((FXThread*)thread)->run(); } catch(...){ }
  if(self()){ self()->busy=false; }
  return code;
  }


// Start thread
FXbool FXThread::start(unsigned long stacksize){
  DWORD thd;
  if(busy){ fxerror("FXThread::start: thread already running.\n"); }
  if(tid){ fxerror("FXThread::start: thread still attached.\n"); }
  busy=true;
  if((tid=(FXThreadID)CreateThread(NULL,stacksize,(LPTHREAD_START_ROUTINE)FXThread::function,this,0,&thd))==NULL) busy=false;
  return busy;
  }


// Suspend calling thread until thread is done
FXbool FXThread::join(FXint& code){
  if(tid && WaitForSingleObject((HANDLE)tid,INFINITE)==WAIT_OBJECT_0){
    GetExitCodeThread((HANDLE)tid,(DWORD*)&code);
    CloseHandle((HANDLE)tid);
    tid=0;
    return true;
    }
  return false;
  }


// Suspend calling thread until thread is done
FXbool FXThread::join(){
  if(tid && WaitForSingleObject((HANDLE)tid,INFINITE)==WAIT_OBJECT_0){
    CloseHandle((HANDLE)tid);
    tid=0;
    return true;
    }
  return false;
  }


// Cancel the thread
FXbool FXThread::cancel(){
  if(tid){
    if(busy && TerminateThread((HANDLE)tid,0)) busy=false;
    if(CloseHandle((HANDLE)tid)){
      tid=0;
      return true;
      }
    }
  return false;
  }


// Detach thread
FXbool FXThread::detach(){
  if(tid && CloseHandle((HANDLE)tid)){
    tid=0;
    return true;
    }
  return false;
  }


// Exit calling thread
void FXThread::exit(FXint code){
  if(self()){ self()->busy=false; }
  ExitThread(code);
  }


// Yield the thread
void FXThread::yield(){
  Sleep(0);
  }


// Get time in nanoseconds since Epoch
FXTime FXThread::time(){
  FXTime now;
  GetSystemTimeAsFileTime((FILETIME*)&now);
#if defined(__CYGWIN__) || defined(__MINGW32__) || defined(__SC__)
  return (now-116444736000000000LL)*100LL;
#else
  return (now-116444736000000000L)*100L;
#endif
  }


// Sleep for some time
void FXThread::sleep(FXTime nsec){
  Sleep((DWORD)(nsec/1000000));
  }


// Wake at appointed time
void FXThread::wakeat(FXTime nsec){
  nsec-=FXThread::time();
  if(nsec<0) nsec=0;
  Sleep((DWORD)(nsec/1000000));
  }


// Return thread id of caller
FXThreadID FXThread::current(){
  return (FXThreadID)GetCurrentThreadId();
  }


// Return number of processors
FXint FXThread::processors(){
  SYSTEM_INFO info;
  GetSystemInfo(&info);
  return info.dwNumberOfProcessors;
  }


// Generate new thread local storage key
FXThreadStorageKey FXThread::createStorageKey(){
  return (FXThreadStorageKey)TlsAlloc();
  }


// Dispose of thread local storage key
void FXThread::deleteStorageKey(FXThreadStorageKey key){
  TlsFree((DWORD)key);
  }


// Get thread local storage pointer using key
void* FXThread::getStorage(FXThreadStorageKey key){
  return TlsGetValue((DWORD)key);
  }


// Set thread local storage pointer using key
void FXThread::setStorage(FXThreadStorageKey key,void* ptr){
  TlsSetValue((DWORD)key,ptr);
  }


// Set thread priority
FXbool FXThread::priority(FXThread::Priority prio){
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
  }


// Return thread priority
FXThread::Priority FXThread::priority() const {
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
  }


// Set thread scheduling policy
FXbool FXThread::policy(FXThread::Policy){
  return true;
  }


// Get thread scheduling policy
FXThread::Policy FXThread::policy() const {
  return PolicyDefault;
  }


// Suspend thread
FXbool FXThread::suspend(){
  return busy && tid && (SuspendThread((HANDLE)tid)!=(DWORD)-1L);
  }


// Resume thread
FXbool FXThread::resume(){
  return busy && tid && (ResumeThread((HANDLE)tid)!=(DWORD)-1L);
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


/*******************************************************************************/

// Unix implementation

#else


// Initialize mutex
FXMutex::FXMutex(FXbool recursive){
  pthread_mutexattr_t mutexatt;
  // If this fails on your machine, determine what value
  // of sizeof(pthread_mutex_t) is supposed to be on your
  // machine and mail it to: jeroen@fox-toolkit.org!!
  //FXTRACE((150,"sizeof(pthread_mutex_t)=%d\n",sizeof(pthread_mutex_t)));
  FXASSERT(sizeof(data)>=sizeof(pthread_mutex_t));
  pthread_mutexattr_init(&mutexatt);
  pthread_mutexattr_settype(&mutexatt,recursive?PTHREAD_MUTEX_RECURSIVE:PTHREAD_MUTEX_DEFAULT);
  pthread_mutex_init((pthread_mutex_t*)data,&mutexatt);
  pthread_mutexattr_destroy(&mutexatt);
  }


// Lock the mutex
void FXMutex::lock(){
  pthread_mutex_lock((pthread_mutex_t*)data);
  }


// Try lock the mutex
FXbool FXMutex::trylock(){
  return pthread_mutex_trylock((pthread_mutex_t*)data)==0;
  }


// Unlock mutex
void FXMutex::unlock(){
  pthread_mutex_unlock((pthread_mutex_t*)data);
  }


// Test if locked
FXbool FXMutex::locked(){
  if(pthread_mutex_trylock((pthread_mutex_t*)data)==0){
    pthread_mutex_unlock((pthread_mutex_t*)data);
    return false;
    }
  return true;
  }


// Delete mutex
FXMutex::~FXMutex(){
  pthread_mutex_destroy((pthread_mutex_t*)data);
  }


/*******************************************************************************/

// Initialize spinlock
FXSpinLock::FXSpinLock(){
  // If this fails on your machine, determine what value
  // of sizeof(pthread_mutex_t) is supposed to be on your
  // machine and mail it to: jeroen@fox-toolkit.org!!
  //FXTRACE((150,"sizeof(pthread_spinlock_t)=%d\n",sizeof(pthread_spinlock_t)));
  FXASSERT(sizeof(data)>=sizeof(pthread_spinlock_t));
  pthread_spin_init((pthread_spinlock_t*)data,PTHREAD_PROCESS_PRIVATE);
  }


// Lock the spinlock
void FXSpinLock::lock(){
  pthread_spin_lock((pthread_spinlock_t*)data);
  }


// Try lock the spinlock
FXbool FXSpinLock::trylock(){
  return pthread_spin_trylock((pthread_spinlock_t*)data)==0;
  }


// Unlock spinlock
void FXSpinLock::unlock(){
  pthread_spin_unlock((pthread_spinlock_t*)data);
  }


// Test if locked
FXbool FXSpinLock::locked(){
  if(pthread_spin_trylock((pthread_spinlock_t*)data)==0){
    pthread_spin_unlock((pthread_spinlock_t*)data);
    return false;
    }
  return true;
  }


// Delete spinlock
FXSpinLock::~FXSpinLock(){
  pthread_spin_destroy((pthread_spinlock_t*)data);
  }

/*******************************************************************************/


// Initialize read/write lock
FXReadWriteLock::FXReadWriteLock(){
  // If this fails on your machine, determine what value
  // of sizeof(pthread_rwlock_t) is supposed to be on your
  // machine and mail it to: jeroen@fox-toolkit.org!!
  //FXTRACE((150,"sizeof(pthread_rwlock_t)=%d\n",sizeof(pthread_rwlock_t)));
  FXASSERT(sizeof(data)>=sizeof(pthread_rwlock_t));
#ifdef __APPLE__
  pthread_rwlock_init((pthread_rwlock_t*)data,NULL);
#else
  pthread_rwlockattr_t rwlockatt;
  pthread_rwlockattr_init(&rwlockatt);
  pthread_rwlockattr_setkind_np(&rwlockatt,PTHREAD_RWLOCK_PREFER_READER_NP);
  pthread_rwlock_init((pthread_rwlock_t*)data,&rwlockatt);
  pthread_rwlockattr_destroy(&rwlockatt);
#endif
  }


// Acquire read lock for read/write lock
void FXReadWriteLock::readLock(){
  pthread_rwlock_rdlock((pthread_rwlock_t*)data);
  }


// Try to acquire read lock for read/write lock
bool FXReadWriteLock::tryReadLock(){
  return pthread_rwlock_tryrdlock((pthread_rwlock_t*)data)==0;
  }


// Unlock read lock
void FXReadWriteLock::readUnlock(){
  pthread_rwlock_unlock((pthread_rwlock_t*)data);
  }


// Acquire write lock for read/write lock
void FXReadWriteLock::writeLock(){
  pthread_rwlock_wrlock((pthread_rwlock_t*)data);
  }


// Try to acquire write lock for read/write lock
bool FXReadWriteLock::tryWriteLock(){
  return pthread_rwlock_trywrlock((pthread_rwlock_t*)data)==0;
  }


// Unlock write lock
void FXReadWriteLock::writeUnlock(){
  pthread_rwlock_unlock((pthread_rwlock_t*)data);
  }


// Delete read/write lock
FXReadWriteLock::~FXReadWriteLock(){
  pthread_rwlock_destroy((pthread_rwlock_t*)data);
  }


/*******************************************************************************/


#ifdef __APPLE__


// Initialize semaphore
FXSemaphore::FXSemaphore(FXint initial){
  // If this fails on your machine, determine what value
  // of sizeof(MPSemaphoreID*) is supposed to be on your
  // machine and mail it to: jeroen@fox-toolkit.org!!
  //FXTRACE((150,"sizeof(MPSemaphoreID*)=%d\n",sizeof(MPSemaphoreID*)));
  FXASSERT(sizeof(data)>=sizeof(MPSemaphoreID*));
  MPCreateSemaphore(2147483647,initial,(MPSemaphoreID*)data);
  }


// Decrement semaphore
void FXSemaphore::wait(){
  MPWaitOnSemaphore(*((MPSemaphoreID*)data),kDurationForever);
  }


// Decrement semaphore but don't block
FXbool FXSemaphore::trywait(){
  return MPWaitOnSemaphore(*((MPSemaphoreID*)data),kDurationImmediate)==noErr;
  }


// Increment semaphore
void FXSemaphore::post(){
  MPSignalSemaphore(*((MPSemaphoreID*)data));
  }


// Delete semaphore
FXSemaphore::~FXSemaphore(){
  MPDeleteSemaphore(*((MPSemaphoreID*)data));
  }

#else

// Initialize semaphore
FXSemaphore::FXSemaphore(FXint initial){
  // If this fails on your machine, determine what value
  // of sizeof(sem_t) is supposed to be on your
  // machine and mail it to: jeroen@fox-toolkit.org!!
  //FXTRACE((150,"sizeof(sem_t)=%d\n",sizeof(sem_t)));
  FXASSERT(sizeof(data)>=sizeof(sem_t));
  sem_init((sem_t*)data,0,(unsigned int)initial);
  }


// Decrement semaphore
void FXSemaphore::wait(){
  sem_wait((sem_t*)data);
  }


// Decrement semaphore but don't block
FXbool FXSemaphore::trywait(){
  return sem_trywait((sem_t*)data)==0;
  }


// Increment semaphore
void FXSemaphore::post(){
  sem_post((sem_t*)data);
  }


// Delete semaphore
FXSemaphore::~FXSemaphore(){
  sem_destroy((sem_t*)data);
  }

#endif

/*******************************************************************************/


// Initialize condition
FXCondition::FXCondition(){
  // If this fails on your machine, determine what value
  // of sizeof(pthread_cond_t) is supposed to be on your
  // machine and mail it to: jeroen@fox-toolkit.org!!
  //FXTRACE((150,"sizeof(pthread_cond_t)=%d\n",sizeof(pthread_cond_t)));
  FXASSERT(sizeof(data)>=sizeof(pthread_cond_t));
  pthread_cond_init((pthread_cond_t*)data,NULL);
  }


// Wake up one single waiting thread
void FXCondition::signal(){
  pthread_cond_signal((pthread_cond_t*)data);
  }


// Wake up all waiting threads
void FXCondition::broadcast(){
  pthread_cond_broadcast((pthread_cond_t*)data);
  }


// Wait for condition indefinitely
FXbool FXCondition::wait(FXMutex& mtx){
  return pthread_cond_wait((pthread_cond_t*)data,(pthread_mutex_t*)mtx.data)==0;
  }


// Wait for condition but fall through after timeout
FXbool FXCondition::wait(FXMutex& mtx,FXTime nsec){
  struct timespec ts;
  ts.tv_sec=nsec/1000000000;
  ts.tv_nsec=nsec%1000000000;
  return pthread_cond_timedwait((pthread_cond_t*)data,(pthread_mutex_t*)mtx.data,&ts)==0;
  }


// Delete condition
FXCondition::~FXCondition(){
  pthread_cond_destroy((pthread_cond_t*)data);
  }


/*******************************************************************************/

// Automatically acquire a thread-local storage key
FXAutoThreadStorageKey::FXAutoThreadStorageKey(){
  FXASSERT(sizeof(FXThreadStorageKey)==sizeof(pthread_key_t));
  pthread_key_create((pthread_key_t*)&value,NULL);
  }


// Automatically release a thread-local storage key
FXAutoThreadStorageKey::~FXAutoThreadStorageKey(){
  pthread_key_delete((pthread_key_t)value);
  }


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
  pthread_setspecific((pthread_key_t)FXThread::selfKey,t);
  }


// Return pointer to calling thread
FXThread* FXThread::self(){
  return (FXThread*)pthread_getspecific((pthread_key_t)FXThread::selfKey);
  }


// Start the thread; we associate the FXThread instance with
// this thread using thread-local storage accessed with self_key.
// Also, we catch any errors thrown by the thread code here.
// If FXThread is still around after run() returns, reset busy to false.
void* FXThread::function(void* thread){
  register FXint code=-1;
  pthread_setspecific((pthread_key_t)FXThread::selfKey,thread);
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
#if defined(__USE_POSIX199506) || defined(__USE_UNIX98)
  sigset_t sigset;
  sigfillset(&sigset);
  pthread_sigmask(SIG_BLOCK,&sigset,0);         // No signals except to main thread
#endif
  try{ code=((FXThread*)thread)->run(); } catch(...){ }
  if(self()){ self()->busy=false; }
  return (void*)(FXival)code;
  }


// Start thread; make sure that stacksize >= PTHREAD_STACK_MIN.
// We can't check for it because not all machines have this the
// PTHREAD_STACK_MIN definition.
FXbool FXThread::start(unsigned long stacksize){
  pthread_attr_t attr;
  if(busy){ fxerror("FXThread::start: thread already running.\n"); }
  if(tid){ fxerror("FXThread::start: thread still attached.\n"); }
  pthread_attr_init(&attr);
  pthread_attr_setinheritsched(&attr,PTHREAD_INHERIT_SCHED);
  //pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
  //pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);
  //pthread_attr_setscope(&attr,PTHREAD_SCOPE_SYSTEM);
  //pthread_attr_setscope(&attr,PTHREAD_SCOPE_PROCESS);
  if(stacksize){ pthread_attr_setstacksize(&attr,stacksize); }
  busy=true;
  if(pthread_create((pthread_t*)&tid,&attr,FXThread::function,(void*)this)!=0) busy=false;
  pthread_attr_destroy(&attr);
  return busy;
  }


// Suspend calling thread until thread is done
FXbool FXThread::join(FXint& code){
  void *trc=NULL;
  if(tid && pthread_join((pthread_t)tid,&trc)==0){
    code=(FXint)(FXival)trc;
    tid=0;
    return true;
    }
  return false;
  }


// Suspend calling thread until thread is done
FXbool FXThread::join(){
  if(tid && pthread_join((pthread_t)tid,NULL)==0){
    tid=0;
    return true;
    }
  return false;
  }


// Cancel the thread
FXbool FXThread::cancel(){
  if(tid){
    if(busy && pthread_cancel((pthread_t)tid)==0) busy=false;
    if(pthread_join((pthread_t)tid,NULL)==0){
      tid=0;
      return true;
      }
    }
  return false;
  }


// Detach thread
FXbool FXThread::detach(){
  if(tid && pthread_detach((pthread_t)tid)==0){
    tid=0;
    return true;
    }
  return false;
  }


// Exit calling thread
void FXThread::exit(FXint code){
  if(self()){ self()->busy=false; }
  pthread_exit((void*)(FXival)code);
  }


// Yield the thread
void FXThread::yield(){
  sched_yield();                // More portable than pthread_yield()
  }


// Get time in nanoseconds since Epoch
FXTime FXThread::time(){
#ifdef __USE_POSIX199309
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
  }


// Sleep for some time
void FXThread::sleep(FXTime nsec){
#ifdef __USE_POSIX199309
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
  }


// Wake at appointed time
void FXThread::wakeat(FXTime nsec){
#ifdef __USE_POSIX199309
  const FXTime seconds=1000000000;
  struct timespec value;
#ifdef __USE_XOPEN2K
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
  }


// Return thread id of caller
FXThreadID FXThread::current(){
  return (FXThreadID)pthread_self();
  }


// Return number of processors
FXint FXThread::processors(){
#if defined(__FreeBSD__)
  int result=1;
  int resultsize=sizeof(int);
  sysctlbyname("hw.ncpu",&result,&resultsize,0,0);
  return result;
#elif defined(__APPLE__)
  return  MPProcessors();
#else
  FXint result=sysconf(_SC_NPROCESSORS_ONLN);
  if(result<0) result=1;
  return result;
#endif
  }


// Generate new thread local storage key
FXThreadStorageKey FXThread::createStorageKey(){
  pthread_key_t key;
  return pthread_key_create(&key,NULL)==0 ? (FXThreadStorageKey)key : ~0;
  }


// Dispose of thread local storage key
void FXThread::deleteStorageKey(FXThreadStorageKey key){
  pthread_key_delete((pthread_key_t)key);
  }


// Get thread local storage pointer using key
void* FXThread::getStorage(FXThreadStorageKey key){
  return pthread_getspecific((pthread_key_t)key);
  }


// Set thread local storage pointer using key
void FXThread::setStorage(FXThreadStorageKey key,void* ptr){
  pthread_setspecific((pthread_key_t)key,ptr);
  }


// Set thread priority
FXbool FXThread::priority(FXThread::Priority prio){
#ifndef __APPLE__
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
#endif
  return false;
  }


// Return thread priority
FXThread::Priority FXThread::priority() const {
  Priority result=PriorityError;
#ifndef __APPLE__
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
      return result;
      }
    }
#endif
  return result;
  }


// Set thread scheduling policy
FXbool FXThread::policy(FXThread::Policy plcy){
#ifndef __APPLE__
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
      return pthread_setschedparam((pthread_t)tid,newplcy,&sched)==0;
      }
    }
#endif
  return false;
  }


// Get thread scheduling policy
FXThread::Policy FXThread::policy() const {
  Policy result=PolicyError;
#ifndef __APPLE__
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
#endif
  return result;
  }


// Suspend thread; return true if success.
FXbool FXThread::suspend(){
#if defined(_HPUX_SOURCE)
  return busy && tid && (pthread_suspend((pthread_t)tid)==0);
#elif defined(SUNOS)
  return busy && tid && (thr_suspend((pthread_t)tid)==0);
#else
  // return busy && tid && (pthread_kill((pthread_t)tid,SIGSTOP)==0);   // FIXME this does not work.
  return false;
#endif
  }


// Resume thread; return true if success.
FXbool FXThread::resume(){
#if defined(_HPUX_SOURCE)
  return busy && tid && (pthread_resume_np((pthread_t)tid,PTHREAD_COUNT_RESUME_NP)==0);
#elif defined(SUNOS)
  return busy && tid && (thr_continue((pthread_t)tid)==0);
#else
  // return busy && tid && (pthread_kill((pthread_t)tid,SIGCONT)==0);   // FIXME this does not work.
  return false;
#endif
  }


// Destroy; if it was running, stop it
FXThread::~FXThread(){
  if(self()==this){
    self(NULL);
    detach();
    }
  else{
    cancel();
    }
  }

#endif

}
