/********************************************************************************
*                                                                               *
*                   V a r a r g s   S c a n f   R o u t i n e s                 *
*                                                                               *
*********************************************************************************
* Copyright (C) 2002,2025 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "fxmath.h"
#include "FXElement.h"
#include "fxascii.h"
#include "FXString.h"

/*
  Notes:
  - It may be not perfect, but at least its the same on all platforms.
  - Handles conversions of the form:

        % [*'] [digits$] [width] [l|ll|h|hh|L|q|t|z] [n|p|d|u|i|x|X|o|D|c|s|[SET]|e|E|f|G|g|b]

  - Assignment suppression and number-grouping:
     '*'        When '*' is passed assignment of the matching quantity is suppressed.
     '''        Commas for thousands, like 1,000,000.

  - Positional argument:
     'digits$'  A sequence of decimal digits indication the position in the parameter
                list, followed by '$'.  The first parameter starts at 1.

  - Width:
     digits     Specifies field width to read, not counting spaces [except for %[] and
                %c and %n directives where spaces do count].

  - Interpretation of size parameters:

     'hh'       convert to FXchar
     'h'        convert to FXshort
     ''         convert to FXint (or FXfloat if real)
     'l'        convert to long (or FXdouble if real)
     'll'       convert to FXlong (64-bit number)
     'L'        ditto
     'q'        ditto
     't'        convert to FXival (size depends on pointer size)
     'z'        convert to FXuval (size depends on pointer size)

  - Conversion specifiers:

     'd'        Decimal integer conversion.
     'b'        Binary integer conversion.
     'i'        Integer conversion from octal, hex, or decimal number.
     'o'        Octal integer conversion.
     'u'        Unsigned decimal integer conversion.
     'x' or 'X' Hexadecimal conversion.
     'c'        String conversion.
     's'        String conversion of printing characters [no spaces].
     'n'        Assign number of characters scanned so far.
     'p'        Hexadecimal pointer conversion.
     [SET]      String conversion of characters in set only.
     'e', 'E',  Floating point conversion.
     'f', 'F'   Floating point conversion.
     'g', 'G'   Floating point conversion.
     'a', 'A'   Floating point conversion.

  - In the case of string (%s or %[...]), do not report a conversion unless at
    least one character is returned.

  - Algorithm accumulate up to 16 digits from the string, starting from most
    significant digit; then convert to real and adjust exponent as necessary.

  - Background: integer numbers larger than 2**53 (52 bits plus one hidden bit)
    can not be represented exactly as double-precision real number, so any
    additional digits past 10**16 are only useful for rounding purposes.

  - Some special numeric values:

    2**53   =     9007199254740992  =  Largest representable mantissa, plus 1.
    10**16  =    10000000000000000  =  Power of 10 slightly larger than 2**53.
    10**18  =  1000000000000000000  =  Power of 10 less than 2**63.
    2**63   =  9223372036854775808  =  Largest 64-bit signed integer, plus 1
    10**19  = 10000000000000000000  =  Power of 10 slightly less than 2**64.
    2**64   = 18446744073709551616  =  Largest 64-bit unsigned integer, plus 1.
*/

#if defined(WIN32)
#ifndef va_copy
#define va_copy(arg,list) ((arg)=(list))
#endif
#endif

using namespace FX;

/*******************************************************************************/

namespace FX {

// Type modifiers
enum {
  ARG_HALFHALF,         // 'hh'
  ARG_HALF,             // 'h'
  ARG_DEFAULT,          // (No specifier)
  ARG_LONG,             // 'l'
  ARG_LONGLONG,         // 'll' / 'L' / 'q'
  ARG_VARIABLE          // Depending on size of pointer
  };


// Normal mode (non-ascii character)
const FXint NORMAL=0x100;

// Thousands separator
const FXint COMMA=',';

// Maximum decimal digits collected
const FXulong DECIMAX=FXULONG(1000000000000000000);

// Maximum hexadecimal digits collected
const FXulong HEXAMAX=FXULONG(0x1000000000000000);

// Declarations
extern FXAPI FXint __sscanf(const FXchar* string,const FXchar* format,...);
extern FXAPI FXint __vsscanf(const FXchar* string,const FXchar* format,va_list arg_ptr);

// Core conversion routines
extern FXAPI FXdouble floatFromDec64(FXulong value,FXint decex);
extern FXAPI FXdouble floatFromHex64(FXulong value,FXint binex);
extern FXAPI FXfloat floatFromDec32(FXulong value,FXint decex);
extern FXAPI FXfloat floatFromHex32(FXulong value,FXint binex);

/*******************************************************************************/

// Scan with va_list arguments
FXint __vsscanf(const FXchar* string,const FXchar* format,va_list args){
  FXint modifier,width,convert,comma,base,digits,count,exponent,expo,done,neg,nex,pos,ww,v;
  const FXchar *start=string;
  const FXchar *ss;
  FXdouble dvalue;
  FXfloat  fvalue;
  FXulong  ivalue;
  FXchar   set[256];
  FXchar  *ptr;
  FXuchar  ch,nn;
  va_list  ag;

  count=0;

  // Process format string
  va_copy(ag,args);
  while((ch=*format++)!='\0'){

    // Check for format-characters
    if(ch=='%'){

      // Get next format character
      ch=*format++;

      // Check for '%%'
      if(ch=='%') goto nml;

      // Default settings
      modifier=ARG_DEFAULT;
      width=0;
      convert=1;
      done=0;
      comma=NORMAL;
      base=0;

      // Parse format specifier
flg:  switch(ch){
        case '*':                                               // Suppress conversion
          convert=0;
          ch=*format++;
          goto flg;
        case '\'':                                              // Print thousandths
          comma=COMMA;                                          // Thousands grouping character
          ch=*format++;
          goto flg;
        case '0':                                               // Width or positional parameter
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          width=ch-'0';
          ch=*format++;
          while(Ascii::isDigit(ch)){
            width=width*10+ch-'0';
            ch=*format++;
            }
          if(ch=='$'){                                          // Positional parameter
            ch=*format++;
            if(width<=0) goto x;                                // Not a legal parameter position
            va_copy(ag,args);
            for(pos=1; pos<width; ++pos){                       // Advance to position prior to arg
              (void)va_arg(ag,void*);
              }
            width=0;                                            // Reset width
            }
          goto flg;
        case 'l':                                               // Long
          modifier=ARG_LONG;
          ch=*format++;
          if(ch=='l'){                                          // Long Long
            modifier=ARG_LONGLONG;
            ch=*format++;
            }
          goto flg;
        case 'h':                                               // Short
          modifier=ARG_HALF;
          ch=*format++;
          if(ch=='h'){                                          // Char
            modifier=ARG_HALFHALF;
            ch=*format++;
            }
          goto flg;
        case 'L':                                               // Long Long
        case 'q':
          modifier=ARG_LONGLONG;
          ch=*format++;
          goto flg;
        case 't':                                               // Size depends on pointer
        case 'z':
          modifier=ARG_VARIABLE;
          ch=*format++;
          goto flg;
        case 'n':                                               // Consumed characters till here
          ivalue=string-start;
          if(convert){
            if(modifier==ARG_DEFAULT){                          // 32-bit always
              *va_arg(ag,FXint*)=(FXint)ivalue;
              }
            else if(modifier==ARG_LONG){                        // Whatever size a long is
              *va_arg(ag,long*)=(long)ivalue;
              }
            else if(modifier==ARG_LONGLONG){                    // 64-bit always
              *va_arg(ag,FXlong*)=ivalue;
              }
            else if(modifier==ARG_HALF){                        // 16-bit always
              *va_arg(ag,FXshort*)=(FXshort)ivalue;
              }
            else if(modifier==ARG_HALFHALF){                    // 8-bit always
              *va_arg(ag,FXchar*)=(FXchar)ivalue;
              }
            else{                                               // Whatever size a pointer is
              *va_arg(ag,FXival*)=(FXival)ivalue;
              }
            }
          break;
        case 'p':                                               // Hex pointer
          modifier=ARG_VARIABLE;
          base=16;
          comma=NORMAL;
          goto integer;
        case 'x':                                               // Hex
        case 'X':
          base=16;
          comma=NORMAL;
          goto integer;
        case 'o':                                               // Octal
          base=8;
          comma=NORMAL;
          goto integer;
        case 'b':                                               // Binary
          base=2;
          comma=NORMAL;
          goto integer;
        case 'd':                                               // Decimal
        case 'u':
          base=10;
        case 'i':                                               // Integer
integer:  ivalue=0;
          digits=0;
          if(width<1) width=2147483647;                         // Width at least 1
          while(Ascii::isSpace(string[0])) string++;            // Skip white space; not included in field width
          if((neg=(string[0]=='-')) || (string[0]=='+')){
            string++;
            width--;
            }
          if(0<width && string[0]=='0'){                        // Got "0"
            if(1<width && (string[1]|32)=='x'){                 // Got a "0x"
              if(base==0 || base==16){                          // Its hex
                string+=2;
                width-=2;
                comma=NORMAL;
                base=16;
                }
              }
            else if(1<width && (string[1]|32)=='b'){            // Got a "0b"
              if(base==0 || base==2){                           // Its binary
                string+=2;
                width-=2;
                comma=NORMAL;
                base=2;
                }
              }
            else{                                               // Got a "0"
              if(base==0 || base==8){                           // Its octal
                comma=NORMAL;
                base=8;
                }
              }
            }
          else{                                                 // Got something else
            if(base==0){                                        // If not set, its decimal now
              base=10;
              }
            }
          while(0<width && 0<=(v=Ascii::digitValue(string[0])) && v<base){
            ivalue=ivalue*base+v;
            string++;
            width--;
            digits++;
            }
          if(3<width && string[0]==comma && 0<digits && digits<4){ // Thousands group is ',ddd' or nothing
            do{
              if((ch=string[1])<'0' || ch>'9') break;
              if((ch=string[2])<'0' || ch>'9') break;
              if((ch=string[3])<'0' || ch>'9') break;
              ivalue=((ivalue*10+string[1])*10+string[2])*10+string[3]-5328;
              string+=4;
              width-=4;
              digits+=3;
              }
            while(3<width && string[0]==comma);
            }
          if(!digits) goto x;                                   // No digits seen!
          if(neg){                                              // Apply sign
            ivalue=0-ivalue;
            }
          if(convert){
            if(modifier==ARG_DEFAULT){                          // 32-bit always
              *va_arg(ag,FXint*)=(FXint)ivalue;
              }
            else if(modifier==ARG_LONG){                        // Whatever size a long is
              *va_arg(ag,long*)=(long)ivalue;
              }
            else if(modifier==ARG_LONGLONG){                    // 64-bit always
              *va_arg(ag,FXlong*)=ivalue;
              }
            else if(modifier==ARG_HALF){                        // 16-bit always
              *va_arg(ag,FXshort*)=(FXshort)ivalue;
              }
            else if(modifier==ARG_HALFHALF){                    // 8-bit always
              *va_arg(ag,FXchar*)=(FXchar)ivalue;
              }
            else{                                               // Whatever size a pointer is
              *va_arg(ag,FXival*)=(FXival)ivalue;
              }
            count++;
            }
          break;
        case 'a':                                               // Floating point
        case 'A':
        case 'e':
        case 'E':
        case 'f':
        case 'F':
        case 'g':
        case 'G':
          exponent=0;
          dvalue=0.0;
          fvalue=0.0;
          ivalue=0;
          digits=0;
          if(width<1) width=2147483647;                         // Width at least 1
          
          // Parse over whitespace; not included in field width
          while(Ascii::isSpace(*string)) string++;
          
          // Found a sign!
          if((neg=(*string=='-')) || (*string=='+')){
            string++;
            width--;
            }

          // Decimal point means its decimal
          if(width && *string=='.') goto dp1;

          // Starts like some kind of number
          if(width && *string<='9' && '0'<=*string){
          
            // Leading with non-0 means its decimal
            if('1'<=*string) goto gtz;
            string++;
            width--;

            // Decimal float
            if(width && (*string|32)!='x'){
              digits++;

              // Leading zeros 
              while(width && *string=='0'){
                digits++;
                string++;
                width--;
                }

              // Decimal point
              if(width && *string=='.'){
dp1:            string++;
                width--;
                while(width && *string=='0'){
                  exponent--;
                  digits++;
                  string++;
                  width--;
                  }
                goto dp2;
                }

              // Decimal significant
gtz:          while(width && '0'<=(ch=*string) && ch<='9'){
                if(ivalue<DECIMAX){ ivalue=ivalue*10+(ch-'0'); exponent--; }
                exponent++;
                digits++;
                string++;
                width--;
                }

              // Thousands grouping of the form \d{1,3}(,\d{3})+
              if(3<width && *string==comma && 0<digits && digits<4){
                do{
                  if(string[1]<'0' || '9'<string[1]) break;
                  if(string[2]<'0' || '9'<string[2]) break;
                  if(string[3]<'0' || '9'<string[3]) break;
                  if(ivalue<DECIMAX){ ivalue=ivalue*10+(string[1]-'0'); exponent--; } exponent++;
                  if(ivalue<DECIMAX){ ivalue=ivalue*10+(string[2]-'0'); exponent--; } exponent++;
                  if(ivalue<DECIMAX){ ivalue=ivalue*10+(string[3]-'0'); exponent--; } exponent++;
                  digits+=3;
                  string+=4;
                  width-=4;
                  }
                while(3<width && string[0]==comma);
                }

              // Decimals following '.'
              if(width && *string=='.'){
                string++;
                width--;
dp2:            while(width && '0'<=(ch=*string) && ch<='9'){
                  if(ivalue<DECIMAX){ ivalue=ivalue*10+(ch-'0'); exponent--; }
                  digits++;
                  string++;
                  width--;
                  }
                }

              // No digits seen!
              if(!digits) goto x;
                                     
              // Tentatively check for exponent
              if(1<width && (string[0]|32)=='e'){
                ss=string;                                      // Rewind point if no match
                ww=width;
                ss++;
                ww--;
                expo=0;
                if((nex=((ch=*ss)=='-')) || (ch=='+')){         // Handle exponent sign
                  ss++;
                  ww--;
                  }
                if(ww && '0'<=(ch=*ss) && ch<='9'){             // Have exponent?
                  do{
                    expo=expo*10+(ch-'0');
                    ss++;
                    ww--;
                    }
                  while(ww && '0'<=(ch=*ss) && ch<='9');
                  if(nex){
                    exponent-=expo;
                    }
                  else{
                    exponent+=expo;
                    }
                  string=ss;                                      // Eat exponent characters
                  width=ww;
                  }
                }
    
              // Convert to result parameter
              if(convert){
                count++;
                if(modifier==ARG_DEFAULT){
                  fvalue=floatFromDec32(ivalue,exponent);
                  if(neg) fvalue=-fvalue;
                  *va_arg(ag,FXfloat*)=fvalue;
                  }
                else{
                  dvalue=floatFromDec64(ivalue,exponent);
                  if(neg) dvalue=-dvalue;
                  *va_arg(ag,FXdouble*)=dvalue;
                  }
                }
              }

            // Hexadecimal float
            else{
              string++;
              width--;

              // Leading zeros 
              while(width && *string=='0'){
                digits++;
                string++;
                width--;
                }

              // Hexadecimal point
              if(width && *string=='.'){                       
                string++;
                width--;
                while(width && *string=='0'){
                  exponent-=4;
                  digits++;
                  string++;
                  width--;
                  }
                goto dp3;
                }

              // Hexadecimal significant
              while(width && Ascii::isHexDigit((ch=*string))){
                if(ivalue<HEXAMAX){ ivalue=(ivalue<<4)+Ascii::digitValue(ch); exponent-=4; }
                exponent+=4;
                digits++;
                string++;
                width--;
                }

              // Hexadecimals point
              if(width && string[0]=='.'){
                string++;
                width--;
dp3:            while(width && Ascii::isHexDigit((ch=*string))){
                  if(ivalue<HEXAMAX){ ivalue=(ivalue<<4)+Ascii::digitValue(ch); exponent-=4; }
                  digits++;
                  string++;
                  width--;
                  }
                }
                
              // No digits seen!
              if(!digits) goto x;               

              // Tentatively check for exponent
              if(1<width && (*string|32)=='p'){               
                ss=string;                                      // Rewind point if no match
                ww=width;
                ss++;
                ww--;
                expo=0;
                if((nex=((ch=*ss)=='-')) || (ch=='+')){         // Handle exponent sign
                  ss++;
                  ww--;
                  }
                if(ww && '0'<=(ch=*ss) && ch<='9'){             // Have exponent?
                  do{
                    expo=expo*10+(ch-'0');
                    ss++;
                    ww--;
                    }
                  while(0<ww && '0'<=(ch=ss[0]) && ch<='9');
                  if(nex){
                    exponent-=expo;
                    }
                  else{
                    exponent+=expo;
                    }
                  string=ss;                                      // Eat exponent characters
                  width=ww;
                  }
                }
    
              // Convert to result parameter
              if(convert){
                count++;
                if(modifier==ARG_DEFAULT){
                  fvalue=floatFromHex32(ivalue,exponent);
                  if(neg) fvalue=-fvalue;
                  *va_arg(ag,FXfloat*)=fvalue;
                  }
                else{
                  dvalue=floatFromHex64(ivalue,exponent);
                  if(neg) dvalue=-dvalue;
                  *va_arg(ag,FXdouble*)=dvalue;
                  }
                }
              }
            }

          // Check for NaN
          else if(2<width && (string[0]|32)=='n' && (string[1]|32)=='a' && (string[2]|32)=='n'){
            string+=3;
            width-=3;
            if(convert){
              count++;
              if(modifier==ARG_DEFAULT){
                fvalue=Math::fpMake(0x7fffffu,255);
                if(neg) fvalue=-fvalue;
                *va_arg(ag,FXfloat*)=fvalue;
                }
              else{
                dvalue=Math::fpMake(FXULONG(0xfffffffffffff),2047);
                if(neg) dvalue=-dvalue;
                *va_arg(ag,FXdouble*)=dvalue;
                }
              }
            }

          // Check for Inf{inity}
          else if(2<width && (string[0]|32)=='i' && (string[1]|32)=='n' && (string[2]|32)=='f'){  
            if(7<width && (string[3]|32)=='i' && (string[4]|32)=='n' && (string[5]|32)=='i' && (string[6]|32)=='t' && (string[7]|32)=='y'){
              string+=5;
              width-=5;
              }
            string+=3;
            width-=3;
            if(convert){
              count++;
              if(modifier==ARG_DEFAULT){
                fvalue=Math::fpMake(0x00000000u,255);
                if(neg) fvalue=-fvalue;
                *va_arg(ag,FXfloat*)=fvalue;
                }
              else{
                dvalue=Math::fpMake(FXULONG(0x0000000000000000),2047);
                if(neg) dvalue=-dvalue;
                *va_arg(ag,FXdouble*)=dvalue;
                }
              }
            }
          break;
        case 'c':                                               // Character(s)
          if(width<1) width=1;                                  // Width at least 1
          if(convert){
            ptr=va_arg(ag,FXchar*);
            while(0<width && (ch=string[0])!='\0'){
              *ptr++=ch;
              string++;
              width--;
              done=1;
              }
            count+=done;
            }
          else{
            while(0<width && string[0]!='\0'){
              string++;
              width--;
              }
            }
          break;
        case 's':                                               // String
          if(width<1) width=2147483647;                         // Width at least 1
          while(Ascii::isSpace(string[0])) string++;            // Skip white space
          if(convert){
            ptr=va_arg(ag,FXchar*);
            while(0<width && (ch=string[0])!='\0' && !Ascii::isSpace(ch)){
              *ptr++=ch;
              string++;
              width--;
              done=1;
              }
            *ptr='\0';
            count+=done;
            }
          else{
            while(0<width && (ch=string[0])!='\0' && !Ascii::isSpace(ch)){
              string++;
              width--;
              }
            }
          break;
        case '[':                                               // Character set
          if(width<1) width=2147483647;                         // Width at least 1
          ch=(FXuchar)*format++;
          v=1;                                                  // Add characters to set
          if(ch=='^'){                                          // Negated character set
            ch=(FXuchar)*format++;
            v=0;                                                // Remove characters from set
            }
          fillElms(set,1-v,256);                                // Initialize set
          if(ch=='\0') goto x;                                  // Format error
          for(;;){                                              // Parse set
            set[ch]=v;
            nn=(FXuchar)*format++;
            if(nn=='\0') goto x;                                // Format error
            if(nn==']') break;
            if(nn=='-'){
              nn=(FXuchar)*format;
              if(nn!=']' && ch<=nn){                            // Range if not at end
                while(ch<nn){ set[++ch]=v; }
                format++;
                }
              else{                                             // Otherwise just '-'
                nn='-';
                }
              }
            ch=nn;
            }
          if(convert){
            ptr=va_arg(ag,FXchar*);
            while(0<width && (ch=*string)!='\0' && set[(FXuchar)ch]){
              *ptr++=ch;
              string++;
              width--;
              done=1;
              }
            *ptr='\0';
            count+=done;
            }
          else{
            while(0<width && (ch=*string)!='\0' && set[(FXuchar)ch]){
              string++;
              width--;
              }
            }
          break;
        default:                                                // Format error
          goto x;
        }
      continue;
      }

    // Check for spaces
nml:if(Ascii::isSpace(ch)){
      while(Ascii::isSpace(*format)) format++;
      while(Ascii::isSpace(*string)) string++;
      continue;
      }

    // Regular characters must match
    if(*string!=ch) break;

    // Next regular character
    string++;
    }
x:va_end(ag);
  return count;
  }


// Scan with variable number of arguments
FXint __sscanf(const FXchar* string,const FXchar* format,...){
  va_list args;
  va_start(args,format);
  FXint result=__vsscanf(string,format,args);
  va_end(args);
  return result;
  }

}
