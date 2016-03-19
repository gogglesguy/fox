/********************************************************************************
*                                                                               *
*                             T h r e a d   P o o l                             *
*                                                                               *
*********************************************************************************
* Copyright (C) 2006,2014 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#ifndef FXTHREADPOOL_H
#define FXTHREADPOOL_H

#ifndef FXRUNNABLE_H
#include "FXRunnable.h"
#endif

namespace FX {


class FXWorker;

/// Task queue
typedef FXLFQueueOf<FXRunnable> FXTaskQueue;


/**
* A Thread Pool manages execution of tasks on a number of worker-threads.
*
* A task executed by the thread pool is queued up until a worker-thread becomes available
* to run it.
* To accomodate fluctuations in workloads, the number of worker-threads can be allowed to
* vary between a minimum and a maximum number.
* Idle worker-threads which receive no assignments within a specified amount of time will
* automatically terminate, thereby reduce system-resources used by the program.
* By default, the minimum number of worker-threads is 1, and the maximum number of worker-
* threads is equal to the number of processors in the system.
* During very peak workloads, the task queue may start to fill up, causing the calling
* thread to block until there is room in the queue for more tasks.
* However, if a non-default value is passed for the blocking-parameter to execute(), the
* calling thread will be blocked for only a finite amount of time (non-zero blocking value)
* or return immediately (zero blocking value).
* Failure to queue up a new task will result in execute() returning a false.
* The jobs which are passed to the thread pool are derived from FXRunnable.  In order
* to perform some useful function, subclasses of FXRunnable should overload the run()
* function.
* FXEceptions thrown by a task are caught in the thread pool and thus won't cause a
* premature exit of either the worker thread of the thread pool itself; other exceptions
* however will possibly cause program termination.
* When the thread pool is stopped, it will wait until all tasks are finished, and then
* cause all worker-threads to terminate.
* The thread pool becomes associated (through a thread-local variable) with the calling
* thread when start() is called; this association lasts until stop() is called.
* In addition, each worker will similarly be associated with the thread pool.
* The thread pool associated this way can be obtained using the static member-function
* instance().
*/
class FXAPI FXThreadPool : public FXRunnable {
private:
  FXTaskQueue     queue;        // Task queue
  FXSemaphore     freeslots;    // Free slots in queue
  FXSemaphore     usedslots;    // Used slots in queue
  FXSemaphore     sayonara;     // Last worker left
  FXuval          stacksize;    // Stack size
  FXTime          expiration;   // Quit if no job within this time
  volatile FXuint processing;   // Number of jobs being processed
  volatile FXuint maximum;      // Maximum number of workers
  volatile FXuint minimum;      // Minimum number of workers
  volatile FXuint workers;      // Number of worker threads
  volatile FXuint running;      // Context is running
private:
  static FXAutoThreadStorageKey reference;
private:
  FXThreadPool(const FXThreadPool&);
  FXThreadPool &operator=(const FXThreadPool&);
private:
  FXbool startWorker();
  void processTasksWhile(volatile FXuint& count,FXTime nsec=forever);
  virtual FXint run();
public:

  /**
  * Construct an empty thread pool, with given job queue size.
  */
  FXThreadPool(FXuint sz=256);

  /// Return true if running
  FXbool active() const;

  /// Change task queue size, return true if success
  FXbool setSize(FXuint sz);

  /// Return task queue size
  FXuint getSize() const;

  /// Return number of threads
  FXuint getRunningThreads() const;

  /// Change minimum number of worker threads; default is 1
  FXbool setMinimumThreads(FXuint n);

  /// Return minimum number of worker threads
  FXuint getMinimumThreads() const { return minimum; }

  /// Change maximum number of worker threads; default is #processors
  FXbool setMaximumThreads(FXuint n);

  /// Return maximum number of worker threads
  FXuint getMaximumThreads() const { return maximum; }

  /// Change expiration time
  FXbool setExpiration(FXTime ns=forever);

  /// Get expiration time
  FXTime getExpiration() const { return expiration; }

  /// Change stack size (0 for default stack size)
  FXbool setStackSize(FXuval sz);

  /// Get stack size
  FXuval getStackSize() const { return stacksize; }

  /// Return calling thread's thread pool
  static FXThreadPool* instance();

  /// Change calling thread's thread pool
  static void instance(FXThreadPool *pool);

  /**
  * Start the thread pool with an initial number of threads equal to count.
  * Returns the number of threads actually started.
  * An association will be established between the calling thread and the thread pool.
  * This association lasts until stop() is called.  If another threadpool was already started
  * before by the calling thread, no new association will be established.
  */
  FXuint start(FXuint count=0);

  /**
  * Execute a task on the thread pool by entering it into the queue.
  * If a spot becomes available in the task queue within the timeout interval,
  * add the task to the queue and return true.
  * Return false if the task could not be added within the given time interval.
  * Possibly starts additional worker threads if the maximum number of worker
  * threads has not yet been exceeded.
  */
  FXbool execute(FXRunnable* task,FXTime blocking=forever);

  /**
  * Execute a task on the thread pool by entering it into the queue.
  * If the task was successfully added, the calling thread will temporarily enter
  * the task-processing loop, and help out the worker-threads until the queue
  * is empty.
  * Return false if the task could not be added within the given time interval.
  * Possibly starts additional worker threads if the maximum number of worker
  * threads has not yet been exceeded.
  */
  FXbool executeAndRun(FXRunnable* task,FXTime blocking=forever);

  /**
  * Execute task on the thread pool by entering int into the queue.
  * If the task was successfully added, the calling thread will temporarily enter
  * the task-processing loop, and help out the worker-threads until either
  * the queue is empty, or the atomic counter becomes zero.
  * Return false if the task could not be added within the given time interval.
  * Possibly starts additional worker threads if the maximum number of worker
  * threads has not yet been exceeded.
  */
  FXbool executeAndRunWhile(FXRunnable* task,volatile FXuint& count,FXTime blocking=forever);

  /**
  * Enter the task-processing loop, helping out the worker-threads until the
  * task queue is empty (NB this is changed from the original implementation!).
  * Return false if the thread pool wasn't running.
  */
  FXbool wait();

  /**
  * Temporarily enter the task-processing loop, helping out the worker-threads until
  * either the task queue is empty, or the counter becomes zero.
  * Return false if the thread pool wasn't running.
  */
  FXbool waitWhile(volatile FXuint& count);

  /**
  * Wait until all tasks in the queue at the time of the call have
  * been removed and finished executing.
  */
  FXbool waitDone();

  /**
  * Stop context.
  * Enter the task-processing loop and help the worker-threads until the task queue is
  * empty, and all tasks have finished executing.
  * The association between the calling thread, established when start() was called,
  * will hereby be dissolved, if the calling thread was associated with this thread pool.
  * Return false if the thread pool wasn't running.
  */
  FXbool stop();

  /**
  * Stop the thread pool and then delete it.
  */
  virtual ~FXThreadPool();
  };


}

#endif
