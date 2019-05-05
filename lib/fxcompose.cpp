/********************************************************************************
*                                                                               *
*       C o m p o s e  /  D e c o m p o s e   U n i c o d e   S t r i n g       *
*                                                                               *
*********************************************************************************
* Copyright (C) 2018 by Jeroen van der Zijp.   All Rights Reserved.             *
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
#include "fxunicode.h"
#include "FXArray.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXString.h"


/*
  Notes:
  - Compose or decompose UTF8 unicode strings.
*/


using namespace FX;

/*******************************************************************************/

namespace FX {

// Hangul decomposition
enum {
  SBase  = 0xAC00,
  LBase  = 0x1100,
  VBase  = 0x1161,
  TBase  = 0x11A7,
  LCount = 19,
  VCount = 21,
  TCount = 28,
  NCount = VCount*TCount,
  SCount = LCount*NCount
  };


// Decompose hangul method, if it is hangul (from TR# 15)
static FXint decomposehangul(FXwchar *result,FXwchar w){
  register FXwchar SIndex=w-SBase;
  register FXwchar L,V,T;
  if(0<=SIndex && SIndex<SCount){
    L=LBase+SIndex/NCount;
    V=VBase+(SIndex%NCount)/TCount;
    T=TBase+SIndex%TCount;
    result[0]=L;
    result[1]=V;
    if(T!=TBase){
      result[2]=T;
      return 3;
      }
    return 2;
    }
  result[0]=w;
  return 1;
  }


// Compose hangul in situ; return new length (from TR# 15)
static FXint composehangul(FXwchar *result,FXint len){
  register FXwchar w,last,LIndex,VIndex,SIndex,TIndex;
  register FXint p,q;
  if(0<len){
    last=result[0];
    for(p=q=1; q<len; q++){
      w=result[q];

      // Check to see if two current characters are L and V
      LIndex=last-LBase;
      if(0<=LIndex && LIndex<LCount){

        // Make syllable of form LV
        VIndex=w-VBase;
        if(0<=VIndex && VIndex<VCount){
          last=SBase+(LIndex*VCount+VIndex)*TCount;
          result[p-1]=last;
          continue;
          }
        }

      // Check to see if two current characters are LV and T
      SIndex=last-SBase;
      if(0<=SIndex && SIndex<SCount && (SIndex%TCount)==0){

        // Make syllable of form LVT
        TIndex=w-TBase;
        if(0<TIndex && TIndex<TCount){
          last+=TIndex;
          result[p-1]=last;
          continue;
          }
        }

      // Otherwise just add the character
      last=w;
      result[p++]=w;
      }
    return p;
    }
  return 0;
  }


// Recursive decomposition of type kind
static FXint decomposerecursive(FXwchar *result,FXwchar w,FXbool canonical){
  register const FXwchar* decomposition=Unicode::charDecompose(w);
  if(decomposition[-2] && ((decomposition[-2]==DecomposeCanonical) || !canonical)){
    register FXint p=0;
    register FXint n=0;
    while(p<decomposition[-1]){
      n+=decomposerecursive(result+n,decomposition[p++],canonical);
      }
    return n;
    }
  return decomposehangul(result,w);
  }


// Canonicalize wide character string s, by rearranging combining marks
static FXwchar *normalize(FXwchar* result,FXint len){
  register FXwchar uf,us,cf,cs;
  register FXint p=0;
  while(p+1<len){

    // Second character is a starter; advance by 2
    us=result[p+1];
    FXASSERT(us<0x110000);
    cs=Unicode::charCombining(us);
    if(cs==0){
      p+=2;
      continue;
      }

    // First character class greater; swap and back off by 1
    uf=result[p];
    FXASSERT(uf<0x110000);
    cf=Unicode::charCombining(uf);
    if(cf>cs){
      result[p]=us;
      result[p+1]=uf;
      if(p>0) p--;
      continue;
      }

    // Already in right order; advance by one
    p++;
    }
  return result;
  }


// Compose characters from canonical/compatible decomposition
static FXint compose(FXwchar* result,FXint len){
  register FXint p,q,cc,starterpos,startercc;
  register FXwchar w;
  if(0<len){
    starterpos=0;
    startercc=0;
    for(q=0; q<len; q++){
      cc=Unicode::charCombining(result[q]);
      if(0<q && (startercc==0 || startercc<cc) && (w=Unicode::charCompose(result[starterpos],result[q]))!=0){
        result[starterpos]=w;
        for(p=q+1; p<len; p++) result[p-1]=result[p];
        len--;
        q--;
        if(q==starterpos)
          startercc=0;
        else
          startercc=Unicode::charCombining(result[q-1]);

        continue;
        }
      if(cc==0) starterpos=q;
      startercc=cc;
      }
    }
  return len;
  }


// Return normalized string
FXString normalize(const FXString& s){
  FXwchar* wcs=(FXwchar*)::malloc(s.length()*sizeof(FXwchar));
  FXString result;
  if(wcs){
    FXint n=utf2wcs(wcs,s.text(),s.length());
    normalize(wcs,n);
    result.assign(wcs,n);
    ::free(wcs);
    }
  return result;
  }


// Return decomposition of string, as utf8; this depends on knowing
// the length of the worst recursive decomposition (18).  If unicode
// tables change, make sure this code is updated.   We have an assert
// just in case.
FXString decompose(const FXString& s,FXbool canonical){
  FXwchar* wcs=(FXwchar*)::malloc(s.length()*sizeof(FXwchar)*18);
  FXString result;
  if(wcs){
    FXwchar* ptr=wcs+s.length()*17;
    FXint m=utf2wcs(ptr,s.text(),s.length());
    FXint p=0;
    FXint n=0;
    while(p<m){
      n+=decomposerecursive(&wcs[n],ptr[p++],canonical);
      }
    FXASSERT(n<=s.length()*18);
    normalize(wcs,n);
    result.assign(wcs,n);
    ::free(wcs);
    }
  return result;
  }


// Return normalized composition of string, as utf8
FXString compose(const FXString& s,FXbool canonical){
  FXwchar* wcs=(FXwchar*)::malloc(s.length()*sizeof(FXwchar)*18);
  FXString result;
  if(wcs){
    FXwchar* ptr=wcs+s.length()*17;
    FXint m=utf2wcs(ptr,s.text(),s.length());
    FXint p=0;
    FXint n=0;
    while(p<m){
      n+=decomposerecursive(&wcs[n],ptr[p++],canonical);
      }
    FXASSERT(n<=s.length()*18);
    normalize(wcs,n);
    n=compose(wcs,n);
    result.assign(wcs,n);
    ::free(wcs);
    }
  return result;
  }

}
