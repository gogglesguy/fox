/********************************************************************************
*                                                                               *
*               T h r e a d - L o c a l   S t o r a g e   C l a s s             *
*                                                                               *
*********************************************************************************
* Copyright (C) 2004,2013 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "FXAutoThreadStorageKey.h"

/*
  Notes:

  - Automatically generate thread-local storage keys in a global constructor;
    clean them up automatically as well using a global destructor.
*/

using namespace FX;


namespace FX {


/*******************************************************************************/


// Automatically acquire a thread-local storage key
FXAutoThreadStorageKey::FXAutoThreadStorageKey(){
#if defined(WIN32)
  FXASSERT(sizeof(FXThreadStorageKey)>=sizeof(DWORD));
  value=(FXThreadStorageKey)TlsAlloc();
#else
  FXASSERT(sizeof(FXThreadStorageKey)>=sizeof(pthread_key_t));
  pthread_key_t key;
  pthread_key_create(&key,NULL);
  value=(FXThreadStorageKey)key;
#endif
  }


// Set thread local storage associated with this key
void FXAutoThreadStorageKey::set(void* ptr) const {
#if defined(WIN32)
  TlsSetValue((DWORD)value,ptr);
#else
  pthread_setspecific((pthread_key_t)value,ptr);
#endif
  }


// Get thread local storage associated with this key
void* FXAutoThreadStorageKey::get() const {
#if defined(WIN32)
  return TlsGetValue((DWORD)value);
#else
  return pthread_getspecific((pthread_key_t)value);
#endif
  }


// Automatically release a thread-local storage key
FXAutoThreadStorageKey::~FXAutoThreadStorageKey(){
#if defined(WIN32)
  TlsFree((DWORD)value);
#else
  pthread_key_delete((pthread_key_t)value);
#endif
  }


}
