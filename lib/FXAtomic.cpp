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
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "FXAtomic.h"

/*
  Notes:

  - THIS LIBRARY REQUIRES LOCKING PRIMITIVES NOT PRESENT ON OLDER MACHINES.
    In the x86 world, you're good on PentiumPro or newer.

  - Code intended to run on ancient hardware CAN NOT use features only present
    on modern processors; you should write your software to use operating-system
    provided locking features such as mutexes and semaphores instead!

  - You can test using atomicsAvailable() to see if these primitives are atomic.

  - The API's are function calls rather than inlines in the header files because
    naive programmers don't specify "submodel" options and may not get the inten-
    ded code generation compared to what FOX was compiled with.  The problem is
    compilers tend to generate code for trailing-edge processor targets unless
    told to do otherwise.

  - We generate locking primitives using in-line assembly in preference to the
    GCC builtins, whenever possible.  Just because the version of GCC has the
    builtins doesn't mean it'll generate them since that depends on submodel
    options being set properly!

  - For optimal performance, keep your atomic variables in their own cache-line;
    threads monitoring atomic variables (e.g. spinlocks) will cause bus traffic
    whenever shared cachelines are updated, even if its not the variable itself
    but something close to it in the same cacheline.
*/


// Use sync for GCC >= 4.1
#if ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 1)))
#define BUILTIN_SYNC   1
#endif

using namespace FX;

namespace FX {

/*******************************************************************************/

// Atomics are available
FXbool atomicsAvailable(){
#if defined(WIN32) && ((_MSC_VER >= 1400) || (__BORLANDC__ >= 0x500))
  return true;
#elif ((defined(__GNUC__) || defined(__INTEL_COMPILER)) && defined(__i386__))
  return true;
#elif ((defined(__GNUC__) || defined(__INTEL_COMPILER)) && defined(__x86_64__))
  return true;
#elif defined(BUILTIN_SYNC)
  return true;
#else
  return false;
#endif
  }

/*******************************************************************************/

// Atomically set variable at ptr to v, and return its old contents
FXint atomicSet(volatile FXint* ptr,FXint v){
#if defined(WIN32)
  return InterlockedExchange((LONG*)ptr,v);
#elif ((defined(__GNUC__) || defined(__INTEL_COMPILER)) && (defined(__i386__) || defined(__x86_64__)))
  register FXint ret=v;
  __asm__ __volatile__("xchgl %0, (%1)\n\t" : "=r"(ret) : "r"(ptr), "0"(ret) : "memory", "cc");
  return ret;
#elif defined(BUILTIN_SYNC)
  return __sync_lock_test_and_set(ptr,v);
#else
  FXint ret=*ptr;
  *ptr=v;
  return ret;
#endif
  }



// Atomically add v to variable at ptr, and return its old contents
FXint atomicAdd(volatile FXint* ptr,FXint v){
#if defined(WIN32)
  return InterlockedExchangeAdd((LONG*)ptr,v);
#elif ((defined(__GNUC__) || defined(__INTEL_COMPILER)) && (defined(__i386__) || defined(__x86_64__)))
  register FXint ret=v;
  __asm__ __volatile__ ("lock\n\t"
                        "xaddl %0, (%1)\n\t" : "=r"(ret) : "r"(ptr), "0"(ret) : "memory", "cc");
  return ret;
#elif defined(BUILTIN_SYNC)
  return __sync_fetch_and_add(ptr,v);
#else
  FXint ret=*ptr;
  *ptr+=v;
  return ret;
#endif
  }


// Atomically compare variable at ptr against expect, setting it to v if equal; returns the old value at ptr
FXint atomicCas(volatile FXint* ptr,FXint expect,FXint v){
#if defined(WIN32) && ((_MSC_VER >= 1400) || (__BORLANDC__ >= 0x500))
  return InterlockedCompareExchange((LONG*)ptr,(LONG)v,(LONG)expect);
#elif ((defined(__GNUC__) || defined(__INTEL_COMPILER)) && (defined(__i386__) || defined(__x86_64__)))
  register FXint ret;
  __asm__ __volatile__("lock\n\t"
                       "cmpxchgl %2, (%1)\n\t" : "=a"(ret) : "r"(ptr), "r"(v), "a"(expect) : "memory", "cc");
  return ret;
#elif defined(BUILTIN_SYNC)
  return __sync_val_compare_and_swap(ptr,expect,v);
#else
  FXint ret=*ptr;
  if(*ptr==expect){
    *ptr=v;
    }
  return ret;
#endif
  }


// Atomically compare variable at ptr against expect, setting it to v if equal and return true, or false otherwise
FXbool atomicBoolCas(volatile FXint* ptr,FXint expect,FXint v){
#if defined(WIN32) && ((_MSC_VER >= 1400) || (__BORLANDC__ >= 0x500))
  return (InterlockedCompareExchange((LONG*)ptr,(LONG)v,(LONG)expect)==(LONG)expect);
#elif ((defined(__GNUC__) || defined(__INTEL_COMPILER)) && (defined(__i386__) || defined(__x86_64__)))
  register FXbool ret;
  __asm__ __volatile__ ("lock\n\t"
                        "cmpxchgl %2, (%1)\n\t"
                        "sete   %%al\n\t"
                        "andl   $1, %%eax\n\t" : "=a"(ret) : "r"(ptr), "r"(v), "a"(expect) : "memory", "cc");
  return ret;
#elif defined(BUILTIN_SYNC)
  return __sync_bool_compare_and_swap(ptr,expect,v);
#else
  if(*ptr==expect){
    *ptr=v;
    return true;
    }
  return false;
#endif
  }

/*******************************************************************************/

// Atomically set variable at ptr to v, and return its old contents
FXuint atomicSet(volatile FXuint* ptr,FXuint v){
#if defined(WIN32)
  return InterlockedExchange((LONG*)ptr,v);
#elif ((defined(__GNUC__) || defined(__INTEL_COMPILER)) && (defined(__i386__) || defined(__x86_64__)))
  register FXuint ret=v;
  __asm__ __volatile__("xchgl %0, (%1)\n\t" : "=r"(ret) : "r"(ptr), "0"(ret) : "memory", "cc");
  return ret;
#elif defined(BUILTIN_SYNC)
  return __sync_lock_test_and_set(ptr,v);
#else
  FXuint ret=*ptr;
  *ptr=v;
  return ret;
#endif
  }


// Atomically add v to variable at ptr, and return its old contents
FXuint atomicAdd(volatile FXuint* ptr,FXuint v){
#if defined(WIN32)
  return InterlockedExchangeAdd((LONG*)ptr,v);
#elif ((defined(__GNUC__) || defined(__INTEL_COMPILER)) && (defined(__i386__) || defined(__x86_64__)))
  register FXuint ret=v;
  __asm__ __volatile__ ("lock\n\t"
                        "xaddl %0, (%1)\n\t" : "=r"(ret) : "r"(ptr), "0"(ret) : "memory", "cc");
  return ret;
#elif defined(BUILTIN_SYNC)
  return __sync_fetch_and_add(ptr,v);
#else
  FXuint ret=*ptr;
  *ptr+=v;
  return ret;
#endif
  }


// Atomically compare variable at ptr against expect, setting it to v if equal; returns the old value at ptr
FXuint atomicCas(volatile FXuint* ptr,FXuint expect,FXuint v){
#if defined(WIN32) && ((_MSC_VER >= 1400) || (__BORLANDC__ >= 0x500))
  return InterlockedCompareExchange((LONG*)ptr,(LONG)v,(LONG)expect);
#elif ((defined(__GNUC__) || defined(__INTEL_COMPILER)) && (defined(__i386__) || defined(__x86_64__)))
  register FXuint ret;
  __asm__ __volatile__("lock\n\t"
                       "cmpxchgl %2, (%1)\n\t" : "=a"(ret) : "r"(ptr), "r"(v), "a"(expect) : "memory", "cc");
  return ret;
#elif defined(BUILTIN_SYNC)
  return __sync_val_compare_and_swap(ptr,expect,v);
#else
  FXuint ret=*ptr;
  if(*ptr==expect){
    *ptr=v;
    }
  return ret;
#endif
  }


// Atomically compare variable at ptr against expect, setting it to v if equal and return true, or false otherwise
FXbool atomicBoolCas(volatile FXuint* ptr,FXuint expect,FXuint v){
#if defined(WIN32) && ((_MSC_VER >= 1400) || (__BORLANDC__ >= 0x500))
  return (InterlockedCompareExchange((LONG*)ptr,(LONG)v,(LONG)expect)==(LONG)expect);
#elif ((defined(__GNUC__) || defined(__INTEL_COMPILER)) && (defined(__i386__) || defined(__x86_64__)))
  register FXbool ret;
  __asm__ __volatile__ ("lock\n\t"
                        "cmpxchgl %2, (%1)\n\t"
                        "sete   %%al\n\t"
                        "andl   $1, %%eax\n\t" : "=a"(ret) : "r"(ptr), "r"(v), "a"(expect) : "memory", "cc");
  return ret;
#elif defined(BUILTIN_SYNC)
  return __sync_bool_compare_and_swap(ptr,expect,v);
#else
  if(*ptr==expect){
    *ptr=v;
    return true;
    }
  return false;
#endif
  }

/*******************************************************************************/

// Atomically set pointer variable at ptr to v, and return its old contents
FXptr atomicSet(volatile FXptr* ptr,FXptr v){
#if defined(WIN32) && ((_MSC_VER >= 1400) || (__BORLANDC__ >= 0x500))
   return (FXptr)InterlockedExchangePointer(ptr,v);
#elif ((defined(__GNUC__) || defined(__INTEL_COMPILER)) && defined(__i386__))
  FXptr ret=v;
  __asm__ __volatile__("xchgl %0, (%1)\n\t" : "=r"(ret) : "r"(ptr), "0"(ret) : "memory", "cc");
  return ret;
#elif ((defined(__GNUC__) || defined(__INTEL_COMPILER)) && defined(__x86_64__))
  FXptr ret=v;
  __asm__ __volatile__("xchgq %0, (%1)\n\t" : "=r"(ret) : "r"(ptr), "0"(ret) : "memory", "cc");
  return ret;
#elif defined(BUILTIN_SYNC)
  return __sync_lock_test_and_set(ptr,v);
#else
  FXptr ret=*ptr;
  *ptr=v;
  return ret;
#endif
  }


// Atomically add v to pointer variable at ptr, and return its old contents
FXptr atomicAdd(volatile FXptr* ptr,FXival v){
#if defined(WIN32) && defined(_WIN64)
  return (FXptr)InterlockedExchangeAdd64((LONGLONG*)ptr,(LONGLONG)v);
#elif defined(WIN32)
  return (FXptr)InterlockedExchangeAdd((LONG*)ptr,(LONG)v);
#elif ((defined(__GNUC__) || defined(__INTEL_COMPILER)) && defined(__i386__))
  register FXptr ret=(void*)v;
  __asm__ __volatile__ ("lock\n\t"
                        "xaddl %0, (%1)\n\t" : "=r"(ret) : "r"(ptr), "0"(ret) : "memory", "cc");
  return ret;
#elif ((defined(__GNUC__) || defined(__INTEL_COMPILER)) && defined(__x86_64__))
  register FXptr ret=(FXptr)v;
  __asm__ __volatile__ ("lock\n\t"
                        "xaddq %0, (%1)\n\t" : "=r"(ret) : "r"(ptr), "0" (ret) : "memory", "cc");
  return ret;
#elif defined(BUILTIN_SYNC)
  return __sync_fetch_and_add(ptr,(FXptr)v);
#else
  FXptr ret=*ptr;
  *((unsigned char**)ptr)+=v;
  return ret;
#endif
  }


// Atomically compare pointer variable at ptr against expect, setting it to v if equal; returns the old value at ptr
FXptr atomicCas(volatile FXptr* ptr,FXptr expect,FXptr v){
#if defined(WIN32) && ((_MSC_VER >= 1400) || (__BORLANDC__ >= 0x500))
  return (FXptr)InterlockedCompareExchangePointer((void**)ptr,v,expect);
#elif ((defined(__GNUC__) || defined(__INTEL_COMPILER)) && defined(__i386__))
  register FXptr ret=(FXptr)v;
  __asm__ __volatile__("lock\n\t"
                       "cmpxchgl %2, (%1)\n\t" : "=a"(ret) : "r"(ptr), "r"(v), "a"(expect) : "memory", "cc");
  return ret;
#elif ((defined(__GNUC__) || defined(__INTEL_COMPILER)) && defined(__x86_64__))
  register FXptr ret;
  __asm__ __volatile__("lock\n\t"
                       "cmpxchgq %2, (%1)\n\t" : "=a"(ret) : "r"(ptr), "r"(v), "a"(expect) : "memory", "cc");
  return ret;
#elif defined(BUILTIN_SYNC)
  return __sync_val_compare_and_swap(ptr,expect,v);
#else
  FXptr ret=*ptr;
  if(*ptr==expect){
    *ptr=v;
    }
  return ret;
#endif
  }


// Atomically compare pointer variable at ptr against expect, setting it to v if equal and return true, or false otherwise
FXbool atomicBoolCas(volatile FXptr* ptr,FXptr expect,FXptr v){
#if defined(WIN32) && ((_MSC_VER >= 1400) || (__BORLANDC__ >= 0x500))
  return (InterlockedCompareExchangePointer((void**)ptr,v,expect)==expect);
#elif ((defined(__GNUC__) || defined(__INTEL_COMPILER)) && defined(__i386__))
  register FXbool ret;
  __asm__ __volatile__ ("lock\n\t"
                        "cmpxchgl %2, (%1)\n\t"
                        "sete   %%al\n\t"
                        "andl   $1, %%eax\n\t" : "=a"(ret) : "r"(ptr), "r"(v), "a"(expect) : "memory", "cc");
  return ret;
#elif ((defined(__GNUC__) || defined(__INTEL_COMPILER)) && defined(__x86_64__))
  register FXbool ret;
  __asm__ __volatile__ ("lock\n\t"
                        "cmpxchgq %2, (%1)\n\t"
                        "sete   %%al\n\t"
                        "andq   $1, %%rax\n\t" : "=a"(ret) : "r"(ptr), "r"(v), "a"(expect) : "memory", "cc");
  return ret;
#elif defined(BUILTIN_SYNC)
  return __sync_bool_compare_and_swap(ptr,expect,v);
#else
  if(*ptr==expect){
    *ptr=v;
    return true;
    }
  return false;
#endif
  }


// Atomically compare pair of variables at ptr against (cmpa,cmpb), setting them to (a,b) if equal and return true, or false otherwise
FXbool atomicBoolDCas(volatile FXptr* ptr,FXptr cmpa,FXptr cmpb,FXptr a,FXptr b){
#if (defined(WIN32) && (_WIN32_WINNT >= 0x0600) && !defined(_WIN64))
  LONGLONG ab=(((LONGLONG)(FXuval)a)|((LONGLONG)(FXuval)b)<<32);
  LONGLONG compab=(((LONGLONG)(FXuval)cmpa)|((LONGLONG)(FXuval)cmpb)<<32);
  return (InterlockedCompareExchange64((LONGLONG volatile *)ptr,ab,compab)==compab);
#elif (defined(WIN32) && defined(_MSC_VER) && defined(_WIN64))
  LONGLONG duet[2]={(LONGLONG)a,(LONGLONG)b};
  return (_InterlockedCompareExchange128((LONGLONG volatile*)ptr,(LONGLONG)cmpb,(LONGLONG)cmpa,duet));
#elif ((defined(__GNUC__) || defined(__INTEL_COMPILER)) && defined(__i386__))
#if (defined(__PIC__) || defined(__PIE__))
  register FXbool ret;
  __asm__ __volatile__ ("xchgl  %%esi, %%ebx\n\t"
                        "lock\n\t"
                        "cmpxchg8b (%1)\n\t"
                        "setz   %%al\n\t"
                        "andl   $1, %%eax\n\t"
                        "xchgl  %%esi, %%ebx\n\t" : "=a"(ret) : "D"(ptr), "a"(cmpa), "d"(cmpb), "S"(a), "c"(b) : "memory", "cc");
  return ret;
#else
  register FXbool ret;
  __asm__ __volatile__ ("lock\n\t"
                        "cmpxchg8b (%1)\n\t"
                        "setz   %%al\n\t"
                        "andl   $1, %%eax\n\t" : "=a"(ret) : "D"(ptr), "a"(cmpa), "d"(cmpb), "b"(a), "c"(b) : "memory", "cc");
  return ret;
#endif
#elif ((defined(__GNUC__) || defined(__INTEL_COMPILER)) && defined(__x86_64__))
  register FXbool ret;
  __asm__ __volatile__ ("lock\n\t"
                        "cmpxchg16b (%1)\n\t"
                        "setz   %%al\n\t"
                        "andq    $1, %%rax\n\t" : "=a"(ret) : "r"(ptr), "a"(cmpa), "d"(cmpb), "b"(a), "c"(b) : "memory", "cc");
  return ret;
#elif (defined(BUILTIN_SYNC) && defined(__LP64__) && defined(__GNUC__))
  __uint128_t expectab=((__uint128_t)(FXuval)cmpa) | (((__uint128_t)(FXuval)cmpb)<<64);
  __uint128_t ab=((__uint128_t)(FXuval)a) | (((__uint128_t)(FXuval)b)<<64);
  return __sync_bool_compare_and_swap((__uint128_t*)ptr,expectab,ab);
#elif (defined(BUILTIN_SYNC) && !defined(__LP64__))
  __uint64_t expectab=((__uint64_t)(FXuval)cmpa) | (((__uint64_t)(FXuval)cmpb)<<32);
  __uint64_t ab=((__uint64_t)(FXuval)a) | (((__uint64_t)(FXuval)b)<<32);
  return __sync_bool_compare_and_swap((__uint64_t*)ptr,expectab,ab);
#else
  if(ptr[0]==cmpa && ptr[1]==cmpb){
    ptr[0]=a;
    ptr[1]=b;
    return true;
    }
  return false;
#endif
  }


}
