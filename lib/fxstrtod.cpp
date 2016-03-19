/********************************************************************************
*                                                                               *
*               S t r i n g   t o   D o u b l e   C o n v e r s i o n           *
*                                                                               *
*********************************************************************************
* Copyright (C) 2005,2011 by Jeroen van der Zijp.   All Rights Reserved.        *
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


/*
  Notes:
  - Thread-safe conversion of strings to double and float, with extra parameter
    for conversion success.
  - Checks both overflow and underflow. Note that characters are consumed in case of
    overflow or underflow, but OK flag is still false when the condition is raised.
*/

/*******************************************************************************/

using namespace FX;

namespace FX {


extern FXAPI FXdouble __strtod(const FXchar *beg,const FXchar** end=NULL,FXbool* ok=NULL);
extern FXAPI FXfloat __strtof(const FXchar *beg,const FXchar** end=NULL,FXbool* ok=NULL);


// Convert string to double
FXdouble __strtod(const FXchar *beg,const FXchar** end,FXbool* ok){
  register const FXchar *s=beg;
  register FXdouble value=0.0;
  register FXint significands=-1;
  register FXint exponent=0;
  register FXint digits=0;
  register FXint expo=0;
  register FXint digs=0;
  register FXint neg;

  // No characters consumed
  if(end) *end=s;

  // Skip whitespace
  while(Ascii::isSpace(s[0])){
    s++;
    }

  // Handle sign
  if((neg=(*s=='-')) || (*s=='+')){
    s++;
    }

  // Read the mantissa
  while('0'<=*s && *s<='9'){
    value=value*10.0+Ascii::digitValue(*s++);
    if(value) significands++;
    digits++;
    }
  if(*s=='.'){
    s++;
    while('0'<=*s && *s<='9'){
      value=value*10.0+Ascii::digitValue(*s++);
      if(value) significands++;
      exponent--;
      digits++;
      }
    }

  // Assume the worst
  if(ok) *ok=false;

  // Got at least a mantissa
  if(0<digits){

    // Adjust sign
    if(neg){
      value=-value;
      }

    // Characters consumed so far
    if(end) *end=s;

    // Try exponent
    if((*s|0x20)=='e'){
      s++;
      if((neg=(*s=='-')) || (*s=='+')){
        s++;
        }
      while('0'<=*s && *s<='9'){
        expo=expo*10+Ascii::digitValue(*s++);
        digs++;
        }

      // Got an exponent
      if(0<digs){
        if(neg){
          exponent-=expo;
          }
        else{
          exponent+=expo;
          }

        // Consumed exponent as well
        if(end) *end=s;
        }
      }

    // Mantissa is not zero
    if(value!=0.0){

      // Bring mantissa to the form X.XXXXXXX
      value*=fxtenToThe(-significands);
      exponent+=significands;

      // Check for overflow
      if((exponent>308) || ((exponent==308) && (value>=1.79769313486231570815))){
        return 1.79769313486231570815E+308;
        }

      // Check for denormal or underflow
      if(exponent<-308){

        // Check underflow
        if((exponent<-324) || ((exponent==-324) && (value<=4.94065645841246544177))){
          return 0.0;
          }

        // Bring exponent into range for denormal
        value*=1.0E-16;
        exponent+=16;
        }

      // In range
      value*=fxtenToThe(exponent);
      }

    // OK
    if(ok) *ok=true;
    }
  return value;
  }


// Convert string to unsigned int
FXfloat __strtof(const FXchar* beg,const FXchar** end,FXbool* ok){
  register FXdouble value=__strtod(beg,end,ok);
  if(__unlikely(value<-FLT_MAX)){
    if(ok) *ok=false;
    return -FLT_MAX;
    }
  if(__unlikely(value>FLT_MAX)){
    if(ok) *ok=false;
    return FLT_MAX;
    }
  return (FXfloat)value;
  }

}
