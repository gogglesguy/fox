/********************************************************************************
*                                                                               *
*       D o u b l e - P r e c i s i o n   3 - E l e m e n t   V e c t o r       *
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
#include "FXVec2d.h"
#include "FXVec3d.h"
#include "FXVec4d.h"


using namespace FX;

/*******************************************************************************/

namespace FX {


// Convert from vector to color
FXColor colorFromVec3d(const FXVec3d& vec){
  return FXRGB((vec.x*255.0+0.5),(vec.y*255.0+0.5),(vec.z*255.0+0.5));
  }


// Convert from color to vector
FXVec3d colorToVec3d(FXColor clr){
  return FXVec3d(0.003921568627*FXREDVAL(clr),0.003921568627*FXGREENVAL(clr),0.003921568627*FXBLUEVAL(clr));
  }


// Normalize vector
FXVec3d normalize(const FXVec3d& v){
  register FXdouble m=v.length2();
  FXVec3d result(v);
  if(__likely(0.0<m)){ result/=sqrt(m); }
  return result;
  }


// Compute normal from three points a,b,c
FXVec3d normal(const FXVec3d& a,const FXVec3d& b,const FXVec3d& c){
  return normalize((b-a)^(c-a));
  }


// Compute approximate normal from four points a,b,c,d
FXVec3d normal(const FXVec3d& a,const FXVec3d& b,const FXVec3d& c,const FXVec3d& d){
  return normalize((c-a)^(d-b));
  }


// Linearly interpolate
FXVec3d lerp(const FXVec3d& u,const FXVec3d& v,FXdouble f){
#if defined(FOX_HAS_SSE2)
  register __m128d u0=_mm_loadu_pd(&u[0]);
  register __m128d u1=_mm_load_sd (&u[2]);
  register __m128d v0=_mm_loadu_pd(&v[0]);
  register __m128d v1=_mm_load_sd (&v[2]);
  register __m128d ff=_mm_set1_pd(f);
  FXVec3d r;
  _mm_storeu_pd(&r[0],_mm_add_pd(u0,_mm_mul_pd(_mm_sub_pd(v0,u0),ff)));
  _mm_store_sd (&r[2],_mm_add_sd(u1,_mm_mul_sd(_mm_sub_sd(v1,u1),ff)));
  return r;
#else
  return FXVec3d(u.x+(v.x-u.x)*f,u.y+(v.y-u.y)*f,u.z+(v.z-u.z)*f);
#endif
  }


// Save vector to stream
FXStream& operator<<(FXStream& store,const FXVec3d& v){
  store << v.x << v.y << v.z;
  return store;
  }


// Load vector from stream
FXStream& operator>>(FXStream& store,FXVec3d& v){
  store >> v.x >> v.y >> v.z;
  return store;
  }

}
