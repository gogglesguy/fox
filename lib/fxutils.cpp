/********************************************************************************
*                                                                               *
*                          U t i l i t y   F u n c t i o n s                    *
*                                                                               *
*********************************************************************************
* Copyright (C) 1998,2010 by Jeroen van der Zijp.   All Rights Reserved.        *
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


// Global flag which controls tracing level.
// Values can be:
//  -1: value not set, no tracing;
//   0: no tracing;
//   N: tracing enabled for statement levels 0...N-1
FXint fxTraceLevel=-1;


// Version number that the library has been compiled with
const FXuchar fxversion[3]={FOX_MAJOR,FOX_MINOR,FOX_LEVEL};


// Thread-safe, xor-shifting random number generator (initial seed value must
// be non-zero.  A suggested seed value is 2463534242.
FXuint fxrandom(FXuint& seed){
  seed^=(seed<<13);
  seed^=(seed>>17);
  seed^=(seed<<5);
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
    if(ReadFile(hImage,&optional_header,sizeof(IMAGE_OPTIONAL_HEADER),&dwBytes,NULL)==0) goto x;

    // Test bytes read
    if(dwBytes!=sizeof(IMAGE_OPTIONAL_HEADER)) goto x;

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
void fxmessage(const FXchar* format,...){
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
void fxassert(const FXchar* expression,const FXchar* filename,unsigned int lineno){
#ifdef WIN32
  fxmessage("%s(%d): FXASSERT(%s) failed.\n",filename,lineno,expression);
#else
//  if(isatty(fileno(stderr))){
//    fxmessage("\033[1;31m%s:%d:\033[0m FXASSERT(%s) failed.\n",filename,lineno,expression);
//    }
//  else{
    fxmessage("%s:%d: FXASSERT(%s) failed.\n",filename,lineno,expression);
//    }
#endif
  }


// Verify failed routine
void fxverify(const FXchar* expression,const FXchar* filename,unsigned int lineno){
#ifdef WIN32
  fxmessage("%s(%d): FXVERIFY(%s) failed.\n",filename,lineno,expression);
#else
//  if(isatty(fileno(stderr))){
//    fxmessage("\033[1;31m%s:%d:\033[0m FXVERIFY(%s) failed.\n",filename,lineno,expression);
//    }
//  else{
    fxmessage("%s:%d: FXVERIFY(%s) failed.\n",filename,lineno,expression);
//    }
#endif
  }


// Trace printout routine
void fxtrace(FXint level,const FXchar* format,...){
  if(fxTraceLevel<0){
    const FXchar* str;
    fxTraceLevel=0;
    if((str=getenv("FOX_TRACE_LEVEL"))!=NULL){
      fxTraceLevel=strtoul(str,NULL,10);
      }
    }
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
void fxerror(const FXchar* format,...){
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
void fxwarning(const FXchar* format,...){
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
void fxsleep(FXuint n){
#ifdef WIN32
  unsigned int zzz=n/1000;
  if(zzz==0) zzz=1;
  Sleep(zzz);
#else
#if  (_POSIX_C_SOURCE >= 199309L)
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
  FXnchar unifile[MAXPATHLEN],unimode[8];
  utf2ncs(unifile,MAXPATHLEN,filename,strlen(filename)+1);
  utf2ncs(unimode,8,mode,strlen(mode)+1);
  return _wfopen(unifile,unimode);
#else
  return fopen(filename,mode);
#endif
  }

extern FXAPI FILE *fxreopen(const char *filename,const char *mode,FILE * stream);

FILE *fxreopen(const char *filename,const char *mode,FILE * stream){
#if defined(WIN32) && defined(UNICODE)
  FXnchar unifile[MAXPATHLEN],unimode[8];
  utf2ncs(unifile,MAXPATHLEN,filename,strlen(filename)+1);
  utf2ncs(unimode,8,mode,strlen(mode)+1);
  return _wfreopen(unifile,unimode,stream);
#else
  return freopen(filename,mode,stream);
#endif
  }


/*******************************************************************************/


// Convert RGB to HSV
void fxrgb_to_hsv(FXfloat& h,FXfloat& s,FXfloat& v,FXfloat r,FXfloat g,FXfloat b){
  register FXfloat mx=FXMAX3(r,g,b);
  register FXfloat mn=FXMIN3(r,g,b);
  register FXfloat d=mx-mn;
  h=0.0f;
  s=0.0f;
  v=mx;
  if(__likely(mx>0.0f)){
    s=d/mx;
    if(__likely(s>0.0f)){
      if(r==mx) h=(g-b)/d;
      else if(g==mx) h=2.0f+(b-r)/d;
      else if(b==mx) h=4.0f+(r-g)/d;
      h*=60.0f;
      if(h<0.0f) h+=360.0f;
      }
    }
  }


// Convert HSV to RGB
void fxhsv_to_rgb(FXfloat& r,FXfloat& g,FXfloat& b,FXfloat h,FXfloat s,FXfloat v){
  register FXfloat f,w,q,t;
  register FXint i;
  r=g=b=v;
  if(__likely(s>0.0f)){
    if(h==360.0f) h=0.0f;
    else h=h/60.0f;
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


// Convert RGB to HSL
void fxrgb_to_hsl(FXfloat& h,FXfloat& s,FXfloat& l,FXfloat r,FXfloat g,FXfloat b){
  register FXfloat mx=FXMAX3(r,g,b);
  register FXfloat mn=FXMIN3(r,g,b);
  register FXfloat d=mx-mn;
  h=0.0f;
  s=0.0f;
  l=(mx+mn)*0.5f;
  if(__likely(d>0.0f)){
    if(l<=0.5f)
      s=d/(mx+mn);
    else
      s=d/(2.0f-mx-mn);
    if(r==mx) h=(g-b)/d;
    else if(g==mx) h=2.0f+(b-r)/d;
    else if(b==mx) h=4.0f+(r-g)/d;
    h*=60.0f;
    if(h<0.0f) h+=360.0f;
    }
  }


// Convert HSL to RGB
void fxhsl_to_rgb(FXfloat& r,FXfloat& g,FXfloat& b,FXfloat h,FXfloat s,FXfloat l){
  register FXfloat f,v,min,mid1,mid2,sv,vsf;
  register FXint i;
  r=g=b=l;
  if(l<0.5f)
    v=l+l*s;
  else
    v=l+s-l*s;
  if(v<=0.0f){
    r=g=b=0.0f;
    }
  else{
    if(h==360.0f) h=0.0f;
    else h=h/60.0f;
    min=2.0f*l-v;
    sv=(v-min)/v;
    i=(FXint)h;
    f=h-i;
    vsf=v*sv*f;
    mid1=min+vsf;
    mid2=v-vsf;
    switch(i){
      case 0: r=v; g=mid1; b=min; break;
      case 1: r=mid2; g=v; b=min; break;
      case 2: r=min; g=v; b=mid1; break;
      case 3: r=min; g=mid2; b=v; break;
      case 4: r=mid1; g=min; b=v; break;
      case 5: r=v; g=min; b=mid2; break;
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

#if defined(__GNUC__) && defined(__linux__) && defined(__x86_64__)

// MXCSR controls SSE(2) operation:
//
// +----+-------+----+----+----+----+----+----+----+----+----+----+----+----+----+
// | FZ |   RC  | PM | UM | OM | ZM | DM | IM | DAZ| PE | UE | OE | ZE | DE | IE |
// +----+-------+----+----+----+----+----+----+----+----+----+----+----+----+----+
// | 15 | 14 13 | 12 | 11 | 10 |  9 |  8 |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |
// +----+-------+----+----+----+----+----+----+----+----+----+----+----+----+----+
//
// FZ  Flush to zero
// RC  Round control (00=nearest, 01=down, 10=up, 11=toward zero)
// PM  Precision exception mask
// UM  Underflow exception mask
// OM  Overflow exception mask
// ZM  Zero divide exception mask
// DM  Denormalized operand exception mask
// IM  Invalid operation exception mask
// DAZ Denormals are zeros
// PE  Precision exception
// UE  Underflow exception
// OE  Overflow exception
// ZE  Zero-divide exception
// DE  Denormalized operand exception
// IE  Invalid operation exception

extern FXAPI FXuint fxgetmxcsr();
extern FXAPI void fxsetmxcsr(FXuint mxcsr);


// Set value to MXCSR control register
void fxsetmxcsr(unsigned int mxcsr){
  __asm("ldmxcsr %0" : : "m"(*&mxcsr));
  }


// Get current value of MXCSR control register
unsigned int fxgetmxcsr(){
  unsigned int mxcsr;
  __asm("stmxcsr %0" : "=m"(*&mxcsr));
  return mxcsr;
  }

#endif


}

