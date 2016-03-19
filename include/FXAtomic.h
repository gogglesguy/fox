/********************************************************************************
*                                                                               *
*                         A t o m i c   O p e r a t i o n s                     *
*                                                                               *
*********************************************************************************
* Copyright (C) 2006,2016 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#ifndef FXATOMIC_H
#define FXATOMIC_H


namespace FX {


/**
* Atomics are available .
* Only single-threaded code works properly if this returns false.
*/
extern FXAPI FXbool atomicsAvailable();


///// Atomic integers

/// Atomically set variable at ptr to v, and return its old contents
extern FXAPI FXint atomicSet(volatile FXint* ptr,FXint v);

/// Atomically add v to variable at ptr, and return its old contents
extern FXAPI FXint atomicAdd(volatile FXint* ptr,FXint v);

/// Atomically compare variable at ptr against expect, setting it to v if equal; returns the old value at ptr
extern FXAPI FXint atomicCas(volatile FXint* ptr,FXint expect,FXint v);

/// Atomically compare variable at ptr against expect, setting it to v if equal and return true, or false otherwise
extern FXAPI FXbool atomicBoolCas(volatile FXint* ptr,FXint expect,FXint v);


///// Atomic unsigned integers

/// Atomically set variable at ptr to v, and return its old contents
extern FXAPI FXuint atomicSet(volatile FXuint* ptr,FXuint v);

/// Atomically add v to variable at ptr, and return its old contents
extern FXAPI FXuint atomicAdd(volatile FXuint* ptr,FXuint v);

/// Atomically compare variable at ptr against expect, setting it to v if equal; returns the old value at ptr
extern FXAPI FXuint atomicCas(volatile FXuint* ptr,FXuint expect,FXuint v);

/// Atomically compare variable at ptr against expect, setting it to v if equal and return true, or false otherwise
extern FXAPI FXbool atomicBoolCas(volatile FXuint* ptr,FXuint expect,FXuint v);


///// Atomic void pointers

/// Atomically set pointer variable at ptr to v, and return its old contents
extern FXAPI FXptr atomicSet(volatile FXptr* ptr,FXptr v);

/// Atomically add v to pointer variable at ptr, and return its old contents
extern FXAPI FXptr atomicAdd(volatile FXptr* ptr,FXival v);

/// Atomically compare pointer variable at ptr against expect, setting it to v if equal; returns the old value at ptr
extern FXAPI FXptr atomicCas(volatile FXptr* ptr,FXptr expect,FXptr v);

/// Atomically compare pointer variable at ptr against expect, setting it to v if equal and return true, or false otherwise
extern FXAPI FXbool atomicBoolCas(volatile FXptr* ptr,FXptr expect,FXptr v);

/// Atomically compare pair of variables at ptr against (cmpa,cmpb), setting them to (a,b) if equal and return true, or false otherwise
extern FXAPI FXbool atomicBoolDCas(volatile FXptr* ptr,FXptr cmpa,FXptr cmpb,FXptr a,FXptr b);


///// Atomic pointers to something else

/// Atomically set pointer variable at ptr to v, and return its old contents
template <typename EType>
inline EType* atomicSet(EType* volatile* ptr,EType* v){
  return (EType*)atomicSet((FXptr volatile*)ptr,(FXptr)v);
  }


/// Atomically add v to pointer variable at ptr, and return its old contents
template <typename EType>
inline EType* atomicAdd(EType* volatile* ptr,FXival v){
  return (EType*)atomicAdd((volatile FXptr*)ptr,v*((FXival)sizeof(EType)));
  }


/// Atomically compare pointer variable at ptr against expect, setting it to v if equal; returns the old value at ptr
template <typename EType>
inline EType* atomicCas(EType* volatile* ptr,EType* expect,EType* v){
  return (EType*)atomicCas((volatile FXptr*)ptr,(FXptr)expect,(FXptr)v);
  }


/// Atomically compare pointer variable at ptr against expect, setting it to v if equal and return true, or false otherwise
template <typename EType>
inline FXbool atomicBoolCas(EType* volatile* ptr,EType* expect,EType* v){
  return atomicBoolCas((volatile FXptr*)ptr,(FXptr)expect,(FXptr)v);
  }


/// Atomically compare pair of variables at ptr against (cmpa,cmpb), setting them to (a,b) if equal and return true, or false otherwise
template <typename EType>
inline FXbool atomicBoolDCas(EType* volatile* ptr,EType* cmpa,EType* cmpb,EType* a,EType* b){
  return atomicBoolDCas((volatile FXptr*)ptr,(FXptr)cmpa,(FXptr)cmpb,(FXptr)a,(FXptr)b);
  }


}

#endif
