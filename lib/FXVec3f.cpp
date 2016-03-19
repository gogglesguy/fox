/********************************************************************************
*                                                                               *
*       S i n g l e - P r e c i s i o n   3 - E l e m e n t   V e c t o r       *
*                                                                               *
*********************************************************************************
* Copyright (C) 1994,2013 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "FXArray.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXObject.h"
#include "FXVec2f.h"
#include "FXVec3f.h"
#include "FXVec4f.h"


using namespace FX;

/*******************************************************************************/

namespace FX {


// Convert from vector to color
FXColor colorFromVec3f(const FXVec3f& vec){
  return FXRGB((vec.x*255.0f+0.5f),(vec.y*255.0f+0.5f),(vec.z*255.0f+0.5f));
  }


// Convert from color to vector
FXVec3f colorToVec3f(FXColor clr){
  return FXVec3f(0.003921568627f*FXREDVAL(clr),0.003921568627f*FXGREENVAL(clr),0.003921568627f*FXBLUEVAL(clr));
  }


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
FXVec3f fastnormalize(const FXVec3f& v){
  register FXfloat m=v.length2();
  FXVec3f result(v);
  if(__likely(FLT_MIN<m)){ result*=rsqrtf(m); }
  return result;
  }


// Normalize vector
FXVec3f normalize(const FXVec3f& v){
  register FXfloat m=v.length2();
  FXVec3f result(v);
  if(__likely(0.0f<m)){ result/=sqrtf(m); }
  return result;
  }


// Compute normal from three points a,b,c
FXVec3f normal(const FXVec3f& a,const FXVec3f& b,const FXVec3f& c){
  return normalize((b-a)^(c-a));
  }


// Compute approximate normal from four points a,b,c,d
FXVec3f normal(const FXVec3f& a,const FXVec3f& b,const FXVec3f& c,const FXVec3f& d){
  return normalize((c-a)^(d-b));
  }


// Linearly interpolate
FXVec3f lerp(const FXVec3f& u,const FXVec3f& v,FXfloat f){
#if defined(FOX_HAS_AVX)
  register __m128 u0=_mm_maskload_ps(&u[0],_mm_set_epi32(0,~0,~0,~0));
  register __m128 v0=_mm_maskload_ps(&v[0],_mm_set_epi32(0,~0,~0,~0));
  register __m128 ff=_mm_set1_ps(f);
  FXVec3f r;
  _mm_maskstore_ps(&r[0],_mm_set_epi32(0,~0,~0,~0),_mm_add_ps(u0,_mm_mul_ps(_mm_sub_ps(v0,u0),ff)));
  return r;
#elif defined(FOX_HAS_SSE2)
  register __m128 u0=_mm_loadh_pi(_mm_load_ss(&u[2]),(const __m64*)&u[0]);      // u1 u0 0 u2
  register __m128 v0=_mm_loadh_pi(_mm_load_ss(&v[2]),(const __m64*)&v[0]);      // v1 v0 0 v2
  register __m128 ff=_mm_set1_ps(f);
  register __m128 rr=_mm_add_ps(u0,_mm_mul_ps(_mm_sub_ps(v0,u0),ff));
  FXVec3f r;
  _mm_storeh_pi((__m64*)&r[0],rr);
  _mm_store_ss(&r[2],rr);
  return r;
#else
  return FXVec3f(u.x+(v.x-u.x)*f,u.y+(v.y-u.y)*f,u.z+(v.z-u.z)*f);
#endif
  }


// Save vector to stream
FXStream& operator<<(FXStream& store,const FXVec3f& v){
  store << v.x << v.y << v.z;
  return store;
  }


// Load vector from stream
FXStream& operator>>(FXStream& store,FXVec3f& v){
  store >> v.x >> v.y >> v.z;
  return store;
  }

}
