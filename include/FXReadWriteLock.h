/********************************************************************************
*                                                                               *
*                  R e a d - W r i t e   L o c k   C l a s s                    *
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
#ifndef FXREADWRITELOCK_H
#define FXREADWRITELOCK_H


namespace FX {


/**
* A read / write lock allows multiple readers but only a single
* writer.
*/
class FXAPI FXReadWriteLock {
private:
  volatile FXuval data[32];
private:
  FXReadWriteLock(const FXReadWriteLock&);
  FXReadWriteLock &operator=(const FXReadWriteLock&);
public:

  /// Initialize the read/write lock
  FXReadWriteLock();

  /// Acquire read lock for read/write lock
  void readLock();

  /// Try to acquire read lock for read/write lock
  FXbool tryReadLock();

  /// Unlock read lock
  void readUnlock();

  /// Acquire write lock for read/write mutex
  void writeLock();

  /// Try to acquire write lock for read/write lock
  FXbool tryWriteLock();

  /// Unlock write mutex
  void writeUnlock();

  /// Delete the read/write lock
 ~FXReadWriteLock();
  };

}

#endif

