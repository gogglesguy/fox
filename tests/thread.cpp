/********************************************************************************
*                                                                               *
*                                Thread Pool Test                               *
*                                                                               *
*********************************************************************************
* Copyright (C) 1999,2008 by Jeroen van der Zijp.   All Rights Reserved.        *
*********************************************************************************
* $Id: thread.cpp,v 1.27 2008/03/26 15:04:04 fox Exp $                          *
********************************************************************************/
#include "fx.h"
#include "FXThreadPool.h"


/*
  Notes:

  - Thread pool test.

*/

/*******************************************************************************/


// Job runner
class Runner : public FXRunnable {
protected:
  FXdouble value;
  FXint    number;
  FXint    count;
public:
  Runner(FXint n,FXint c):value(1.0),number(n),count(c){}
  virtual FXint run();
  };


// Job producer
class Producer : public FXRunnable {
protected:
  FXThreadPool *pool;
  FXint         count;
public:
  Producer(FXThreadPool *p,FXint c):pool(p),count(c){}
  virtual FXint run();
  };


// Run jobs
FXint Runner::run(){
  fprintf(stderr,"runner %d start\n",number);
  value=1.0;
  for(FXint i=0; i<count; i++){
    value=cos(value);
    }
  fprintf(stderr,"runner %d done\n",number);
  delete this;
  return 1;
  }


// Generate jobs
FXint Producer::run(){
  fprintf(stderr,"producer start\n");
  for(FXint i=0; i<count; i++){
    fprintf(stderr,"producer job %d\n",i);
    pool->execute(new Runner(i,(FXint)(100000000*drand48())));
    }
  fprintf(stderr,"producer done\n");
  delete this;
  return 1;
  }



// Start
int main(int,char**){

  // Trace
  fxTraceLevel=151;

  // Make thread pool
  FXThreadPool pool;

  fprintf(stderr,"Found %d processors\n",FXThread::processors());

  fprintf(stderr,"starting pool\n");
  pool.start(0,8,0);
  fprintf(stderr,"started pool %d\n",pool.getRunningThreads());
  getchar();

  fprintf(stderr,"starting jobs\n");
  pool.execute(new Producer(&pool,20));
  fprintf(stderr,"running jobs\n");

//  getchar();
//  fprintf(stderr,"waiting\n");
//  pool.wait();
//  fprintf(stderr,"all done\n");

  getchar();
  fprintf(stderr,"stopping\n");
  pool.stop();
  fprintf(stderr,"stopped\n");

  getchar();
  fprintf(stderr,"bye\n");
  return 1;
  }
