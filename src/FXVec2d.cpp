/********************************************************************************
*                                                                               *
*       D o u b l e - P r e c i s i o n   2 - E l e m e n t   V e c t o r       *
*                                                                               *
*********************************************************************************
* Copyright (C) 1994,2008 by Jeroen van der Zijp.   All Rights Reserved.        *
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
* $Id: FXVec2d.cpp,v 1.13 2008/01/04 15:42:41 fox Exp $                         *
********************************************************************************/
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXObject.h"
#include "FXVec2d.h"
#include "FXVec3d.h"
#include "FXMat3d.h"


using namespace FX;

/*******************************************************************************/

namespace FX {


FXVec2d normalize(const FXVec2d& v){
  register FXdouble t=v.length();
  if(t>0.0){ return FXVec2d(v.x/t,v.y/t); }
  return FXVec2d(0.0,0.0);
  }


// Vector times matrix
FXVec2d FXVec2d::operator*(const FXMat3d& m) const {
  FXASSERT(m[0][2]==0.0 && m[1][2]==0.0 && m[2][2]==1.0);
  return FXVec2d(x*m[0][0]+y*m[1][0]+m[2][0], x*m[0][1]+y*m[1][1]+m[2][1]);
  }


FXStream& operator<<(FXStream& store,const FXVec2d& v){
  store << v.x << v.y;
  return store;
  }


FXStream& operator>>(FXStream& store,FXVec2d& v){
  store >> v.x >> v.y;
  return store;
  }

}
