/********************************************************************************
*                                                                               *
*                           Generic Element Handling                            *
*                                                                               *
*********************************************************************************
* Copyright (C) 1997,2013 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#ifndef FXELEMENT_H
#define FXELEMENT_H


namespace FX {

/****************************  D e f i n i t i o n  ****************************/

// Generic implementations for generic objects


// In-situ construct element at pointer
template<class EType>
inline EType* construct(EType* ptr){
  ::new ((void*)ptr) EType(); return ptr;
  }


// In-situ construct element at pointer, with argument
template<class EType,class Arg>
inline EType* construct(EType* ptr,Arg arg){
  ::new ((void*)ptr) EType(arg); return ptr;
  }


// In-situ destroy element at pointer
template<class EType>
inline void destruct(EType* ptr){
  ptr->~EType();
  }


/// Construct some elements at a location
template<class EType>
inline void constructElms(EType* ptr,FXuval n){
  while(n--){ construct(ptr); ptr++; }
  }


/// Construct some elements at a location, with argument
template<class EType,class Arg>
inline void constructElms(EType* ptr,Arg arg,FXuval n){
  while(n--){ construct(ptr,arg); ptr++; }
  }


/// Destruct some elements at a location
template<class EType>
inline void destructElms(EType* ptr,FXuval n){
  while(n--){ destruct(ptr); ptr++; }
  }


/// Copy some elements from one place to another
template<class EType, class OType>
inline void copyElms(EType* dst,const OType* src,FXuval n){
  while(n--){ *dst++ = *src++; }
  }


/// Move some elements from overlapping place to another
template<class EType>
inline void moveElms(EType* dst,const EType* src,FXuval n){
  if(src>dst){
    while(n--){ *dst++ = *src++; }
    }
  else if(dst>src){
    dst+=n;
    src+=n;
    while(n--){ *--dst = *--src; }
    }
  }


/// Swap element dst and src
template<class EType>
inline EType& swap(EType& dst,EType& src){
  EType t=dst; dst=src; src=t; 
  return dst;
  }


/// Swap some elements from one place with another
template<class EType>
inline void swapElms(EType* dst,const EType* src,FXuval n){
  while(n--){ swap(*dst++,*src++); }
  }


/// Test elements for equality, using equality operator
template<class EType>
inline FXbool equalElms(const EType* dst,const EType* src,FXuval n){
  while(n--){ if(!(*dst++ == *src++)) return false; }
  return true;
  }


/// Fill array of elements with given element
template<class EType, class OType>
inline void fillElms(EType* dst,const OType& src,FXuval n){
  while(n--){ *dst++ = src; }
  }


/// Zero out array of elements
template<class EType>
inline void clearElms(EType* dst,FXuval n){
  memset(dst,0,sizeof(EType)*n);
  }


/// Allocate array of elements, uninitialized
template<class EType>
inline FXbool allocElms(EType*& ptr,FXuval n){
  return fxmalloc((void**)&ptr,sizeof(EType)*n);
  }


/// Allocate array of elements, initialized with zero
template<class EType>
inline FXbool callocElms(EType*& ptr,FXuval n){
  return fxcalloc((void**)&ptr,sizeof(EType)*n);
  }


/// Allocate array of elements, initialized with bit-wise copy of src array
template<class EType>
inline FXbool dupElms(EType*& ptr,const EType* src,FXuval n){
  return fxmemdup((void**)&ptr,src,sizeof(EType)*n);
  }


/// Resize array of elements, without constructor or destructor
template<class EType>
inline FXbool resizeElms(EType*& ptr,FXuval n){
  return fxresize((void**)&ptr,sizeof(EType)*n);
  }


/// Free array of elements, without destruction
template<class EType>
inline void freeElms(EType*& ptr){
  fxfree((void**)&ptr);
  }


/**********************  I m p l e m e n t a t i o n  ************************/

// Specific implementations for built-in types


// No-op constructors for array of basic type
inline void constructElms(FXuchar*,FXuval){ }
inline void constructElms(FXchar*,FXuval){ }
inline void constructElms(FXushort*,FXuval){ }
inline void constructElms(FXshort*,FXuval){ }
inline void constructElms(FXuint*,FXuval){ }
inline void constructElms(FXint*,FXuval){ }
inline void constructElms(FXulong*,FXuval){ }
inline void constructElms(FXlong*,FXuval){ }
inline void constructElms(FXfloat*,FXuval){ }
inline void constructElms(FXdouble*,FXuval){ }

// No-op constructors for array of pointers to any type
template<class EType> inline void constructElms(EType**,FXuval){ }


// No-op destructors for array of basic type
inline void destructElms(FXuchar*,FXuval){ }
inline void destructElms(FXchar*,FXuval){ }
inline void destructElms(FXushort*,FXuval){ }
inline void destructElms(FXshort*,FXuval){ }
inline void destructElms(FXuint*,FXuval){ }
inline void destructElms(FXint*,FXuval){ }
inline void destructElms(FXulong*,FXuval){ }
inline void destructElms(FXlong*,FXuval){ }
inline void destructElms(FXfloat*,FXuval){ }
inline void destructElms(FXdouble*,FXuval){ }

// No-op destructors for array of pointers to any type
template<class EType> inline void destructElms(EType**,FXuval){ }


// Simple bit-wise copy for array of basic type
inline void copyElms(FXuchar* dst,const FXuchar* src,FXuval n){ memcpy(dst,src,n); }
inline void copyElms(FXchar* dst,const FXchar* src,FXuval n){ memcpy(dst,src,n); }
inline void copyElms(FXushort* dst,const FXushort* src,FXuval n){ memcpy(dst,src,n<<1); }
inline void copyElms(FXshort* dst,const FXshort* src,FXuval n){ memcpy(dst,src,n<<1); }
inline void copyElms(FXuint* dst,const FXuint* src,FXuval n){ memcpy(dst,src,n<<2); }
inline void copyElms(FXint* dst,const FXint* src,FXuval n){ memcpy(dst,src,n<<2); }
inline void copyElms(FXulong* dst,const FXulong* src,FXuval n){ memcpy(dst,src,n<<3); }
inline void copyElms(FXlong* dst,const FXlong* src,FXuval n){ memcpy(dst,src,n<<3); }
inline void copyElms(FXfloat* dst,const FXfloat* src,FXuval n){ memcpy(dst,src,n<<2); }
inline void copyElms(FXdouble* dst,const FXdouble* src,FXuval n){ memcpy(dst,src,n<<3); }

// Simple bit-wise copy for array of pointers to any type
template<class EType> inline void copyElms(EType** dst,const EType** src,FXuval n){ memcpy(dst,src,n*sizeof(void*)); }


// Simple bit-wise move for array of basic type
inline void moveElms(FXuchar* dst,const FXuchar* src,FXuval n){ memmove(dst,src,n); }
inline void moveElms(FXchar* dst,const FXchar* src,FXuval n){ memmove(dst,src,n); }
inline void moveElms(FXushort* dst,const FXushort* src,FXuval n){ memmove(dst,src,n<<1); }
inline void moveElms(FXshort* dst,const FXshort* src,FXuval n){ memmove(dst,src,n<<1); }
inline void moveElms(FXuint* dst,const FXuint* src,FXuval n){ memmove(dst,src,n<<2); }
inline void moveElms(FXint* dst,const FXint* src,FXuval n){ memmove(dst,src,n<<2); }
inline void moveElms(FXulong* dst,const FXulong* src,FXuval n){ memmove(dst,src,n<<3); }
inline void moveElms(FXlong* dst,const FXlong* src,FXuval n){ memmove(dst,src,n<<3); }
inline void moveElms(FXfloat* dst,const FXfloat* src,FXuval n){ memmove(dst,src,n<<2); }
inline void moveElms(FXdouble* dst,const FXdouble* src,FXuval n){ memmove(dst,src,n<<3); }

// Simple bit-wise move for array of pointers to any type
template<class EType> inline void moveElms(EType** dst,const EType** src,FXuval n){ memmove(dst,src,n*sizeof(void*)); }


// Simple bit-wise swap for array of basic type
inline void swapElms(FXuchar* dst,FXuchar* src,FXuval n){ memswap(dst,src,n); }
inline void swapElms(FXchar* dst,FXchar* src,FXuval n){ memswap(dst,src,n); }
inline void swapElms(FXushort* dst,FXushort* src,FXuval n){ memswap(dst,src,n<<1); }
inline void swapElms(FXshort* dst,FXshort* src,FXuval n){ memswap(dst,src,n<<1); }
inline void swapElms(FXuint* dst,FXuint* src,FXuval n){ memswap(dst,src,n<<2); }
inline void swapElms(FXint* dst,FXint* src,FXuval n){ memswap(dst,src,n<<2); }
inline void swapElms(FXulong* dst,FXulong* src,FXuval n){ memswap(dst,src,n<<3); }
inline void swapElms(FXlong* dst,FXlong* src,FXuval n){ memswap(dst,src,n<<3); }
inline void swapElms(FXfloat* dst,FXfloat* src,FXuval n){ memswap(dst,src,n<<2); }
inline void swapElms(FXdouble* dst,FXdouble* src,FXuval n){ memswap(dst,src,n<<3); }

// Simple bit-wise swap for array of pointers to any type
template<class EType> inline void swapElms(EType** dst,EType** src,FXuval n){ memswap(dst,src,n*sizeof(void*)); }


// Simple bit-wise comparison for array of basic type
inline FXbool equalElms(const FXuchar* dst,const FXuchar* src,FXuval n){ return memcmp(dst,src,n)==0; }
inline FXbool equalElms(const FXchar* dst,const FXchar* src,FXuval n){ return memcmp(dst,src,n)==0; }
inline FXbool equalElms(const FXushort* dst,const FXushort* src,FXuval n){ return memcmp(dst,src,n<<1)==0; }
inline FXbool equalElms(const FXshort* dst,const FXshort* src,FXuval n){ return memcmp(dst,src,n<<1)==0; }
inline FXbool equalElms(const FXuint* dst,const FXuint* src,FXuval n){ return memcmp(dst,src,n<<2)==0; }
inline FXbool equalElms(const FXint* dst,const FXint* src,FXuval n){ return memcmp(dst,src,n<<2)==0; }
inline FXbool equalElms(const FXulong* dst,const FXulong* src,FXuval n){ return memcmp(dst,src,n<<3)==0; }
inline FXbool equalElms(const FXlong* dst,const FXlong* src,FXuval n){ return memcmp(dst,src,n<<3)==0; }
inline FXbool equalElms(const FXfloat* dst,const FXfloat* src,FXuval n){ return memcmp(dst,src,n<<2)==0; }
inline FXbool equalElms(const FXdouble* dst,const FXdouble* src,FXuval n){ return memcmp(dst,src,n<<3)==0; }

// Simple bit-wise comparison for array of pointers to any type
template<class EType> inline FXbool equalElms(EType** dst,const EType** src,FXuval n){ return memcmp(dst,src,n*sizeof(void*))==0; }


// Fill byte arrays with constant
inline void fillElms(FXuchar* dst,const FXuchar& src,FXuval n){ memset(dst,src,n); }
inline void fillElms(FXchar* dst,const FXchar& src,FXuval n){ memset(dst,src,n); }

}

#endif
