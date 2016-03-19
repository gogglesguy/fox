/********************************************************************************
*                                                                               *
*                         A t o m i c   O p e r a t i o n s                     *
*                                                                               *
*********************************************************************************
* Copyright (C) 2006,2010 by Jeroen van der Zijp.   All Rights Reserved.        *
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


///// Atomic integers

/// Atomically set variable at ptr to v, and return its old contents
extern FXAPI FXint atomicSet(volatile FXint* ptr,FXint v);

/// Atomically add v to variable at ptr, and return its old contents
extern FXAPI FXint atomicAdd(volatile FXint* ptr,FXint v);

/// Atomically compare variable at ptr against expect, setting it to v if equal; returns the old value at ptr
extern FXAPI FXint atomicCas(volatile FXint* ptr,FXint expect,FXint v);

/// Atomically compare variable at ptr against expect, setting it to v if equal and return true, or false otherwise
extern FXAPI FXbool atomicBoolCas(volatile FXint* ptr,FXint expect,FXint v);


///// Atomic void pointers

/// Atomically set pointer variable at ptr to v, and return its old contents
extern FXAPI void* atomicSet(void* volatile* ptr,void* v);

/// Atomically add v to pointer variable at ptr, and return its old contents
extern FXAPI void* atomicAdd(void* volatile* ptr,FXival v);

/// Atomically compare pointer variable at ptr against expect, setting it to v if equal; returns the old value at ptr
extern FXAPI void* atomicCas(void* volatile* ptr,void* expect,void* v);

/// Atomically compare pointer variable at ptr against expect, setting it to v if equal and return true, or false otherwise
extern FXAPI FXbool atomicBoolCas(void* volatile* ptr,void* expect,void* v);

/// Atomically compare pair of variables at ptr against (cmpa,cmpb), setting them to (a,b) if equal and return true, or false otherwise
extern FXAPI FXbool atomicBoolDCas(void* volatile* ptr,void* cmpa,void* cmpb,void* a,void* b);


///// Atomic pointers to something else

/// Atomically set pointer variable at ptr to v, and return its old contents
template <class EType>
inline EType* atomicSet(EType* volatile* ptr,EType* v){
  return (EType*)atomicSet((void*volatile*)ptr,(void*)v);
  }


/// Atomically add v to pointer variable at ptr, and return its old contents
template <class EType>
inline EType* atomicAdd(EType* volatile* ptr,FXival v){
  return (EType*)atomicAdd((void* volatile*)ptr,v*((FXival)sizeof(EType)));
  }


/// Atomically compare pointer variable at ptr against expect, setting it to v if equal; returns the old value at ptr
template <class EType>
inline EType* atomicCas(EType* volatile* ptr,EType* expect,EType* v){
  return (EType*)atomicCas((void* volatile*)ptr,(void*)expect,(void*)v);
  }


/// Atomically compare pointer variable at ptr against expect, setting it to v if equal and return true, or false otherwise
template <class EType>
inline FXbool atomicBoolCas(EType* volatile* ptr,EType* expect,EType* v){
  return atomicBoolCas((void* volatile*)ptr,(void*)expect,(void*)v);
  }


/// Atomically compare pair of variables at ptr against (cmpa,cmpb), setting them to (a,b) if equal and return true, or false otherwise
template <class EType>
inline FXbool atomicBoolDCas(EType* volatile* ptr,EType* cmpa,EType* cmpb,EType* a,EType* b){
  return atomicBoolDCas((void* volatile*)ptr,(void*)cmpa,(void*)cmpb,(void*)a,(void*)b);
  }


}

#endif
