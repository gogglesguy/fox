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
#ifndef FXCONCURRENT_H
#define FXCONCURRENT_H

namespace FX {


class FXWorker;
class FXConcurrent;


/**
* Generic task object.
* The task object passed to FXConcurrent::execute is called argc times,
* each with a void* argument from the corresponding argv argument vector.
* The default implementation of Task::exec() does nothing; you must
* subclass it and reimplement exec() appropriately.
*/
class FXTask {
private:
  FXTask(const FXTask&);
  FXTask &operator=(const FXTask&);
public:
  virtual void exec(void*) const = 0;
  virtual ~FXTask(){}
  };


/**
* Notify object.
* An optional notify object may be passed to FXConcurrent::execute().
* When argc new tasks are started, the expected count of completions is
* incremented by argc.
* Upon the completion of each task, the expected count is decremented by
* one; when the count reaches zero, the notify() routine is called.
* The default implemenation of notify() does nothing.  You should subclass
* Completion and reimplement notify() appropriately.
*/
class FXCompletion {
  friend class FXConcurrent;
private:
  FXCondition    condition;
  FXMutex        mutex;
  volatile FXint count;
private:
  FXCompletion(const FXCompletion&);
  FXCompletion &operator=(const FXCompletion&);
private:
  void expect(FXint cnt);
  void notify();
public:

  /// Create completion object
  FXCompletion();

  /// Wait till associated tasks are done
  void wait();

  /// Return true if associated tasks are done
  FXbool done() const;

  /// Destroy completion object
  virtual ~FXCompletion();
  };


/// A work queue
class FXWorkQueue;

/// List of work queues, one for each active thread
typedef FXPtrListOf<FXWorkQueue> FXWorkQueues;


/**
* FXConcurrent provides a convenient facility to perform data-parallel tasks.
*
* FXConcurrent allows multiple threads to work on a single task.  Each thread
* performs the same function, on a different piece of data.  Thus, on multi-
* processor systems a large computation can be performed in parallel, and take
* less time than on a single processor.
* FXConcurrent is started by calling its start() API, passing the number of threads
* to start and the size of the work-queue for each thread (default is 32).
* To minimize needless context-switching, it is usually best to start no more threads
* than there are processors in the system (you can use FXThread::processors() to find 
* out how many processors are in your system).
* Parallel tasks are started by calling execute(), passing the task to be performed
* and the arguments (data) to perform the task on.  A total of argc tasks is started,
* each task is allocated on its own thread, the first one on 0 (indx if given).
* If there are more tasks than threads, some threads may have to perform multiple tasks
* in sequence.  Thus, you can decompose the problem in a way that will scale to more
* parallelism when moving to higher-end machines.
* The argument to FXTask is either a simple integer (0,1,...argc-1) or a void pointer,
* if argv is passed in the execute() API.
* If a FXCompletion object is passed to the execute() API, the calling thread can
* wait to determine when the parallel tasks are done, by calling the wait() API on
* the completion object.  The wait() API will block until the last thread on the
* task is done.
* It is OK to pass the same FXCompletion object to multiple calls to execute(); the
* FXCompletion object will be signalled when ALL tasks are done in that case.
* FXConcurrent's run() API can be called by other threads; run() will return when
* stop() is called.
* Note that calling stop() does not mean that all threads in run() return immediately;
* the tasks already queued up will be finished first.
*/
class FXAPI FXConcurrent : public FXRunnable {
private:
  FXWorkQueues    queues;       // One argument for each thread
  FXMutex         qmutex;       // Queues mutex
  FXCondition     qcondition;   // Queues condition
  volatile FXint  started;      // Started threads
  volatile FXint  stopped;      // Stopped threads
  volatile FXbool running;      // Are we running
  FXuint          slots;        // Slots in queues
private:
  FXWorker* startWorker();
  FXbool appendThreadQueue(FXWorkQueue* wq);
  FXbool removeThreadQueue(FXWorkQueue* wq);
private:
  FXConcurrent(const FXConcurrent&);
  FXConcurrent &operator=(const FXConcurrent&);
protected:
  virtual FXWorker* createWorker();
public:

  /// Create ane initialize
  FXConcurrent();

  /// Start with cnt worker threads; each thread's queue will be sz.
  FXint start(FXint cnt=1,FXint sz=32);

  /// Return true if running
  FXbool active() const;

  /// Return number of threads
  FXint numThreads() const;

  /// Called by a thread
  virtual FXint run();

  /// Perform N-way parallel task, each with argument i, where i=[0..argc>
  FXint execute(const FXTask* task,FXint argc,FXint indx=0);

  /// Perform N-way parallel task, each with argument argv[i], where i=[0..argc>
  FXint execute(const FXTask* task,void** argv,FXint argc,FXint indx=0);

  /// Perform N-way parallel task, with completion, each with argument i, where i=[0..argc>
  FXint execute(FXCompletion* completion,const FXTask* task,FXint argc,FXint indx=0);

  /// Perform N-way parallel task, with completion, each with argument argv[i], where i=[0..argc>
  FXint execute(FXCompletion* completion,const FXTask* task,void** argv,FXint argc,FXint indx=0);

  /// Wait till all threads have terminated
  FXint wait();

  /// Stop executor, causing all threads to terminate eventually
  FXint stop();

  /// Destroy
  virtual ~FXConcurrent();
  };


}

#endif

