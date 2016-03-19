/********************************************************************************
*                                                                               *
*                             T h r e a d   P o o l                             *
*                                                                               *
*********************************************************************************
* Copyright (C) 2006,2012 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "FXElement.h"
#include "FXPtrQueue.h"
#include "FXMutex.h"
#include "FXCondition.h"
#include "FXAutoThreadStorageKey.h"
#include "FXRunnable.h"
#include "FXThread.h"
#include "FXWorker.h"
#include "FXThreadPool.h"
#include "FXException.h"


/*
  Notes:
  - When the current number of worker threads is greater than spare number to keep,
    terminate the supernumerary ones if, after a small delay, no new jobs come in.
  - Note, only FXWorker threads should be terminated; other threads running the pool
    should probably stay in.
  - But we need some mechanism whereby to identify such non-FXWorker threads.
  - Expiration time is adjustable.
  - Maybe need API to change queue size; but only if FXThreadPool is not running.
  - When adding new job, start new worker if maximum is not yet exceeded and no
    workers are waiting (idle).
  - Who can change what variable?
      - running, waiting: Change only when worker thread is outside cmutex CS.
      - maximum, minimum: Change only when worker thread is outside cmutex CS.
      - queue head:       Only changed by producer.
      - queue tail:       Only changed by consumer (worker threads).
  - How to break a PARTICULAR thread from the FXThreadPool run loop?
  - How to make sure certain thread(s) are NOT removed from the run loop?
  - Minor problem: may overshoot maximum number of threads in a few cases
    due to race between statement in execute() and incrementing running count
    in run().  Previous solution was no good since we could have non-FXWorker
    thread run the threadpool as well.
  - We want a wait() function which returns when the jobs in the queue at the
    time of the call are finished, irrespective of any jobs preceding or following
    it.  The current wait() is not satisfactory since we only return if all threads
    are idle (which is more restrictive than called for).
  - Avoid signalling producer if no producers are waiting for room in the queue.
  - Separate variable #started and #stopped needed because of possibility of race
    between start() and stop() in short succession.
*/

using namespace FX;


namespace FX {

/*******************************************************************************/


// Construct an empty thread pool
FXThreadPool::FXThreadPool(FXuint sz):queue(sz),expire(forever),maximum(0),minimum(0),started(0),stopped(0),pwaiting(0),cwaiting(0),watching(0),runs(false){
  FXTRACE((100,"FXThreadPool::FXThreadPool %p\n",this));
  }


// Change queue size, return true if success
FXbool FXThreadPool::setSize(FXuint sz){
  FXbool result=false;
  mutex.lock();
  if(!runs && queue.setSize(sz)){
    result=true;
    }
  mutex.unlock();
  return result;
  }


// Return queue size
FXuint FXThreadPool::getSize() const {
  return queue.getSize();
  }


// Is pool running
FXbool FXThreadPool::active() const {
  return runs;
  }


// Return number of waiting threads
FXuint FXThreadPool::getWaitingThreads() const {
  return cwaiting;
  }


// Return number of running worker threads
FXuint FXThreadPool::getRunningThreads() const {
  return (started-stopped);
  }


// Change minimum number of worker threads
void FXThreadPool::setMinimumThreads(FXuint n){
  mutex.lock();
  minimum=n;
  mutex.unlock();
  }


// Return minimum number of worker threads
FXuint FXThreadPool::getMinimumThreads() const {
  return minimum;
  }


// Change maximum number of worker threads
void FXThreadPool::setMaximumThreads(FXuint n){
  mutex.lock();
  maximum=n;
  mutex.unlock();
  }


// Return maximum number of worker threads
FXuint FXThreadPool::getMaximumThreads() const {
  return maximum;
  }


// Change expiration time
void FXThreadPool::setExpiration(FXTime ns){
  expire=ns;
  }


// Get expiration time
FXTime FXThreadPool::getExpiration() const {
  return expire;
  }


// Create and start a worker
FXWorker* FXThreadPool::startWorker(){
  FXWorker *worker=new FXWorker(this);
  if(!worker->start()){
    delete worker;
    return NULL;
    }
  return worker;
  }


// Start pool
FXuint FXThreadPool::start(FXuint min,FXuint max,FXuint cnt){
  FXuint result=0;
  FXTRACE((150,"FXThreadPool::start(%d,%d,%d)\n",min,max,cnt));
  if(min<=max && cnt<=min){
    mutex.lock();
    if(!runs){

      // Set thread count range
      minimum=min;
      maximum=max;

      // Start workers right away
      if(0<cnt){

        // Start number of workers
        FXuint total=started;
        while(result<cnt && startWorker()){
          result++;
          }

        // Wait till workers are ready
        total+=result;
        while(0<(total-started)){
          wcond.wait(mutex);
          }
        }

      // We're running
      runs=true;
      }
    mutex.unlock();
    }
  return result;
  }


// Wait until all jobs currently in the queue are finished
FXbool FXThreadPool::wait(){
  mutex.lock();

  // Add watcher
  ++watching;

  // Wait until queue is empty and all are waiting
  while(!queue.isEmpty() || cwaiting<getRunningThreads()){
    wcond.wait(mutex);
    }

  // Remove watcher
  --watching;

  mutex.unlock();
  return runs;
  }


// Stop pool
void FXThreadPool::stop(){
  mutex.lock();
  if(runs){

    // Prevent new jobs
    runs=false;

    // Wake all producers
    pcond.broadcast();

    // Add watcher
    ++watching;

    // Wait until queue is empty and all are waiting
    while(!queue.isEmpty() || cwaiting<getRunningThreads()){
      wcond.wait(mutex);
      }

    // Remove watcher
    --watching;

    // All workers should terminate
    minimum=0;
    maximum=0;

    // Wake all idle workers to quit
    ccond.broadcast();

    // Wait till they've stopped running
    while(started!=stopped){
      wcond.wait(mutex);
      }
    }
  mutex.unlock();
  }


/*
FIXME wait for ticket

// Wait for job with given ticket to be completed;
// if job was completed, return true.
// if ticket number invalid, return false.
// otherwise, wait.
FXbool FXThreadPool::wait(FXint ticket){

  // Check for invalid ticket
  // Two cases:
  //  1.  Ticket has not yet dispatched (item still in queue)
  //      In this case ticket number must be inside range of
  //      oldest to newest ticket.
  //  2.  Ticket has already been dispatched.  In this case,
  //      visit linked list of ticket-lumps to see if this
  //      ticket is in the list.
  //
  // If neither of these is the case, the ticket has expired already
  // and we can return with false.
  //
  // Otherwise, create wait-lump on the stack, linked from threadpool the
  // in order of increasing ticket number, and wait on completion
  // condition variable.

  // NOTE a worker thread can also call wait, but we must make sure
  // it would not be waiting on its own ticket...
  // For this purpose store the thread ID into the ticket-lump.
  }
*/

// Perform a job
FXbool FXThreadPool::execute(FXRunnable *job,FXTime blocking){
  FXbool result=false;
  mutex.lock();
  if(__likely(runs && job)){
    if(0<cwaiting || maximum<=getRunningThreads() || startWorker()){

      // Check and possibly wait for room in the queue
      while(runs && !(result=queue.push(job)) && 0<blocking){
        ++pwaiting;
        if(!pcond.wait(mutex,blocking)){ --pwaiting; break; }
        --pwaiting;
        }

      // Signal only if anyone's waiting
      if(result && cwaiting){
        ccond.signal();
        }
      }
    }
  mutex.unlock();
  return result;                // FIXME instead of true/false, return ticket number
  }


// Called by thread to process jobs
FXint FXThreadPool::run(){
  // FIXME ticket-lump on stack here, linked from thread pool
  // this keeps track of currently worked-on tickets.
  // Store the thread ID into the ticket-lump in order to allow
  // checking if a job tries to wait on itself (which is an error).
  FXRunnable* job;
  mutex.lock();
  ++started;
  ++cwaiting;
  wcond.signal();
  do{
    --cwaiting;
    while(queue.pop(job)){
      // FIXME write the new job's ticket value into the ticket-lump of
      // this worker thread.
      if(pwaiting) pcond.signal();
      if(cwaiting && !queue.isEmpty()) ccond.signal();
      mutex.unlock();
      try{
        job->run();
        }
      catch(...){
        }
      mutex.lock();
      }
    ++cwaiting;
    // FIXME job has just completed.
    // If the list of wait-lumps linked from threadpool is non-empty,
    // visit the list to see if there's a matching ticket with the
    // one we've been working on.  If a match, signal the completion
    // condition.  If there is no match, then just do next job.
    if(watching) wcond.broadcast();
    }
  while(cwaiting<=minimum && ccond.wait(mutex,expire));         // FIXME it would be nice if a *particular* thread could exit the loop
  --cwaiting;
  ++stopped;
  wcond.signal();
  mutex.unlock();
  // FIXME unlink ticket-lump from linked list of ticket lumps
  // since its now gone.
  return 0;
  }


// Harvest workers and quit
FXThreadPool::~FXThreadPool(){
  stop();
  FXTRACE((100,"FXThreadPool::~FXThreadPool %p\n",this));
  }

}
