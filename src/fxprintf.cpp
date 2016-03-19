/********************************************************************************
*                                                                               *
*                  V a r a r g s   P r i n t f   R o u t i n e s                *
*                                                                               *
*********************************************************************************
* Copyright (C) 2002,2007 by Jeroen van der Zijp.   All Rights Reserved.        *
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
* $Id: fxprintf.cpp,v 1.59 2008/01/02 15:16:29 fox Exp $                        *
********************************************************************************/
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "fxascii.h"



/*
  Notes:
  - Handles conversions of the form:

        % [#0-+ ] [width] [.precision] [l|ll|h|hh|L|q|t|z] [d|i|o|u|x|X|e|E|f|F|g|G|c|s|n|p]

  - Interpretation of the flags:
     '#'        Alternate form (prefix '0' for octal, '0x' for hex, decimal point if float.
     '0'        Zero-padding.
     '-'        Left-adjust.
     '+'        Show sign always.
     ' '        Leave blank for positive numbers.
     '''        Insert comma's for thousands, like 1,000,000.

  - Interpretation of size parameters:
     'hh'       convert from FXchar.
     'h'        convert from FXshort.
     ''         convert from FXint (or FXfloat if real).
     'l'        convert from long (or FXdouble if real).
     'll'       convert from FXlong (64-bit number).
     'L'        ditto.
     'q'        ditto.
     't'        convert from FXival (size depends on pointer size).
     'z'        ditto.

  - Conversion specifiers:

     'd'        Decimal integer conversion.
     'b'        Binary integer conversion.
     'i'        Integer conversion from octal, hex, or decimal number.
     'o'        Octal integer conversion.
     'u'        Unsigned decimal integer conversion.
     'x' or 'X' Hexadecimal conversion.
     's'        String conversion of printing characters [no spaces].
     'c'        String conversion.
     'n'        Assign number of characters printed so far.
     'p'        Hexadecimal pointer conversion.
     'e', 'E',  Exponential notation floating point conversion.
     'f', 'F'   Simple point conversion.
     'g', 'G'   Shortest representation point conversion.

  - Width may be digits, or '*'.
  - Precision is a sequence of digits, or '*'.
  - Exponent base 10: x = log10(2^y) = y*log10(2) = y*0.301029995663981
    so, roughly x = y*0.301025390625 = (y*1233)>>12.
  - FIXME positional arguments [N$] will be implemented as well some time.
*/

#define CONVERTSIZE     512     // Convertsion buffer
#define NDIG            512     // Maximum space used for numbers

using namespace FX;

/*******************************************************************************/

namespace FX {


// Declarations
extern FXAPI FXint __snprintf(FXchar* string,FXint length,const FXchar* format,...);
extern FXAPI FXint __vsnprintf(FXchar* string,FXint length,const FXchar* format,va_list args);


// Type modifiers
enum {
  ARG_DEFAULT  = 0,     // (No specifier)
  ARG_HALFHALF = 1,     // 'hh'
  ARG_HALF     = 2,     // 'h'
  ARG_LONG     = 3,     // 'l'
  ARG_LONGLONG = 4,     // 'll' / 'L' / 'q'
  ARG_VARIABLE = 5      // Depending on size of pointer
  };

// Conversion flags
enum {
  FLG_DEFAULT  = 0,     // Default option
  FLG_BLANK    = 1,     // Print blank if positive
  FLG_SIGN     = 2,     // Always show sign
  FLG_ZERO     = 4,     // Pad with zeroes if numeric
  FLG_LEFT     = 8,     // Left aligned
  FLG_ALTER    = 16,    // Alternate form
  FLG_UPPER    = 32,    // Use upper case
  FLG_UNSIGNED = 64,    // Unsigned
  FLG_THOUSAND = 128,   // Print comma's for thousands
  FLG_EXPONENT = 256    // Exponential notation
  };


static const FXchar lower_digits[]="0123456789abcdef";
static const FXchar upper_digits[]="0123456789ABCDEF";

/*******************************************************************************/

/* buf must have at least NDIG bytes */
static FXchar* _cvt(FXchar* buffer,FXdouble arg,FXint& decimal,FXint& negative,FXint ndigits,FXint eflag){
  register FXchar *ptr=buffer;
  register FXchar *p;
  register FXint d=0;
  FXdouble fi;
  FXdouble fj;

  // Clamp to maximum digits
  if(ndigits>=NDIG-1) ndigits=NDIG-2;

  // Make positive
  negative=0;
  if(arg<0.0){
    negative=1;
    arg=-arg;
    }

  // Split into whole and fraction
  arg=modf(arg,&fi);

  // Generate integer part
  p=buffer+NDIG;
  if(fi!=0.0){
    while(buffer<p && fi!=0.0){
      fj=modf(fi/10.0,&fi);
      *--p=(FXint)((fj+0.03)*10.0)+'0';
      d++;
      }
    while(p<buffer+NDIG){
      *ptr++=*p++;
      }
    }

  // Else shift fraction until >=1
  else if(0.0<arg){
    while((fj=arg*10.0)<1.0){
      arg=fj;
      d--;
      }
    }

  // End of digits to generate
  p=buffer+ndigits;

  // In normal mode, add the ones before the decimal point
  if(eflag==0) p+=d;

  // Not enough room for expected digits
  if(p<buffer){
    decimal=-ndigits;           // With prec=5 0.000001 becomes 0.00000
    buffer[0]='\0';
    return buffer;
    }

  // Decimal point location
  decimal=d;

  // Generate fraction of up to ndigits, plus one
  while(ptr<=p && ptr<buffer+NDIG){
    arg*=10.0;
    arg=modf(arg,&fj);
    *ptr++=(FXint)fj+'0';
    }

  // No more space
  if(p>=buffer+NDIG){
    buffer[NDIG-1]='\0';
    return buffer;
    }

  // Round the result
  ptr=p;
  *p+=5;
  while('9' < *p){
    *p='0';
    if(buffer<p){
      ++*--p;
      }
    else{
      *p='1';
      decimal++;
      if(eflag==0){             // Extra zero at the end for normal mode
        if(buffer<ptr) *ptr='0';
        ptr++;
        }
      }
    }

  // Terminate
  *ptr='\0';

  // Done
  return buffer;
  }


// Convert exponential mode
static FXchar* _ecvt(FXchar *buffer,FXdouble arg,FXint& decimal,FXint& negative,FXint prec){
  return _cvt(buffer,arg,decimal,negative,prec,1);
  }


// Convert normal mode
static FXchar* _fcvt(FXchar *buffer,FXdouble arg,FXint& decimal,FXint& negative,FXint prec){
  return _cvt(buffer,arg,decimal,negative,prec,0);
  }


/*******************************************************************************/


// Convert minimal length mode
static FXchar *convertGeneral(FXchar *buffer,FXint& len,FXdouble number,FXint prec,FXuint flags){
  register FXchar *ptr=buffer;
  register FXchar *p;
  register FXint i;
  FXchar buf1[NDIG];
  FXint negative;
  FXint decimal;
  FXbool mode;

  // Convert the number as if by exponential mode
  p=_ecvt(buf1,number,decimal,negative,prec);

  // Deal with sign
  if(negative){
    *ptr++='-';
    }
  else if(flags&FLG_SIGN){
    *ptr++='+';
    }
  else if(flags&FLG_BLANK){
    *ptr++=' ';
    }

  // Figure mode
  mode=(prec<decimal) || (decimal<-3);

  // Eliminate trailing zeroes; not done for alternate mode
  if(!(flags&FLG_ALTER)){
    while(0<prec && p[prec-1]=='0') prec--;
    }

  // Exponential notation +d.dddddE+dd
  if(mode){
    *ptr++=*p++;                                // One before the decimal point
    if((1<prec) || (flags&FLG_ALTER)){          // Decimal point needed
      *ptr++='.';
      }
    for(i=1; i<prec; i++){                      // Remaining fraction, if any
      *ptr++=*p++;
      }
    *ptr++=(flags&FLG_UPPER)?'E':'e';           // Exponent
    decimal--;
    if(decimal<0){                              // Sign of exponent
      decimal=-decimal;
      *ptr++='-';
      }
    else{
      *ptr++='+';
      }
    if(99<decimal){                             // Decimal should be [-308...308]
      *ptr++=(decimal/100)+'0';
      *ptr++=((decimal/10))%10+'0';
      *ptr++=(decimal%10)+'0';
      }
    else{                                       // Typical case
      *ptr++=(decimal/10)+'0';
      *ptr++=(decimal%10)+'0';
      }
    }

  // Fraction-only notation +0.0000dddddd
  else if(decimal<=0){
    *ptr++='0';
    if(decimal<0 || 1<=prec) *ptr++='.';        // Decimal point only if followed by at least one digit
    while(decimal++<0){                         // Generate leading zeroes after decimal point
      *ptr++='0';
      }
    for(i=1; i<=prec; i++){                     // Generate prec digits
      *ptr++=*p++;
      }
    }

  // Integral notation +ddddddd00.
  else if(prec<=decimal){
    for(i=1; i<=prec; i++){                     // Generate prec digits
      *ptr++=*p++;
      }
    while(prec++<decimal){                      // Append zeroes until we get to decimal point
      *ptr++='0';
      }
    if(flags&FLG_ALTER) *ptr++='.';             // End with decimal point if alternate mode
    }

  // Normal notation +dddd.dd
  else{
    for(i=1; i<=decimal; i++){                  // Generate decimal digits
      *ptr++=*p++;
      }
    *ptr++='.';                                 // Output decimal point
    while(decimal++<prec){                      // Append more digits until we get prec
      *ptr++=*p++;
      }
    }

  // Set length
  len=ptr-buffer;

  // Done
  return buffer;
  }


/*******************************************************************************/


// Convert double number
static FXchar* convertDouble(FXchar* buffer,FXint& len,FXdouble value,FXint prec,FXint flags){
  register FXchar *ptr=buffer;
  register FXchar *p;
  FXchar buf1[NDIG];
  FXint negative;
  FXint decimal;

  // Exponential notation
  if(flags&FLG_EXPONENT){
    p=_ecvt(buf1,value,decimal,negative,prec+1);
    }

  // Normal notation
  else{
    p=_fcvt(buf1,value,decimal,negative,prec);
    }

/*
  // Check for Infinity and NaN
  if(apr_isalpha(*p)){
    *len = strlen(p);
    memcpy(buf, p, *len + 1);
    *is_negative = FALSE;
    return buffer;
    }
*/
  // Deal with sign
  if(negative){
    *ptr++='-';
    }
  else if(flags&FLG_SIGN){
    *ptr++='+';
    }
  else if(flags&FLG_BLANK){
    *ptr++=' ';
    }

  // Exponential notation +d.dddddE+dd
  if(flags&FLG_EXPONENT){
    *ptr++=*p++;                                // One digit before decimal point
    if((0<prec) || (flags&FLG_ALTER)){          // Decimal point needed
      *ptr++='.';
      }
    while(*p){                                  // Copy fraction
      *ptr++=*p++;
      }
    *ptr++=(flags&FLG_UPPER)?'E':'e';           // Exponent
    if(value!=0.0){
      decimal--;
      if(decimal<0){
        decimal=-decimal;
        *ptr++='-';
        }
      else{
        *ptr++='+';
        }
      if(99<decimal){                           // Decimal should be [-308...308]
        *ptr++=(decimal/100)+'0';
        *ptr++=((decimal/10))%10+'0';
        *ptr++=(decimal%10)+'0';
        }
      else{                                     // Typical case
        *ptr++=(decimal/10)+'0';
        *ptr++=(decimal%10)+'0';
        }
      }
    else{
      *ptr++='+';
      *ptr++='0';
      *ptr++='0';
      }
    }

  // Fractional notation +0.0000ddddd
  else if(decimal<=0){                          // Decimal point is negative or zero
    *ptr++='0';
    if(0<prec){
      *ptr++='.';
      while(decimal++<0){                       // Output a bunch of zeroes preceeded by '0.'
        *ptr++='0';
        }
      }
    else if(flags&FLG_ALTER){                   // Always output '.' even if prec==0
      *ptr++='.';
      }
    while(*p){                                  // Copy fraction
      *ptr++=*p++;
      }
    }

  // Normal notation +ddd.dd
  else{                                         // Decimal point is positive
    while(decimal-->0){
      *ptr++=*p++;
      }
    if((0<prec) || (flags&FLG_ALTER)){          // Decimal point needed
      *ptr++='.';
      }
    while(*p){                                  // Copy fraction
      *ptr++=*p++;
      }
    }

  // Set length
  len=ptr-buffer;

  // Done
  return buffer;
  }


/*******************************************************************************/


// Convert long value
static FXchar* convertLong(FXchar* buffer,FXint& len,FXlong value,FXuint base,FXint prec,FXuint flags){
  const FXchar *digits=(flags&FLG_UPPER)?upper_digits:lower_digits;
  FXchar *end=buffer+CONVERTSIZE-1;
  FXchar *ptr=end;
  FXulong number=value;
  FXchar sign=0;
  FXint digs=0;

  // Deal with sign
  if(!(flags&FLG_UNSIGNED)){
    if(value<0){
      sign='-';
      number=-value;
      }
    else if(flags&FLG_SIGN){
      sign='+';
      }
    else if(flags&FLG_BLANK){
      sign=' ';
      }
    }

  // Terminate string
  *ptr='\0';

  // Convert to string using base
  do{
    *--ptr=digits[number%base];
    number/=base;
    if(flags&FLG_THOUSAND){
      if(++digs%3==0 && number) *--ptr=',';
      }
    prec--;
    }
  while(number);

  // Pad to precision
  while(0<prec){
    *--ptr='0';
    --prec;
    }

  // Alternate form
  if(flags&FLG_ALTER){
    if(base==8 && *ptr!='0'){           // Prepend '0'
      *--ptr='0';
      }
    else if(base==16 && value){         // Prepend '0x'
      *--ptr=(flags&FLG_UPPER)?'X':'x';
      *--ptr='0';
      }
    else if(base==2 && value){          // Prepend '0b'
      *--ptr=(flags&FLG_UPPER)?'B':'b';
      *--ptr='0';
      }
    }

  // Prepend sign
  if(sign){
    *--ptr=sign;
    }

  // Return length
  len=end-ptr;
  return ptr;
  }


/*******************************************************************************/


// Print using format
FXint __vsnprintf(FXchar* string,FXint length,const FXchar* format,va_list args){
  FXint ch,count,flags,width,precision,modifier,len,i;
  FXchar buffer[CONVERTSIZE+2],*str;
  FXdouble number;
  FXlong value;

  count=0;

  // Process format string
  while((ch=*format++)!='\0'){

    // Check for format-characters
    if(ch=='%'){

      // Get next format character
      ch=*format++;

      // Check for '%%'
      if(ch=='%') goto nml;

      // Default settings
      modifier=ARG_DEFAULT;
      flags=FLG_DEFAULT;
      precision=-1;
      width=-1;

      // Check flag characters
flg:  if(ch==' '){ flags|=FLG_BLANK; ch=*format++; goto flg; }
      if(ch=='-'){ flags|=FLG_LEFT; ch=*format++; goto flg; }
      if(ch=='+'){ flags|=FLG_SIGN; ch=*format++; goto flg; }
      if(ch=='0'){ flags|=FLG_ZERO; ch=*format++; goto flg; }
      if(ch=='#'){ flags|=FLG_ALTER; ch=*format++; goto flg; }
      if(ch=='\''){ flags|=FLG_THOUSAND; ch=*format++; goto flg; }

      // Field width
      if(Ascii::isDigit(ch)){                   // In format
        width=ch-'0';
        ch=*format++;
        while(Ascii::isDigit(ch)){
          width=width*10+ch-'0';
          ch=*format++;
          }
        }
      else if(ch=='*'){                         // In argument
        width=va_arg(args,FXint);
        ch=*format++;
        if(width<0){
          width=-width;
          flags|=FLG_LEFT;
          }
        }

      // Precision
      if(ch=='.'){
        ch=*format++;
        precision=0;                            // Default is zero
        if(Ascii::isDigit(ch)){                 // In format
          precision=ch-'0';
          ch=*format++;
          while(Ascii::isDigit(ch)){
            precision=precision*10+ch-'0';
            ch=*format++;
            }
          if(precision>100) precision=100;
          }
        else if(ch=='*'){                       // In argument
          precision=va_arg(args,FXint);
          ch=*format++;
          if(precision<0) precision=0;
          if(precision>100) precision=100;
          }
        }

      // Type modifier
      if(ch=='l'){                                      // Long
        modifier=ARG_LONG;
        ch=*format++;
        if(ch=='l'){                                    // Long Long
          modifier=ARG_LONGLONG;
          ch=*format++;
          }
        }
      else if(ch=='h'){                                 // Short
        modifier=ARG_HALF;
        ch=*format++;
        if(ch=='h'){                                    // Char
          modifier=ARG_HALFHALF;
          ch=*format++;
          }
        }
      else if(ch=='L' || ch=='q'){                      // Long Long
        modifier=ARG_LONGLONG;
        ch=*format++;
        }
      else if(ch=='t' || ch=='z'){                      // Size depends on pointer
        modifier=ARG_VARIABLE;
        ch=*format++;
        }

      // Conversion specifier
      switch(ch){
        case 'u':
          flags|=FLG_UNSIGNED;
          if(modifier==ARG_DEFAULT){                    // 32-bit always
            value=(FXulong)va_arg(args,FXuint);
            }
          else if(modifier==ARG_LONG){                  // Whatever size a long is
            value=(FXulong)va_arg(args,unsigned long);
            }
          else if(modifier==ARG_LONGLONG){              // 64-bit always
            value=(FXulong)va_arg(args,FXulong);
            }
          else if(modifier==ARG_HALF){                  // 16-bit always
            value=(FXulong)(FXushort)va_arg(args,FXuint);
            }
          else if(modifier==ARG_HALFHALF){              // 8-bit always
            value=(FXulong)(FXuchar)va_arg(args,FXuint);
            }
          else{                                         // Whatever size a pointer is
            value=(FXulong)va_arg(args,FXuval);
            }
          str=convertLong(buffer,len,value,10,precision,flags);
          break;
        case 'd':
        case 'i':
          if(modifier==ARG_DEFAULT){                    // 32-bit always
            value=(FXlong)va_arg(args,FXint);
            }
          else if(modifier==ARG_LONG){                  // Whatever size a long is
            value=(FXlong)va_arg(args,long);
            }
          else if(modifier==ARG_LONGLONG){              // 64-bit always
            value=(FXlong)va_arg(args,FXlong);
            }
          else if(modifier==ARG_HALF){                  // 16-bit always
            value=(FXlong)(FXshort)va_arg(args,FXint);
            }
          else if(modifier==ARG_HALFHALF){              // 8-bit always
            value=(FXlong)(signed char)va_arg(args,FXint);
            }
          else{                                         // Whatever size a pointer is
            value=(FXlong)va_arg(args,FXival);
            }
          str=convertLong(buffer,len,value,10,precision,flags);
          break;
        case 'b':
          flags|=FLG_UNSIGNED;
          if(modifier==ARG_DEFAULT){                    // 32-bit always
            value=(FXulong)va_arg(args,FXuint);
            }
          else if(modifier==ARG_LONG){                  // Whatever size a long is
            value=(FXulong)va_arg(args,unsigned long);
            }
          else if(modifier==ARG_LONGLONG){              // 64-bit always
            value=(FXulong)va_arg(args,FXulong);
            }
          else if(modifier==ARG_HALF){                  // 16-bit always
            value=(FXulong)(FXushort)va_arg(args,FXuint);
            }
          else if(modifier==ARG_HALFHALF){              // 8-bit always
            value=(FXulong)(FXuchar)va_arg(args,FXuint);
            }
          else{                                         // Whatever size a pointer is
            value=(FXulong)va_arg(args,FXuval);
            }
          str=convertLong(buffer,len,value,2,precision,flags);
          break;
        case 'o':
          flags|=FLG_UNSIGNED;
          if(modifier==ARG_DEFAULT){                    // 32-bit always
            value=(FXulong)va_arg(args,FXuint);
            }
          else if(modifier==ARG_LONG){                  // Whatever size a long is
            value=(FXulong)va_arg(args,unsigned long);
            }
          else if(modifier==ARG_LONGLONG){              // 64-bit always
            value=(FXulong)va_arg(args,FXulong);
            }
          else if(modifier==ARG_HALF){                  // 16-bit always
            value=(FXulong)(FXushort)va_arg(args,FXuint);
            }
          else if(modifier==ARG_HALFHALF){              // 8-bit always
            value=(FXulong)(FXuchar)va_arg(args,FXuint);
            }
          else{                                         // Whatever size a pointer is
            value=(FXulong)va_arg(args,FXuval);
            }
          str=convertLong(buffer,len,value,8,precision,flags);
          break;
        case 'X':
          flags|=FLG_UPPER;
        case 'x':
          flags|=FLG_UNSIGNED;
          if(modifier==ARG_DEFAULT){                    // 32-bit always
            value=(FXulong)va_arg(args,FXuint);
            }
          else if(modifier==ARG_LONG){                  // Whatever size a long is
            value=(FXulong)va_arg(args,unsigned long);
            }
          else if(modifier==ARG_LONGLONG){              // 64-bit always
            value=(FXulong)va_arg(args,FXulong);
            }
          else if(modifier==ARG_HALF){                  // 16-bit always
            value=(FXulong)(FXushort)va_arg(args,FXuint);
            }
          else if(modifier==ARG_HALFHALF){              // 8-bit always
            value=(FXulong)(FXuchar)va_arg(args,FXuint);
            }
          else{                                         // Whatever size a pointer is
            value=(FXulong)va_arg(args,FXuval);
            }
          str=convertLong(buffer,len,value,16,precision,flags);
          break;
        case 'F':
          flags|=FLG_UPPER;
        case 'f':
          number=va_arg(args,FXdouble);
          if(precision<0) precision=6;
          str=convertDouble(buffer,len,number,precision,flags);
          break;
        case 'E':
          flags|=FLG_UPPER;
        case 'e':
          flags|=FLG_EXPONENT;
          number=va_arg(args,FXdouble);
          if(precision<0) precision=6;
          str=convertDouble(buffer,len,number,precision,flags);
          break;
        case 'G':
          flags|=FLG_UPPER;
        case 'g':
          number=va_arg(args,FXdouble);
          if(precision<0) precision=6;
          if(precision<1) precision=1;
          str=convertGeneral(buffer,len,number,precision,flags);
          break;
        case 'c':                                       // Single character
          flags&=~FLG_ZERO;
          buffer[0]=va_arg(args,FXint);
          str=buffer;
          len=1;
          break;
        case 's':
          flags&=~FLG_ZERO;
          str=va_arg(args,FXchar*);                     // String value
          if(str){
            len=strlen(str);
            if(0<=precision && precision<len) len=precision;
            }
          else{                                         // NULL string passed
            str="(null)";
            len=6;
            }
          break;
        case 'n':
          if(modifier==ARG_DEFAULT){                    // 32-bit always
            *va_arg(args,FXint*)=(FXint)count;
            }
          else if(modifier==ARG_LONG){                  // Whatever size a long is
            *va_arg(args,long*)=(long)count;
            }
          else if(modifier==ARG_LONGLONG){              // 64-bit always
            *va_arg(args,FXlong*)=count;
            }
          else if(modifier==ARG_HALF){                  // 16-bit always
            *va_arg(args,FXshort*)=(FXshort)count;
            }
          else if(modifier==ARG_HALFHALF){              // 8-bit always
            *va_arg(args,FXchar*)=(FXchar)count;
            }
          else{                                         // Whatever size a pointer is
            *va_arg(args,FXival*)=(FXival)count;
            }
          continue;                                     // No printout 
        case 'p':
          flags&=~FLG_ZERO;
          value=(FXulong)va_arg(args,FXuval);
          str=convertLong(buffer,len,value,16,precision,flags);
          break;
        default:                                        // Format error
          goto x;
        }

      // Justify to the right
      if(!(flags&FLG_LEFT)){
        if(flags&FLG_ZERO){                     // Pad on left with zeroes
          if(*str=='+' || *str=='-' || *str==' '){
            if(count<length){ *string++=*str++; }
            count++;
            width--;
            len--;
            }
          else if(*str=='0' && (*(str+1)=='x' || *(str+1)=='X')){
            if(count<length){ *string++=*str++; }
            count++;
            if(count<length){ *string++=*str++; }
            count++;
            width-=2;
            len-=2;
            }
          while(width>len){
            if(count<length){ *string++='0'; }
            count++;
            width--;
            }
          }
        else{                                   // Pad on left with spaces
          while(width>len){
            if(count<length){ *string++=' '; }
            count++;
            width--;
            }
          }
        }

      // Output the string str
      for(i=0; i<len; i++){
        if(count<length){ *string++=*str++; }
        count++;
        }

      // Justify to the left
      if(flags&FLG_LEFT){                       // Pad on right always with spaces
        while(width>len){
          if(count<length){ *string++=' '; }
          count++;
          width--;
          }
        }

      // Get format character
      continue;
      }

    // Regular characters just added
nml:if(count<length){ *string++=ch; }
    count++;
    }

  // Last character
  if(count<length){ *string++='\0'; }

  // Done
x:return count;
  }


// Print using format
FXint __snprintf(FXchar* string,FXint length,const FXchar* format,...){
  va_list args;
  va_start(args,format);
  FXint result=__vsnprintf(string,length,format,args);
  va_end(args);
  return result;
  }

}
