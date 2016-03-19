/********************************************************************************
*                                                                               *
*                            W o r k e r   T h r e a d                          *
*                                                                               *
*********************************************************************************
* Copyright (C) 2006,2012 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "FXElement.h"
#include "FXPtrQueue.h"
#include "FXMutex.h"
#include "FXCondition.h"
#include "FXAutoThreadStorageKey.h"
#include "FXRunnable.h"
#include "FXThread.h"
#include "FXWorker.h"


/*
  Notes:
  - A Worker runs a runnable, and then terminates when the runnable
    is done.
  - This is used in FXThreadPool and FXConcurrent.
*/

using namespace FX;


namespace FX {

/*******************************************************************************/


// Create worker for runnable
FXWorker::FXWorker(FXRunnable *r):runnable(r){
  FXTRACE((100,"FXWorker::FXWorker %p\n",this));
  }


// Change runnable if not started yet
FXbool FXWorker::setRunnable(FXRunnable* r){
  if(!running()){
    runnable=r;
    return true;
    }
  return false;
  }
  
  
// Worker runs jobs, then dies
FXint FXWorker::run(){
  if(runnable){
    try{ runnable->run(); } catch(...){ }
    }
  delete this;
  return 0;
  }


// Destroy worker
FXWorker::~FXWorker(){
  FXTRACE((100,"FXWorker::~FXWorker %p\n",this));
  }

}
