/********************************************************************************
*                                                                               *
*                             B a r r i e r   C l a s s                         *
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
#ifndef FXBARRIER_H
#define FXBARRIER_H

namespace FX {


/**
* A thread barrier.
*/
class FXAPI FXBarrier {
private:
  FXCondition condition;
  FXMutex     mutex;
  FXuint      generation;
  FXuint      threshold;
  FXuint      counter;
private:
  FXBarrier(const FXBarrier&);
  FXBarrier &operator=(const FXBarrier&);
public:

  /// Initialize the barrier
  FXBarrier(FXuint count=1);

  /// Wait for all threads to hit the barrier
  FXbool wait();

  /// Delete the barrier
 ~FXBarrier();
  };


}

#endif

