/********************************************************************************
*                                                                               *
*                             B a r r i e r   C l a s s                         *
*                                                                               *
*********************************************************************************
* Copyright (C) 2004,2012 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "FXMutex.h"
#include "FXCondition.h"
#include "FXBarrier.h"

/*
  Notes:

  - Barrier synchronization primitive.
  - For now, Barrier is implemented in terms of mutex and condition
    variables; a future version may use native facilities.
*/

using namespace FX;


namespace FX {


/*******************************************************************************/


// Initialize the barrier
FXBarrier::FXBarrier(FXuint count):generation(0),threshold(count),counter(count){
  if(count<1){ fxerror("FXBarrier::FXBarrier: bad count argument.\n"); }
  }


// Wait for all threads to hit the barrier
FXbool FXBarrier::wait(){
  FXMutexLock locker(mutex);
  FXuint gen=generation;
  if(--counter==0){
    counter=threshold;
    generation++;
    condition.broadcast();
    return true;
    }
  while(gen==generation){
    condition.wait(mutex);
    }
  return false;
  }


// Delete the barrier
FXBarrier::~FXBarrier(){
  }

}
