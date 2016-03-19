/********************************************************************************
*                                                                               *
*                     FOX Definitions, Types, and Macros                        *
*                                                                               *
*********************************************************************************
* Copyright (C) 1997,2011 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#ifndef FXDEFS_H
#define FXDEFS_H


/********************************  Definitions  ********************************/

// Truth values
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef MAYBE
#define MAYBE 2
#endif
#ifndef NULL
#define NULL 0
#endif

// Path separator
#ifdef WIN32
#define PATHSEP '\\'
#define PATHSEPSTRING "\\"
#define PATHLISTSEP ';'
#define PATHLISTSEPSTRING ";"
#define ISPATHSEP(c) ((c)=='\\' || (c)=='/')
#else
#define PATHSEP '/'
#define PATHSEPSTRING "/"
#define PATHLISTSEP ':'
#define PATHLISTSEPSTRING ":"
#define ISPATHSEP(c) ((c)=='/')
#endif

// End Of Line
#ifdef WIN32
#define ENDLINE "\r\n"
#else
#define ENDLINE "\n"
#endif


// For Windows
#ifdef _DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#ifdef _NDEBUG
#ifndef NDEBUG
#define NDEBUG
#endif
#endif


// Shared library support
#ifdef WIN32
#define FXLOCAL
#define FXEXPORT __declspec(dllexport)
#define FXIMPORT __declspec(dllimport)
#else
#if defined(__GNUC__) && (__GNUC__ >= 4)
#define FXLOCAL  __attribute__((visibility("hidden")))
#define FXEXPORT __attribute__((visibility("default")))
#define FXIMPORT
#else
#define FXLOCAL
#define FXEXPORT
#define FXIMPORT
#endif
#endif


// Define FXAPI for DLL builds
#ifdef FOXDLL
#ifdef FOXDLL_EXPORTS
#define FXAPI FXEXPORT
#define FXTEMPLATE_EXTERN
#else
#define FXAPI FXIMPORT
#define FXTEMPLATE_EXTERN extern
#endif
#else
#define FXAPI
#define FXTEMPLATE_EXTERN
#endif


// Data alignment attribute
#if defined(__GNUC__)
#define __align(x)    __attribute__((aligned(x)))
#elif defined(_MSC_VER)
#define __align(x)    __declspec(align(x))
#else
#define __align(x)
#endif

// Thread-local storage attribute
#if defined(__GNUC__)
#define __threadlocal   __thread
#elif defined(_MSC_VER)
#define __threadlocal   __declspec(thread)
#else
#define __threadlocal
#endif

// Branch prediction optimization
#if __GNUC__ >= 3
#define __likely(cond)    __builtin_expect(!!(cond),1)
#define __unlikely(cond)  __builtin_expect(!!(cond),0)
#else
#define __likely(cond)    (!!(cond))
#define __unlikely(cond)  (!!(cond))
#endif

// Callback
#ifdef WIN32
#ifndef CALLBACK
#define CALLBACK __stdcall
#endif
#endif


// Disable some warnings in VC++
#ifdef _MSC_VER
#pragma warning(disable: 4251)
#pragma warning(disable: 4231)
#pragma warning(disable: 4244)
#endif


// Checking printf and scanf format strings
#if defined(_CC_GNU_) || defined(__GNUG__) || defined(__GNUC__)
#define FX_PRINTF(fmt,arg) __attribute__((format(printf,fmt,arg)))
#define FX_SCANF(fmt,arg)  __attribute__((format(scanf,fmt,arg)))
#define FX_FORMAT(arg) __attribute__((format_arg(arg)))
#else
#define FX_PRINTF(fmt,arg)
#define FX_SCANF(fmt,arg)
#define FX_FORMAT(arg)
#endif

// Raw event type
#ifdef WIN32
struct tagMSG;
#else
union _XEvent;
#endif


namespace FX {


/// Exponent display
enum FXExponent {
  EXP_NEVER=FALSE,                  /// Never use exponential notation
  EXP_ALWAYS=TRUE,                  /// Always use exponential notation
  EXP_AUTO=MAYBE                    /// Use exponential notation if needed
  };


/// Search modes for search/replace dialogs
enum {
  SEARCH_FORWARD      = 0,    /// Search forward (default)
  SEARCH_BACKWARD     = 1,    /// Search backward
  SEARCH_NOWRAP       = 0,    /// Don't wrap (default)
  SEARCH_WRAP         = 2,    /// Wrap around to start
  SEARCH_EXACT        = 0,    /// Exact match (default)
  SEARCH_IGNORECASE   = 4,    /// Ignore case
  SEARCH_REGEX        = 8,    /// Regular expression match
  SEARCH_PREFIX       = 16    /// Prefix of subject string
  };


/*********************************  Typedefs  **********************************/

// Forward declarations
class                          FXObject;
class                          FXStream;
class                          FXString;


// Streamable types; these are fixed size!
typedef char                   FXchar;
typedef unsigned char          FXuchar;
typedef bool                   FXbool;
typedef unsigned short         FXushort;
typedef short                  FXshort;
typedef unsigned int           FXuint;
typedef int                    FXint;
typedef float                  FXfloat;
typedef double                 FXdouble;
#if defined(WIN32)
typedef unsigned int           FXwchar;
#if defined(_MSC_VER) && !defined(_NATIVE_WCHAR_T_DEFINED)
typedef unsigned short         FXnchar;
#elif defined(__WATCOMC__) && !defined(_WCHAR_T_DEFINED)
typedef long char              FXnchar;
#else
typedef wchar_t                FXnchar;
#endif
#else
typedef wchar_t                FXwchar;
typedef unsigned short         FXnchar;
#endif
#if defined(__LP64__) || defined(_LP64) || (_MIPS_SZLONG == 64) || (__WORDSIZE == 64)
typedef unsigned long          FXulong;
typedef long                   FXlong;
#elif defined(_MSC_VER) || (defined(__BCPLUSPLUS__) && __BORLANDC__ > 0x500) || defined(__WATCOM_INT64__)
typedef unsigned __int64       FXulong;
typedef __int64                FXlong;
#elif defined(__GNUG__) || defined(__GNUC__) || defined(__SUNPRO_CC) || defined(__MWERKS__) || defined(__SC__) || defined(_LONGLONG)
typedef unsigned long long     FXulong;
typedef long long              FXlong;
#else
#error "FXlong and FXulong not defined for this architecture!"
#endif


// Integral types large enough to hold value of a pointer
#if defined(_MSC_VER) && defined(_WIN64)
typedef __int64                FXival;
typedef unsigned __int64       FXuval;
#else
typedef long                   FXival;
typedef unsigned long          FXuval;
#endif


// Suffixes for 64-bit long constants
#if defined(WIN32) && !defined(__GNUC__)
#define FXLONG(c)  c ## i64
#define FXULONG(c) c ## ui64
#else
#define FXLONG(c)  c ## LL
#define FXULONG(c) c ## ULL
#endif


// Handle to something in server
#ifdef WIN32
typedef void*                  FXID;
#else
typedef unsigned long          FXID;
#endif

// Time since January 1, 1970 (UTC)
typedef FXlong                 FXTime;

// Pixel type (could be color index)
typedef unsigned long          FXPixel;

// RGBA pixel value
typedef FXuint                 FXColor;

// Hot key
typedef FXuint                 FXHotKey;

// Input source handle type
#ifdef WIN32
typedef void*                  FXInputHandle;
#else
typedef FXint                  FXInputHandle;
#endif

// Thread ID type
#if defined(WIN32)
typedef void*                  FXThreadID;
#else
typedef unsigned long          FXThreadID;
#endif

// Thread-local storage key
typedef FXuval                 FXThreadStorageKey;

// Raw event type
#ifdef WIN32
typedef tagMSG    FXRawEvent;
#else
typedef _XEvent   FXRawEvent;
#endif


/// Drag and drop data type
#ifdef WIN32
typedef FXushort  FXDragType;
#else
typedef FXID      FXDragType;
#endif


/// Pi
const FXdouble PI=3.1415926535897932384626433833;

/// Euler constant
const FXdouble EULER=2.7182818284590452353602874713;

/// Multiplier for degrees to radians
const FXdouble DTOR=0.0174532925199432957692369077;

/// Multiplier for radians to degrees
const FXdouble RTOD=57.295779513082320876798154814;

/// A time in the far, far future
const FXTime forever=FXLONG(9223372036854775807);


/**********************************  Macros  ***********************************/

/// Abolute value
#define FXABS(val)         (((val)>=0)?(val):-(val))

/// Return 1 if val >= 0 and -1 otherwise
#define FXSGN(val)         (((val)<0)?-1:1)

/// Return 1 if val > 0, -1 if val < 0, and 0 otherwise
#define FXSGNZ(val)        ((val)<0?-1:(val)>0?1:0)

/// Return the maximum of a or b
#define FXMAX(a,b)         (((a)>(b))?(a):(b))

/// Return the minimum of a or b
#define FXMIN(a,b)         (((a)>(b))?(b):(a))

/// Return the minimum of x, y and z
#define FXMIN3(x,y,z)      ((x)<(y)?FXMIN(x,z):FXMIN(y,z))

/// Return the maximum of x, y and z
#define FXMAX3(x,y,z)      ((x)>(y)?FXMAX(x,z):FXMAX(y,z))

/// Return the minimum of x, y, z, and w
#define FXMIN4(x,y,z,w)    (FXMIN(FXMIN(x,y),FXMIN(z,w)))

/// Return the maximum of of x, y, z, and w
#define FXMAX4(x,y,z,w)    (FXMAX(FXMAX(x,y),FXMAX(z,w)))

/// Return minimum and maximum of a, b
#define FXMINMAX(lo,hi,a,b) ((a)<(b)?((lo)=(a),(hi)=(b)):((lo)=(b),(hi)=(a)))

/// Clamp value x to range [lo..hi]
#define FXCLAMP(lo,x,hi)   ((x)<(lo)?(lo):((x)>(hi)?(hi):(x)))

/// Swap a pair of numbers
#define FXSWAP(a,b,t)      ((t)=(a),(a)=(b),(b)=(t))

/// Linear interpolation between a and b, where 0<=f<=1
#define FXLERP(a,b,f)      ((a)+((b)-(a))*(f))

/// Offset of member in a structure
#define STRUCTOFFSET(str,member) (((char *)(&(((str *)0)->member)))-((char *)0))

/// Number of elements in a static array
#define ARRAYNUMBER(array) (sizeof(array)/sizeof(array[0]))

/// Container class of a member class
#define CONTAINER(ptr,str,mem) ((str*)(((char*)(ptr))-STRUCTOFFSET(str,mem)))

/// Make int out of two shorts
#define MKUINT(l,h)        ((((FX::FXuint)(l))&0xffff) | (((FX::FXuint)(h))<<16))

/// Make selector from message type and message id
#define FXSEL(type,id)     ((((FX::FXuint)(id))&0xffff) | (((FX::FXuint)(type))<<16))

/// Get type from selector
#define FXSELTYPE(s)       ((FX::FXushort)(((s)>>16)&0xffff))

/// Get ID from selector
#define FXSELID(s)         ((FX::FXushort)((s)&0xffff))

/// Test if character c is at the start of a utf8 sequence (not a follower byte)
#define FXISUTF8(c)        (((c)&0xC0)!=0x80)

/// Check if c is leader/follower of a utf8 multi-byte sequence
#define FXISLEADUTF8(c)    (((c)&0xC0)==0xC0)
#define FXISFOLLOWUTF8(c)  (((c)&0xC0)==0x80)

/// Check if c is part of a utf8 multi-byte sequence
#define FXISSEQUTF8(c)     (((c)&0x80)==0x80)

/// Test if character c is at start of utf16 sequence (not a follower from surrogate pair)
#define FXISUTF16(c)       (((c)&0xFC00)!=0xDC00)

/// Check if c is leader/follower of a utf16 surrogate pair sequence
#define FXISLEADUTF16(c)   (((c)&0xFC00)==0xD800)
#define FXISFOLLOWUTF16(c) (((c)&0xFC00)==0xDC00)

/// Check if c is part of a utf16 surrogate pair sequence
#define FXISSEQUTF16(c)    (((c)&0xF800)==0xD800)

/// Test if c is a legal utf32 character
#define FXISUTF32(c)       ((c)<0x110000)

/// Average of two FXColor ca and FXColor cb
#define FXAVGCOLOR(ca,cb)  (((ca)&(cb))+((((ca)^(cb))&0xFEFEFEFE)>>1))


// Definitions for big-endian machines
#if FOX_BIGENDIAN == 1

/// Make RGBA color
#define FXRGBA(r,g,b,a)    (((FX::FXuint)(FX::FXuchar)(a)) | ((FX::FXuint)(FX::FXuchar)(r)<<8) | ((FX::FXuint)(FX::FXuchar)(g)<<16) | ((FX::FXuint)(FX::FXuchar)(b)<<24))

/// Make RGB color
#define FXRGB(r,g,b)       (((FX::FXuint)(FX::FXuchar)(r)<<8) | ((FX::FXuint)(FX::FXuchar)(g)<<16) | ((FX::FXuint)(FX::FXuchar)(b)<<24) | 0x000000ff)

/// Get red value from RGBA color
#define FXREDVAL(rgba)     ((FX::FXuchar)(((rgba)>>8)&0xff))

/// Get green value from RGBA color
#define FXGREENVAL(rgba)   ((FX::FXuchar)(((rgba)>>16)&0xff))

/// Get blue value from RGBA color
#define FXBLUEVAL(rgba)    ((FX::FXuchar)(((rgba)>>24)&0xff))

/// Get alpha value from RGBA color
#define FXALPHAVAL(rgba)   ((FX::FXuchar)((rgba)&0xff))

/// Get component value of RGBA color
#define FXRGBACOMPVAL(rgba,comp) ((FX::FXuchar)(((rgba)>>((comp)<<3))&0xff))

#endif


// Definitions for little-endian machines
#if FOX_BIGENDIAN == 0

/// Make RGBA color
#define FXRGBA(r,g,b,a)    (((FX::FXuint)(FX::FXuchar)(a)<<24) | ((FX::FXuint)(FX::FXuchar)(r)<<16) | ((FX::FXuint)(FX::FXuchar)(g)<<8) | ((FX::FXuint)(FX::FXuchar)(b)))

/// Make RGB color
#define FXRGB(r,g,b)       (((FX::FXuint)(FX::FXuchar)(r)<<16) | ((FX::FXuint)(FX::FXuchar)(g)<<8) | ((FX::FXuint)(FX::FXuchar)(b)) | 0xff000000)

/// Get red value from RGBA color
#define FXREDVAL(rgba)     ((FX::FXuchar)(((rgba)>>16)&0xff))

/// Get green value from RGBA color
#define FXGREENVAL(rgba)   ((FX::FXuchar)(((rgba)>>8)&0xff))

/// Get blue value from RGBA color
#define FXBLUEVAL(rgba)    ((FX::FXuchar)((rgba)&0xff))

/// Get alpha value from RGBA color
#define FXALPHAVAL(rgba)   ((FX::FXuchar)(((rgba)>>24)&0xff))

/// Get component value of RGBA color
#define FXRGBACOMPVAL(rgba,comp) ((FX::FXuchar)(((rgba)>>((3-(comp))<<3))&0xff))

#endif


/**
* FXASSERT() prints out a message when the expression fails,
* and nothing otherwise.  Unlike assert(), FXASSERT() will not
* terminate the execution of the application.
* When compiling your application for release, all assertions
* are compiled out; thus there is no impact on execution speed.
*/
#ifndef NDEBUG
#define FXASSERT(exp) (__likely(exp)?((void)0):(void)FX::fxassert(#exp,__FILE__,__LINE__))
#else
#define FXASSERT(exp) ((void)0)
#endif


/**
* FXVERIFY prints out a message when the expression fails,
* and nothing otherwise.
* When compiling your application for release, these messages
* are compiled out, but unlike FXASSERT, FXVERIFY will still execute
* the expression.
*/
#ifndef NDEBUG
#define FXVERIFY(exp) (__likely(exp)?((void)0):(void)FX::fxverify(#exp,__FILE__,__LINE__))
#else
#define FXVERIFY(exp) ((void)(exp))
#endif


/**
* FXTRACE() allows you to trace the execution of your application
* with increasing levels of detail the higher the trace level.
* The trace level is determined by variable fxTraceLevel, which
* may be set from the command line with "-tracelevel <level>".
* When compiling your application for release, all trace statements
* are compiled out, just like FXASSERT.
* A statement like: FXTRACE((10,"The value of x=%d\n",x)) will
* generate output only if fxTraceLevel is set to 11 or greater.
* The default value fxTraceLevel=0 will block all trace outputs.
* Note the double parentheses!
*/
#ifndef NDEBUG
#define FXTRACE(arguments) FX::fxtrace arguments
#else
#define FXTRACE(arguments) ((void)0)
#endif

/**
* Allocate a memory block of no elements of type and store a pointer
* to it into the address pointed to by ptr.
* Return false if size!=0 and allocation fails, true otherwise.
* An allocation of a zero size block returns a NULL pointer.
*/
#define FXMALLOC(ptr,type,no)     (FX::fxmalloc((void **)(ptr),sizeof(type)*(no)))

/**
* Allocate a zero-filled memory block no elements of type and store a pointer
* to it into the address pointed to by ptr.
* Return false if size!=0 and allocation fails, true otherwise.
* An allocation of a zero size block returns a NULL pointer.
*/
#define FXCALLOC(ptr,type,no)     (FX::fxcalloc((void **)(ptr),sizeof(type)*(no)))

/**
* Resize the memory block referred to by the pointer at the address ptr, to a
* hold no elements of type.
* Returns false if size!=0 and reallocation fails, true otherwise.
* If reallocation fails, pointer is left to point to old block; a reallocation
* to a zero size block has the effect of freeing it.
* The ptr argument must be the address where the pointer to the allocated
* block is to be stored.
*/
#define FXRESIZE(ptr,type,no)     (FX::fxresize((void **)(ptr),sizeof(type)*(no)))

/**
* Allocate and initialize memory from another block.
* Return false if size!=0 and source!=NULL and allocation fails, true otherwise.
* An allocation of a zero size block returns a NULL pointer.
* The ptr argument must be the address where the pointer to the allocated
* block is to be stored.
*/
#define FXMEMDUP(ptr,src,type,no) (FX::fxmemdup((void **)(ptr),(const void*)(src),sizeof(type)*(no)))

/**
* Free a block of memory allocated with either FXMALLOC, FXCALLOC, FXRESIZE, or FXMEMDUP.
* It is OK to call free a NULL pointer.  The argument must be the address of the
* pointer to the block to be released.  The pointer is set to NULL to prevent
* any further references to the block after releasing it.
*/
#define FXFREE(ptr)               (FX::fxfree((void **)(ptr)))


/**
* These are some of the ISO C99 standard single-precision transcendental functions.
* On LINUX, specify _GNU_SOURCE or _ISOC99_SOURCE to enable native implementation;
* otherwise, these macros will be used.  Apple OS-X implements fabsf(x), ceilf(x),
* floorf(x), and fmodf(x,y).
* Define FLOAT_MATH_FUNCTIONS if these functions are available in some other
* library you're linking to.
*/
#ifdef __OpenBSD__
#define FLOAT_MATH_FUNCTIONS
#endif
#ifndef FLOAT_MATH_FUNCTIONS
#ifndef __USE_ISOC99
#ifndef __APPLE__
#define fabsf(x)    ((float)fabs((double)(x)))
#define ceilf(x)    ((float)ceil((double)(x)))
#define floorf(x)   ((float)floor((double)(x)))
#define fmodf(x,y)  ((float)fmod((double)(x),(double)(y)))
#endif
#define sqrtf(x)    ((float)sqrt((double)(x)))
#define sinf(x)     ((float)sin((double)(x)))
#define cosf(x)     ((float)cos((double)(x)))
#define tanf(x)     ((float)tan((double)(x)))
#define asinf(x)    ((float)asin((double)(x)))
#define acosf(x)    ((float)acos((double)(x)))
#define atanf(x)    ((float)atan((double)(x)))
#define atan2f(y,x) ((float)atan2((double)(y),(double)(x)))
#define powf(x,y)   ((float)pow((double)(x),(double)(y)))
#define expf(x)     ((float)exp((double)(x)))
#define logf(x)     ((float)log((double)(x)))
#define log10f(x)   ((float)log10((double)(x)))
#endif
#endif


/**********************************  Globals  **********************************/

/// Simple, thread-safe xor-shifting random number generator (initial seed should be non-zero)
extern FXAPI FXuint fxrandom(FXuint& seed);

/// Allocate memory
extern FXAPI FXbool fxmalloc(void** ptr,unsigned long size);

/// Allocate cleaned memory
extern FXAPI FXbool fxcalloc(void** ptr,unsigned long size);

/// Resize memory
extern FXAPI FXbool fxresize(void** ptr,unsigned long size);

/// Duplicate memory
extern FXAPI FXbool fxmemdup(void** ptr,const void* src,unsigned long size);

/// Free memory, resets ptr to NULL afterward
extern FXAPI void fxfree(void** ptr);

/// Error routine
extern FXAPI void fxerror(const FXchar* format,...) FX_PRINTF(1,2) ;

/// Warning routine
extern FXAPI void fxwarning(const FXchar* format,...) FX_PRINTF(1,2) ;

/// Log message to [typically] stderr
extern FXAPI void fxmessage(const FXchar* format,...) FX_PRINTF(1,2) ;

/// Assert failed routine:- usually not called directly but called through FXASSERT
extern FXAPI void fxassert(const FXchar* expression,const FXchar* filename,unsigned int lineno);

/// Verify failed routine:- usually not called directly but called through FXVERIFY
extern FXAPI void fxverify(const FXchar* expression,const FXchar* filename,unsigned int lineno);

/// Trace printout routine:- usually not called directly but called through FXTRACE
extern FXAPI void fxtrace(FXint level,const FXchar* format,...) FX_PRINTF(2,3) ;

/// Sleep n microseconds
extern FXAPI void fxsleep(FXuint n);

/// Convert string of length len to MSDOS; return new string and new length
extern FXAPI FXbool fxtoDOS(FXchar*& string,FXint& len);

/// Convert string of length len from MSDOS; return new string and new length
extern FXAPI FXbool fxfromDOS(FXchar*& string,FXint& len);

/// Duplicate string
extern FXAPI FXchar *fxstrdup(const FXchar* str);

/// Calculate a hash value from a string
extern FXAPI FXuint fxstrhash(const FXchar* str);

/// Convert RGB to HSV
extern FXAPI void fxrgb_to_hsv(FXfloat& h,FXfloat& s,FXfloat& v,FXfloat r,FXfloat g,FXfloat b);

/// Convert HSV to RGB
extern FXAPI void fxhsv_to_rgb(FXfloat& r,FXfloat& g,FXfloat& b,FXfloat h,FXfloat s,FXfloat v);

/// Convert RGB to HSL
extern FXAPI void fxrgb_to_hsl(FXfloat& h,FXfloat& s,FXfloat& l,FXfloat r,FXfloat g,FXfloat b);

/// Convert HSL to RGB
extern FXAPI void fxhsl_to_rgb(FXfloat& r,FXfloat& g,FXfloat& b,FXfloat h,FXfloat s,FXfloat l);

/// Float number classification: 0=OK, +/-1=Inf, +/-2=NaN
extern FXAPI FXint fxieeefloatclass(FXfloat number);

/// Double number classification: 0=OK, +/-1=Inf, +/-2=NaN
extern FXAPI FXint fxieeedoubleclass(FXdouble number);

/// Test for finite float
extern FXAPI FXbool fxIsFinite(FXfloat number);

/// Test for finite double
extern FXAPI FXbool fxIsFinite(FXdouble number);

/// Test for infinite float
extern FXAPI FXbool fxIsInf(FXfloat number);

/// Test for infinite double
extern FXAPI FXbool fxIsInf(FXdouble number);

/// Text for not-a-number float
extern FXAPI FXbool fxIsNan(FXfloat number);

/// Text for not-a-number double
extern FXAPI FXbool fxIsNan(FXdouble number);

/// Raise 10 to an integer power e in the range [-308..308]
extern FXAPI FXdouble fxtenToThe(FXint e);

/// Convert keysym to unicode character
extern FXAPI FXwchar fxkeysym2ucs(FXwchar sym);

/// Convert unicode character to keysym
extern FXAPI FXwchar fxucs2keysym(FXwchar ucs);

/// Parse geometry, a-la X11 geometry specification
extern FXAPI FXint fxparsegeometry(const FXchar *string,FXint& x,FXint& y,FXint& w,FXint& h);

/// True if executable with given path is a console application
extern FXAPI FXbool fxisconsole(const FXchar *path);

/// Return clock ticks from cpu tick-counter
extern FXAPI FXTime fxgetticks();

/// Version number that the library has been compiled with
extern FXAPI const FXuchar fxversion[3];

/// Controls tracing level
extern FXAPI FXint fxTraceLevel;


/// Return wide character from utf8 string at ptr
extern FXAPI FXwchar wc(const FXchar *ptr);

/// Return wide character from utf16 string at ptr
extern FXAPI FXwchar wc(const FXnchar *ptr);


/// Increment to start of next wide character in utf8 string
extern FXAPI const FXchar* wcinc(const FXchar* ptr);

/// Increment to start of next wide character in utf8 string
extern FXAPI FXchar* wcinc(FXchar* ptr);

/// Increment to start of next wide character in utf16 string
extern FXAPI const FXnchar* wcinc(const FXnchar* ptr);

/// Increment to start of next wide character in utf16 string
extern FXAPI FXnchar* wcinc(FXnchar* ptr);

/// Decrement to start of previous wide character in utf8 string
extern FXAPI const FXchar* wcdec(const FXchar* ptr);

/// Decrement to start of previous wide character in utf8 string
extern FXAPI FXchar* wcdec(FXchar* ptr);

/// Decrement to start of previous wide character in utf16 string
extern FXAPI const FXnchar* wcdec(const FXnchar* ptr);

/// Decrement to start of previous wide character in utf16 string
extern FXAPI FXnchar* wcdec(FXnchar* ptr);

/// Adjust ptr to point to leader of multi-byte sequence
extern FXAPI const FXchar* wcstart(const FXchar* ptr);

/// Adjust ptr to point to leader of multi-byte sequence
extern FXAPI FXchar* wcstart(FXchar* ptr);

/// Adjust ptr to point to leader of surrogate pair sequence
extern FXAPI const FXnchar* wcstart(const FXnchar *ptr);

/// Adjust ptr to point to leader of surrogate pair sequence
extern FXAPI FXnchar* wcstart(FXnchar *ptr);

/// Return number of FXchar's of wide character at ptr
extern FXAPI FXint wclen(const FXchar *ptr);

/// Return number of FXnchar's of narrow character at ptr
extern FXAPI FXint wclen(const FXnchar *ptr);

/// Check if valid utf8 wide character representation; returns length or 0
extern FXAPI FXint wcvalid(const FXchar* ptr);

/// Check if valid utf16 wide character representation; returns length or 0
extern FXAPI FXint wcvalid(const FXnchar* ptr);


/// Return number of bytes for utf8 representation of wide character w
extern FXAPI FXint wc2utf(FXwchar w);

/// Return number of narrow characters for utf16 representation of wide character w
extern FXAPI FXint wc2nc(FXwchar w);

/// Return number of bytes for utf8 representation of wide character string
extern FXAPI FXint wcs2utf(const FXwchar* ptr,FXint len);
extern FXAPI FXint wcs2utf(const FXwchar* ptr);

/// Return number of bytes for utf8 representation of narrow character string
extern FXAPI FXint ncs2utf(const FXnchar* ptr,FXint len);
extern FXAPI FXint ncs2utf(const FXnchar* ptr);

/// Return number of wide characters for utf8 character string
extern FXAPI FXint utf2wcs(const FXchar *ptr,FXint len);
extern FXAPI FXint utf2wcs(const FXchar *ptr);

/// Return number of narrow characters for utf8 character string
extern FXAPI FXint utf2ncs(const FXchar *ptr,FXint len);
extern FXAPI FXint utf2ncs(const FXchar *ptr);


/// Convert wide character to utf8 string
extern FXAPI FXint wc2utf(FXchar *dst,FXwchar w);

/// Convert wide character to narrow character string
extern FXAPI FXint wc2nc(FXnchar *dst,FXwchar w);

/// Convert wide character string to utf8 string
extern FXAPI FXint wcs2utf(FXchar *dst,const FXwchar* src,FXint dlen,FXint slen);
extern FXAPI FXint wcs2utf(FXchar *dst,const FXwchar* src,FXint dlen);

/// Convert narrow character string to utf8 string
extern FXAPI FXint ncs2utf(FXchar *dst,const FXnchar* src,FXint dlen,FXint slen);
extern FXAPI FXint ncs2utf(FXchar *dst,const FXnchar* src,FXint dlen);

/// Convert utf8 string to wide character string
extern FXAPI FXint utf2wcs(FXwchar *dst,const FXchar* src,FXint dlen,FXint slen);
extern FXAPI FXint utf2wcs(FXwchar *dst,const FXchar* src,FXint dlen);

/// Convert utf8 string to narrow character string
extern FXAPI FXint utf2ncs(FXnchar *dst,const FXchar* src,FXint dlen,FXint slen);
extern FXAPI FXint utf2ncs(FXnchar *dst,const FXchar* src,FXint dlen);

}

#endif
