/********************************************************************************
*                                                                               *
*        S t r i n g   t o   S i g n e d   L o n g   C o n v e r s i o n        *
*                                                                               *
*********************************************************************************
* Copyright (C) 2005,2025 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "fxascii.h"
#include "FXString.h"

/*
  Notes:
  - Thread-safe conversion of strings to signed long and int, with extra parameter
    'ok' for conversion success.
*/


// Largest and smallest unsigned long value
#ifndef LLONG_MAX
#define LLONG_MAX  FXLONG(9223372036854775807)
#endif
#ifndef LLONG_MIN
#define LLONG_MIN  (-LLONG_MAX-FXLONG(1))
#endif

using namespace FX;

/*******************************************************************************/

namespace FX {

extern FXAPI FXlong __strtoll(const FXchar *beg,const FXchar** end=nullptr,FXint base=0,FXbool* ok=nullptr);
extern FXAPI FXint __strtol(const FXchar *beg,const FXchar** end=nullptr,FXint base=0,FXbool* ok=nullptr);


// Convert string to signed long
FXlong __strtoll(const FXchar *s,const FXchar** e,FXint base,FXbool* ok){
  FXulong value=0;
  FXulong prior=0;
  FXulong ovf=0;
  FXint neg;
  FXint v;

  // Assume the worst
  if(ok) *ok=false;

  // No characters consumed
  if(e) *e=s;

  // Keep it reasonable
  if(base<=36){

    // Skip whitespace
    while(Ascii::isSpace(*s)){
      s++;
      }

    // Handle sign
    if((neg=(s[0]=='-')) || (s[0]=='+')){
      ++s;
      }

    // Check for '0x...', '0b...', or '00...'
    if(s[0]=='0' && base!=10){
      if((s[1]|0x20)=='x'){
        if(__likely(base==16 || !base)){ base=16; s+=2; }
        }
      else if((s[1]|0x20)=='b'){
        if(__likely(base==2 || !base)){ base=2; s+=2; }
        }
      else{
        base=8;
        }
      }

    // Base=10 if not already set
    if(!base) base=10;

    // Scan digits and aggregate number
    v=Ascii::digitValue(*s);
    if(0<=v && v<base){

      // Parse digits
      do{
        prior=value;
        value*=base;
        ovf|=((FXlong)(value-prior))>>63; // Check overflow, branch-free
        value+=v;
        v=Ascii::digitValue(*++s);
        }
      while(0<=v && v<base);

      // Update end of number
      if(e) *e=s;

      // Success
      if(__likely(!ovf)){
        if(ok) *ok=true;
        if(neg) value=-(FXlong)value;
        }

      // Overflow
      else{
        value=neg?LLONG_MIN:LLONG_MAX;
        }
      }
    }
  return (FXlong)value;
  }


// Convert string to signed int
FXint __strtol(const FXchar *s,const FXchar** e,FXint base,FXbool* ok){
  FXlong value=__strtoll(s,e,base,ok);
  if(__unlikely(value<INT_MIN)){
    if(ok) *ok=false;
    return INT_MIN;
    }
  if(__unlikely(value>INT_MAX)){
    if(ok) *ok=false;
    return INT_MAX;
    }
  return (FXint)value;
  }

}
