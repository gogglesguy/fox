/********************************************************************************
*                                                                               *
*                                Thread Pool Test                               *
*                                                                               *
*********************************************************************************
* Copyright (C) 1999,2009 by Jeroen van der Zijp.   All Rights Reserved.        *
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
class Producer : public FXThread {
protected:
  FXThreadPool *pool;
  FXint         count;
  FXint         groups;
public:
  Producer(FXThreadPool *p,FXint c,FXint g):pool(p),count(c),groups(g){}
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
  FXint job=0;
  fprintf(stderr,"producer start\n");
  FXuint seed=1013904223u;
  for(FXint g=0; g<groups; ++g){
    for(FXint c=0; c<count; c++){
      //FXThread::sleep(50000000);
      if(!pool->execute(new Runner(job,fxrandom(seed)/1000))) goto x;
      fprintf(stderr,"producer job %d\n",job);
      job++;
      }
    fprintf(stderr,"producer waiting\n");
    pool->wait();
    fprintf(stderr,"producer resumed\n");
    }
x:fprintf(stderr,"producer done\n");
  return 1;
  }



// Start
int main(int,char**){
  int cpus=FXThread::processors();
  int started;

  // Trace
  fxTraceLevel=151;

  // Make thread pool
  FXThreadPool pool(10);

  // Make producer thread
  Producer producer(&pool,100,10);
  
  fprintf(stderr,"Found %d processors\n",cpus);

  fprintf(stderr,"starting pool\n");
  started=pool.start(1,8,1);
  getchar();
  fprintf(stderr,"started pool %d\n",started);
  getchar();

  fprintf(stderr,"starting jobs\n");
  producer.start();
  fprintf(stderr,"running jobs\n");

  getchar();
  fprintf(stderr,"stopping\n");
  pool.stop();
  fprintf(stderr,"stopped\n");

  getchar();
  producer.join();
  fprintf(stderr,"bye\n");
  return 1;
  }
