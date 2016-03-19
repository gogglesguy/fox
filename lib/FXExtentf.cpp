/********************************************************************************
*                                                                               *
*          S i n g l e - P r e c i s i o n    E x t e n t    C l a s s          *
*                                                                               *
*********************************************************************************
* Copyright (C) 2004,2010 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "FXVec2f.h"
#include "FXExtentf.h"

/*
  Notes:
*/


using namespace FX;

/**************************  E x t e n t   C l a s s   *************************/

namespace FX {

// Longest side
FXfloat FXExtentf::longest() const {
  register FXfloat x=upper.x-lower.x;
  register FXfloat y=upper.y-lower.y;
  return FXMAX(x,y);
  }


// Shortest side
FXfloat FXExtentf::shortest() const {
  register FXfloat x=upper.x-lower.x;
  register FXfloat y=upper.y-lower.y;
  return FXMIN(x,y);
  }


// Length of diagonal
FXfloat FXExtentf::diameter() const {
  register FXfloat x=upper.x-lower.x;
  register FXfloat y=upper.y-lower.y;
  return sqrtf(x*x+y*y);
  }


// Get radius of box
FXfloat FXExtentf::radius() const {
  return diameter()*0.5f;
  }


// Get diagonal of box
FXVec2f FXExtentf::diagonal() const {
  return upper-lower;
  }


// Get center of box
FXVec2f FXExtentf::center() const {
  return 0.5f*(upper+lower);
  }


// Test if empty
FXbool FXExtentf::empty() const {
  return upper.x<lower.x || upper.y<lower.y;
  }

// Test if box contains point
FXbool FXExtentf::contains(FXfloat x,FXfloat y) const {
  return lower.x<=x && x<=upper.x && lower.y<=y && y<=upper.y;
  }


// Test if box contains point p
FXbool FXExtentf::contains(const FXVec2f& p) const {
  return lower.x<=p.x && p.x<=upper.x && lower.y<=p.y && p.y<=upper.y;
  }


// Test if box contains another box
FXbool FXExtentf::contains(const FXExtentf& ext) const {
  return lower.x<=ext.lower.x && ext.upper.x<=upper.x && lower.y<=ext.lower.y && ext.upper.y<=upper.y;
  }


// Include point into range
FXExtentf& FXExtentf::include(FXfloat x,FXfloat y){
  if(x<lower.x) lower.x=x; if(x>upper.x) upper.x=x;
  if(y<lower.y) lower.y=y; if(y>upper.y) upper.y=y;
  return *this;
  }


// Include point into range
FXExtentf& FXExtentf::include(const FXVec2f& v){
  return include(v.x,v.y);
  }


// Include given box into box's range
FXExtentf& FXExtentf::include(const FXExtentf& ext){
  if(ext.lower.x<lower.x) lower.x=ext.lower.x; if(ext.upper.x>upper.x) upper.x=ext.upper.x;
  if(ext.lower.y<lower.y) lower.y=ext.lower.y; if(ext.upper.y>upper.y) upper.y=ext.upper.y;
  return *this;
  }


// Test if overlap
FXbool overlap(const FXExtentf& a,const FXExtentf& b){
  return a.upper.x>=b.lower.x && a.lower.x<=b.upper.x && a.upper.y>=b.lower.y && a.lower.y<=b.upper.y;
  }


// Union of two boxes
FXExtentf unite(const FXExtentf& a,const FXExtentf& b){
  return FXExtentf(lo(a.lower,b.lower),hi(a.upper,b.upper));
  }


// Intersection of two boxes
FXExtentf intersect(const FXExtentf& a,const FXExtentf& b){
  return FXExtentf(hi(a.lower,b.lower),lo(a.upper,b.upper));
  }


// Saving
FXStream& operator<<(FXStream& store,const FXExtentf& ext){
  store << ext.lower.x << ext.upper.x;
  store << ext.lower.y << ext.upper.y;
  return store;
  }


// Loading
FXStream& operator>>(FXStream& store,FXExtentf& ext){
  store >> ext.lower.x >> ext.upper.x;
  store >> ext.lower.y >> ext.upper.y;
  return store;
  }

}

