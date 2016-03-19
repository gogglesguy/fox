/********************************************************************************
*                                                                               *
*                     P a r a l l e l   C o m p u t a t i o n                   *
*                                                                               *
*********************************************************************************
* Copyright (C) 2012,2013 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#ifndef FXPARALLEL_H
#define FXPARALLEL_H


namespace FX {


/////////////////////////////  UNDER DEVELOPMENT  ///////////////////////////////


class FXTask;


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

/*******************************************************************************/

/**
* FXParallelCallFunctor is a helper for FXParallelInvoke.  It executes a functor on 
* a thread provided by the FXThreadPool.
*/
template <typename Functor>
class FXParallelCallFunctor : public FXRunnable {
  const Functor& functor;
private:
  FXParallelCallFunctor(const FXParallelCallFunctor&);
  FXParallelCallFunctor &operator=(const FXParallelCallFunctor&);
public:
  FXParallelCallFunctor(const Functor& fun):functor(fun){ }
  virtual FXint run(){ functor(); return 0; }
  };


/**
* Perform a parallel call to functors fun1 and fun2 using the given FXThreadPool.
* Return when all functors have completed.
*/
template <typename Functor1,typename Functor2>
void FXParallelInvoke(FXThreadPool* pool,const Functor1& fun1,const Functor2& fun2){
  FXTaskGroup group(pool);
  FXParallelCallFunctor<Functor1> task1(fun1);
  FXParallelCallFunctor<Functor2> task2(fun2);
  group.execute(&task1);
  group.executeAndRun(&task2);
  }


/**
* Perform a parallel call to functors fun1 and fun2 using the FXThreadPool 
* associated with the calling thread.  
* Return when all functors have completed.
*/
template <typename Functor1,typename Functor2>
void FXParallelInvoke(const Functor1& fun1,const Functor2& fun2){
  FXParallelInvoke(FXThreadPool::instance(),fun1,fun2);
  }


/**
* Perform a parallel call to functors fun1, fun2, and fun3, using the given FXThreadPool. 
* Return when all functors have completed.
*/
template <typename Functor1,typename Functor2,typename Functor3>
void FXParallelInvoke(FXThreadPool* pool,const Functor1& fun1,const Functor2& fun2,const Functor3& fun3){
  FXTaskGroup group(pool);
  FXParallelCallFunctor<Functor1> task1(fun1);
  FXParallelCallFunctor<Functor2> task2(fun2);
  FXParallelCallFunctor<Functor3> task3(fun3);
  group.execute(&task1);
  group.execute(&task2);
  group.executeAndRun(&task3);
  }


/**
* Perform a parallel call to functors fun1, fun2, and fun3, using the FXThreadPool 
* associated with the calling thread.   
* Return when all functors have completed.
*/
template <typename Functor1,typename Functor2,typename Functor3>
void FXParallelInvoke(const Functor1& fun1,const Functor2& fun2,const Functor3& fun3){
  FXParallelInvoke(FXThreadPool::instance(),fun1,fun2,fun3);
  }


/**
* Perform a parallel call to functors fun1, fun2, fun3, and fun4, using the given FXThreadPool. 
* Return when all functors have completed.
*/
template <typename Functor1,typename Functor2,typename Functor3,typename Functor4>
void FXParallelInvoke(FXThreadPool* pool,const Functor1& fun1,const Functor2& fun2,const Functor3& fun3,const Functor4& fun4){
  FXTaskGroup group(pool);
  FXParallelCallFunctor<Functor1> task1(fun1);
  FXParallelCallFunctor<Functor2> task2(fun2);
  FXParallelCallFunctor<Functor3> task3(fun3);
  FXParallelCallFunctor<Functor4> task4(fun4);
  group.execute(&task1);
  group.execute(&task2);
  group.execute(&task3);
  group.executeAndRun(&task4);
  }

/**
* Perform a parallel call to functors fun1, fun2, fun3, and fun4, using the FXThreadPool 
* associated with the calling thread.  
* Return when all functors have completed.
*/
template <typename Functor1,typename Functor2,typename Functor3,typename Functor4>
void FXParallelInvoke(const Functor1& fun1,const Functor2& fun2,const Functor3& fun3,const Functor4& fun4){
  FXParallelInvoke(FXThreadPool::instance(),fun1,fun2,fun3,fun4);
  }


/**
* Perform a parallel call to functors fun1, fun2, fun3, fun4, and fun5, using the given FXThreadPool.
* Return when all functors have completed.
*/
template <typename Functor1,typename Functor2,typename Functor3,typename Functor4,typename Functor5>
void FXParallelInvoke(FXThreadPool* pool,const Functor1& fun1,const Functor2& fun2,const Functor3& fun3,const Functor4& fun4,const Functor4& fun5){
  FXTaskGroup group(pool);
  FXParallelCallFunctor<Functor1> task1(fun1);
  FXParallelCallFunctor<Functor2> task2(fun2);
  FXParallelCallFunctor<Functor3> task3(fun3);
  FXParallelCallFunctor<Functor4> task4(fun4);
  FXParallelCallFunctor<Functor5> task5(fun5);
  group.execute(&task1);
  group.execute(&task2);
  group.execute(&task3);
  group.execute(&task4);
  group.executeAndRun(&task5);
  }

/**
* Perform a parallel call to functors fun1, fun2, fun3, fun4, and fun5, using the 
* FXThreadPool associated with the calling thread.  
* Return when all functors have completed.
*/
template <typename Functor1,typename Functor2,typename Functor3,typename Functor4,typename Functor5>
void FXParallelInvoke(const Functor1& fun1,const Functor2& fun2,const Functor3& fun3,const Functor4& fun4,const Functor4& fun5){
  FXParallelInvoke(FXThreadPool::instance(),fun1,fun2,fun3,fun4,fun5);
  }


/**
* Perform a parallel call to functors fun1, fun2, fun3, fun4, fun5, and fun6, using the given FXThreadPool. 
* Return when all functors have completed.
*/
template <typename Functor1,typename Functor2,typename Functor3,typename Functor4,typename Functor5,typename Functor6>
void FXParallelInvoke(FXThreadPool* pool,const Functor1& fun1,const Functor2& fun2,const Functor3& fun3,const Functor4& fun4,const Functor5& fun5,const Functor6& fun6){
  FXTaskGroup group(pool);
  FXParallelCallFunctor<Functor1> task1(fun1);
  FXParallelCallFunctor<Functor2> task2(fun2);
  FXParallelCallFunctor<Functor3> task3(fun3);
  FXParallelCallFunctor<Functor4> task4(fun4);
  FXParallelCallFunctor<Functor5> task5(fun5);
  FXParallelCallFunctor<Functor6> task6(fun6);
  group.execute(&task1);
  group.execute(&task2);
  group.execute(&task3);
  group.execute(&task4);
  group.execute(&task5);
  group.executeAndRun(&task6);
  }


/**
* Perform a parallel call to functors fun1, fun2, fun3, fun4, fun5, and fun6, using the 
* FXThreadPool associated with the calling thread.  
* Return when all functors have completed.
*/
template <typename Functor1,typename Functor2,typename Functor3,typename Functor4,typename Functor5,typename Functor6>
void FXParallelInvoke(Functor1& fun1,const Functor2& fun2,const Functor3& fun3,const Functor4& fun4,const Functor5& fun5,const Functor6& fun6){
  FXParallelInvoke(FXThreadPool::instance(),fun1,fun2,fun3,fun4,fun5,fun6);
  }


/**
* Perform a parallel call to functors fun1, fun2, fun3, fun4, fun5, fun6, and fun7, using the given FXThreadPool. 
* Return when all functors have completed.
*/
template <typename Functor1,typename Functor2,typename Functor3,typename Functor4,typename Functor5,typename Functor6,typename Functor7>
void FXParallelInvoke(FXThreadPool* pool,const Functor1& fun1,const Functor2& fun2,const Functor3& fun3,const Functor4& fun4,const Functor5& fun5,const Functor6& fun6,const Functor7& fun7){
  FXTaskGroup group(pool);
  FXParallelCallFunctor<Functor1> task1(fun1);
  FXParallelCallFunctor<Functor2> task2(fun2);
  FXParallelCallFunctor<Functor3> task3(fun3);
  FXParallelCallFunctor<Functor4> task4(fun4);
  FXParallelCallFunctor<Functor5> task5(fun5);
  FXParallelCallFunctor<Functor6> task6(fun6);
  FXParallelCallFunctor<Functor7> task7(fun7);
  group.execute(&task1);
  group.execute(&task2);
  group.execute(&task3);
  group.execute(&task4);
  group.execute(&task6);
  group.executeAndRun(&task7);
  }


/**
* Perform a parallel call to functors fun1, fun2, fun3, fun4, fun5, fun6, and fun7, using the 
* FXThreadPool associated with the calling thread.  
* Return when all functors have completed.
*/
template <typename Functor1,typename Functor2,typename Functor3,typename Functor4,typename Functor5,typename Functor6,typename Functor7>
void FXParallelInvoke(Functor1& fun1,const Functor2& fun2,const Functor3& fun3,const Functor4& fun4,const Functor5& fun5,const Functor6& fun6,const Functor7& fun7){
  FXParallelInvoke(FXThreadPool::instance(),fun1,fun2,fun3,fun4,fun5,fun6,fun7);
  }


/**
* Perform a parallel call to functors fun1, fun2, fun3, fun4, fun5, fun6, fun7, and fun8, using 
* the given FXThreadPool. 
* Return when all functors have completed.
*/
template <typename Functor1,typename Functor2,typename Functor3,typename Functor4,typename Functor5,typename Functor6,typename Functor7,typename Functor8>
void FXParallelInvoke(FXThreadPool* pool,const Functor1& fun1,const Functor2& fun2,const Functor3& fun3,const Functor4& fun4,const Functor5& fun5,const Functor6& fun6,const Functor7& fun7,const Functor8& fun8){
  FXTaskGroup group(pool);
  FXParallelCallFunctor<Functor1> task1(fun1);
  FXParallelCallFunctor<Functor2> task2(fun2);
  FXParallelCallFunctor<Functor3> task3(fun3);
  FXParallelCallFunctor<Functor4> task4(fun4);
  FXParallelCallFunctor<Functor5> task5(fun5);
  FXParallelCallFunctor<Functor6> task6(fun6);
  FXParallelCallFunctor<Functor7> task7(fun7);
  FXParallelCallFunctor<Functor8> task8(fun8);
  group.execute(&task1);
  group.execute(&task2);
  group.execute(&task3);
  group.execute(&task4);
  group.execute(&task6);
  group.execute(&task7);
  group.executeAndRun(&task8);
  }


/**
* Perform a parallel call to functors fun1, fun2, fun3, fun4, fun5, fun6, fun7, and fun8, using the 
* FXThreadPool associated with the calling thread.  
* Return when all functors have completed.
*/
template <typename Functor1,typename Functor2,typename Functor3,typename Functor4,typename Functor5,typename Functor6,typename Functor7,typename Functor8>
void FXParallelInvoke(Functor1& fun1,const Functor2& fun2,const Functor3& fun3,const Functor4& fun4,const Functor5& fun5,const Functor6& fun6,const Functor7& fun7,const Functor8& fun8){
  FXParallelInvoke(FXThreadPool::instance(),fun1,fun2,fun3,fun4,fun5,fun6,fun7,fun8);
  }

/*******************************************************************************/

/**
* FXParallelLoopFunctor is a helper for FXParallelFor.  It executes a subrange of the
* global indexing range on a thread provided by the FXThreadPool.
*/
template <typename Functor,typename Index>
class FXParallelLoopFunctor : public FXRunnable {
  const Functor& functor;
  const Index    fm;
  const Index    to;
  const Index    by;
private:
  FXParallelLoopFunctor(const FXParallelLoopFunctor&);
  FXParallelLoopFunctor &operator=(const FXParallelLoopFunctor&);
public:
  FXParallelLoopFunctor(const Functor& fun,Index f,Index t,Index b):functor(fun),fm(f),to(t),by(b){ }
  virtual FXint run(){ for(Index ix=fm;ix<to;ix+=by){ functor(ix); } return 0; }
  };


/**
* Perform parallel for-loop executing functor fun(i) for indexes x=fm+by*i, where x<to.
* The index range is split into at most nc pieces.  Each piece is executed in parallel
* using the given FXThreadPool.
*/
template <typename Functor,typename Index>
void FXParallelFor(FXThreadPool* pool,Index fm,Index to,Index by,Index nc,const Functor& fun){
  const FXuval size=sizeof(FXParallelLoopFunctor<Functor,Index>);
  if(fm<to){
    FXTaskGroup group(pool);
    FXuchar space[128*size];
    Index nits=1+(to-fm-1)/by,ni,c;
    if(nc>128) nc=128;
    if(nc>nits) nc=nits;
    for(c=0; c<nc; fm+=ni*by,++c){
      ni=(nits+nc-1-c)/nc;
      new (&space[c*size]) FXParallelLoopFunctor<Functor,Index>(fun,fm,fm+ni*by,by);
      group.execute((FXParallelLoopFunctor<Functor,Index>*)&space[c*size]);
      }
    group.wait();
    }
  }


/**
* Perform parallel for-loop executing functor fun(i) for indexes x=fm+by*i, where x<to.
* The index range is split into at most nc pieces.  Each piece is executed in parallel
* on the FXThreadPool associated with the calling thread.
*/
template <typename Functor,typename Index>
void FXParallelFor(Index fm,Index to,Index by,Index nc,const Functor& fun){
  FXParallelFor(FXThreadPool::instance(),fm,to,by,nc,fun);
  }


/**
* Perform parallel for loop executing functor fun(i) for indexes x=fm+by*i, where x<to.
* The index range is split into at most N pieces, where N is the number of processors on the 
* system.  Each piece is executed in parallel using the given FXThreadPool.
*/
template <typename Functor,typename Index>
void FXParallelFor(FXThreadPool* pool,Index fm,Index to,Index by,const Functor& fun){
  FXParallelFor(pool,fm,to,by,(Index)FXThread::processors(),fun);
  }


/**
* Perform parallel for loop executing functor fun(i) for indexes x=fm+by*i, where x<to.
* The index range is split into at most N pieces, where N is the number of processors on the 
* system.  Each piece is executed in parallel on the FXThreadPool associated with the calling
* thread.
*/
template <typename Functor,typename Index>
void FXParallelFor(Index fm,Index to,Index by,const Functor& fun){
  FXParallelFor(FXThreadPool::instance(),fm,to,by,(Index)FXThread::processors(),fun);
  }

}

#endif
