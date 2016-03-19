/********************************************************************************
*                                                                               *
*                             T h r e a d   P o o l                             *
*                                                                               *
*********************************************************************************
* Copyright (C) 2006,2007 by Jeroen van der Zijp.   All Rights Reserved.        *
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
* $Id: FXThreadPool.cpp,v 1.67 2007/07/09 16:27:16 fox Exp $                    *
********************************************************************************/
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "FXThread.h"
#include "FXThreadPool.h"
#include "FXException.h"


/*
  Notes:
  - Thing is when to terminate workers when current number is greater than spare
    number to keep. (most likely solution: when a worker finishes a job, and there
    is no immediate job for it available, check if the count exceeds spares.  If so,
    then let the job terminate).
  - Always last-in, first-out, i.e. assign new job to most recently finished worker;
    this worker still as current working-set in cache and therefore is best suited
    to tackle new job [if we did lifo then the working set would be stale and that
    might be slower].
  - Default implementation simply "whacks" all workers when destructor runs; suggested
    subclasses or usage is to call some API to terminate gracefully [hang around till
    all jobs are finished and all that].
  - Ideas:
    1) execute() should check if a free worker is available; if so, then that
       worker will perform FXRunnable.
    2) If not, we check current number of threads v.s. maximum number of threads,
       and if maximum not yet exceeded, spawn a new thread to perform the job.
    3) Otherwise, we block until a worker is ready to perform the job.
       An alternate possibility is to perform the job using the calling thread
       itself.  This might be useful for debugging code [simply set maximum
       number of threads to 0].  We may opt to exercise this option only if
       maximum number of threads has been set to zero [i.e. no hope of a existing
       worker to become available, while at the same time also being unable to
       create a new worker.
*/

using namespace FX;


namespace FX {

/*******************************************************************************/


// Create worker
FXWorker::FXWorker(FXThreadPool *ptr,FXRunnable* job):pool(ptr),next(NULL),task(job),runs(true){
  pool->appendWorker(this);
  FXTRACE((100,"FXWorker::FXWorker %p\n",this));
  }


// Is worker running
FXbool FXWorker::active(){
  mutex.lock();
  FXbool result=runs;
  mutex.unlock();
  return result;
  }


// Return thread pool
FXThreadPool* FXWorker::getPool() const {
  return pool;
  }


// Get task
FXRunnable* FXWorker::getTask(){
  register FXRunnable* job;
  mutex.lock();
  job=task;
  task=NULL;
  mutex.unlock();
  return job;
  }


// Wait for task
FXRunnable* FXWorker::waitTask(){
  register FXRunnable* job;
  mutex.lock();
  while(!task && runs){ condition.wait(mutex); }
  job=task;
  task=NULL;
  mutex.unlock();
  return job;
  }


// Set task
void FXWorker::setTask(FXRunnable* job){
  mutex.lock();
  task=job;
  runs=(job!=NULL);
  condition.signal();                           // Signal thread that task is available
  mutex.unlock();
  }


// Run worker
FXint FXWorker::run(){
  register FXRunnable* job;
  while((job=pool->getTask(this))!=NULL){       // While there are jobs
    try{
      job->run();                               // Run the job
      }
    catch(...){                                 // Catch any exceptions that may have ensued
      }
    }
  delete this;                                  // Then die!
  return 0;
  }


// Destroy worker
FXWorker::~FXWorker(){
  FXTRACE((100,"FXWorker::~FXWorker %p\n",this));
  pool->removeWorker(this);
  }


/*******************************************************************************/

// Construct an empty thread pool
FXThreadPool::FXThreadPool():mutex(true),waiters(NULL),maximum(0),minimum(0),running(0),waiting(0),runs(false){
  FXTRACE((100,"FXThreadPool::FXThreadPool %p\n",this));
  }


// Construct an thread pool and start it
FXThreadPool::FXThreadPool(FXint min,FXint max,FXint run):mutex(true),waiters(NULL),maximum(0),minimum(0),running(0),waiting(0),runs(false){
  FXTRACE((100,"FXThreadPool::FXThreadPool %p\n",this));
  start(min,max,run);
  }


// Is pool running
FXbool FXThreadPool::active(){
  mutex.lock();
  FXbool result=runs;
  mutex.unlock();
  return result;
  }


// Return number of waiting threads
FXint FXThreadPool::getWaitingThreads(){
  mutex.lock();
  FXint result=waiting;
  mutex.unlock();
  return result;
  }


// Return number of running worker threads
FXint FXThreadPool::getRunningThreads(){
  mutex.lock();
  FXint result=running;
  mutex.unlock();
  return result;
  }


// Change minimum number of worker threads
void FXThreadPool::setMinimumThreads(FXint n){
  mutex.lock();
  minimum=n;
  mutex.unlock();
  }


// Return minimum number of worker threads
FXint FXThreadPool::getMinimumThreads(){
  mutex.lock();
  FXint result=minimum;
  mutex.unlock();
  return result;
  }


// Change maximum number of worker threads
void FXThreadPool::setMaximumThreads(FXint n){
  mutex.lock();
  maximum=n;
  mutex.unlock();
  }


// Return maximum number of worker threads
FXint FXThreadPool::getMaximumThreads(){
  mutex.lock();
  FXint result=maximum;
  mutex.unlock();
  return result;
  }


// Get next task
FXRunnable* FXThreadPool::getTask(FXWorker* wrk){
  register FXRunnable *job=wrk->getTask();
  if(job==NULL){
    mutex.lock();
    if(waiting<minimum){
      wrk->next=waiters;
      waiters=wrk;
      waiting++;
      condition.signal();               // Notify waiting-list has changed
      mutex.unlock();
      return wrk->waitTask();           // Block thread
      }
    mutex.unlock();
    }
  return job;
  }


// Add new worker to the worker list
void FXThreadPool::appendWorker(FXWorker*){
  mutex.lock();
  running++;
  FXTRACE((100,"appendWorker running=%d\n",running));
  mutex.unlock();
  }


// Remove worker from the worker list
void FXThreadPool::removeWorker(FXWorker*){
  mutex.lock();
  running--;
  FXTRACE((100,"removeWorker running=%d\n",running));
  condition.signal();			// Notify that running-list has changed
  mutex.unlock();
  }


// Create and start a worker
FXWorker *FXThreadPool::startWorker(FXRunnable* job){
  register FXWorker *wrk=new FXWorker(this,job);
  if(wrk->start()){
    return wrk;
    }
  delete wrk;
  return NULL;
  }


// Start pool
FXint FXThreadPool::start(FXint min,FXint max,FXint run){
  register FXWorker *wrk;
  if(min<0 || run<0 || min>max || run>min){ fxerror("FXThreadPool::start: bad argument.\n"); }
  if(!runs){
    mutex.lock();
    minimum=min;
    maximum=max;
    runs=true;
    while(running<run){                 // Start a number of workers immediately
      wrk=startWorker(NULL);
      if(!wrk) break;
      }
    FXTRACE((150,"FXThreadPool::start(%d,%d,%d), %d running\n",min,max,run,running));
    mutex.unlock();
    }
  return running;
  }


// Wait till all worker threads are idle
FXint FXThreadPool::wait(){
  if(runs){
    mutex.lock();
    while(waiting<running && runs){     // Tricky: another thread may have stopped the pool
      condition.wait(mutex);            // Reawaken when either waiting or running is changed
      }
    FXTRACE((150,"FXThreadPool::wait(), %d waiting, %d running\n",waiting,running));
    mutex.unlock();
    }
  return waiting;
  }


// Stop pool
FXint FXThreadPool::stop(){
  register FXWorker *wrk;
  if(runs){
    mutex.lock();
    minimum=0;                          // All workers will exit
    maximum=0;
    runs=false;
    while(waiting){                     // Tell all available (blocked) workers to quit
      wrk=waiters;
      waiters=wrk->next;
      waiting--;
      wrk->setTask(NULL);
      }
    while(running){                     // Wait till all workers have quit
      condition.wait(mutex);
      }
    FXTRACE((150,"FXThreadPool::stop(), end %d running %d waiting\n",running,waiting));
    mutex.unlock();
    }
  return running;
  }


// Perform a job
FXWorker* FXThreadPool::execute(FXRunnable *job,FXbool block){
  register FXWorker *wrk=NULL;
  if(!job){ fxerror("FXThreadPool::execute: NULL job argument.\n"); }
  if(runs){
    mutex.lock();
    while(!waiters && running>=maximum && block && runs){       // Tricky: another thread may have stopped the pool!
      condition.wait(mutex);
      }
    if(waiters){
      wrk=waiters;
      waiters=wrk->next;
      waiting--;
      wrk->setTask(job);
      }
    else if(running<maximum){
      wrk=startWorker(job);
      }
    mutex.unlock();
    }
  return wrk;
  }


// Harvest workers and quit
FXThreadPool::~FXThreadPool(){
  stop();
  FXTRACE((100,"FXThreadPool::~FXThreadPool %p\n",this));
  }


}
