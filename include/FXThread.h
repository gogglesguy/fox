/********************************************************************************
*                                                                               *
*                 M u l i t h r e a d i n g   S u p p o r t                     *
*                                                                               *
*********************************************************************************
* Copyright (C) 2004,2011 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#ifndef FXTHREAD_H
#define FXTHREAD_H

#ifndef FXRUNNABLE_H
#include "FXRunnable.h"
#endif

namespace FX {


/**
* FXThread provides system-independent support for threads.
* Subclasses must implement the run() function do implement
* the desired functionality of the thread.
* The storage of the FXThread object is to be managed by the
* calling thread, not by the thread itself.
*/
class FXAPI FXThread : public FXRunnable {
private:
  volatile FXThreadID tid;
  volatile FXbool     busy;
private:
  static FXAutoThreadStorageKey selfKey;
private:
  FXThread(const FXThread&);
  FXThread &operator=(const FXThread&);
#if defined(WIN32)
  static unsigned int CALLBACK function(void*);
#else
  static void* function(void*);
#endif
protected:
  static void self(FXThread* t);
public:

  /// Thread priority levels
  enum Priority {
    PriorityError=-1,   /// Failed to get priority
    PriorityDefault,  	/// Default scheduling priority
    PriorityMinimum,    /// Minimum scheduling priority
    PriorityLower,      /// Lower scheduling priority
    PriorityMedium,     /// Medium priority
    PriorityHigher,     /// Higher scheduling priority
    PriorityMaximum     /// Maximum scheduling priority
    };

  /// Thread scheduling policies
  enum Policy {
    PolicyError=-1,     /// Failed to get policy
    PolicyDefault,      /// Default scheduling
    PolicyFifo,         /// First in, first out scheduling
    PolicyRoundRobin 	/// Round-robin scheduling
    };

public:

  /// Initialize thread object.
  FXThread();

  /**
  * Return handle of this thread object.
  * This handle is valid in the context of the thread which
  * called start().
  */
  FXThreadID id() const;

  /**
  * Return true if this thread is running.
  */
  FXbool running() const;

  /**
  * Start thread; the thread is started as attached.
  * The thread is given stacksize for its stack; a value of
  * zero for stacksize will give it the default stack size.
  * This invokes the run() function in the context of the new
  * thread.
  */
  FXbool start(unsigned long stacksize=0);

  /**
  * Suspend calling thread until thread is done.  The FXThreadID is
  * reset back to zero.
  */
  FXbool join();

  /**
  * Suspend calling thread until thread is done, and set code to the
  * return value of run() or the argument passed into exit().  The
  * FXThreadID is reset back to zero.
  * If an exception happened in the thread, return -1.
  */
  FXbool join(FXint& code);

  /**
  * Cancel the thread, stopping it immediately, running or not.
  * If the calling thread is this thread, nothing happens.
  * It is probably better to wait until it is finished, in case the
  * thread currently holds mutexes.
  * The FXThreadID is reset back to zero after the thread has been
  * stopped.
  */
  FXbool cancel();

  /**
  * Detach thread, so that a no join() is necessary to harvest the
  * resources of this thread.  The thread continues to run until
  * normal completion.
  */
  FXbool detach();

  /**
  * Exit the calling thread.
  * No destructors are invoked for objects on thread's stack;
  * to invoke destructors, throw an exception instead.
  */
  static void exit(FXint code=0);

  /**
  * Make the thread yield its time quantum.
  */
  static void yield();

  /**
  * Return time in nanoseconds since Epoch (Jan 1, 1970).
  */
  static FXTime time();

  /**
  * Make the calling thread sleep for a number of nanoseconds.
  */
  static void sleep(FXTime nsec);

  /**
  * Wake at appointed time specified in nanoseconds since Epoch.
  */
  static void wakeat(FXTime nsec);

  /**
  * Return pointer to the FXThread instance associated
  * with the calling thread; it returns NULL for the main
  * thread and all threads not created by FOX.
  */
  static FXThread* self();

  /**
  * Return thread id of calling thread.
  */
  static FXThreadID current();

  /**
  * Return number of available processors (cores) in the system.
  */
  static FXint processors();

  /**
  * Generate new thread local storage key.
  */
  static FXThreadStorageKey createStorageKey();

  /**
  * Dispose of thread local storage key.
  */
  static void deleteStorageKey(FXThreadStorageKey key);

  /**
  * Get thread local storage pointer using key.
  */
  static void* getStorage(FXThreadStorageKey key);

  /**
  * Set thread local storage pointer using key.
  */
  static void setStorage(FXThreadStorageKey key,void* ptr);

  /**
  * Set thread scheduling priority.
  */
  FXbool priority(Priority prio);

  /**
  * Return thread scheduling priority.
  */
  Priority priority() const;

  /**
  * Set thread scheduling policy.
  */
  FXbool policy(Policy plcy);

  /**
  * Get thread scheduling policy.
  */
  Policy policy() const;

  /**
  * Change thread's processor affinity, i.e. the set of
  * processors onto which the thread may be scheduled.
  */
  FXbool affinity(FXulong mask);

  /** 
  * Get thread's processor affinity.
  */
  FXulong affinity() const;

  /**
  * Suspend thread; return true if success.
  */
  FXbool suspend();

  /**
  * Resume thread; return true if success.
  */
  FXbool resume();

  /**
  * Destroy the thread immediately, running or not.
  * It is probably better to wait until it is finished, in case
  * the thread currently holds mutexes.
  */
  virtual ~FXThread();
  };

}

#endif

