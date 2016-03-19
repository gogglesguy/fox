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
#ifndef FXTASKGROUP_H
#define FXTASKGROUP_H


namespace FX {


/**
* An FXTaskGroup manages a number of tasks, executing on the associated FXThreadPool.
* In a typical use, an FXTaskGroup is constructed on the stack of the calling function.
* If an explicit FXThreadPool is passed to the constructor of the FXTaskGroup, this is
* the FXThreadPool that will be used to execute the tasks.  Otherwise, an instance of
* FXThreadPool will be located through a thread-local variable from the calling thread.
* This thread-local variable will be set if the calling thread is a worker thread from
* the FXThreadPool, or is the thread that called start() on the FXThreadPool.
* Tasks managed by the FXTaskGroup may be started at any time during the FXTaskGroup's
* lifetime.  However, the FXTaskGroup can not be destroyed until the last task in the
* group has finished execution: the FXTaskGroup's destructor will wait until all tasks
* (if any) have finished executing.
* The calling thread can enter the task-processing loop in the FXThreadPool; it returns
* when either the tasks from this FXTaskGroup have been completed (count reaching zero),
* or when the task-queue is empty (in this case another thread may still be working on
* a task from the FXTaskGroup). Thus, the calling thread should block on the completion
* semaphore to ensure all tasks are completed.
*/
class FXAPI FXTaskGroup {
  class Task;
  friend class Task;
private:
  class Task : public FXRunnable {
  private:
    FXTaskGroup *taskgroup;
    FXRunnable  *runnable;
  private:
    Task();
    Task(const Task&);
    Task &operator=(const Task&);
  public:
    Task(FXTaskGroup* g,FXRunnable *r);
    virtual FXint run();
    virtual ~Task();
    };
private:
  FXThreadPool   *threadpool;   // Thread pool
  FXSemaphore     completion;   // Completion semaphore
  volatile FXuint counter;      // Completion counter
private:
  FXTaskGroup(const FXTaskGroup&);
  FXTaskGroup &operator=(const FXTaskGroup&);
  void incrementAndReset();
  void decrementAndNotify();
public:

  /**
  * Create new task group, using the calling thread's associated
  * thread pool.
  */
  FXTaskGroup();

  /**
  * Create new task group, using the given thread pool.
  */
  FXTaskGroup(FXThreadPool* p);

  /**
  * Return threadpool.
  */
  FXThreadPool* getThreadPool() const { return threadpool; }

  /**
  * Start a task in this task group.
  */
  FXbool execute(FXRunnable* task);

  /**
  * Start task in this task group, and then enter the task-processing
  * loop, returning when either the completion count reaches zero, or the
  * thread pool's task queue becomes empty.
  */
  FXbool executeAndRun(FXRunnable* task);

  /**
  * Enter the task processing loop and return when either the completion count
  * reaches zero, or the thread pool's task queue becomes empty.
  */
  FXbool wait();

  /**
  * Wait until all tasks of this group have finished executing, then return.
  * The completion semaphore is reset after being signaled by the last completed
  * task.
  */
  FXbool waitDone();

  /**
  * Wait for the semaphore to be signaled, then destroy the task group.
  */
  virtual ~FXTaskGroup();
  };

}

#endif
