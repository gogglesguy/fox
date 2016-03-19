/********************************************************************************
*                                                                               *
*            C o n c u r r e n t   T a s k   E x e c u t i o n                  *
*                                                                               *
*********************************************************************************
* Copyright (C) 2010,2011 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include <new>
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "FXAtomic.h"
#include "FXElement.h"
#include "FXArray.h"
#include "FXPtrList.h"
#include "FXException.h"
#include "FXRunnable.h"
#include "FXMutex.h"
#include "FXCondition.h"
#include "FXSemaphore.h"
#include "FXAutoThreadStorageKey.h"
#include "FXThread.h"
#include "FXWorker.h"
#include "FXConcurrent.h"

/*
  Notes:

  - Entering/leaving FXConcurrent::run only happens when allowed.
  - Changing list of queues only one thread at a time.
  - Executor::execute only returns after jobs are dispatched (not necessarily
    completed yet).
  - Completion class gets incremented when new jobs are spawned, decremented
    when they are completed.  When reaching zero, the waiting thread returns.
  - This is done so a single Completion object may be passed to multiple calls
    to FXConcurrent::execute.  We fall through wait() when ALL of them are done.
  - Note that if you start taskset #1 and then taskset #2, both having completion
    objects, its entirely possible for completion object of taskset #2 to finish
    before taskset #1!
*/

using namespace FX;

namespace FX {

/*******************************************************************************/

// Create completion object
FXCompletion::FXCompletion():count(0){
  }


// Change the number of running tasks
void FXCompletion::expect(FXint cnt){
  atomicAdd(&count,cnt);
  }


// Task was completed; wake waiting thread if it was the last task
void FXCompletion::notify(){
  if(atomicAdd(&count,-1)==1){
    FXMutexLock locker(mutex);
    condition.signal();
    }
  }


// Wait till associated tasks are done
void FXCompletion::wait(){
  if(!done()){
    FXMutexLock locker(mutex);
    while(!done()){condition.wait(mutex);}
    }
  }


// Return true if associated tasks are done
FXbool FXCompletion::done() const {
  return count==0;
  }


// Destroy completion object
FXCompletion::~FXCompletion(){
  }

/*******************************************************************************/



// Work load queue
class FXWorkQueue {
private:
  struct WL {
    const FXTask *task; // Task
    void         *arg;  // Optional argument of the task
    FXCompletion *cmp;  // Optional completion
    };
private:
  FXArray<WL>     list; // Work packets
  FXSemaphore     ec;   // Empty cells
  FXSemaphore     fc;   // Filled cells
  FXint           rp;   // Read pointer
  FXint           wp;   // Write pointer
private:
  FXWorkQueue(const FXWorkQueue&);
  FXWorkQueue &operator=(const FXWorkQueue&);
public:

  // Initialize queue and allocate given size ring buffer
  FXWorkQueue(FXint sz=32):list(sz),ec(sz),fc(0),rp(0),wp(0){
    }

  // Add new object to queue
  FXbool push(const FXTask* task,void* arg=NULL,FXCompletion* cmp=NULL){
    ec.wait();
    list[wp].task=task;
    list[wp].arg=arg;
    list[wp].cmp=cmp;
    wp=(wp+1)%list.no();
    fc.post();
    return true;
    }

  // Try to add new object to queue
  FXbool trypush(const FXTask* task,void* arg=NULL,FXCompletion* cmp=NULL){
    if(ec.trywait()){
      list[wp].task=task;
      list[wp].arg=arg;
      list[wp].cmp=cmp;
      wp=(wp+1)%list.no();
      fc.post();
      return true;
      }
    return false;
    }

  // Pop element from queue
  FXbool pop(const FXTask*& task,void*& arg,FXCompletion*& cmp){
    fc.wait();
    task=list[rp].task;
    arg=list[rp].arg;
    cmp=list[rp].cmp;
    rp=(rp+1)%list.no();
    ec.post();
    return (task!=NULL);
    }

  // Destructor
 ~FXWorkQueue(){
    }
  };


/*******************************************************************************/

// Create executor with no workers
FXConcurrent::FXConcurrent():started(0),stopped(0),running(false),slots(32){
  }


// Create a (subclass of) Worker
FXWorker* FXConcurrent::createWorker(){
  return new FXWorker(this);
  }
  
  
// Make a new worker
FXWorker* FXConcurrent::startWorker(){
  FXWorker* worker=createWorker();
  if(!worker->start()){
    delete worker;
    return NULL;
    }
  return worker;
  }


// Start executor with given number of threads; queues will have given number of slots
FXint FXConcurrent::start(FXint cnt,FXint slts){
  FXScopedMutex locker(qmutex);
  register FXint result=0;
  if(!running && 0<cnt && 1<slts){
    FXint total=started;
    slots=slts;
    running=true;
    while(result<cnt && startWorker()){
      result++;
      }
    total+=result;
    while((total-started)>0){
      qcondition.wait(qmutex);
      }
    }
  return result;
  }


// Return true if running
FXbool FXConcurrent::active() const {
  return running;
  }


// Return number of threads
FXint FXConcurrent::numThreads() const {
  return (started-stopped);
  }


// Add work queue for athread
FXbool FXConcurrent::appendThreadQueue(FXWorkQueue* wq){
  FXScopedMutex locker(qmutex);
  if(queues.append(wq)){
    started++;
    qcondition.signal();
    return true;
    }
  return false;
  }


// Remove work queue of a thread
FXbool FXConcurrent::removeThreadQueue(FXWorkQueue* wq){
  FXScopedMutex locker(qmutex);
  if(queues.remove(wq)){
    stopped++;
    qcondition.signal();
    return true;
    }
  return false;
  }


// Called by processor or any other thread
FXint FXConcurrent::run(){
  FXWorkQueue   wq(slots);
  const FXTask *task;
  void         *arg;
  FXCompletion *cmp;

  // Add this thread's work queue
  if(!appendThreadQueue(&wq)) return false;

  // Run jobs from queue
  while(wq.pop(task,arg,cmp)){
    try{ task->exec(arg); } catch(...) { }
    if(cmp) cmp->notify();
    }

  // Remove this thread's work queue
  if(!removeThreadQueue(&wq)) return false;

  return true;
  }


// Compute a bunch of things
FXint FXConcurrent::execute(const FXTask* task,FXint argc,FXint indx){
  FXScopedMutex locker(qmutex);
  register FXint result=0;
  if(running && 0<queues.no() && 0<argc){
    for(FXint i=0; i<argc; ++i){
      if(queues[(i+indx)%queues.no()]->push(task,(void*)(FXival)i)) result++;
      }
    }
  return result;
  }


// Compute a bunch of things
FXint FXConcurrent::execute(const FXTask* task,void** argv,FXint argc,FXint indx){
  FXScopedMutex locker(qmutex);
  register FXint result=0;
  if(running && 0<queues.no() && 0<argc && argv){
    for(FXint i=0; i<argc; ++i){
      if(queues[(i+indx)%queues.no()]->push(task,argv[i])) result++;
      }
    }
  return result;
  }


// Compute a bunch of things
FXint FXConcurrent::execute(FXCompletion* completion,const FXTask* task,FXint argc,FXint indx){
  FXScopedMutex locker(qmutex);
  register FXint result=0;
  if(running && 0<queues.no() && 0<argc){
    completion->expect(argc);           // Add expected number to running count
    for(FXint i=0; i<argc; ++i){
      if(queues[(i+indx)%queues.no()]->push(task,(void*)(FXival)i,completion)) result++;
      }
    completion->expect(result-argc);    // Substract the ones that didn't start
    }
  return result;
  }


// Compute a bunch of things
FXint FXConcurrent::execute(FXCompletion* completion,const FXTask* task,void** argv,FXint argc,FXint indx){
  FXScopedMutex locker(qmutex);
  register FXint result=0;
  if(running && 0<queues.no() && 0<argc && argv){
    completion->expect(argc);           // Add expected number to running count
    for(FXint i=0; i<argc; ++i){
      if(queues[(i+indx)%queues.no()]->push(task,argv[i],completion)) result++;
      }
    completion->expect(result-argc);    // Substract the ones that didn't start
    }
  return result;
  }


// Wait till all threads have terminated
FXint FXConcurrent::wait(){
  FXScopedMutex locker(qmutex);
  while((started-stopped)>0){
    qcondition.wait(qmutex);
    }
  return (started-stopped);
  }


// Stop executor, causing all threads to terminate
FXint FXConcurrent::stop(){
  FXScopedMutex locker(qmutex);
  if(running){
    running=false;
    for(FXint i=0; i<queues.no(); ++i){
      queues[i]->push(NULL,NULL,NULL);
      }
    }
  return (started-stopped);
  }


// Delete executors
FXConcurrent::~FXConcurrent(){
  stop();
  wait();
  }

}
