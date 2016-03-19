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
* Execute concurrent tasks.
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
public:

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

