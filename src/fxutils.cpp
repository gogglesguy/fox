/********************************************************************************
*                                                                               *
*                          U t i l i t y   F u n c t i o n s                    *
*                                                                               *
*********************************************************************************
* Copyright (C) 1998,2007 by Jeroen van der Zijp.   All Rights Reserved.        *
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
* $Id: fxutils.cpp,v 1.164 2007/07/09 16:27:26 fox Exp $                        *
********************************************************************************/
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "fxascii.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXString.h"
#include "FXElement.h"


/*
  Notes:
  - Handy global utility functions.
*/


// Make it larger if you need
#ifndef MAXMESSAGESIZE
#define MAXMESSAGESIZE 1024
#endif


using namespace FX;

// Allows GNU autoconfigure to find FOX
extern "C" FXAPI void fxfindfox(void){ }


/*******************************************************************************/

namespace FX {


// Furnish our own version
extern FXAPI FXint __snprintf(FXchar* string,FXint length,const FXchar* format,...);
extern FXAPI FXint __vsnprintf(FXchar* string,FXint length,const FXchar* format,va_list args);


// Global flag which controls tracing level
FXuint fxTraceLevel=0;


// Version number that the library has been compiled with
const FXuchar fxversion[3]={FOX_MAJOR,FOX_MINOR,FOX_LEVEL};


// Thread-safe, linear congruential random number generator from Knuth & Lewis.
FXuint fxrandom(FXuint& seed){
  seed=1664525UL*seed+1013904223UL;
  return seed;
  }


#ifdef WIN32

// Return true if console application
FXbool fxisconsole(const FXchar *path){
  IMAGE_OPTIONAL_HEADER optional_header;
  IMAGE_FILE_HEADER     file_header;
  IMAGE_DOS_HEADER      dos_header;
  DWORD                 dwCoffHeaderOffset;
  DWORD                 dwNewOffset;
  DWORD                 dwMoreDosHeader[16];
  ULONG                 ulNTSignature;
  HANDLE                hImage;
  DWORD                 dwBytes;
  FXbool                flag=false;     // Assume false on Windows is safest!

  // Open the application file.
  hImage=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
  if(hImage!=INVALID_HANDLE_VALUE){

    // Read MS-Dos image header.
    if(ReadFile(hImage,&dos_header,sizeof(IMAGE_DOS_HEADER),&dwBytes,NULL)==0) goto x;

    // Test bytes read
    if(dwBytes!=sizeof(IMAGE_DOS_HEADER)) goto x;

    // Test signature
    if(dos_header.e_magic!=IMAGE_DOS_SIGNATURE) goto x;

    // Read more MS-Dos header.
    if(ReadFile(hImage,dwMoreDosHeader,sizeof(dwMoreDosHeader),&dwBytes,NULL)==0) goto x;

    // Test bytes read
    if(dwBytes!=sizeof(dwMoreDosHeader)) goto x;

    // Move the file pointer to get the actual COFF header.
    dwNewOffset=SetFilePointer(hImage,dos_header.e_lfanew,NULL,FILE_BEGIN);
    dwCoffHeaderOffset=dwNewOffset+sizeof(ULONG);
    if(dwCoffHeaderOffset==0xFFFFFFFF) goto x;

    // Read NT signature of the file.
    if(ReadFile(hImage,&ulNTSignature,sizeof(ULONG),&dwBytes,NULL)==0) goto x;

    // Test bytes read
    if(dwBytes!=sizeof(ULONG)) goto x;

    // Test NT signature
    if(ulNTSignature!=IMAGE_NT_SIGNATURE) goto x;

    if(ReadFile(hImage,&file_header,IMAGE_SIZEOF_FILE_HEADER,&dwBytes,NULL)==0) goto x;

    // Test bytes read
    if(dwBytes!=IMAGE_SIZEOF_FILE_HEADER) goto x;

    // Read the optional header of file.
    if(ReadFile(hImage,&optional_header,IMAGE_SIZEOF_NT_OPTIONAL_HEADER,&dwBytes,NULL)==0) goto x;

    // Test bytes read
    if(dwBytes!=IMAGE_SIZEOF_NT_OPTIONAL_HEADER) goto x;

    // Switch on systems
    switch(optional_header.Subsystem){
      case IMAGE_SUBSYSTEM_WINDOWS_GUI:     // Windows GUI (2)
      case IMAGE_SUBSYSTEM_WINDOWS_CE_GUI:  // Windows CE GUI (9)
        flag=false;
        break;
      case IMAGE_SUBSYSTEM_WINDOWS_CUI:     // Windows Console (3)
      case IMAGE_SUBSYSTEM_OS2_CUI:         // OS/2 Console (5)
      case IMAGE_SUBSYSTEM_POSIX_CUI:       // Posix Console (7)
        flag=true;
        break;
      case IMAGE_SUBSYSTEM_NATIVE:          // Native (1)
      case IMAGE_SUBSYSTEM_NATIVE_WINDOWS:  // Native Win9x (8)
      case IMAGE_SUBSYSTEM_UNKNOWN:         // Unknown (0)
      default:
        flag=false;
        break;
      }
x:  CloseHandle(hImage);
    }
  return flag;
  }


#else

// Return true if console application
FXbool fxisconsole(const FXchar*){
  return true;
  }

#endif


// Log message to [typically] stderr
void fxmessage(const char* format,...){
  char msg[MAXMESSAGESIZE];
  va_list arguments;
  va_start(arguments,format);
  __vsnprintf(msg,sizeof(msg),format,arguments);
  va_end(arguments);
#ifdef WIN32
#ifdef _WINDOWS
  OutputDebugStringA(msg);
  fputs(msg,stderr);    // if a console is available
  fflush(stderr);
#else
  fputs(msg,stderr);
  fflush(stderr);
#endif
#else
  fputs(msg,stderr);
  fflush(stderr);
#endif
  }


// Assert failed routine
void fxassert(const char* expression,const char* filename,unsigned int lineno){
#ifdef WIN32
  fxmessage("%s(%d): FXASSERT(%s) failed.\n",filename,lineno,expression);
#else
  fxmessage("%s:%d: FXASSERT(%s) failed.\n",filename,lineno,expression);
#endif
  }


// Trace printout routine
void fxtrace(unsigned int level,const char* format,...){
  if(fxTraceLevel>level){
    char msg[MAXMESSAGESIZE];
    va_list arguments;
    va_start(arguments,format);
    __vsnprintf(msg,sizeof(msg),format,arguments);
    va_end(arguments);
#ifdef WIN32
#ifdef _WINDOWS
    OutputDebugStringA(msg);
    fputs(msg,stderr);    // if a console is available
    fflush(stderr);
    va_end(arguments);
#else
    fputs(msg,stderr);
    fflush(stderr);
#endif
#else
    fputs(msg,stderr);
    fflush(stderr);
#endif
    }
  }


// Error routine
void fxerror(const char* format,...){
  char msg[MAXMESSAGESIZE];
  va_list arguments;
  va_start(arguments,format);
  __vsnprintf(msg,sizeof(msg),format,arguments);
  va_end(arguments);
#ifdef WIN32
#ifdef _WINDOWS
  OutputDebugStringA(msg);
  fputs(msg,stderr);    // if a console is available
  fflush(stderr);
  MessageBoxA(NULL,msg,NULL,MB_OK|MB_ICONEXCLAMATION|MB_APPLMODAL);
  DebugBreak();
#else
  fputs(msg,stderr);
  fflush(stderr);
  abort();
#endif
#else
  fputs(msg,stderr);
  fflush(stderr);
  abort();
#endif
  }


// Warning routine
void fxwarning(const char* format,...){
  char msg[MAXMESSAGESIZE];
  va_list arguments;
  va_start(arguments,format);
  __vsnprintf(msg,sizeof(msg),format,arguments);
  va_end(arguments);
#ifdef WIN32
#ifdef _WINDOWS
  OutputDebugStringA(msg);
  fputs(msg,stderr);    // if a console is available
  fflush(stderr);
  MessageBoxA(NULL,msg,NULL,MB_OK|MB_ICONINFORMATION|MB_APPLMODAL);
#else
  fputs(msg,stderr);
  fflush(stderr);
#endif
#else
  fputs(msg,stderr);
  fflush(stderr);
#endif
  }


// Sleep n microseconds
void fxsleep(unsigned int n){
#ifdef WIN32
  unsigned int zzz=n/1000;
  if(zzz==0) zzz=1;
  Sleep(zzz);
#else
#ifdef __USE_POSIX199309
  struct timespec value;
  value.tv_nsec = 1000 * (n%1000000);
  value.tv_sec = n/1000000;
  nanosleep(&value,NULL);
#else
#ifndef BROKEN_SELECT
  struct timeval value;
  value.tv_usec = n % 1000000;
  value.tv_sec = n / 1000000;
  select(1,0,0,0,&value);
#else
  unsigned int zzz=n/1000000;
  if(zzz==0) zzz=1;
  if(zzz){
    while((zzz=sleep(zzz))>0) ;
    }
#endif
#endif
#endif
  }


// Get highlight color
FXColor makeHiliteColor(FXColor clr){
  FXuint r,g,b;
  r=FXREDVAL(clr);
  g=FXGREENVAL(clr);
  b=FXBLUEVAL(clr);
  r=FXMAX(31,r);
  g=FXMAX(31,g);
  b=FXMAX(31,b);
  r=(133*r)/100;
  g=(133*g)/100;
  b=(133*b)/100;
  r=FXMIN(255,r);
  g=FXMIN(255,g);
  b=FXMIN(255,b);
  return FXRGB(r,g,b);
  }


// Get shadow color
FXColor makeShadowColor(FXColor clr){
  FXuint r,g,b;
  r=FXREDVAL(clr);
  g=FXGREENVAL(clr);
  b=FXBLUEVAL(clr);
  r=(66*r)/100;
  g=(66*g)/100;
  b=(66*b)/100;
  return FXRGB(r,g,b);
  }


/*******************************************************************************/

// Convert string of length len to MSDOS; return new string and new length
FXbool fxtoDOS(FXchar*& string,FXint& len){
  register FXint f=0,t=0;
  while(f<len && string[f]!='\0'){
    if(string[f++]=='\n') t++; t++;
    }
  len=t;
  if(!resizeElms(string,len+1)) return false;
  while(0<t){
    if((string[--t]=string[--f])=='\n') string[--t]='\r';
    }
  string[len]='\0';
  return true;
  }


// Convert string of length len from MSDOS; return new string and new length
FXbool fxfromDOS(FXchar*& string,FXint& len){
  register FXint f=0,t=0,c;
  while(f<len && string[f]!='\0'){
    if((c=string[f++])!='\r') string[t++]=c;
    }
  len=t;
  if(!resizeElms(string,len+1)) return false;
  string[len]='\0';
  return true;
  }


// Get process id
FXint fxgetpid(){
#ifdef WIN32
  return (int)GetCurrentProcessId();
#else
  return getpid();
#endif
  }

extern FXAPI FILE *fxopen(const char *filename,const char *mode);

FILE *fxopen(const char *filename,const char *mode){
#if defined(WIN32) && defined(UNICODE)
  FXnchar unifile[1024];
  FXnchar unimode[1024];
  utf2ncs(unifile,filename,strlen(filename)+1);
  utf2ncs(unimode,mode,strlen(mode)+1);
  return _wfopen(unifile,unimode);
#else
  return fopen(filename,mode);
#endif
  }

extern FXAPI FILE *fxreopen(const char *filename,const char *mode,FILE * stream);

FILE *fxreopen(const char *filename,const char *mode,FILE * stream){
#if defined(WIN32) && defined(UNICODE)
  FXnchar unifile[1024];
  FXnchar unimode[1024];
  utf2ncs(unifile,filename,strlen(filename)+1);
  utf2ncs(unimode,mode,strlen(mode)+1);
  return _wfreopen(unifile,unimode,stream);
#else
  return freopen(filename,mode,stream);
#endif
  }


/*******************************************************************************/


// Convert RGB to HSV
void fxrgb_to_hsv(FXfloat& h,FXfloat& s,FXfloat& v,FXfloat r,FXfloat g,FXfloat b){
  FXfloat t,delta;
  v=FXMAX3(r,g,b);
  t=FXMIN3(r,g,b);
  delta=v-t;
  if(v!=0.0f)
    s=delta/v;
  else
    s=0.0f;
  if(s==0.0f){
    h=0.0f;
    }
  else{
    if(r==v)
      h=(g-b)/delta;
    else if(g==v)
      h=2.0f+(b-r)/delta;
    else if(b==v)
      h=4.0f+(r-g)/delta;
    h=h*60.0f;
    if(h<0.0f) h=h+360.0f;
    }
  }


// Convert to RGB
void fxhsv_to_rgb(FXfloat& r,FXfloat& g,FXfloat& b,FXfloat h,FXfloat s,FXfloat v){
  FXfloat f,w,q,t;
  FXint i;
  if(s==0.0f){
    r=v;
    g=v;
    b=v;
    }
  else{
    if(h==360.0f) h=0.0f;
    h=h/60.0f;
    i=(FXint)h;
    f=h-i;
    w=v*(1.0f-s);
    q=v*(1.0f-(s*f));
    t=v*(1.0f-(s*(1.0f-f)));
    switch(i){
      case 0: r=v; g=t; b=w; break;
      case 1: r=q; g=v; b=w; break;
      case 2: r=w; g=v; b=t; break;
      case 3: r=w; g=q; b=v; break;
      case 4: r=t; g=w; b=v; break;
      case 5: r=v; g=w; b=q; break;
      }
    }
  }


// Calculate a hash value from a string; algorithm same as in perl
FXuint fxstrhash(const FXchar* str){
  register const FXuchar *s=(const FXuchar*)str;
  register FXuint h=0;
  register FXuint c;
  while((c=*s++)!='\0'){
    h = ((h << 5) + h) ^ c;
    }
  return h;
  }


/*******************************************************************************/

// Single Precision IEEE 754 number layout
//            31 30           23 22             0
// +------------+---------------+---------------+
// | s[31] 1bit | e[30:23] 8bit | f[22:0] 23bit |
// +------------+---------------+---------------+


// Double Precision IEEE 754 number layout
//            63 62            52 51            32 31             0
// +------------+----------------+----------------+---------------+
// | s[63] 1bit | e[62:52] 11bit | f[51:32] 20bit | f[31:0] 32bit |
// +------------+----------------+----------------+---------------+


// Split a float or double into pieces
#if FOX_BIGENDIAN == 1
union FloatStruct { FXfloat f; struct { FXuint s:1; FXuint e:8; FXuint m:23; } n; };
union DoubleStruct { FXdouble d; struct { FXuint s:1; FXuint e:11; FXuint h:20; FXuint l:32; } n; };
#else
union FloatStruct { FXfloat f; struct { FXuint m:23; FXuint e:8; FXuint s:1; } n; };
union DoubleStruct { FXdouble d; struct { FXuint l:32; FXuint h:20; FXuint e:11; FXuint s:1; } n; };
#endif


// Float number classification: 0=OK, +/-1=Inf, +/-2=NaN
FXint fxieeefloatclass(FXfloat number){
  FloatStruct *fs=(FloatStruct*)&number;
  if(fs->n.e==255){
    if(fs->n.m==0) return fs->n.s ? -1 : 1;     // Inf
    return fs->n.s ? -2 : 2;                    // NaN
    }
  return 0;
  }


// Double number classification: 0=OK, +/-1=Inf, +/-2=NaN
FXint fxieeedoubleclass(FXdouble number){
  DoubleStruct *fs=(DoubleStruct*)&number;
  if(fs->n.e==2047){
    if(fs->n.l==0 && fs->n.h==0) return fs->n.s ? -1 : 1;       // Inf
    return fs->n.s ? -2 : 2;                                    // NaN
    }
  return 0;
  }


// Test for finite float
FXbool fxIsFinite(FXfloat number){
  return (((FloatStruct*)&number)->n.e!=255);
  }


// Test for finite double
FXbool fxIsFinite(FXdouble number){
  return (((DoubleStruct*)&number)->n.e!=2047);
  }


// Test for infinite float
FXbool fxIsInf(FXfloat number){
  return (((FloatStruct*)&number)->n.e==255) && (((FloatStruct*)&number)->n.m==0);
  }


// Test for infinite double
FXbool fxIsInf(FXdouble number){
  return (((DoubleStruct*)&number)->n.e==2047) && ((((DoubleStruct*)&number)->n.l==0) && (((DoubleStruct*)&number)->n.h==0));
  }


// Text for not-a-number float
FXbool fxIsNan(FXfloat number){
  return (((FloatStruct*)&number)->n.e==255) && (((FloatStruct*)&number)->n.m!=0);
  }


// Text for not-a-number double
FXbool fxIsNan(FXdouble number){
  return (((DoubleStruct*)&number)->n.e==2047) && !((((DoubleStruct*)&number)->n.l==0) && (((DoubleStruct*)&number)->n.h==0));
  }


// Fast integer power of 10
extern FXAPI FXdouble tenToThe(FXint e);

// Table of 1E+0,...1E+31, in steps of 1
static FXdouble posPowOfTen1[32]={1E+0,1E+1,1E+2,1E+3,1E+4,1E+5,1E+6,1E+7,1E+8,1E+9,1E+10,1E+11,1E+12,1E+13,1E+14,1E+15,1E+16,1E+17,1E+18,1E+19,1E+20,1E+21,1E+22,1E+23,1E+24,1E+25,1E+26,1E+27,1E+28,1E+29,1E+30,1E+31};

// Table of 1E+0,...1E+288, in steps of 32
static FXdouble posPowOfTen32[10]={1E+0,1E+32,1E+64,1E+96,1E+128,1E+160,1E+192,1E+224,1E+256,1E+288};

// Table of 1E-0,...1E-31, in steps of 1
static FXdouble negPowOfTen1[32]={1E-0,1E-1,1E-2,1E-3,1E-4,1E-5,1E-6,1E-7,1E-8,1E-9,1E-10,1E-11,1E-12,1E-13,1E-14,1E-15,1E-16,1E-17,1E-18,1E-19,1E-20,1E-21,1E-22,1E-23,1E-24,1E-25,1E-26,1E-27,1E-28,1E-29,1E-30,1E-31};

// Table of 1E-0,...1E-288, in steps of 32
static FXdouble negPowOfTen32[10]={1E-0,1E-32,1E-64,1E-96,1E-128,1E-160,1E-192,1E-224,1E-256,1E-288};


// Fast integer power of 10; this is based on the mathematical
// identity 10^(a+b) = 10^a * 10^b.  We could also use a really large
// table of 308 entries, but that would take a lot of space...
// The exponent should be in the range -308 to 308, these being the limits
// of double precision IEEE754 standard floating point.
FXdouble tenToThe(FXint e){
  return e<0 ? negPowOfTen1[-e&31]*negPowOfTen32[-e>>5] : posPowOfTen1[e&31]*posPowOfTen32[e>>5];
  }


/*******************************************************************************/

#if defined(__GNUC__) && defined(__linux__) && defined(__i386__)

extern FXAPI FXuint fxcpuid();

// Capabilities
#define CPU_HAS_TSC             0x001
#define CPU_HAS_MMX             0x002
#define CPU_HAS_MMXEX           0x004
#define CPU_HAS_SSE             0x008
#define CPU_HAS_SSE2            0x010
#define CPU_HAS_3DNOW           0x020
#define CPU_HAS_3DNOWEXT        0x040
#define CPU_HAS_SSE3            0x080
#define CPU_HAS_HT              0x100


// The CPUID instruction returns stuff in eax, ecx, edx.
#define cpuid(op,eax,ecx,edx)	\
  asm volatile ("pushl %%ebx \n\t"    	\
                "cpuid       \n\t"    	\
                "popl  %%ebx \n\t"      \
                : "=a" (eax),		\
                  "=c" (ecx),           \
                  "=d" (edx)            \
                : "a" (op)              \
                : "cc")


/*
* Find out some useful stuff about the CPU we're running on.
* We don't care about everything, but just MMX, XMMS, SSE, SSE2, 3DNOW, 3DNOWEXT,
* for the obvious reasons.
* If we're generating for Pentium or above then assume CPUID is present; otherwise,
* test if CPUID is present first using the recommended code...
*/
FXuint fxcpuid(){
  FXuint eax, ecx, edx, caps;

  // Generating code for pentium or better :- don't bother checking for CPUID presence.
#if !(defined(__i586__) || defined(__i686__) || defined(__athlon__) || defined(__pentium4__) || defined(__x86_64__))

  // If EFLAGS bit 21 can be changed, we have CPUID capability.
  asm volatile ("pushfl                 \n\t"
                "popl   %0              \n\t"
                "movl   %0,%1           \n\t"
                "xorl   $0x200000,%0    \n\t"
                "pushl  %0              \n\t"
                "popfl                  \n\t"
                "pushfl                 \n\t"
                "popl   %0              \n\t"
                : "=a" (eax),
                  "=d" (edx)
                :
                : "cc");

  // Yes, we have no CPUID!
  if(eax==edx) return 0;
#endif

  // Capabilities
  caps=0;

  // Get vendor string; this also returns the highest CPUID code in eax.
  // If highest CPUID code is zero, we can't call any other CPUID functions.
  cpuid(0x00000000,eax,ecx,edx);
  if(eax){

    // AMD:   ebx="Auth" edx="enti" ecx="cAMD",
    // Intel: ebx="Genu" edx="ineI" ecx="ntel"
    // VIAC3: ebx="Cent" edx="aurH" ecx="auls"

    // Test for AMD
    if((ecx==0x444d4163) && (edx==0x69746e65)){

      // Any extended capabilities; this returns highest extended CPUID code in eax.
      cpuid(0x80000000,eax,ecx,edx);
      if(eax>0x80000000){

        // Test extended athlon capabilities
        cpuid(0x80000001,eax,ecx,edx);
        if(edx&0x08000000) caps|=CPU_HAS_MMXEX;
        if(edx&0x80000000) caps|=CPU_HAS_3DNOW;
        if(edx&0x40000000) caps|=CPU_HAS_3DNOWEXT;
        }
      }

    // Standard CPUID code 1.
    cpuid(0x00000001,eax,ecx,edx);
    if(edx&0x00000010) caps|=CPU_HAS_TSC;
    if(edx&0x00800000) caps|=CPU_HAS_MMX;
    if(edx&0x02000000) caps|=CPU_HAS_SSE;
    if(edx&0x04000000) caps|=CPU_HAS_SSE2;
    if(edx&0x10000000) caps|=CPU_HAS_HT;
    if(ecx&0x00000001) caps|=CPU_HAS_SSE3;
    }

  // Return capabilities
  return caps;
  }
#endif

}

