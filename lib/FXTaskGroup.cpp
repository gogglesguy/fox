/********************************************************************************
*                                                                               *
*                        T a s k   G r o u p   C l a s s                        *
*                                                                               *
*********************************************************************************
* Copyright (C) 2012,2014 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "FXException.h"
#include "FXElement.h"
#include "FXArray.h"
#include "FXPtrList.h"
#include "FXAtomic.h"
#include "FXMutex.h"
#include "FXSemaphore.h"
#include "FXCondition.h"
#include "FXBarrier.h"
#include "FXRunnable.h"
#include "FXAutoThreadStorageKey.h"
#include "FXThread.h"
#include "FXWorker.h"
#include "FXLFQueue.h"
#include "FXThreadPool.h"
#include "FXTaskGroup.h"


/*
  Notes:

  - FXTaskGroup executes a special runnable (FXTaskGroup::Task) executing the passed runnable.
    This special runnable has backlink to group, and group gets signaled by this special
    runnable when its passed runnable is done.

  - The interface can be passed ANY FXRunnable. Why? because this is the most general,
    and runnables that were used for FXThreadPool could now be used for FXTaskGroup instead.

  - Of course, passing special runnable DIRECTLY may be a bit more efficient; but one problem
    is you'd have to re-implement the run() to explicitly update the completion counter, and
    thus this becomes something the user has to do rather than it being something done in the
    FOX library itself.  Someone will mess up.

  - Early termination:

      o It should be possible to terminate a parallel group early.

      o This means one thread sets some flag in the group, and this flag
        is periodically examined by the other threads to determine early
        termination.

      o A special case of early termination is abort. An abort should probably
        terminate not only the group, but also all the parent groups.
        Normal termination just causes early exit of the group.

      o Should we also use FXRunnable's return code?

*/

using namespace FX;

/*******************************************************************************/

namespace FX {


// Construct new task group task
FXTaskGroup::Task::Task(FXTaskGroup* g,FXRunnable *r):taskgroup(g),runnable(r){
  taskgroup->incrementAndReset();
  }


// Process task
FXint FXTaskGroup::Task::run(){
  try{
    runnable->run();
    }
  catch(...){
    delete this;
    throw;
    }
/*
  if(!group->isCancelled()){
    try{
      runnable->run();
      }
    catch(const FXResourceException&){         // FIXME perhaps dedicated FXException type to cancel the group, and another FXException type to cancel everything recursively...
      group->setCancelled(true);
      }
    }
*/
  delete this;
  return 0;
  }


// Destroy task group task
FXTaskGroup::Task::~Task(){
  taskgroup->decrementAndNotify();
  }

/*******************************************************************************/

// Create new group of tasks
FXTaskGroup::FXTaskGroup():threadpool(FXThreadPool::instance()),completion(1),counter(0){
  if(!threadpool){ fxerror("FXTaskGroup::FXTaskGroup: No thread pool was set."); }
  }


// Create new group of tasks
FXTaskGroup::FXTaskGroup(FXThreadPool* p):threadpool(p),completion(1),counter(0){
  if(!threadpool){ fxerror("FXTaskGroup::FXTaskGroup: No thread pool was set."); }
  }


// Task was started
void FXTaskGroup::incrementAndReset(){
  if(atomicAdd(&counter,1)==0){
    completion.trywait();
    }
  }


// Task has completed
void FXTaskGroup::decrementAndNotify(){
  if(atomicAdd(&counter,-1)==1){
    completion.post();
    }
  }


// Start task
FXbool FXTaskGroup::execute(FXRunnable* task){
  if(__likely(task)){
    if(threadpool->execute(new FXTaskGroup::Task(this,task))){
      return true;
      }
    }
  return false;
  }


// Start task and wait
FXbool FXTaskGroup::executeAndRun(FXRunnable* task){
  if(__likely(task)){
    if(threadpool->executeAndRunWhile(new FXTaskGroup::Task(this,task),counter)){
      return true;
      }
    }
  return false;
  }


// Wait for completion
FXbool FXTaskGroup::wait(){
  return threadpool->waitWhile(counter);
  }


// Wait for completion
FXbool FXTaskGroup::waitDone(){
  if(threadpool->waitWhile(counter)){
    completion.wait();
    completion.post();
    return true;
    }
  return false;
  }


// Wait for stuff
FXTaskGroup::~FXTaskGroup(){
  if(wait()){
    completion.wait();
    }
  }

}
