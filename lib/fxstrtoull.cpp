/********************************************************************************
*                                                                               *
*      S t r i n g   t o   U n s i g n e d   L o n g   C o n v e r s i o n      *
*                                                                               *
*********************************************************************************
* Copyright (C) 2005,2026 by Jeroen van der Zijp.   All Rights Reserved.        *
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
  - Thread-safe conversion of strings to unsigned long and int, with extra parameter
    'ok' for conversion success.
*/


// Largest unsigned long value
#ifndef ULLONG_MAX
#define ULLONG_MAX FXULONG(18446744073709551615)
#endif

using namespace FX;

/*******************************************************************************/

namespace FX {

extern FXAPI FXulong __strtoull(const FXchar* beg,const FXchar** end=nullptr,FXint base=0,FXbool* ok=nullptr);
extern FXAPI FXuint __strtoul(const FXchar* beg,const FXchar** end=nullptr,FXint base=0,FXbool* ok=nullptr);


// Convert string to unsigned long
FXulong __strtoull(const FXchar *s,const FXchar** e,FXint base,FXbool* ok){
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
    while(Ascii::isSpace(s[0])){
      s++;
      }

    // Handle sign
    if((neg=(s[0]=='-')) || (s[0]=='+')){
      ++s;
      }

    // Check for '0x...', '0b...', or '00...'
    if(s[0]=='0'){
      ++s;
      if((s[0]|32)=='x'){
        if(base==0 || base==16){ base=16; ++s; }
        }
      else if((s[0]|32)=='b'){
        if(base==0 || base==2){ base=2; ++s; }
        }
      else{
        if(base==0){ base=8; }
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
        value=ULLONG_MAX;
        }
      }
    }
  return value;
  }


// Convert string to unsigned int
FXuint __strtoul(const FXchar* s,const FXchar** e,FXint base,FXbool* ok){
  FXulong value=__strtoull(s,e,base,ok);
  if(__unlikely(value>UINT_MAX)){
    if(ok) *ok=false;
    return UINT_MAX;
    }
  return (FXuint)value;
  }

}
