/********************************************************************************
*                                                                               *
*       S i n g l e - P r e c i s i o n   2 - E l e m e n t   V e c t o r       *
*                                                                               *
*********************************************************************************
* Copyright (C) 1994,2009 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "FXHash.h"
#include "FXStream.h"
#include "FXObject.h"
#include "FXVec2f.h"
#include "FXVec3f.h"
#include "FXMat3f.h"


using namespace FX;

/*******************************************************************************/

namespace FX {


#if defined(__GNUC__) && defined(__linux__) && defined(__x86_64__)

static inline FXfloat rsqrtf(FXfloat r){
  register FXfloat q=r;
  asm volatile("rsqrtss %0, %0" : "=x" (q) : "0" (q) );
  return (r*q*q-3.0f)*q*-0.5f;
  }

#else

static inline FXfloat rsqrtf(FXfloat r){
  return 1.0f/sqrtf(r);
  }

#endif


// Fast normalize vector
FXVec2f fastnormalize(const FXVec2f& v){
  register FXfloat m=v.length2();
  FXVec2f result(v);
  if(FLT_MIN<m){ result*=rsqrtf(m); }
  return result;
  }


// Normalize vector
FXVec2f normalize(const FXVec2f& v){
  register FXfloat m=v.length2();
  FXVec2f result(v);
  if(0.0f<m){ result/=sqrtf(m); }
  return result;
  }


// Vector times matrix
FXVec2f FXVec2f::operator*(const FXMat3f& m) const {
  return FXVec2f(x*m[0][0]+y*m[1][0]+m[2][0], x*m[0][1]+y*m[1][1]+m[2][1]);
  }


FXStream& operator<<(FXStream& store,const FXVec2f& v){
  store << v.x << v.y;
  return store;
  }


FXStream& operator>>(FXStream& store,FXVec2f& v){
  store >> v.x >> v.y;
  return store;
  }

}
