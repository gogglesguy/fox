/********************************************************************************
*                                                                               *
*                           S p i n l o c k   C l a s s                         *
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
#ifndef FXSPINLOCK_H
#define FXSPINLOCK_H


namespace FX {


/**
* FXSpinLock can be used to provide safe access to very small
* critical sections.  It is cheaper than FXMutex, but unlike
* FXMutex, threads which are unable to obtain the lock will
* not block, but spin in a tight loop until the lock can be
* obtained.
*/
class FXAPI FXSpinLock {
private:
  volatile FXuval data[4];
private:
  FXSpinLock(const FXSpinLock&);
  FXSpinLock &operator=(const FXSpinLock&);
public:

  /// Initialize the spinlock
  FXSpinLock();

  /// Lock the mutex
  void lock();

  /// Return true if succeeded locking the spinlock
  FXbool trylock();

  /// Return true if spinlock is already locked
  FXbool locked();

  /// Unlock spinlock
  void unlock();

  /// Delete the spinlock
  ~FXSpinLock();
  };


/**
* Establish a correspondence between a C++ scope and a spin lock,
* so that entering and leaving the scope in which the scoped lock
* is defined will automatically lock and unlock the associated
* spin lock.
* This will typically result in much less coding, and in addition
* will make the code safe from exceptions.
*/
class FXAPI FXScopedSpinLock {
private:
  FXSpinLock& spn;
private:
  FXScopedSpinLock();
  FXScopedSpinLock(const FXScopedSpinLock&);
  FXScopedSpinLock& operator=(const FXScopedSpinLock&);
public:

  /// Construct & lock associated mutex
  FXScopedSpinLock(FXSpinLock& s):spn(s){ lock(); }

  /// Return reference to associated mutex
  FXSpinLock& spinlock(){ return spn; }

  /// Lock mutex
  void lock(){ spn.lock(); }

  /// Return true if succeeded locking the mutex
  FXbool trylock(){ return spn.trylock(); }

  /// Return true if mutex is already locked
  FXbool locked(){ return spn.locked(); }

  /// Unlock mutex
  void unlock(){ spn.unlock(); }

  /// Destroy and unlock associated mutex
  ~FXScopedSpinLock(){ unlock(); }
  };

}

#endif

