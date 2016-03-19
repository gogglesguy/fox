/********************************************************************************
*                                                                               *
*                 M u l i t h r e a d i n g   S u p p o r t                     *
*                                                                               *
*********************************************************************************
* Copyright (C) 2004,2006 by Jeroen van der Zijp.   All Rights Reserved.        *
*********************************************************************************
* This library is free software; you can redistribute it and/or                 *
* modify it under the terms of the GNU Lesser General Public                    *
* License as published by the Free Software Foundation; either                  *
* version 2.1 of the License, or (at your option) any later version.            *
*                                                                               *
* This library is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             *
* Lesser General Public License for more details.                               *
*                                                                               *
* You should have received a copy of the GNU Lesser General Public              *
* License along with this library; if not, write to the Free Software           *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.    *
*********************************************************************************
* $Id: FXThread.cpp,v 1.57 2006/04/06 05:43:52 fox Exp $                        *
********************************************************************************/
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "FXThread.h"

#ifndef WIN32
#ifdef APPLE
#include <mach/mach_init.h>
#include <mach/semaphore.h>
#include <mach/task.h>
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

  - I now believe its safe to set tid=0 after run returns; if FXThread
    is destroyed then the execution is stopped immediately; if the thread
    exits, tid is also set to 0.  If the thread is cancelled, tid is also
    set to 0.  In no circumstance I can see is it possible for run() to
    return when FXThread no longer exists.
*/

using namespace FX;


namespace FX {

/*******************************************************************************/

// Unix implementation

#ifndef WIN32


// Initialize mutex
FXMutex::FXMutex(bool recursive){
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
bool FXMutex::trylock(){
  return pthread_mutex_trylock((pthread_mutex_t*)data)==0;
  }


// Unlock mutex
void FXMutex::unlock(){
  pthread_mutex_unlock((pthread_mutex_t*)data);
  }


// Test if locked
bool FXMutex::locked(){
  if(pthread_mutex_trylock((pthread_mutex_t*)data)==EBUSY) return true;
  pthread_mutex_unlock((pthread_mutex_t*)data);
  return false;
  }


// Delete mutex
FXMutex::~FXMutex(){
  pthread_mutex_destroy((pthread_mutex_t*)data);
  }


/*******************************************************************************/


#ifdef APPLE


// Initialize semaphore
FXSemaphore::FXSemaphore(FXint initial){
  // If this fails on your machine, determine what value
  // of sizeof(MPSemaphoreID*) is supposed to be on your
  // machine and mail it to: jeroen@fox-toolkit.org!!
  //FXTRACE((150,"sizeof(MPSemaphoreID*)=%d\n",sizeof(MPSemaphoreID*)));
  FXASSERT(sizeof(data)>=sizeof(MPSemaphoreID*));
  MPCreateSemaphore(2147483647,0,(MPSemaphoreID*)data);
  }


// Decrement semaphore
void FXSemaphore::wait(){
  MPWaitOnSemaphore(*((MPSemaphoreID*)data),kDurationForever);
  }


// Decrement semaphore but don't block
bool FXSemaphore::trywait(){
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
bool FXSemaphore::trywait(){
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
void FXCondition::wait(FXMutex& mtx){
  pthread_cond_wait((pthread_cond_t*)data,(pthread_mutex_t*)&mtx.data);
  }


// Wait for condition but fall through after timeout
bool FXCondition::wait(FXMutex& mtx,FXTime nsec){
  register int result;
  struct timespec ts;
  ts.tv_sec=nsec/1000000000;
  ts.tv_nsec=nsec%1000000000;
x:result=pthread_cond_timedwait((pthread_cond_t*)data,(pthread_mutex_t*)&mtx.data,&ts);
  if(result==EINTR) goto x;
  return result!=ETIMEDOUT;
  }


// Delete condition
FXCondition::~FXCondition(){
  pthread_cond_destroy((pthread_cond_t*)data);
  }


/*******************************************************************************/

// Thread local storage key for back-reference to FXThread
static pthread_key_t self_key;

// Global initializer for the self_key variable
struct TLSKEYINIT {
  TLSKEYINIT(){ pthread_key_create(&self_key,NULL); }
 ~TLSKEYINIT(){ pthread_key_delete(self_key); }
  };


// Extern declaration prevents overzealous optimizer from noticing we're
// never using this object, and subsequently eliminating it from the code.
extern TLSKEYINIT initializer;

// Dummy object causes global initializer to run
TLSKEYINIT initializer;


// Initialize thread
FXThread::FXThread():tid(0){
  }


// Return thread id of this thread object.
// Purposefully NOT inlined, the tid may be changed by another
// thread and therefore we must force the compiler to fetch
// this value fresh each time it is needed!
FXThreadID FXThread::id() const {
  return tid;
  }


// Return TRUE if this thread is running
bool FXThread::running() const {
  return tid!=0;
  }


// Start the thread; we associate the FXThread instance with
// this thread using thread-local storage accessed with self_key.
// Also, we catch any errors thrown by the thread code here.
void* FXThread::execute(void* thread){
  register FXint code=-1;
  pthread_setspecific(self_key,thread);
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
  try{ code=((FXThread*)thread)->run(); } catch(...){ }
  ((FXThread*)thread)->tid=0;
  return (void*)(FXival)code;
  }


// Start thread; make sure that stacksize >= PTHREAD_STACK_MIN.
// We can't check for it because not all machines have this the
// PTHREAD_STACK_MIN definition.
bool FXThread::start(unsigned long stacksize){
  register bool code;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setinheritsched(&attr,PTHREAD_INHERIT_SCHED);
  if(stacksize){ pthread_attr_setstacksize(&attr,stacksize); }
  //pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
  code=pthread_create((pthread_t*)&tid,&attr,FXThread::execute,(void*)this)==0;
  pthread_attr_destroy(&attr);
  return code;
  }


// Suspend calling thread until thread is done
bool FXThread::join(FXint& code){
  void *trc=NULL;
  if(tid && pthread_join((pthread_t)tid,&trc)==0){
    code=(FXint)(FXival)trc;
    tid=0;
    return true;
    }
  return false;
  }


// Suspend calling thread until thread is done
bool FXThread::join(){
  if(tid && pthread_join((pthread_t)tid,NULL)==0){
    tid=0;
    return true;
    }
  return false;
  }


// Cancel the thread
bool FXThread::cancel(){
  if(tid && pthread_cancel((pthread_t)tid)==0){
    pthread_join((pthread_t)tid,NULL);
    tid=0;
    return true;
    }
  return false;
  }


// Detach thread
bool FXThread::detach(){
  return tid && pthread_detach((pthread_t)tid)==0;
  }


// Exit calling thread
void FXThread::exit(FXint code){
  if(self()){ self()->tid=0; }
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


// Return pointer to calling thread's instance
FXThread* FXThread::self(){
  return (FXThread*)pthread_getspecific(self_key);
  }


// Return thread id of caller
FXThreadID FXThread::current(){
  return (FXThreadID)pthread_self();
  }


// Set thread priority
void FXThread::priority(FXint prio){
  sched_param sched={0};
  int priomin,priomax;
  if(tid){
#if defined(SCHED_OTHER)
#if defined(_POSIX_PRIORITY_SCHEDULING)
     priomax=sched_get_priority_max(SCHED_OTHER);
     priomin=sched_get_priority_min(SCHED_OTHER);
     sched.sched_priority=FXCLAMP(priomin,prio,priomax);
#elif defined(PTHREAD_MINPRIORITY)
     sched.sched_priority=FXCLAMP(PTHREAD_MIN_PRIORITY,prio,PTHREAD_MAX_PRIORITY);
#endif
     pthread_setschedparam((pthread_t)tid,SCHED_OTHER,&sched);
#endif
    }
  }


// Return thread priority
FXint FXThread::priority(){
  sched_param sched={0};
  int policy;
  if(tid){
    pthread_getschedparam((pthread_t)tid,&policy,&sched);
    return sched.sched_priority;
    }
  return 0;
  }


// Destroy; if it was running, stop it
FXThread::~FXThread(){
  if(tid){ pthread_cancel((pthread_t)tid); }
  }


/*******************************************************************************/

// Windows implementation

#else

// Initialize mutex
FXMutex::FXMutex(bool){
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
bool FXMutex::trylock(){
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
bool FXMutex::locked(){
#if(_WIN32_WINNT >= 0x0400)
  if(TryEnterCriticalSection((CRITICAL_SECTION*)data)==0) return false;
  LeaveCriticalSection((CRITICAL_SECTION*)data);
#endif
  return true;
  }


// Delete mutex
FXMutex::~FXMutex(){
  DeleteCriticalSection((CRITICAL_SECTION*)data);
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
bool FXSemaphore::trywait(){
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
void FXCondition::wait(FXMutex& mtx){
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
  }


// Wait using single global mutex
bool FXCondition::wait(FXMutex& mtx,FXTime nsec){
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
  return result!=WAIT_TIMEOUT;
  }


// Delete condition
FXCondition::~FXCondition(){
  CloseHandle((HANDLE)data[0]);
  CloseHandle((HANDLE)data[1]);
  DeleteCriticalSection((CRITICAL_SECTION*)&data[3]);
  }


/*******************************************************************************/

// Thread local storage key for back-reference to FXThread
static DWORD self_key=0xffffffff;

// Global initializer for the self_key variable
struct TLSKEYINIT {
  TLSKEYINIT(){ self_key=TlsAlloc(); }
 ~TLSKEYINIT(){ TlsFree(self_key); }
  };

// Extern declaration prevents overzealous optimizer from noticing we're
// never using this object, and subsequently eliminating it from the code.
extern TLSKEYINIT initializer;

// Dummy object causes global initializer to run
TLSKEYINIT initializer;


// Initialize thread
FXThread::FXThread():tid(0){
  }


// Return thread id of this thread object.
// Purposefully NOT inlined, the tid may be changed by another
// thread and therefore we must force the compiler to fetch
// this value fresh each time it is needed!
FXThreadID FXThread::id() const {
  return tid;
  }


// Return TRUE if this thread is running
bool FXThread::running() const {
  return tid!=0;
  }


// Start the thread; we associate the FXThread instance with
// this thread using thread-local storage accessed with self_key.
// Also, we catch any errors thrown by the thread code here.
unsigned int CALLBACK FXThread::execute(void* thread){
  register FXint code=-1;
  TlsSetValue(self_key,thread);
  try{ code=((FXThread*)thread)->run(); } catch(...){ }
  ((FXThread*)thread)->tid=0;
  return code;
  }


// Start thread
bool FXThread::start(unsigned long stacksize){
  DWORD thd;
  tid=(FXThreadID)CreateThread(NULL,stacksize,(LPTHREAD_START_ROUTINE)FXThread::execute,this,0,&thd);
  return tid!=NULL;
  }


// Suspend calling thread until thread is done
bool FXThread::join(FXint& code){
  if(tid && WaitForSingleObject((HANDLE)tid,INFINITE)==WAIT_OBJECT_0){
    GetExitCodeThread((HANDLE)tid,(DWORD*)&code);
    CloseHandle((HANDLE)tid);
    tid=0;
    return true;
    }
  return false;
  }


// Suspend calling thread until thread is done
bool FXThread::join(){
  if(tid && WaitForSingleObject((HANDLE)tid,INFINITE)==WAIT_OBJECT_0){
    CloseHandle((HANDLE)tid);
    tid=0;
    return true;
    }
  return false;
  }


// Cancel the thread
bool FXThread::cancel(){
  if(tid && TerminateThread((HANDLE)tid,0)){
    CloseHandle((HANDLE)tid);
    tid=0;
    return true;
    }
  return false;
  }


// Detach thread
bool FXThread::detach(){
  return tid!=0;
  }


// Exit calling thread
void FXThread::exit(FXint code){
  if(self()){ self()->tid=0; }
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


// Return pointer to calling thread's instance
FXThread* FXThread::self(){
  return (FXThread*)TlsGetValue(self_key);
  }


// Set thread priority
void FXThread::priority(FXint prio){
  if(tid){
    SetThreadPriority((HANDLE)tid,prio);
    }
  }


// Return thread priority
FXint FXThread::priority(){
  if(tid){
    return GetThreadPriority((HANDLE)tid);
    }
  return 0;
  }


// Destroy
FXThread::~FXThread(){
  if(tid){ TerminateThread((HANDLE)tid,0); CloseHandle((HANDLE)tid); }
  }


#endif


}
