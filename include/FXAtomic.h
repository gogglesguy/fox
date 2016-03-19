/********************************************************************************
*                                                                               *
*                         A t o m i c   I n t e g e r                           *
*                                                                               *
*********************************************************************************
* Copyright (C) 2006,2007 by Jeroen van der Zijp.   All Rights Reserved.        *
*********************************************************************************
* This library is free software; you can redistribute it and/or                 *
* modify it under the terms of the GNU Lesser General Public                    *
* License as published by the Free Software Foundation; either                  *
* version 2.1 of the License, or (at your option) any later version.            *
*                                                                               *
* This library is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             *
* Lesser General Public License for more details.                               *
*                                                                               *
* You should have received a copy of the GNU Lesser General Public              *
* License along with this library; if not, write to the Free Software           *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.    *
*********************************************************************************
* $Id: FXAtomic.h,v 1.12 2007/04/27 03:56:51 fox Exp $                          *
********************************************************************************/
#ifndef FXATOMIC_H
#define FXATOMIC_H

//#ifdef __GLIBCPP__
//#include <bits/atomicity.h>
//#endif

namespace FX {



/**
* An atomically incrementable integer
*/
class FXAtomic {
  volatile int val;
public:

  /// Constructs an atomic integer with a given initial value.
  FXAtomic(int v=0):val(v){}

  /// Returns the value of the integer
  operator int() const { return val; }

  /// Atomically add something to the integer and return its old value
  inline int exchange_and_add(int v){
#if defined(WIN32)
    return ::InterlockedExchangeAdd((LONG*)&val,v);
#elif defined(__GNUC__) && defined(__i386__)
    register volatile int* reference=&val;
    register int result=v;
    __asm__ __volatile__ ("lock; xaddl %0,(%1)"
			  : "=r" (result), "=r" (reference)
			  : "0" (result), "1" (reference));
    return result;
#elif defined(__GNUC__) && defined(__x86_64__)
    register volatile int* reference=&val;
    register int result=v;
    __asm__ __volatile__ ("lock; xaddl %0,(%1)"
			  : "=r" (result), "=r" (reference)
			  : "0" (result), "1" (reference));
    return result;
#else
//#elif defined(__GNUC__) && defined(__GLIBCPP__)
    return __gnu_cxx::__exchange_and_add((LONG*)&val,v);
//#elif defined(__GNUC__) && defined(__i386__)
//    register int result=a;
//    __asm__ __volatile__ ("lock; xaddl %1,%0"
//		          : "=m" (val), "=r" (result),
//		          : "m" (val), "r" (result));
//    return result;
//#else
//#error "Value not defined for this architecture!"
#endif
    }

  /// Atomically add something to the integer
  inline FXAtomic& add(int v){
#if defined(WIN32)
    ::InterlockedExchangeAdd((LONG*)&val,v);
//#elif defined(__GNUC__) && defined(__GLIBCPP__)
//#elif (defined(__GNUC__) || defined(__ICC)) && defined(__linux__) && defined(__i386__)
#else
   // __gnu_cxx::__atomic_add(&val,a);
//#elif defined(__GNUC__) && defined(__i386__)
//    __asm__ __volatile__ ("lock; addl %1,%0"
//                          :"=m" (val)
//                          :"ir" (v), "m" (val));
//#else
//#error "add not implemented on this architecture!"
#endif
    return *this;
    }

  /// Atomically substract something to the integer
  inline FXAtomic& sub(int v){
    return add(-v);
    }

  /// Atomically add something to the integer
  inline FXAtomic& operator+=(int v){
    return add(v);
    }

  /// Atomically substract something from the integer
  inline FXAtomic& operator-=(int v){
    return add(-v);
    }

  /// Atomically increment the integer, prefix version
  inline FXAtomic& operator++(){
    return add(1);
    }

  /// Atomically decrement the integer, prefix version
  inline FXAtomic& operator--(){
    return add(-1);
    }

  /// Atomically increment value, postfix version
  inline int operator++(int){
    return exchange_and_add(1);
    }

  /// Atomically decrement value, postfix version
  inline int operator--(int){
    return exchange_and_add(-1);
    }
  };

}

#endif
