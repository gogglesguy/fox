/********************************************************************************
*                                                                               *
*                     E v e n t   R e a c t o r   C l a s s                     *
*                                                                               *
*********************************************************************************
* Copyright (C) 2006,2007 by Jeroen van der Zijp.   All Rights Reserved.        *
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
* $Id: FXReactor.cpp,v 1.31 2007/02/23 20:12:16 fox Exp $                       *
********************************************************************************/
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXObject.h"
#include "FXThread.h"
#include "FXElement.h"
#include "FXReactor.h"
#include "FXException.h"

/*
  Notes:

  - Need TWO mutexes:

    o One mutex to keep locked except during select().

    o Another mutex around select() to ensure only one thread blocks on the devices,
      i.e. implement leader/follower pattern; in principle, dispatch may be parallel
      while blocking is serialized.

    This allows event stream to be processed by multiple threads, potentially
    having multiple devices processing their dispatch() routine in parallel.

  - Special subclass of FXDevice handles display.

  - Special subclass of FXReactor handles GUI loop.

  - Use new pselect() to have (1) nanosecond wait-time and (2) dispatch non-immediate
    signal handlers properly (see man 2 select_tut).

  - Idea: when inside dispatch, substract fd of dispatch from the set, until dispatch
    returns.  Thus, if reactor processed by multiple threads, no two threads manage
    the same fd.  So the set of fd's being watched is the total set of fd's minus
    the ones currently being dispatched.

  - Process handles in circular fashion so each one gets equal opportunity of being
    handled.
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


// Handles to be watched
struct FXHandles {
#ifdef WIN32
  HANDLE hnd[MAXIMUM_WAIT_OBJECTS];
#else
  fd_set hnd[3];
#endif
  };



/*******************************************************************************/

// Thread-local storage key for reactor
FXAutoThreadStorageKey FXReactor::reactorStorageKey;


FXIMPLEMENT(FXReactor,FXObject,NULL,0)


// Construct reactor object
FXReactor::FXReactor():accessing(true),working(true),current(0),initialized(false){
  memset(signotified,0,sizeof(signotified));
  callocElms(handles,1);
  sigreceived=0;
  maxhandle=-1;
  }


// Initialize reactor
void FXReactor::init(){
  if(!initialized){
    working.lock();
    FXThread::setStorage(reactorStorageKey,this);
    initialized=true;
    }
  }


// Exit reactor
void FXReactor::exit(){
  if(initialized){
    FXThread::setStorage(reactorStorageKey,NULL);
    initialized=false;
    working.unlock();
    }
  }


/*******************************************************************************/

// Append signal to signal-set observed by the reactor
FXbool FXReactor::addSignal(FXint sig){
  if(0<=sig && sig<64){
    accessing.lock();
#ifdef WIN32
#else
    sigset_t singlesigset;
    struct sigaction sigact;
    sigemptyset(&singlesigset);
    sigaddset(&singlesigset,sig);
    pthread_sigmask(SIG_BLOCK,&singlesigset,NULL);
    sigact.sa_handler=signalhandler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags=0;
    sigaction(sig,&sigact,NULL);
#endif
    signotified[sig]=false;
    accessing.unlock();
    return true;
    }
  return false;
  }


// Remove signal from signal-set observed by the reactor
FXbool FXReactor::remSignal(FXint sig){
  if(0<=sig && sig<64){
    accessing.lock();
#ifdef WIN32
#else
    sigset_t singlesigset;
    struct sigaction sigact;
    sigemptyset(&singlesigset);
    sigaddset(&singlesigset,sig);
    pthread_sigmask(SIG_UNBLOCK,&singlesigset,NULL);
    sigact.sa_handler=SIG_DFL;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags=0;
    sigaction(sig,&sigact,NULL);
#endif
    signotified[sig]=false;
    accessing.unlock();
    return true;
    }
  return false;
  }


// Check if handle hnd is in signal-set observed by the reactor
FXbool FXReactor::hasSignal(FXint sig){
  if(0<=sig && sig<64){
    accessing.lock();
#ifdef WIN32
    return true;
#else
    struct sigaction sigact;
    sigaction(sig,NULL,&sigact);
    /// (sigact.sa_handler==signalhandler);
#endif
    accessing.unlock();
    }
  return false;
  }


// Signal handler; note this is a single write operation
// which can not be interrupted by another handler!
void FXReactor::signalhandler(int sig){
  FXReactor *reactor=(FXReactor*)FXThread::getStorage(reactorStorageKey);
  if(reactor){
    reactor->signotified[sig]=true;
    reactor->sigreceived=sig;
    }
  }


/*******************************************************************************/

// Add handle hnd to list
FXbool FXReactor::addHandle(FXInputHandle hnd,FXuint mode){
#ifdef WIN32
  if(hnd!=BadHandle){
    accessing.lock();
    for(FXint i=maxhandle; i>=0; --i){
      if(handles->hnd[i]==hnd){ goto x; }
      }
    handles->hnd[++maxhandle]=hnd;
x:  accessing.unlock();
    return true;
    }
#else
  if(0<=hnd && hnd<FD_SETSIZE){
    accessing.lock();
    if(mode&InputRead){ FD_SET(hnd,&handles->hnd[0]); }
    if(mode&InputWrite){ FD_SET(hnd,&handles->hnd[1]); }
    if(mode&InputExcept){ FD_SET(hnd,&handles->hnd[2]); }
    if(maxhandle<hnd){
      maxhandle=hnd;
      }
    accessing.unlock();
    return true;
    }
#endif
  return false;
  }


// Remove handle hnd from list
FXbool FXReactor::remHandle(FXInputHandle hnd,FXuint mode){
#ifdef WIN32
  if(hnd!=BadHandle){
    accessing.lock();
    for(FXint i=maxhandle; i>=0; --i){
      if(handles->hnd[i]==hnd){
        handles->hnd[i]=handles->hnd[maxhandle--]; break;
        }
      }
    current=0;
    accessing.unlock();
    return true;
    }
#else
  if(0<=hnd && hnd<FD_SETSIZE){
    accessing.lock();
    if(mode&InputRead){ FD_CLR(hnd,&handles->hnd[0]); }
    if(mode&InputWrite){ FD_CLR(hnd,&handles->hnd[1]); }
    if(mode&InputExcept){ FD_CLR(hnd,&handles->hnd[2]); }
    if(maxhandle==hnd){
      while(0<=hnd && !FD_ISSET(hnd,&handles->hnd[0]) && !FD_ISSET(hnd,&handles->hnd[1]) && !FD_ISSET(hnd,&handles->hnd[2])) --hnd;
      maxhandle=hnd;
      }
    current=0;
    accessing.unlock();
    return true;
    }
#endif
  return false;
  }


// Check if handle was watched
FXbool FXReactor::hasHandle(FXInputHandle hnd,FXuint mode){
  FXbool result=false;
#ifdef WIN32
  if(hnd!=BadHandle){
    accessing.lock();
    for(FXint i=maxhandle; i>=0; --i){
      if(handles->hnd[i]==hnd){ result=true; break; }
      }
    accessing.unlock();
    }
#else
  if(0<=hnd && hnd<FD_SETSIZE){
    accessing.lock();
    result=((mode&InputRead) && FD_ISSET(hnd,&handles->hnd[0])) || ((mode&InputWrite) && FD_ISSET(hnd,&handles->hnd[1])) || ((mode&InputExcept) && FD_ISSET(hnd,&handles->hnd[2]));
    accessing.unlock();
    }
#endif
  return result;
  }


/*******************************************************************************/


// Dispatch when handle is signalled
FXbool FXReactor::doHandle(FXInputHandle,FXint){
  return true;
  }


// Dispatch when timeout expires
FXbool FXReactor::doTimeout(FXTime){
  return true;
  }


// Dispatch when a signal was fired.
FXbool FXReactor::doSignal(FXint sig){
  return true;
  }


// Dispatch when idle
FXbool FXReactor::doIdle(){
  return true;
  }


#ifdef WIN32

// Process active handles
FXint FXReactor::processActiveHandles(FXTime block,FXuint flags){
  return 1;
  }

#else

// Process active handles
FXint FXReactor::processActiveHandles(FXTime block,FXuint flags){
  FXint maxhand,nhand,old,sig,nxt,mode;
  sigset_t emptyset,savedset;
#ifdef __USE_XOPEN2K
  struct timespec delta;
#else
  struct timeval delta;
#endif
  fd_set hand[3];

  // Empty set
  sigemptyset(&emptyset);

  // Prepare handles to check
  hand[0]=handles->hnd[0];
  hand[1]=handles->hnd[1];
  hand[2]=handles->hnd[2];
  maxhand=maxhandle;

  // Do a quick poll for any ready events or inputs
#ifdef __USE_XOPEN2K
  delta.tv_nsec=0;
  delta.tv_sec=0;
  nhand=pselect(maxhand+1,&hand[0],&hand[1],&hand[2],&delta,&emptyset);
#else
  delta.tv_usec=0;
  delta.tv_sec=0;
  pthread_sigmask(SIG_SETMASK,&emptyset,&savedset);
  nhand=select(maxhand+1,&hand[0],&hand[1],&hand[2],&delta);
  pthread_sigmask(SIG_SETMASK,&savedset,NULL);
#endif

  // No handle active yet
  if(nhand==0){

    // Do some idle stuff
    doIdle();

    // We're not blocking
    if(block<=0) return 0;

    // Prepare handles to check
    hand[0]=handles->hnd[0];
    hand[1]=handles->hnd[1];
    hand[2]=handles->hnd[2];
    maxhand=maxhandle;

    // Waiting for finite amount of time?
    if(block<forever){
      block=block-FXThread::time();

      // Time is already due; return right away!
      if(block<=0) return 0;

      // Exit critical section
      working.unlock();

      // Block till timer or event or interrupt
#ifdef __USE_XOPEN2K
      delta.tv_nsec=block%1000000000;
      delta.tv_sec=block/1000000000;
      nhand=pselect(maxhand+1,&hand[0],&hand[1],&hand[2],&delta,&emptyset);
#else
      delta.tv_usec=(block/1000)%1000000;
      delta.tv_sec=block/1000000000;
      pthread_sigmask(SIG_SETMASK,&emptyset,&savedset);
      nhand=select(maxhand+1,&hand[0],&hand[1],&hand[2],&delta);
      pthread_sigmask(SIG_SETMASK,&savedset,NULL);
#endif

      // Enter critical section
      working.lock();
      }

    // If no timers, we block till event or interrupt
    else{

      // Exit critical section
      working.unlock();

      // Block until something happens
#ifdef __USE_XOPEN2K
      nhand=pselect(maxhand+1,&hand[0],&hand[1],&hand[2],NULL,&emptyset);
#else
      pthread_sigmask(SIG_SETMASK,&emptyset,&savedset);
      nhand=select(maxhand+1,&hand[0],&hand[1],&hand[2],NULL);
      pthread_sigmask(SIG_SETMASK,&savedset,NULL);
#endif

      // Enter critical section
      working.lock();
      }
    }

  // Normal case
  if(0<=hand){

    // Any handles active?
    if(0<nhand){
      old=current;
      FXASSERT(0<=current && current<=maxhand);
      do{
        mode=0;
        if(FD_ISSET(current,&hand[0])) mode|=InputRead;
        if(FD_ISSET(current,&hand[1])) mode|=InputWrite;
        if(FD_ISSET(current,&hand[2])) mode|=InputExcept;
        if(mode){
          doHandle(current,mode);
          }
        current=(current+1)%(maxhand+1);
        }
      while(current!=old);
      return 1;
      }

    // Timed out
    return 0;
    }

  // Check for signals
  if(sigreceived){
    sig=sigreceived;
    nxt=64;
    signotified[sig]=false;
    while(--nxt && !signotified[nxt]);
    sigreceived=nxt;
    doSignal(sig);
    return 0;
    }

  return 0;
  }

#endif


/*******************************************************************************/

// Destroy reactor object
FXReactor::~FXReactor(){
  freeElms(handles);
  }


}
