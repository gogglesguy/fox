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
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "FXThread.h"

/*
  Notes:

  - THIS LIBRARY REQUIRES LOCKING PRIMITIVES NOT PRESENT ON OLDER MACHINES.  In
    the x86 world, you're good on PentiumPro or newer.
    Code intended to run on ancient hardware can not use features only present
    on modern processors; you should write your software to use operating-system
    provided locking features such as mutexes and semaphores instead!

  - Workaround: compile this with -DOLDPENTIUM when compiling this library; this
    causes fallback to general-purpose O.S. supported primitives.

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

  - We fall back on a global mutex if no atomic primitives don't exist.  This is
    of course orders of magnitude slower than the preferred method.
*/


// Determine if builtin __sync_XXXX() functions are available
#if defined( __GNUC__ ) && defined ( __GNUC_MINOR__ ) && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 1)))
#define HAVE_BUILTIN_SYNC 1
#if defined( __arm__ )  || defined( __armel__ )
#undef HAVE_BUILTIN_SYNC
#endif
#if defined( __hppa ) || defined( __hppa__ )
#undef HAVE_BUILTIN_SYNC
#endif
#if defined( __m68k__ )
#undef HAVE_BUILTIN_SYNC
#endif
#if defined( __sparc__ )
#undef HAVE_BUILTIN_SYNC
#endif
#if defined( __INTEL_COMPILER ) && !defined( __ia64__ )
#undef HAVE_BUILTIN_SYNC
#endif
#endif

// Have inline assembly only when using GNU C++ or Intel C++
#if (defined(__GNUC__) || defined(__INTEL_COMPILER))
#define HAVE_INLINE_ASSEMBLY 1
#endif

// Code for old Pentium, i486, or i386
#if defined(OLDPENTIUM)
#undef HAVE_INLINE_ASSEMBLY
#undef HAVE_BUILTIN_SYNC
#endif

using namespace FX;


namespace FX {


/*******************************************************************************/

// FIXME
// Either works with real atomic primitives,
// or doesn't work at all.
// Have API to find out which.

// If neither windows, nor inline assembly, then fallback to global mutex
#if !(defined(WIN32) || (defined(HAVE_INLINE_ASSEMBLY) && (defined(__i386__) || defined(__x86_64__))))
static pthread_mutex_t global_mutex=PTHREAD_MUTEX_INITIALIZER;
#endif


// Atomically set variable at ptr to v, and return its old contents
FXint atomicSet(volatile FXint* ptr,FXint v){
#if defined(WIN32)
  return InterlockedExchange((LONG*)ptr,v);
#elif (defined(HAVE_INLINE_ASSEMBLY) && (defined(__i386__) || defined(__x86_64__)))
  register FXint ret=v;
  __asm__ __volatile__("xchgl %0,%1\n\t" : "=r"(ret),"=m" (*ptr) : "0" (ret), "m"(*ptr) : "memory", "cc");
  return ret;
#elif defined(HAVE_BUILTIN_SYNC)
  return __sync_lock_test_and_set(ptr,v);
#else
  register FXint ret;
  pthread_mutex_lock(&global_mutex);
  ret=*ptr;
  *ptr=v;
  pthread_mutex_unlock(&global_mutex);
  return ret;
#endif
  }


// Atomically add v to variable at ptr, and return its old contents
FXint atomicAdd(volatile FXint* ptr,FXint v){
#if defined(WIN32)
  return InterlockedExchangeAdd((LONG*)ptr,v);
#elif (defined(HAVE_INLINE_ASSEMBLY) && (defined(__i386__) || defined(__x86_64__)))
  register FXint ret=v;
  __asm__ __volatile__ ("lock\n\t"
                        "xaddl %0,%1\n\t" : "=r"(ret), "=m"(*ptr) : "0" (ret), "m" (*ptr) : "memory", "cc");
  return ret;
#elif defined(HAVE_BUILTIN_SYNC)
  return __sync_fetch_and_add(ptr,v);
#else
  register FXint ret;
  pthread_mutex_lock(&global_mutex);
  ret=*ptr;
  *ptr+=v;
  pthread_mutex_unlock(&global_mutex);
  return ret;
#endif
  }


// Atomically compare variable at ptr against expect, setting it to v if equal; returns the old value at ptr
FXint atomicCas(volatile FXint* ptr,FXint expect,FXint v){
#if defined(WIN32)
  return InterlockedCompareExchange((volatile LONG*)ptr,v,expect);
#elif (defined(HAVE_INLINE_ASSEMBLY) && (defined(__i386__) || defined(__x86_64__)))
  register FXint ret;
  __asm__ __volatile__("lock\n\t"
                       "cmpxchgl %1,%2\n\t" : "=a"(ret), "=r"(v), "=m"(*ptr) : "0"(expect), "1"(v), "m"(*ptr) : "memory", "cc");
  return ret;
#elif defined(HAVE_BUILTIN_SYNC)
  return __sync_val_compare_and_swap(ptr,expect,v);
#else
  register FXint ret;
  pthread_mutex_lock(&global_mutex);
  ret=*ptr;
  if(*ptr==expect) *ptr=v;
  pthread_mutex_unlock(&global_mutex);
  return ret;
#endif
  }


// Atomically compare variable at ptr against expect, setting it to v if equal and return true, or false otherwise
FXbool atomicBoolCas(volatile FXint* ptr,FXint expect,FXint v){
#if defined(WIN32)
  return (InterlockedCompareExchange((LONG*)ptr,v,expect)==expect);
#elif (defined(HAVE_INLINE_ASSEMBLY) && (defined(__i386__) || defined(__x86_64__)))
  register FXbool ret;
  __asm__ __volatile__ ("lock\n\t"
                        "cmpxchgl %1,%2\n\t"
                        "setz %0 \n\t" : "=a"(ret), "=r"(v), "=m"(*ptr) : "a"(expect), "r"(v), "m"(*ptr) : "memory", "cc");
  return ret;
#elif defined(HAVE_BUILTIN_SYNC)
  return __sync_bool_compare_and_swap(ptr,expect,v);
#else
  register FXbool ret=false;
  pthread_mutex_lock(&global_mutex);
  if(*ptr==expect){ *ptr=v; ret=true; }
  pthread_mutex_unlock(&global_mutex);
  return ret;
#endif
  }


// Atomically set pointer variable at ptr to v, and return its old contents
void* atomicSet(void* volatile* ptr,void* v){
#if defined(WIN32)
   return InterlockedExchangePointer(ptr,v);
#elif (defined(HAVE_INLINE_ASSEMBLY) && defined(__i386__))
  void* ret=v;
  __asm__ __volatile__("xchgl %0,%1\n\t" : "=r"(ret),"=m" (*ptr) : "0" (ret), "m"(*ptr) : "memory", "cc");
  return ret;
#elif (defined(HAVE_INLINE_ASSEMBLY) && defined(__x86_64__))
  void* ret=v;
  __asm__ __volatile__("xchgq %0,%1\n\t" : "=r"(ret),"=m" (*ptr) : "0" (ret), "m"(*ptr) : "memory", "cc");
  return ret;
#elif defined(HAVE_BUILTIN_SYNC)
  return __sync_lock_test_and_set(ptr,v);
#else
  register void* ret;
  pthread_mutex_lock(&global_mutex);
  ret=*ptr;
  *ptr=v;
  pthread_mutex_unlock(&global_mutex);
  return ret;
#endif
  }


// Atomically add v to pointer variable at ptr, and return its old contents
void* atomicAdd(void* volatile* ptr,FXival v){
#if defined(WIN32) && defined(_WIN64)
  return (void*)InterlockedExchangeAdd64((LONGLONG*)ptr,(LONGLONG)v);
#elif defined(WIN32)
  return (void*)InterlockedExchangeAdd((LONG*)ptr,(LONG)v);
#elif (defined(HAVE_INLINE_ASSEMBLY) && defined(__i386__))
  register void* ret=(void*)v;
  __asm__ __volatile__ ("lock\n\t"
                        "xaddl %0,%1\n\t" : "=r"(ret), "=m"(*ptr) : "0" (ret), "m" (*ptr) : "memory", "cc");
  return ret;
#elif (defined(HAVE_INLINE_ASSEMBLY) && defined(__x86_64__))
  register void* ret=(void*)v;
  __asm__ __volatile__ ("lock\n\t"
                        "xaddq %0,%1\n\t" : "=r"(ret), "=m"(*ptr) : "0" (ret), "m" (*ptr) : "memory", "cc");
  return ret;
#elif defined(HAVE_BUILTIN_SYNC)
  return __sync_fetch_and_add(ptr,v);
#else
  register void* ret;
  pthread_mutex_lock(&global_mutex);
  ret=*ptr;
  *((unsigned char**)ptr)+=v;
  pthread_mutex_unlock(&global_mutex);
  return ret;
#endif
  }


// Atomically compare pointer variable at ptr against expect, setting it to v if equal; returns the old value at ptr
void* atomicCas(void* volatile* ptr,void* expect,void* v){
#if defined(WIN32)
  return InterlockedCompareExchangePointer(ptr,v,expect);
#elif (defined(HAVE_INLINE_ASSEMBLY) && defined(__i386__))
  register void* ret;
  __asm__ __volatile__("lock\n\t"
                       "cmpxchgl %1,%2\n\t" : "=a"(ret), "=r"(v), "=m"(*ptr) : "0"(expect), "1"(v), "m"(*ptr) : "memory", "cc");
  return ret;
#elif (defined(HAVE_INLINE_ASSEMBLY) && defined(__x86_64__))
  register void* ret;
  __asm__ __volatile__("lock\n\t"
                       "cmpxchgq %1,%2\n\t" : "=a"(ret), "=r"(v), "=m"(*ptr) : "0"(expect), "1"(v), "m"(*ptr) : "memory", "cc");
  return ret;
#elif defined(HAVE_BUILTIN_SYNC)
  return __sync_val_compare_and_swap(ptr,expect,v);
#else
  register void* ret;
  pthread_mutex_lock(&global_mutex);
  ret=*ptr;
  if(*ptr==expect) *ptr=v;
  pthread_mutex_unlock(&global_mutex);
  return ret;
#endif
  }



// Atomically compare pointer variable at ptr against expect, setting it to v if equal and return true, or false otherwise
FXbool atomicBoolCas(void* volatile* ptr,void* expect,void* v){
#if defined(WIN32)
  return (InterlockedCompareExchangePointer(ptr,v,expect)==expect);
#elif (defined(HAVE_INLINE_ASSEMBLY) && defined(__i386__))
  register FXbool ret;
  __asm__ __volatile__ ("lock\n\t"
                        "cmpxchgl %1,%2\n\t"
                        "setz %0\n\t" : "=a"(ret), "=r"(v), "=m"(*ptr) : "a"(expect), "r"(v), "m"(*ptr) : "memory", "cc");
  return ret;
#elif (defined(HAVE_INLINE_ASSEMBLY) && defined(__x86_64__))
  register FXbool ret;
  __asm__ __volatile__ ("lock\n\t"
                        "cmpxchgq %1,%2\n\t"
                        "setz %0 \n\t" : "=a"(ret), "=r"(v), "=m"(*ptr) : "a"(expect), "r"(v), "m"(*ptr) : "memory", "cc");
  return ret;
#elif defined(HAVE_BUILTIN_SYNC)
  return __sync_bool_compare_and_swap(ptr,expect,v);
#else
  register FXbool ret=false;
  pthread_mutex_lock(&global_mutex);
  if(*ptr==expect){ *ptr=v; ret=true; }
  pthread_mutex_unlock(&global_mutex);
  return ret;
#endif
  }


// 1. If the class is MEMORY, pass the argument on the stack.
// 2. If the class is INTEGER, the next available register of the sequence %rdi, %rsi, %rdx, %rcx, %r8 and %r9 is used13 .
// 3. If the class is SSE, the next available vector register is used, the registers are taken in the order from %xmm0 to %xmm7.
// 4. If the class is SSEUP, the eightbyte is passed in the next available eightbyte chunk of the last used vector register.
// 5. If the class is X87, X87UP or COMPLEX_X87, it is passed in memory.

// Atomically compare pair of variables at ptr against (cmpa,cmpb), setting them to (a,b) if equal and return true, or false otherwise
FXbool atomicBoolDCas(void* volatile* ptr,void* cmpa,void* cmpb,void* a,void* b){
#if defined(WIN32)
  // FIXME //
  return false;
#elif (defined(HAVE_INLINE_ASSEMBLY) && defined(__i386__))
  register FXbool ret;
  // CMPXCHG8B: if(EDX:EAX == MEM64){ MEM64 = ECX:EBX } else { EDX:EAX = MEM64; }
  __asm__ __volatile__ ("xchgl %%ebx,%%esi\n\t"                 // Swap EBX to ESI to get a into EBX and save EBX
                        "lock\n\t"
                        "cmpxchg8b (%%edi)\n\t"                 // The pointer was forced into EDI
                        "movl %%esi,%%ebx\n\t"                  // Swap ESI back to restore EBX
                        "setz %0\n\t" : "=a"(ret), "=D"(ptr) : "D"(ptr), "a"(cmpa), "d"(cmpb), "S"(a), "c"(b) : "memory", "cc");
  return ret;
#elif (defined(HAVE_INLINE_ASSEMBLY) && defined(__x86_64__))
  register FXbool ret;
  // CMPXCHG16B: if(RDX:RAX == MEM128){ MEM128 = RCX:RBX } else { RDX:RAX = MEM128; }
  __asm__ __volatile__ ("lock\n\t"
                        "cmpxchg16b %1\n\t"
                        "setz %0\n\t" : "=a"(ret), "=m"(*ptr) : "a"(cmpa), "m"(*ptr), "d"(cmpb), "b"(a), "c"(b) : "memory", "cc");
  return ret;
//#elif (defined(HAVE_BUILTIN_SYNC) && defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8))
//  struct Pair { unsigned long a; unsigned long b; };
//  union Duet { Pair p; unsigned long long w; };
//  Duet cmp;
//  Duet val;
//  cmp.p.a=(unsigned long)cmpa;
//  cmp.p.b=(unsigned long)cmpb;
//  val.p.a=(unsigned long)a;
//  val.p.b=(unsigned long)b;
//  return __sync_bool_compare_and_swap((unsigned long long*)ptr,cmp.w,val.w);
//#elif (defined(HAVE_BUILTIN_SYNC) && defined(__LP64__) && defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_16))
//  struct Pair { unsigned long a; unsigned long b; };
//  union Duet { Pair p; __int128_t w; };
//  Duet cmp;
//  Duet val;
//  cmp.p.a=(unsigned long)cmpa;
//  cmp.p.b=(unsigned long)cmpb;
//  val.p.a=(unsigned long)a;
//  val.p.b=(unsigned long)b;
//  return __sync_bool_compare_and_swap_16((__int128_t*)ptr,cmp.w,val.w);
#else
  register FXbool ret=false;
  pthread_mutex_lock(&global_mutex);
  if(ptr[0]==cmpa && ptr[1]==cmpb){
    ptr[0]=a;
    ptr[1]=b;
    ret=true;
    }
  pthread_mutex_unlock(&global_mutex);
  return ret;
#endif
  }

}
