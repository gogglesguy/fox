/********************************************************************************
*                                                                               *
*                              C P U I D   S u p p o r t                        *
*                                                                               *
*********************************************************************************
* Copyright (C) 1998,2011 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#ifndef FXCPUID_H
#define FXCPUID_H


namespace FX {


// CPU Features for x86/x86-64
enum {
  CPU_HAS_TSC       = 0x00001,
  CPU_HAS_MMX       = 0x00002,
  CPU_HAS_CMOV      = 0x00004,
  CPU_HAS_SSE       = 0x00008,
  CPU_HAS_SSE2      = 0x00010,
  CPU_HAS_SSE3      = 0x00020,
  CPU_HAS_SSSE3     = 0x00040,
  CPU_HAS_SSE4A     = 0x00080,
  CPU_HAS_SSE41     = 0x00100,
  CPU_HAS_SSE42     = 0x00200,
  CPU_HAS_POPCNT    = 0x00400,
  CPU_HAS_MOVBE     = 0x00800,
  CPU_HAS_CMPXCHG8  = 0x01000,
  CPU_HAS_CMPXCHG16 = 0x02000,
  CPU_HAS_3DNOWEXT  = 0x04000,
  CPU_HAS_3DNOW     = 0x08000
  };


/**
* Check if cpuid is available.
*/
extern FXAPI FXbool fxCPUIDPresent();

/**
* On x86 or x86-64 processors, check the number of feature-requests
* available on the current processor.  If available, the largest feature
* request index+1 is returned, otherwise, this function returns 0.
* Extended caps levels may be obtained by passing 0x80000000 instead of
* zero.
*/
extern FXAPI FXuint fxCPUCaps(FXuint level=0);

/**
* Perform processor feature request of given level.
* The resulting output values are written into the array features,
* in the order EAX,EBX,ECX,EDX (for x86/x86-64).
* The function returns true if successful, and false if the processor
* does not support feature requests, or if the feature request level
* exceeds the number of levels reported by fxCPUCaps().
*/
extern FXAPI FXbool fxCPUGetCaps(FXuint level,FXuint features[]);

/**
* Perform processor extended feature request of given level, and given count
* parameter.
* The resulting output values are written into the array features,
* in the order EAX,EBX,ECX,EDX (for x86/x86-64).
* The function returns true if successful, and false if the processor
* does not support feature requests, or if the feature request level
* exceeds the number of levels reported by fxCPUCaps().
*/
extern FXAPI FXbool fxCPUGetXCaps(FXuint level,FXuint count,FXuint features[]);

/**
* Return CPU features.  For example, CPU_HAS_SSE2 means the CPU has
* integer vector math support.
*/
extern FXAPI FXuint fxCPUFeatures();

/**
* Return CPU Identification.  For example, on AMD processors this returns
* "AuthenticAMD".
* Name should be at least 16 bytes, plus 1 for end-of-string.
*/
extern FXAPI FXbool fxCPUName(FXchar name[]);

}

#endif
