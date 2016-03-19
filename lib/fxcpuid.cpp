/********************************************************************************
*                                                                               *
*                              C P U I D   S u p p o r t                        *
*                                                                               *
*********************************************************************************
* Copyright (C) 1998,2012 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "fxcpuid.h"


/*
  Notes:
  - Two major functions:

      o Check for presence of CPUID instruction.  This is x86 architecture
        from pentium processor on up.

      o CPUID instruction to obtain information on run-time platform.

  - Utility functions to quickly obtain commonly interesting information.

*/

// Have inline assembly only when using GNU C++ or Intel C++
#if (defined(__GNUC__) || defined(__INTEL_COMPILER))
#define HAVE_INLINE_ASSEMBLY 1
#endif

// In 64-bit, we can use EAX, EBX, ECX, EDX
#if defined(__x86_64__)

// The CPUID instruction returns stuff in eax, ebx, ecx, edx.
#define mycpuid(op,eax,ebx,ecx,edx)  \
  __asm__ __volatile__("cpuid           \n\t"  : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "0" (op) : "cc")

// The CPUID instruction returns stuff in eax, ebx, ecx, edx.
#define mycpuidcount(op,count,eax,ebx,ecx,edx)  \
  __asm__ __volatile__("cpuid           \n\t" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "0"(op), "2"(count) : "cc")

// In 32-bit, don't use EBX it contains address in PIC
#elif defined(__i386__)

// The CPUID instruction returns stuff in eax, ebx, ecx, edx.
#define mycpuid(op,eax,ebx,ecx,edx)  \
  __asm__ __volatile__("xchgl %%ebx, %1 \n\t"  \
                       "cpuid           \n\t"  \
                       "xchgl %%ebx, %1 \n\t"  : "=a"(eax), "=r"(ebx), "=c"(ecx), "=d"(edx) : "0" (op) : "cc")

// The CPUID instruction returns stuff in eax, ebx, ecx, edx.
#define mycpuidcount(op,count,eax,ebx,ecx,edx)  \
  __asm__ __volatile__("xchgl %%ebx, %1 \n\t"   \
                       "cpuid           \n\t"   \
                       "xchgl %%ebx, %1 \n\t" : "=a"(eax), "=r"(ebx), "=c"(ecx), "=d"(edx) : "0"(op), "2"(count) : "cc")

#endif

using namespace FX;

/*******************************************************************************/

namespace FX {


// Return true if CPUID present
FXbool fxCPUIDPresent(){
#if defined(WIN32)
  return false;         // FIXME change later
#elif (defined(HAVE_INLINE_ASSEMBLY) && defined(__x86_64__))
  return true;
#elif (defined(HAVE_INLINE_ASSEMBLY) && defined(__i386__))
  register FXuint eax,edx;
  __asm__ __volatile__("pushfl\n\t"
                       "popl   %0\n\t"
                       "movl   %0,%1\n\t"
                       "xorl   $0x00200000,%0\n\t"
                       "pushl  %0\n\t"
                       "popfl  \n\t"
                       "pushfl \n\t"
                       "popl   %0\n\t" : "=a" (eax), "=d" (edx) : /**/ : "cc");
  return ((eax^edx)>>21)&1;
#else
  return false;
#endif
  }


// Return number of levels of CPUID feature-requests supported
FXuint fxCPUCaps(FXuint level){
#if defined(WIN32)
  return 0;             // FIXME change later
#elif (defined(HAVE_INLINE_ASSEMBLY) && (defined(__i386__) || defined(__x86_64__)))
  if(__likely(fxCPUIDPresent())){
    register FXuint eax,ebx,ecx,edx;
    level&=0x80000000;
    mycpuid(level,eax,ebx,ecx,edx);
    return eax+1;
    }
#endif
  return 0;
  }


// Get CPU info
FXbool fxCPUGetCaps(FXuint level,FXuint features[]){
#if defined(WIN32)
  return false;         // FIXME change later
#elif (defined(HAVE_INLINE_ASSEMBLY) && (defined(__i386__) || defined(__x86_64__)))
  if(__likely(level<fxCPUCaps(level))){
    mycpuid(level,features[0],features[1],features[2],features[3]);
    return true;
    }
  return false;
#else
  return false;
#endif
  }


// Get CPU info
FXbool fxCPUGetXCaps(FXuint level,FXuint count,FXuint features[]){
#if defined(WIN32)
  return false;         // FIXME change later
#elif (defined(HAVE_INLINE_ASSEMBLY) && (defined(__i386__) || defined(__x86_64__)))
  if(__likely(level<fxCPUCaps(level))){
    mycpuidcount(level,count,features[0],features[1],features[2],features[3]);
    return true;
    }
  return false;
#else
  return false;
#endif
  }


// Return exciting features
FXuint fxCPUFeatures(){
#if defined(WIN32)
  // FIXME //
#elif (defined(HAVE_INLINE_ASSEMBLY) && (defined(__i386__) || defined(__x86_64__)))
  if(__likely(fxCPUIDPresent())){
    FXuint feature[4],caps=0;
    mycpuid(1,feature[0],feature[1],feature[2],feature[3]);
    if(feature[2]&0x00000001) caps|=CPU_HAS_SSE3;
    if(feature[2]&0x00000200) caps|=CPU_HAS_SSSE3;
    if(feature[2]&0x00002000) caps|=CPU_HAS_CMPXCHG16;
    if(feature[2]&0x00080000) caps|=CPU_HAS_SSE41;
    if(feature[2]&0x00100000) caps|=CPU_HAS_SSE42;
    if(feature[2]&0x00800000) caps|=CPU_HAS_POPCNT;
    if(feature[2]&0x00400000) caps|=CPU_HAS_MOVBE;
    if(feature[3]&0x00000010) caps|=CPU_HAS_TSC;
    if(feature[3]&0x00000100) caps|=CPU_HAS_CMPXCHG8;
    if(feature[3]&0x00008000) caps|=CPU_HAS_CMOV;
    if(feature[3]&0x00800000) caps|=CPU_HAS_MMX;
    if(feature[3]&0x02000000) caps|=CPU_HAS_SSE;
    if(feature[3]&0x04000000) caps|=CPU_HAS_SSE2;
    mycpuid(0,feature[0],feature[1],feature[2],feature[3]);
    if(__likely(feature[1]==0x68747541) && __likely(feature[2]==0x444d4163) && __likely(feature[3]==0x69746e65)){
      mycpuid(0x80000001,feature[0],feature[1],feature[2],feature[3]);
      if(feature[2]&0x00000040) caps|=CPU_HAS_SSE4A;
      if(feature[3]&0x40000000) caps|=CPU_HAS_3DNOWEXT;
      if(feature[3]&0x80000000) caps|=CPU_HAS_3DNOW;
      }
    return caps;
    }
#endif
  return 0;
  }


// Return CPU Identification.
FXbool fxCPUName(FXchar name[]){
#if defined(WIN32)
  // FIXME //
#elif (defined(HAVE_INLINE_ASSEMBLY) && (defined(__i386__) || defined(__x86_64__)))
  if(__likely(fxCPUIDPresent())){
    FXuint feature[4];
    mycpuid(0,feature[0],feature[1],feature[2],feature[3]);
    name[0]=((char*)feature)[4];
    name[1]=((char*)feature)[5];
    name[2]=((char*)feature)[6];
    name[3]=((char*)feature)[7];
    name[4]=((char*)feature)[12];
    name[5]=((char*)feature)[13];
    name[6]=((char*)feature)[14];
    name[7]=((char*)feature)[15];
    name[8]=((char*)feature)[8];
    name[9]=((char*)feature)[9];
    name[10]=((char*)feature)[10];
    name[11]=((char*)feature)[11];
    name[12]='\0';
    return true;
    }
#endif
  name[0]='\0';
  return false;
  }

/*
FXchar name[32];
FXuint flags;
fxCPUName(name);
fxmessage("name=%s\n",name);
flags=fxCPUFeatures();
fxmessage("bits=%b\n",flags);
FXchar name[32];
FXuint features[4];
FXuint flags;
fxCPUName(name);
fxmessage("name=%s\n",name);
flags=fxCPUFeatures();
fxmessage("bits=%b\n",flags);
if(fxCPUGetCaps(0,features)){
  fxmessage("feature[0]=0x%08x 0b%032b\n",features[0],features[0]);
  fxmessage("feature[1]=0x%08x 0b%032b\n",features[1],features[1]);
  fxmessage("feature[2]=0x%08x 0b%032b\n",features[2],features[2]);
  fxmessage("feature[3]=0x%08x 0b%032b\n",features[3],features[3]);
  }

  void* cmpa=(void*)1;
  void* cmpb=(void*)2;
  void* a=(void*)3;
  void* b=(void*)4;
  void* volatile dd[2]={(void*)1,(void*)2};
  fxmessage("dd=(%p,%p)\n",dd[0],dd[1]);
  fxmessage("swapped: ");
  if(atomicBoolDCas(dd,cmpa,cmpb,a,b)){
    fxmessage("yes\n");
    }
  else{
    fxmessage("no\n");
    }
  fxmessage("dd=(%p,%p)\n",dd[0],dd[1]);
  fxmessage("swapped: ");
  if(atomicBoolDCas(dd,cmpa,cmpb,a,b)){
    fxmessage("yes\n");
    }
  else{
    fxmessage("no\n");
    }
  fxmessage("dd=(%p,%p)\n",dd[0],dd[1]);
  exit(0);

*/

}

