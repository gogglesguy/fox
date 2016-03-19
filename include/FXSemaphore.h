/********************************************************************************
*                                                                               *
*                          S e m a p h o r e   C l a s s                        *
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
#ifndef FXSEMAPHORE_H
#define FXSEMAPHORE_H


namespace FX {


/**
* A semaphore allows for protection of a resource that can
* be accessed by a fixed number of simultaneous threads.
*/
class FXAPI FXSemaphore {
private:
  volatile FXuval data[16];
private:
  FXSemaphore(const FXSemaphore&);
  FXSemaphore& operator=(const FXSemaphore&);
public:

  /// Initialize semaphore with given count
  FXSemaphore(FXint initial=1);

  /// Get semaphore value
  FXint value() const;

  /// Decrement semaphore, waiting if count is zero
  void wait();

  /// Decrement semaphore; returning false if count is zero
  FXbool trywait();

  /// Increment semaphore
  void post();

  /// Delete semaphore
  ~FXSemaphore();
  };

}

#endif

