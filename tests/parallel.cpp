/********************************************************************************
*                                                                               *
*                P a r a l l e l   P r o g r a m m i n g   T e s t              *
*                                                                               *
*********************************************************************************
* Copyright (C) 2012,2014 by Jeroen van der Zijp.   All Rights Reserved.        *
********************************************************************************/

#include "xincs.h"
#include "fx.h"


/*******************************************************************************/

// Stop QTest threads
volatile FXbool stopit=false;

/*******************************************************************************/

// QTest thread
class QTest : public FXScopedThread {
private:
  FXLFQueue *queue;
public:
  QTest(FXLFQueue* q):queue(q){}
  virtual FXint run();
  };


// QTest tests lock-free queue by hammering it with pushes/pulls
FXint QTest::run(){
  register FXival niter=0;
  register FXlong npush=0;
  register FXlong npull=0;
  register FXlong ncros=0;
  register FXTime tot=0;
  register FXTime tmp;
  FXptr ptr;
  tot=fxgetticks();
  do{
    ptr=(FXptr)niter;
    if(!queue->push(ptr)){
      fxmessage("%p: %ld: full\n",self(),niter);
      }
    else{
      npush++;
      }
    if(!queue->pop(ptr)){
      fxmessage("%p: %ld: empty\n",self(),niter);
      }
    else{
      npull++;
      }
    ncros+=(niter!=(FXival)ptr);
    niter++;
    }
  while(!stopit);
  tot=fxgetticks()-tot;
  fxmessage("%p: npush=%lld\n",self(),npush);
  fxmessage("%p: npull=%lld\n",self(),npull);
  fxmessage("%p: ncros=%lld\n",self(),ncros);
  fxmessage("%p: niter=%ld\n",self(),niter);
  fxmessage("%p: avg=%lld\n",self(),tot/(npush+npull));
  fxmessage("%p: ticks=%lld tick/(push+pull)=%.2lf\n",self(),tot,FXdouble(tot)/FXdouble(npush+npull));
  return 0;
  }

/*******************************************************************************/

// Churn cpu for a random while, then return
void churn(){
  FXRandom random(FXThread::time()+128628761545);
  fxmessage("Churn start th %p core %d/%d\n",FXThread::current(),FXThread::processor(),FXThread::processors());
  while(random.randDouble()<0.999999999){ }
  fxmessage("Churn done  th %p code %d/%d\n",FXThread::current(),FXThread::processor(),FXThread::processors());
  }


// Loop through index range
void looping(FXint i){
  FXRandom random(FXThread::time()+128628761545);
  fxmessage("Looping %05d begin th %p core %d/%d\n",i,FXThread::current(),FXThread::processor(),FXThread::processors());
  while(random.randDouble()<0.9999999){ }
  fxmessage("Looping %05d finis th %p core %d/%d\n",i,FXThread::current(),FXThread::processor(),FXThread::processors());
  }


// Churn cpu for a random while, then return
void churnSplit(){
  FXParallelInvoke(churn,churn);
  }


// Do something
class Job : public FXRunnable {
public:
  Job(){}
  virtual FXint run();
  };


// Run
FXint Job::run(){
  churn();
  delete this;
  return 0;
  }

/*******************************************************************************/

// Print options
void printusage(const char* prog){
  fxmessage("%s options:\n",prog);
  fxmessage("  --threads <number>          Number of threads to start.\n");
  fxmessage("  --minimum <number>          Minimum number of threads to around.\n");
  fxmessage("  --maximum <number>          Maximum number of threads to be started.\n");
  fxmessage("  --jobs <number>             Number of jobs to run.\n");
  fxmessage("  --size <number>             Queue size.\n");
  fxmessage("  --pieces <number>           Split in this many pieces.\n");
  fxmessage("  -W, --wait                  Calling thread waits.\n");
  fxmessage("  -h, --help                  Print help.\n");
  fxmessage("  -Q, --queue                 Test job queue.\n");
  fxmessage("  -P, --pool                  Test thread pool.\n");
  fxmessage("  -L, --loop                  Test parallel loop.\n");
  fxmessage("  -I, --invoke                Test parallel invoke.\n");
  }


// Find power of two
FXuint powoftwo(FXuint n){
  --n;
  n|=n>>1;
  n|=n>>2;
  n|=n>>4;
  n|=n>>8;
  n|=n>>16;
  ++n;
  return n;
  }


// Test program
int main(int argc,char* argv[]){
  FXuint pieces=FXThread::processors();
  FXuint maximum=FXThread::processors();
  FXuint minimum=1;
  FXuint nthreads=1;
  FXuint size=512;
  FXuint njobs=0;
  FXuint test=2;
  FXuint wait=0;

  // Grab a few arguments
  for(FXint arg=1; arg<argc; ++arg){
    if(strcmp(argv[arg],"-h")==0 || strcmp(argv[arg],"--help")==0){
      printusage(argv[0]);
      exit(0);
      }
    else if(strcmp(argv[arg],"--threads")==0){
      if(++arg>=argc){ fxmessage("Missing threads number argument.\n"); exit(1); }
      nthreads=strtoul(argv[arg],NULL,0);
      if(nthreads<0){ fxmessage("Value for threads (%d) too small.\n",nthreads); exit(1); }
      }
    else if(strcmp(argv[arg],"--pieces")==0){
      if(++arg>=argc){ fxmessage("Missing pieces number argument.\n"); exit(1); }
      pieces=strtoul(argv[arg],NULL,0);
      if(minimum<1){ fxmessage("Value for pieces number of pieces (%d) too small.\n",pieces); exit(1); }
      }
    else if(strcmp(argv[arg],"--minimum")==0){
      if(++arg>=argc){ fxmessage("Missing threads number argument.\n"); exit(1); }
      minimum=strtoul(argv[arg],NULL,0);
      if(minimum<0){ fxmessage("Value for minimum number of threads (%d) too small.\n",minimum); exit(1); }
      }
    else if(strcmp(argv[arg],"--maximum")==0){
      if(++arg>=argc){ fxmessage("Missing threads number argument.\n"); exit(1); }
      maximum=strtoul(argv[arg],NULL,0);
      if(maximum<0){ fxmessage("Value for maximum number of threads (%d) too small.\n",minimum); exit(1); }
      }
    else if(strcmp(argv[arg],"--size")==0){
      if(++arg>=argc){ fxmessage("Missing size argument.\n"); exit(1); }
      size=strtoul(argv[arg],NULL,0);
      size=powoftwo(size);
      if(size<4){ fxmessage("Value for size (%d) too small.\n",size); }
      }
    else if(strcmp(argv[arg],"--jobs")==0){
      if(++arg>=argc){ fxmessage("Missing jobs count argument.\n"); exit(1); }
      njobs=strtoul(argv[arg],NULL,0);
      if(njobs<0){ fxmessage("Value for njobs (%d) too small.\n",njobs); exit(1); }
      }
    else if(strcmp(argv[arg],"-W")==0 || strcmp(argv[arg],"--wait")==0){
      wait=1;
      }
    else if(strcmp(argv[arg],"-Q")==0 || strcmp(argv[arg],"--queue")==0){
      test=0;
      }
    else if(strcmp(argv[arg],"-P")==0 || strcmp(argv[arg],"--pool")==0){
      test=1;
      }
    else if(strcmp(argv[arg],"-L")==0 || strcmp(argv[arg],"--loop")==0){
      test=2;
      }
    else if(strcmp(argv[arg],"-I")==0 || strcmp(argv[arg],"--invoke")==0){
      test=3;
      }
    else{
      fxmessage("Bad argument.\n");
      printusage(argv[0]);
      exit(1);
      }
    }

  fxmessage("main thread %p\n",FXThread::current());

  // Test queue
  if(0==test){
    FXLFQueue queue(size);

    QTest thread1(&queue);
    QTest thread2(&queue);
    QTest thread3(&queue);
    QTest thread4(&queue);

    thread1.start();
    thread2.start();
    thread3.start();
    thread4.start();

    fxmessage("press return\n");

    // Wait for user
    getchar();

    stopit=true;
    }

  // Test context
  if(1==test){
    FXThreadPool pool(size);

    // Set number of threads
    pool.setMinimumThreads(minimum);
    pool.setMaximumThreads(maximum);
    pool.setExpiration(1000000);

    fxmessage("starting %d of maximum of %d threads, keeping at most %d\n",nthreads,maximum,minimum);

    // Start context
    pool.start(nthreads);

    fxmessage("running: %d!\n",pool.getRunningThreads());

    if(0<njobs){
      fxmessage("starting execute: %d jobs\n",njobs);

      // Start njobs-1 jobs
      for(FXuint j=wait; j<njobs; ++j){
        pool.execute(new Job);
        }

      // Main thread also runs jobs
      if(wait){
        pool.executeAndRun(new Job);
        }
      fxmessage("finished execute: %d jobs\n",njobs);
      }

    fxmessage("running: %d!\n",pool.getRunningThreads());

    // Stop context
    fxmessage("stopping...\n");
    pool.stop();
    fxmessage("done!\n");
    }

  // Test task groups
  if(2==test){
    FXThreadPool pool(size);

    pool.setMinimumThreads(minimum);
    pool.setMaximumThreads(maximum);
    pool.setExpiration(1000000);

    fxmessage("starting %d of maximum of %d threads, keeping at most %d\n",nthreads,maximum,minimum);

    pool.start(nthreads);

    fxmessage("running: %d!\n",pool.getRunningThreads());

    fxmessage("main thread %p\n",FXThread::current());


    // 8-way parallelism if you got the cores
    fxmessage("%d-way parallel call...\n",nthreads);
    FXParallelInvoke(churn,churn,churn,churn,churn,churn,churn,churn);
    fxmessage("...done\n");

    fxmessage("%d-way parallel for-loop...\n",nthreads);
    FXParallelFor(0U,njobs,1U,pieces,looping);
    fxmessage("...done\n");

    fxmessage("stopping...\n");
    pool.stop();
    fxmessage("...done!\n");
    }

  // Test parallel invoke
  if(3==test){
    FXThreadPool pool(size);

    pool.setMinimumThreads(minimum);
    pool.setMaximumThreads(maximum);
    pool.setExpiration(1000000);

    fxmessage("starting %d of maximum of %d threads, keeping at most %d\n",nthreads,maximum,minimum);

    pool.start(nthreads);

    fxmessage("running: %d!\n",pool.getRunningThreads());

    fxmessage("main thread %p\n",FXThread::current());


    // 8-way parallelism if you got the cores
    fxmessage("%d-way parallel call...\n",nthreads);
    FXParallelInvoke(churn,churn,churn,churn,churn,churn,churn,churn);
    fxmessage("...done\n");
    }
  return 0;
  }


