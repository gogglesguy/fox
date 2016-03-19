/********************************************************************************
*                                                                               *
*                             T h r e a d   P o o l                             *
*                                                                               *
*********************************************************************************
* Copyright (C) 2006,2011 by Jeroen van der Zijp.   All Rights Reserved.        *
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


/// Job queue
typedef FXPtrQueueOf<FXRunnable> FXJobQueue;


/**
* A Thread Pool manages execution of jobs on a number of worker threads.
* For compute-bound tasks, the amount of parallelism in a program is limited by the
* number of physical processors available; however for I/O-bound tasks, it makes sense
* to create more threads than the number of physical processors, in order to more fully
* utilize available processors.
* The thread pool executes incoming jobs in parallel, assigning each job to the first
* available thread out of a fixed pool of worker threads.
* Fluctuations in work-load can be accomodated by creating a few extra worker threads
* during peak loads, while terminating superfluous worker threads during periods of
* low activity, thus minimizing resources.
* In order to prevent falling behind on incoming jobs, the calling thread can be made
* to block scheduling the next job until a worker thread becomes available to handle
* it.
* When the thread pool is deleted, all worker threads are allowed to complete their
* tasks prior to destroying the thread pool.
* The jobs which are passed to the thread pool are derived from FXRunnable.  In order
* to perform some useful function, a subclass of FXRunnable should overload the run()
* function.  Any exceptions thrown by this function are caught in FXWorker, thus the
* worker thread will remain running despite exceptions thrown by the job object.
*/
class FXAPI FXThreadPool : public FXRunnable {
private:
  FXJobQueue      queue;        // Job queue
  FXMutex         mutex;        // Pool mutex
  FXCondition     pcond;        // Producer condition
  FXCondition     ccond;        // Consumer condition
  FXCondition     wcond;        // Wait condition
  FXTime          expire;       // Quit if no job within this time
  volatile FXuint maximum;      // Maximum number of workers
  volatile FXuint minimum;      // Minimum number of workers
  volatile FXuint started;      // Number of workers started
  volatile FXuint stopped;      // Number of workers stopped
  volatile FXuint pwaiting;     // Producers waiting
  volatile FXuint cwaiting;     // Consumers waiting
  volatile FXuint watching;     // Threads watching completion
  volatile FXbool runs;         // Thread pool is running
private:
  FXWorker* startWorker();
private:
  FXThreadPool(const FXThreadPool&);
  FXThreadPool &operator=(const FXThreadPool&);
public:

  /**
  * Construct an empty thread pool, with given job queue size.
  */
  FXThreadPool(FXuint sz=256);

  /// Change job queue size, return true if success
  FXbool setSize(FXuint sz);

  /// Return job queue size
  FXuint getSize() const;

  /// Is pool running
  FXbool active() const;

  /// Return number of waiting threads
  FXuint getWaitingThreads() const;

  /// Return number of worker threads
  FXuint getRunningThreads() const;

  /// Change minimum number of worker threads
  void setMinimumThreads(FXuint n);

  /// Return minimum number of worker threads
  FXuint getMinimumThreads() const;

  /// Change maximum number of worker threads
  void setMaximumThreads(FXuint n);

  /// Return maximum number of worker threads
  FXuint getMaximumThreads() const;

  /// Change expiration time
  void setExpiration(FXTime ns=forever);

  /// Get expiration time
  FXTime getExpiration() const;

  /**
  * Start the thread pool; the number of workers will
  * vary between min and max, depending on work-load.
  * A total of cnt workers will be started immediately;
  * additional workers may be started on as-needed basis.
  * When the number of available workers exceeds min, any
  * additional workers which finish their assigned job will
  * terminate gracefully so as to minimize the number of
  * inactive threads.
  * Returns the number of workers successfully started.
  */
  FXuint start(FXuint min=1,FXuint max=32,FXuint cnt=0);

  /**
  * Wait until all jobs currently in the queue are finished.
  * Returns true if all worker threads have become idle, i.e. the
  * queue is empty.
  * If the thread pool has been stopped, it returns false.
  */
  FXbool wait();

  /**
  * Stop pool.
  * Wait until all workers have terminated gracefully, i.e. until
  * the last job has been completed and all threads have stopped running.
  */
  void stop();

  /**
  * Execute a job on the thread pool by entering it into the job queue.
  * If no worker threads are available to process the job and the maximum number of
  * threads has not yet been exceeded, a new worker thread will be started.
  * Otherwise, wait for a timeout interval given by blocking for a spot in the job
  * queue to become available.
  * If a spot becomes available in the job queue within the timeout interval,
  * add the job to the queue and return true.
  * Return false if the thread pool wasn't running yet, if a new worker thread could not
  * be created when desired, or the job could not be added within the given time interval.
  */
  FXbool execute(FXRunnable *job,FXTime blocking=forever);

  /**
  * Called by a (worker) thread to process jobs from the job queue.
  * Returns when the thread pool is stopped, the number of threads
  * exceeds the minimum number of threads, or no new jobs arrive within
  * a set amount of time.
  * Returns true if the pool is still running.
  */
  virtual FXint run();

  /**
  * Signal the running workers to terminate, and wait until
  * all jobs have finished.
  */
  virtual ~FXThreadPool();
  };

}

#endif
