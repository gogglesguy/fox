/********************************************************************************
*                                                                               *
*                         E v e n t   D i s p a t c h e r                       *
*                                                                               *
*********************************************************************************
* Copyright (C) 2006,2015 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "FXArray.h"
#include "FXAtomic.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXRectangle.h"
#include "FXString.h"
#include "FXEvent.h"
#include "FXObject.h"
#include "FXMutex.h"
#include "FXAutoThreadStorageKey.h"
#include "FXThread.h"
#include "FXElement.h"
#include "FXDispatcher.h"
#include "FXException.h"

/*
  Notes:

  - Special subclass of FXDispatcher for when there is a GUI event source.

  - FXDispatcher does NOT loop; but does maintain set of event sources and raised
    events; thus, recursive event loops don't lose track.

  - Process IO handles in circular fashion so each one gets equal opportunity of being
    handled; always return to called after each dispatch.

  - If we allow another thread to add/remove event sources, then we will need a
    signaling pipe to notify the blocking thread (except when using epoll).

  - Use new pselect() to have (1) nanosecond wait-time and (2) dispatch non-immediate
    signal handlers properly (see man 2 select_tut).

  - Signal handling:

      o Synchronous signals are dispatched from FXDispatcher.  We can use signalfd()
        maybe.  Point is, they're part of the regular event loop and are dispatched
        from FXDispatcher.

      o All signal masks are left unchanged as per program-startup.  When setting a
        handler for a signal, that particular signal will be masked (blocked) until
        we get into pselect(). When a signal handler is removed, the old state will
        be reinstated.

  - Idea: when inside dispatch, substract fd of dispatch from the set, until dispatch
    returns.  Thus, if dispatcher processed by multiple threads, no two threads manage
    the same fd.  So the set of fd's being watched is the total set of fd's minus
    the ones currently being dispatched.

  - Look at: extern int timerfd_create ();
  - Look at: epoll_create().
  - Look at: eventfd().
  - Look at: signalfd().
  - Must be multi-thread capable.
  - Threads which are inside run-loop have their TLS dispatcherStorageKey pointed
    to the thread.  This association is broken when they leave.
  - Add static function to return calling thread's FXDispatcher.
  - Threads can enter/leave loop individually or collectively.

  - NEW:
    FXDispatcher (or subclasses thereof) don't loop.  Just block and dispatch.
    FXInvocation represents a loop.  They're linked, and per-thread.  It
    calls FXDispatcher::processEvents() repeatedly until some condition calls
    for it to stop. Several FXInvocations may share a single FXDispatcher.

  - Event/handler objects to have back-link to FXDispatcher, so we know which
    FXDispatcher they belong to (in case more than one FXDispatcher).
*/


// Bad handle value
#ifdef WIN32
#define BadHandle INVALID_HANDLE_VALUE
#else
#define BadHandle -1
#endif


using namespace FX;

/*******************************************************************************/

namespace FX {

// Handles being watched
struct FXDispatcher::FXHandles {
#if defined(WIN32)
  HANDLE     handles[MAXIMUM_WAIT_OBJECTS];     // Handles
  HANDLE     watched[MAXIMUM_WAIT_OBJECTS];     // Watched handles
#elif defined(HAVE_EPOLL_CREATE1)
  struct epoll_event events[1024];              // Events
  int        handle;                            // Poll handle
  sigset_t   signals;                           // Handled signals
#else
  fd_set     watched[3];                        // Watched handles
  fd_set     handles[3];                        // Known handles
  sigset_t   signals;                           // Handled signals
#endif
  };


/*******************************************************************************/

FXIMPLEMENT(FXDispatcher,FXObject,NULL,0)


// Construct dispatcher object
FXDispatcher::FXDispatcher():handles(NULL),numhandles(0),numwatched(0),numraised(0),current(0),initialized(false){
  }


// Initialize dispatcher
FXbool FXDispatcher::init(){
  if(!isInitialized()){
    if(allocElms(handles,1)){
#if defined(WIN32)
      clearElms(handles,ARRAYNUMBER(handles));
      clearElms(watched,ARRAYNUMBER(handles));
#elif defined(HAVE_EPOLL_CREATE1)
      sigemptyset(&handles->signals);
      clearElms(handles->events,ARRAYNUMBER(handles->events));
      handles->handle=epoll_create1(EPOLL_CLOEXEC);
      if(handles->handle<0){ freeElms(handles); return false; }
#else
      sigemptyset(&handles->signals);
      FD_ZERO(&handles->handles[0]);            // No handles
      FD_ZERO(&handles->handles[1]);
      FD_ZERO(&handles->handles[2]);
      FD_ZERO(&handles->watched[0]);
      FD_ZERO(&handles->watched[1]);
      FD_ZERO(&handles->watched[2]);
#endif
      numhandles=0;
      numwatched=0;
      numraised=0;
      current=0;
      initialized=true;
      return true;
      }
    }
  return false;
  }


/*******************************************************************************/

// Return timeout when something needs to happen
FXTime FXDispatcher::getTimeout(){
  return forever;
  }

/*******************************************************************************/

// Signal sig was raised
volatile FXbool FXDispatcher::signotified[64];

// Most recent signal received
volatile FXint FXDispatcher::sigreceived=0;


// Signal handler simply sets flag to indicate it has gone off; also
// record most recently raised signal for efficiency
void FXDispatcher::signalhandler(int sig){
  signotified[sig]=true;
  sigreceived=sig;
  }


// Check if dispatcher handles given signal
FXbool FXDispatcher::hasSignal(FXint sig){
  if(isInitialized()){
#ifdef WIN32
#else
    return sigismember(&handles->signals,sig)==1;
#endif
    }
  return false;
  }


// Append signal to signal-set observed by the dispatcher
FXbool FXDispatcher::addSignal(FXint sig){
  if(isInitialized()){
#ifdef WIN32
#else
    if(sigismember(&handles->signals,sig)==0){
      struct sigaction sigact;
      signotified[sig]=false;                   // Set non-raised
      sigact.sa_handler=signalhandler;
      sigfillset(&sigact.sa_mask);              // Block other signals while running handler
      sigact.sa_flags=0;
      sigaction(sig,&sigact,NULL);              // Set handler
      sigaddset(&handles->signals,sig);
      return true;
      }
#endif
    }
  return false;
  }


// Remove signal from signal-set observed by the dispatcher
FXbool FXDispatcher::remSignal(FXint sig){
  if(isInitialized()){
#ifdef WIN32
#else
    if(sigismember(&handles->signals,sig)==1){
      struct sigaction sigact;
      sigact.sa_handler=SIG_DFL;
      sigemptyset(&sigact.sa_mask);             // Unblock other signals
      sigact.sa_flags=0;
      sigaction(sig,&sigact,NULL);              // First, unset handler
      signotified[sig]=false;                   // Set non-raised
      sigdelset(&handles->signals,sig);
      return true;
      }
#endif
    }
  return false;
  }

/*******************************************************************************/


// Check if handle was watched
FXbool FXDispatcher::hasHandle(FXInputHandle hnd,FXuint mode){
  if(isInitialized()){
#if defined(WIN32)
    if(hnd!=BadHandle){
      for(FXint i=numhandles-1; i>=0; --i){
        if(handles->handles[i]==hnd) return true;
        }
      }
#elif defined(HAVE_EPOLL_CREATE1)
    if(0<=hnd){
      // FIXME //
      }
#else
    if(0<=hnd && hnd<numhandles){
      if((mode&InputRead) && FD_ISSET(hnd,&handles->handles[0])) return true;
      if((mode&InputWrite) && FD_ISSET(hnd,&handles->handles[1])) return true;
      if((mode&InputExcept) && FD_ISSET(hnd,&handles->handles[2])) return true;
      }
#endif
    }
  return false;
  }


// Add handle hnd to list
FXbool FXDispatcher::addHandle(FXInputHandle hnd,FXuint mode){
  if(isInitialized()){
#if defined(WIN32)
    if(hnd!=BadHandle && numhandles<MAXIMUM_WAIT_OBJECTS){
      for(FXint i=numhandles-1; i>=0; --i){
        if(handles->handles[i]==hnd) return true;
        }
      handles->handles[numhandles++]=hnd;
      return true;
      }
#elif defined(HAVE_EPOLL_CREATE1)
    if(0<=hnd){
      struct epoll_event ev;
      ev.events=0;
      if(mode&InputRead) ev.events|=EPOLLIN;
      if(mode&InputWrite) ev.events|=EPOLLOUT;
      if(mode&InputExcept) ev.events|=EPOLLPRI;
      ev.data.fd=hnd;
      if(epoll_ctl(handles->handle,EPOLL_CTL_ADD,hnd,&ev)==0){
        // FIXME //
// FIXME    EPOLL_CTL_MOD if already added
        return true;
        }
      }
#else
    if(0<=hnd && hnd<FD_SETSIZE){
      if(mode&InputRead){ FD_SET(hnd,&handles->handles[0]); }
      if(mode&InputWrite){ FD_SET(hnd,&handles->handles[1]); }
      if(mode&InputExcept){ FD_SET(hnd,&handles->handles[2]); }
      if(numhandles<=hnd){
        numhandles=hnd+1;
        }
      return true;
      }
#endif
    }
  return false;
  }


// Remove handle hnd from list
FXbool FXDispatcher::remHandle(FXInputHandle hnd,FXuint mode){
  if(isInitialized()){
#if defined(WIN32)
    if(hnd!=BadHandle){
      for(FXint i=numhandles-1; i>=0; --i){
        if(handles->handles[i]==hnd){
          handles->handles[i]=handles->handles[--numhandles];
          break;
          }
        }
      return true;
      }
#elif defined(HAVE_EPOLL_CREATE1)
    if(0<=hnd){
      struct epoll_event ev;
      ev.events=0;
      if(mode&InputRead) ev.events|=EPOLLIN;
      if(mode&InputWrite) ev.events|=EPOLLOUT;
      if(mode&InputExcept) ev.events|=EPOLLPRI;
      ev.data.fd=hnd;
      if(epoll_ctl(handles->handle,EPOLL_CTL_DEL,hnd,&ev)==0){
        // FIXME //
// FIXME    EPOLL_CTL_MOD if already added
        return true;
        }
      }
#else
    if(0<=hnd && hnd<numhandles){
      if(mode&InputRead){ FD_CLR(hnd,&handles->handles[0]); }
      if(mode&InputWrite){ FD_CLR(hnd,&handles->handles[1]); }
      if(mode&InputExcept){ FD_CLR(hnd,&handles->handles[2]); }
      if(numhandles==hnd+1){
        while(0<numhandles && !FD_ISSET(numhandles-1,&handles->handles[0]) && !FD_ISSET(numhandles-1,&handles->handles[1]) && !FD_ISSET(numhandles-1,&handles->handles[2])){
          numhandles--;
          }
        }
      return true;
      }
#endif
    }
  return false;
  }

/*******************************************************************************/

// The dispatch driver determines which events have taken place and calls the
// appropriate handling routine.
// Only one event is handled each time through the dispatch() routine, since
// event sources may be added or removed each time a handler is called.
// Thus, once a set of events is determined, we need to keep track of this set
// and only check for new events once all the events from the last check have
// been dealt with.

#if defined(WIN32) //////////////////////////////////////////////////////////////

// Dispatch driver
FXint FXDispatcher::dispatch(FXTime timeout,FXuint flags){
  if(isInitialized()){
    FXTime now,due,delay,interval;
    FXint sig,nxt;

    // Loop till we got something
    while(1){

      // Check for timeout
      delay=forever;
      due=getTimeout();
      if(due<forever){
        now=FXThread::time();
        delay=due-now;
        if(delay<1000L){
          if(dispatchTimeout(due)) return true;
          delay=forever;
          }
        }

      // Check for signal
      sig=sigreceived;
      if(signotified[sig] && sigismember(&handles->signals,sig)==1){
        signotified[sig]=false;
        nxt=64;
        while(--nxt && !signotified[nxt]){ }
        sigreceived=nxt;
        if(dispatchSignal(sig)) return true;
        }

      // Check active handles
      while(0<numraised){
        current=current%numhandles;
        if((WaitForSingleObject(handles->handles[current],0)==WAIT_OBJECT_0){
          numraised--;
          if(dispatchHandle(current,InputRead)) return true; // Which of InputRead or InputWrite ??
          }
        current=current+1;
        }

      // Select active handles and check signals; don't block
      signaled=WaitForMultipleObjects(numhandles,handles->handles,false,0,QS_ALLINPUT);

      // Start scanning with current
      if(WAIT_OBJECT_0<=signaled && signaled<WAIT_OBJECT_0+numhandles){ current=signaled-WAIT_OBJECT_0; numraised=numhandles; }

      // Bad stuff happened
      if(signaled==WAIT_FAILED){ throw FXFatalException("FXDispatcher::dispatch: error waiting on handles."); }

      // No handles were active
      if(signaled==WAIT_TIMEOUT){

        // Idle callback if we're about to block
        if(dispatchIdle()) return true;

        // We're not blocking
        if(blocking<=0) return false;

        interval=FXMIN(delay,blocking);

        // Select active handles and check signals, waiting for timeout or maximum block time
        signaled=WaitForMultipleObjects(numhandles,handles->handles,false,(interval==forever)?INFINITE:(DWORD)(interval/1000000L),QS_ALLINPUT);

        // Bad stuff happened
        if(signaled==WAIT_FAILED){ throw FXFatalException("FXDispatcher::dispatch: error waiting on handles."); }

        // Return if there was no timeout within maximum block time
        if(signaled==WAIT_TIMEOUT && delay>=blocking) return false;
        }
      }
    }
  return false;
  }

#elif defined(HAVE_EPOLL_CREATE1) ///////////////////////////////////////////////

// Dispatch driver
FXbool FXDispatcher::dispatch(FXTime blocking,FXuint flags){
  if(isInitialized()){
    FXTime now,due,delay;
    FXint sig,nxt;

    // Loop till we got something
    while(1){

      // Check for timeout
      delay=forever;
      due=getTimeout();
      if(due<forever){
        now=FXThread::time();
        delay=due-now;
        if(delay<1000L){
          if(dispatchTimeout(due)) return true;
          delay=forever;
          }
        }

      // Check for signal
      sig=sigreceived;
      if(signotified[sig] && sigismember(&handles->signals,sig)==1){
        signotified[sig]=false;
        nxt=64;
        while(--nxt && !signotified[nxt]){ }
        sigreceived=nxt;
        if(dispatchSignal(sig)) return true;
        }

      // Check active handles
      while(0<numraised){
        current=(current+1)%numwatched;
        if(handles->events[current].events&EPOLLIN){
          handles->events[current].events&=~EPOLLIN;
          numraised--;
          if(dispatchHandle(handles->events[current].data.fd,InputRead)) return true;
          }
        if(handles->events[current].events&EPOLLOUT){
          handles->events[current].events&=~EPOLLOUT;
          numraised--;
          if(dispatchHandle(handles->events[current].data.fd,InputWrite)) return true;
          }
        if(handles->events[current].events&EPOLLERR){
          handles->events[current].events&=~EPOLLERR;
          numraised--;
          if(dispatchHandle(handles->events[current].data.fd,InputExcept)) return true;
          }
        }

      FXASSERT(numraised==0);

      // Prepare handles to check
      numwatched=numhandles;

      // Select active handles and check signals; don't block
      numraised=epoll_pwait(handles->handle,handles->events,ARRAYNUMBER(handles->events),0,NULL);

      // Bad stuff happened
      if(numraised<0 && errno!=EAGAIN && errno!=EINTR){ throw FXFatalException("FXDispatcher::dispatch: error waiting on handles."); }

      // No handles were active
      if(numraised==0){

        // Idle callback if we're about to block
        if(dispatchIdle()) return true;

        // We're not blocking
        if(blocking<=0) return false;

        // Prepare handles to check
        numwatched=numhandles;

        // Select active handles and check signals, waiting for timeout or maximum block time
        numraised=epoll_pwait(handles->handle,handles->events,ARRAYNUMBER(handles->events),FXMIN(delay,blocking)/1000000L,NULL);

        // Bad stuff happened
        if(numraised<0 && errno!=EAGAIN && errno!=EINTR){ throw FXFatalException("FXDispatcher::dispatch: error waiting on handles."); }

        // Return if there was no timeout within maximum block time
        if(numraised==0 && delay>=blocking) return false;
        }
      }
    }
  return false;
  }

#else ///////////////////////////////////////////////////////////////////////////

// Helper function
static FXint sselect(FXint nfds,fd_set* readfds,fd_set* writefds,fd_set* errorfds,FXTime wait,const sigset_t* watchset){
#if (_POSIX_C_SOURCE >= 200112L) || (_XOPEN_SOURCE >= 600)
  FXint result;
  if(wait<forever){
    struct timespec delta;
    delta.tv_nsec=wait%1000000000LL;
    delta.tv_sec=wait/1000000000LL;
    result=pselect(nfds,readfds,writefds,errorfds,&delta,watchset);
    }
  else{
    result=pselect(nfds,readfds,writefds,errorfds,NULL,watchset);
    }
  return result;
#else
  FXint result;
  if(wait<forever){
    struct timeval delta;
    wait=(wait+500LL)/1000LL;
    delta.tv_usec=wait%1000000LL;
    delta.tv_sec=wait/1000000LL;
    result=select(nfds,readfds,writefds,errorfds,&delta);
    }
  else{
    result=select(nfds,readfds,writefds,errorfds,NULL);
    }
  return result;
#endif
  }


// Dispatch driver
FXbool FXDispatcher::dispatch(FXTime blocking,FXuint flags){
  if(isInitialized()){
    FXTime now,due,delay;
    FXint sig,nxt;

    // Loop till we got something
    while(1){

      // Check for timeout
      delay=forever;
      due=getTimeout();
      if(due<forever){
        now=FXThread::time();
        delay=due-now;
        if(delay<1000L){
          if(dispatchTimeout(due)) return true;
          delay=forever;
          }
        }

      // Check for signal
      sig=sigreceived;
      if(signotified[sig] && sigismember(&handles->signals,sig)==1){
        signotified[sig]=false;
        nxt=64;
        while(--nxt && !signotified[nxt]){ }
        sigreceived=nxt;
        if(dispatchSignal(sig)) return true;
        }

      // Check active handles
      while(0<numraised){
        current=(current+1)%numwatched;
        if(FD_ISSET(current,&handles->watched[0])){
          FD_CLR(current,&handles->watched[0]);
          numraised--;
          if(dispatchHandle(current,InputRead)) return true;
          }
        if(FD_ISSET(current,&handles->watched[1])){
          FD_CLR(current,&handles->watched[1]);
          numraised--;
          if(dispatchHandle(current,InputWrite)) return true;
          }
        if(FD_ISSET(current,&handles->watched[2])){
          FD_CLR(current,&handles->watched[2]);
          numraised--;
          if(dispatchHandle(current,InputExcept)) return true;
          }
        }

      FXASSERT(numraised==0);

      // Prepare handles to check
      handles->watched[0]=handles->handles[0];
      handles->watched[1]=handles->handles[1];
      handles->watched[2]=handles->handles[2];
      numwatched=numhandles;

      // Select active handles and check signals; don't block
      numraised=sselect(numwatched,&handles->watched[0],&handles->watched[1],&handles->watched[2],0LL,NULL);

      // Bad stuff happened
      if(numraised<0 && errno!=EAGAIN && errno!=EINTR){ throw FXFatalException("FXDispatcher::dispatch: error waiting on handles."); }

      // No handles were active
      if(numraised==0){

        // Idle callback if we're about to block
        if(dispatchIdle()) return true;

        // We're not blocking
        if(blocking<=0) return false;

        // Prepare handles to check
        handles->watched[0]=handles->handles[0];
        handles->watched[1]=handles->handles[1];
        handles->watched[2]=handles->handles[2];
        numwatched=numhandles;

        // Select active handles and check signals, waiting for timeout or maximum block time
        numraised=sselect(numwatched,&handles->watched[0],&handles->watched[1],&handles->watched[2],FXMIN(delay,blocking),NULL);

        // Bad stuff happened
        if(numraised<0 && errno!=EAGAIN && errno!=EINTR){ throw FXFatalException("FXDispatcher::dispatch: error waiting on handles."); }

        // Return if there was no timeout within maximum block time
        if(numraised==0 && delay>=blocking) return false;
        }
      }
    }
  return false;
  }

#endif //////////////////////////////////////////////////////////////////////////

/*******************************************************************************/

// Dispatch when when handle hnd is signaled with mode
FXbool FXDispatcher::dispatchHandle(FXint,FXuint){
  return false;
  }


// Dispatch when a signal was fired
FXbool FXDispatcher::dispatchSignal(FXint){
  return false;
  }


// Dispatch when timeout expires
FXbool FXDispatcher::dispatchTimeout(FXTime){
  return false;
  }


// Dispatch when idle
FXbool FXDispatcher::dispatchIdle(){
  return false;
  }

/*******************************************************************************/

// Exit dispatcher
FXbool FXDispatcher::exit(){
  if(isInitialized()){
#if defined(WIN32)
    ///////////////
#elif defined(HAVE_EPOLL_CREATE1)
    close(handles->handle);
#else
    ///////////////
#endif
    freeElms(handles);
    initialized=false;
    return true;
    }
  return false;
  }


// Destroy dispatcher object
FXDispatcher::~FXDispatcher(){
  exit();
  }

}
