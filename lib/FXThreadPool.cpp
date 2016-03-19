/********************************************************************************
*                                                                               *
*                             T h r e a d   P o o l                             *
*                                                                               *
*********************************************************************************
* Copyright (C) 2006,2009 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "FXThread.h"
#include "FXThreadPool.h"
#include "FXException.h"


/*
  Notes:
  - When the current number of worker threads is greater than spare number to keep,
    terminate the supernumerary ones if, after a small delay, no new jobs come in.
  - Note, only FXWorker threads will be terminated; other threads running the pool
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
*/

using namespace FX;


namespace FX {

/*******************************************************************************/


// Create worker
FXWorker::FXWorker(FXThreadPool *p):pool(p){
  FXTRACE((100,"FXWorker::FXWorker %p\n",this));
  }


// Worker runs jobs, then dies
FXint FXWorker::run(){
  pool->run();
  delete this;
  return 0;
  }


// Destroy worker
FXWorker::~FXWorker(){
  FXTRACE((100,"FXWorker::~FXWorker %p\n",this));
  }


/*******************************************************************************/


// Create job queue with initial size
FXQueue::FXQueue(FXuint sz):jobs(NULL),head(0),tail(0),size(0){
  if(callocElms(jobs,sz)){ size=sz; }
  }


// Change size of queue; return true if success
FXbool FXQueue::setSize(FXuint sz){
  if(resizeElms(jobs,sz)){
    size=sz;
    head=0;
    tail=0;
    return true;
    }
  return false;
  }


// Return size
FXuint FXQueue::getSize() const {
  return size;
  }


// Return jobs
FXuint FXQueue::getCount() const {
  return (head-tail+size)%size;
  }


// Return head
FXuint FXQueue::getHead() const {
  return head;
  }


// Return tail
FXuint FXQueue::getTail() const {
  return tail;
  }


// Check if queue is empty
FXbool FXQueue::isEmpty() const {
  return (head==tail);
  }


// Check if queue is full
FXbool FXQueue::isFull() const {
  FXuint next=(head+1)%size;
  return (next==tail);
  }


// Add job to queue, return true if success
FXbool FXQueue::push(FXRunnable* job){
  FXuint next=(head+1)%size;
  if(next!=tail){
    jobs[head]=job;
    head=next;
    return true;
    }
  return false;
  }


// Remove job from queue, return true if success
FXbool FXQueue::pop(FXRunnable*& job){
  if(head!=tail){
    FXuint next=(tail+1)%size;
    job=jobs[tail];
    tail=next;
    return true;
    }
  return false;
  }


// Destroy job queue
FXQueue::~FXQueue(){
  freeElms(jobs);
  }


/*******************************************************************************/


// Construct an empty thread pool
FXThreadPool::FXThreadPool(FXuint sz):queue(sz),expire(forever),maximum(0),minimum(0),running(0),waiting(0),runs(false){
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
FXbool FXThreadPool::active(){
  return runs;
  }


// Return number of waiting threads
FXuint FXThreadPool::getWaitingThreads(){
  return waiting;
  }


// Return number of running worker threads
FXuint FXThreadPool::getRunningThreads(){
  return running;
  }


// Change minimum number of worker threads
void FXThreadPool::setMinimumThreads(FXuint n){
  mutex.lock();
  minimum=n;
  mutex.unlock();
  }


// Return minimum number of worker threads
FXuint FXThreadPool::getMinimumThreads(){
  return minimum;
  }


// Change maximum number of worker threads
void FXThreadPool::setMaximumThreads(FXuint n){
  mutex.lock();
  maximum=n;
  mutex.unlock();
  }


// Return maximum number of worker threads
FXuint FXThreadPool::getMaximumThreads(){
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
FXbool FXThreadPool::startWorker(){
  FXWorker *wrk=new FXWorker(this);
  if(wrk->start()){
    return true;
    }
  delete wrk;
  return false;
  }


// Start pool
FXuint FXThreadPool::start(FXuint min,FXuint max,FXuint cnt){
  FXuint result=0;
  FXTRACE((150,"FXThreadPool::start(%d,%d,%d)\n",min,max,cnt));
  if(min<=max && cnt<=max){
    mutex.lock();
    if(!runs){
      runs=true;
      minimum=min;
      maximum=max;
      while(result<cnt && startWorker()) result++;
      }
    mutex.unlock();
    }
  return result;
  }


// Wait till all worker threads are idle
FXbool FXThreadPool::wait(){
  mutex.lock();
  while(running && waiting<running){
    wcond.wait(mutex);
    }
  mutex.unlock();
  return runs;
  }


// Stop pool
void FXThreadPool::stop(){
  mutex.lock();
  if(runs){

    // Prevent new jobs
    runs=false;
    minimum=0;
    maximum=0;

    // Wake all producers
    pcond.broadcast();

    // Wait till all workers are idle
    while(running && waiting<running){
      wcond.wait(mutex);
      }

    // Wake all idle workers to quit
    ccond.broadcast();
    }
  mutex.unlock();
  }


// Perform a job
FXbool FXThreadPool::execute(FXRunnable *job,FXTime blocking){
  FXbool result=false;
  mutex.lock();
  if(runs && job){
    if(0<waiting || running>=maximum || startWorker()){
      while(runs && !(result=queue.push(job)) && 0<blocking){
        if(!pcond.wait(mutex,blocking)) break;
        }
      if(result){
        ccond.signal();
        }
      }
    }
  mutex.unlock();
  return result;
  }


// Called by thread to process jobs
FXint FXThreadPool::run(){
  FXRunnable* job;
  mutex.lock();
  ++running;
  ++waiting;
  do{
    --waiting;
    while(queue.pop(job)){
      pcond.signal();
      mutex.unlock();
      try{
        job->run();
        }
      catch(...){
        }
      mutex.lock();
      }
    ++waiting;
    wcond.broadcast();  // Wake all threads
    }
  while(waiting<=minimum && ccond.wait(mutex,expire) && runs);
  --waiting;
  --running;
  mutex.unlock();
  return runs;
  }


// Harvest workers and quit
FXThreadPool::~FXThreadPool(){
  stop();
  FXTRACE((100,"FXThreadPool::~FXThreadPool %p\n",this));
  }

}
