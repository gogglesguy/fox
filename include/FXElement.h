/********************************************************************************
*                                                                               *
*                           Generic Element Handling                            *
*                                                                               *
*********************************************************************************
* Copyright (C) 1997,2007 by Jeroen van der Zijp.   All Rights Reserved.        *
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
*********************************************************************************
* $Id: FXElement.h,v 1.25 2007/07/09 16:02:43 fox Exp $                         *
********************************************************************************/
#ifndef FXELEMENT_H
#define FXELEMENT_H

namespace FX {

/****************************  D e f i n i t i o n  ****************************/

// Generic implementations for generic objects


/// Construct some elements at a location
template<class TYPE>
inline void constructElms(TYPE* ptr,unsigned long n){
  while(n--){ ::new ((void*)ptr) TYPE; ptr++; }
  }


/// Destruct some elements at a location
template<class TYPE>
inline void destructElms(TYPE* ptr,unsigned long n){
  while(n--){ ptr->~TYPE(); ptr++; }
  }


/// Copy some elements from one place to another
template<class TYPE>
inline void copyElms(TYPE* dst,const TYPE* src,unsigned long n){
  while(n--){ *dst++ = *src++; }
  }


/// Move some elements from overlapping place to another
template<class TYPE>
inline void moveElms(TYPE* dst,const TYPE* src,unsigned long n){
  if(src>dst){
    while(n--){ *dst++ = *src++; }
    }
  else if(dst>src){
    dst+=n;
    src+=n;
    while(n--){ *--dst = *--src; }
    }
  }


/// Fill array of elements with given element
template<class TYPE>
inline void fillElms(TYPE* dst,const TYPE& src,unsigned long n){
  while(n--){ *dst++ = src; }
  }


/// Zero out array of elements
template<class TYPE>
inline void clearElms(TYPE* dst,unsigned long n){
  memset(dst,0,sizeof(TYPE)*n);
  }


/// Allocate array of elements, uninitialized
template<class TYPE>
inline FXbool allocElms(TYPE*& ptr,unsigned long n){
  return fxmalloc((void**)&ptr,sizeof(TYPE)*n);
  }


/// Allocate array of elements, initialized with zero
template<class TYPE>
inline FXbool callocElms(TYPE*& ptr,unsigned long n){
  return fxcalloc((void**)&ptr,sizeof(TYPE)*n);
  }


/// Allocate array of elements, initialized with bit-wise copy of src array
template<class TYPE>
inline FXbool dupElms(TYPE*& ptr,const TYPE* src,unsigned long n){
  return fxmemdup((void**)&ptr,src,sizeof(TYPE)*n);
  }


/// Resize array of elements, without constructor or destructor
template<class TYPE>
inline FXbool resizeElms(TYPE*& ptr,unsigned long n){
  return fxresize((void**)&ptr,sizeof(TYPE)*n);
  }


/// Free array of elements, without destruction
template<class TYPE>
inline void freeElms(TYPE*& ptr){
  fxfree((void**)&ptr);
  }


/**********************  I m p l e m e n t a t i o n  ************************/

// Specific implementations for built-in types


// No-op constructors for array of basic type
inline void constructElms(FXuchar*,unsigned long){ }
inline void constructElms(FXchar*,unsigned long){ }
inline void constructElms(FXushort*,unsigned long){ }
inline void constructElms(FXshort*,unsigned long){ }
inline void constructElms(FXuint*,unsigned long){ }
inline void constructElms(FXint*,unsigned long){ }
inline void constructElms(FXfloat*,unsigned long){ }
inline void constructElms(FXdouble*,unsigned long){ }

// No-op destructors for array of basic type
inline void destructElms(FXuchar*,unsigned long){ }
inline void destructElms(FXchar*,unsigned long){ }
inline void destructElms(FXushort*,unsigned long){ }
inline void destructElms(FXshort*,unsigned long){ }
inline void destructElms(FXuint*,unsigned long){ }
inline void destructElms(FXint*,unsigned long){ }
inline void destructElms(FXfloat*,unsigned long){ }
inline void destructElms(FXdouble*,unsigned long){ }

// Simple bit-wise copy for array of basic type
inline void copyElms(FXuchar* dst,const FXuchar* src,unsigned long n){ memcpy(dst,src,n); }
inline void copyElms(FXchar* dst,const FXchar* src,unsigned long n){ memcpy(dst,src,n); }
inline void copyElms(FXushort* dst,const FXushort* src,unsigned long n){ memcpy(dst,src,n<<1); }
inline void copyElms(FXshort* dst,const FXshort* src,unsigned long n){ memcpy(dst,src,n<<1); }
inline void copyElms(FXuint* dst,const FXuint* src,unsigned long n){ memcpy(dst,src,n<<2); }
inline void copyElms(FXint* dst,const FXint* src,unsigned long n){ memcpy(dst,src,n<<2); }
inline void copyElms(FXfloat* dst,const FXfloat* src,unsigned long n){ memcpy(dst,src,n<<2); }
inline void copyElms(FXdouble* dst,const FXdouble* src,unsigned long n){ memcpy(dst,src,n<<3); }

// Simple bit-wise copy for array of pointers to any type
template<class TYPE> inline void copyElms(TYPE** dst,const TYPE** src,unsigned long n){ memcpy(dst,src,n*sizeof(void*)); }

// Simple bit-wise move for array of basic type
inline void moveElms(FXuchar* dst,const FXuchar* src,unsigned long n){ memmove(dst,src,n); }
inline void moveElms(FXchar* dst,const FXchar* src,unsigned long n){ memmove(dst,src,n); }
inline void moveElms(FXushort* dst,const FXushort* src,unsigned long n){ memmove(dst,src,n<<1); }
inline void moveElms(FXshort* dst,const FXshort* src,unsigned long n){ memmove(dst,src,n<<1); }
inline void moveElms(FXuint* dst,const FXuint* src,unsigned long n){ memmove(dst,src,n<<2); }
inline void moveElms(FXint* dst,const FXint* src,unsigned long n){ memmove(dst,src,n<<2); }
inline void moveElms(FXfloat* dst,const FXfloat* src,unsigned long n){ memmove(dst,src,n<<2); }
inline void moveElms(FXdouble* dst,const FXdouble* src,unsigned long n){ memmove(dst,src,n<<3); }

// Simple bit-wise move for array of pointers to any type
template<class TYPE> inline void moveElms(TYPE** dst,const TYPE** src,unsigned long n){ memmove(dst,src,n*sizeof(void*)); }

// Fill byte arrays with constant
inline void fillElms(FXuchar* dst,const FXuchar& src,unsigned long n){ memset(dst,src,n); }
inline void fillElms(FXchar* dst,const FXchar& src,unsigned long n){ memset(dst,src,n); }

}

#endif
