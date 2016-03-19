/********************************************************************************
*                                                                               *
*       S i n g l e - P r e c i s i o n   3 - E l e m e n t   V e c t o r       *
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
*********************************************************************************
* $Id: FXVec3f.cpp,v 1.23 2009/02/05 14:57:14 fox Exp $                         *
********************************************************************************/
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXObject.h"
#include "FXVec2f.h"
#include "FXVec3f.h"
#include "FXVec4f.h"
#include "FXMat3f.h"
#include "FXMat4f.h"


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


// Normalize vector
FXVec3f normalize(const FXVec3f& v){
  register FXfloat m=v.length2();
  FXVec3f result(v);
  if(m>0.0f){ result/=sqrtf(m); }
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


// Vector times matrix
FXVec3f FXVec3f::operator*(const FXMat3f& m) const {
  return FXVec3f(x*m[0][0]+y*m[1][0]+z*m[2][0], x*m[0][1]+y*m[1][1]+z*m[2][1], x*m[0][2]+y*m[1][2]+z*m[2][2]);
  }


// Vector times matrix
FXVec3f FXVec3f::operator*(const FXMat4f& m) const {
  FXASSERT(m[0][3]==0.0f && m[1][3]==0.0f && m[2][3]==0.0f && m[3][3]==1.0f);
  return FXVec3f(x*m[0][0]+y*m[1][0]+z*m[2][0]+m[3][0], x*m[0][1]+y*m[1][1]+z*m[2][1]+m[3][1], x*m[0][2]+y*m[1][2]+z*m[2][2]+m[3][2]);
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
