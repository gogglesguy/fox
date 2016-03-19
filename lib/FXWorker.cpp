/********************************************************************************
*                                                                               *
*                            W o r k e r   T h r e a d                          *
*                                                                               *
*********************************************************************************
* Copyright (C) 2006,2013 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "FXException.h"
#include "FXAutoThreadStorageKey.h"
#include "FXRunnable.h"
#include "FXThread.h"
#include "FXWorker.h"


/*
  Notes:
  - A Worker is a thread that performs a Runnable.
  - When the worker thread completes the execution of the runnable, the worker thread
    and its memory are automatically reclaimed.
  - The runnable itself is not deleted by the worker; it will thus outlive the worker
    that runs it.
  - Resource exceptions thrown by the runnable cause early termination of the runnable,
    and are caught by the worker; other exceptions cause program termination, normally.
*/

using namespace FX;


namespace FX {

/*******************************************************************************/

// Create worker for runnable
FXWorker::FXWorker(FXRunnable* rn):runnable(rn){
  FXTRACE((100,"FXWorker::FXWorker %p\n",this));
  }


// Worker runs a job, then cleans itself up
// Exceptions are passed through after cleanup of worker
FXint FXWorker::run(){
  if(runnable){
    try{
      runnable->run();
      }
    catch(...){
      delete this;
      throw;
      }
    }
  delete this;
  return 0;
  }


// Create and start a worker executing a given runnable
FXWorker* FXWorker::execute(FXRunnable* rn,FXuval stacksize){
  if(rn){
    FXWorker* worker=new FXWorker(rn);
    if(worker){
      if(worker->start(stacksize)){ return worker; }
      delete worker;
      }
    }
  return NULL;
  }


// Destroy
FXWorker::~FXWorker(){
  FXTRACE((100,"FXWorker::~FXWorker %p\n",this));
  runnable=(FXRunnable*)-1L;
  }

}
