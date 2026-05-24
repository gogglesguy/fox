/********************************************************************************
*                                                                               *
*       D o u b l e - P r e c i s i o n   2 - E l e m e n t   V e c t o r       *
*                                                                               *
*********************************************************************************
* Copyright (C) 1994,2025 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "FXHash.h"
#include "FXStream.h"
#include "FXVec2d.h"
#include "FXVec3d.h"


using namespace FX;

/*******************************************************************************/

namespace FX {


// Clip vector to box -xlo...xhi and -ylo...yhi (xlo<0, ylo<0, 0<xhi, 0<yhi).
FXVec2d clip(const FXVec2d& v,FXdouble xlo,FXdouble xhi,FXdouble ylo,FXdouble yhi){
  FXVec2d result(v);
  if(__unlikely(xhi<result.x)){
    result.y*=xhi/result.x;
    result.x=xhi;
    }
  else if(__unlikely(result.x<xlo)){
    result.y*=xlo/result.x;
    result.x=xlo;
    }
  if(__unlikely(yhi<result.y)){
    result.x*=yhi/result.y;
    result.y=yhi;
    }
  else if(__unlikely(result.y<ylo)){
    result.x*=ylo/result.y;
    result.y=ylo;
    }
  return result;
  }


// Clip vector to box -xmx...xmx and -ymx...ymx (0<xmx, 0<ymx).
FXVec2d clip(const FXVec2d& v,FXdouble xmx,FXdouble ymx){
  return clip(v,-xmx,xmx,-ymx,ymx);
  }


// Clip vector to box -mx...mx (0<mx).
FXVec2d clip(const FXVec2d& v,FXdouble mx){
  return clip(v,-mx,mx,-mx,mx);
  }


// Save vector to a stream
FXStream& operator<<(FXStream& store,const FXVec2d& v){
  store << v.x << v.y;
  return store;
  }


// Load vector from a stream
FXStream& operator>>(FXStream& store,FXVec2d& v){
  store >> v.x >> v.y;
  return store;
  }

}
