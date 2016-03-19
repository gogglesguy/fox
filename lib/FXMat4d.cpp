/********************************************************************************
*                                                                               *
*            D o u b l e - P r e c i s i o n   4 x 4   M a t r i x              *
*                                                                               *
*********************************************************************************
* Copyright (C) 1994,2010 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "FXVec2d.h"
#include "FXVec3d.h"
#include "FXVec4d.h"
#include "FXQuatd.h"
#include "FXMat3d.h"
#include "FXMat4d.h"


/*
  Notes:
  - Transformations pre-multiply.
  - Goal is same effect as OpenGL.
*/


using namespace FX;

/*******************************************************************************/

namespace FX {


// Initialize matrix from scalar
FXMat4d::FXMat4d(FXdouble s){
  m[0][0]=s; m[0][1]=s; m[0][2]=s; m[0][3]=s;
  m[1][0]=s; m[1][1]=s; m[1][2]=s; m[1][3]=s;
  m[2][0]=s; m[2][1]=s; m[2][2]=s; m[2][3]=s;
  m[3][0]=s; m[3][1]=s; m[3][2]=s; m[3][3]=s;
  }


// Initialize with 3x3 rotation and scaling matrix
FXMat4d::FXMat4d(const FXMat3d& s){
  m[0][0]=s[0][0]; m[0][1]=s[0][1]; m[0][2]=s[0][2]; m[0][3]=0.0;
  m[1][0]=s[1][0]; m[1][1]=s[1][1]; m[1][2]=s[1][2]; m[1][3]=0.0;
  m[2][0]=s[2][0]; m[2][1]=s[2][1]; m[2][2]=s[2][2]; m[2][3]=0.0;
  m[3][0]=0.0;     m[3][1]=0.0;     m[3][2]=0.0;     m[3][3]=1.0;
  }


// Initialize matrix from another matrix
FXMat4d::FXMat4d(const FXMat4d& other){
  m[0]=other[0];
  m[1]=other[1];
  m[2]=other[2];
  m[3]=other[3];
  }


// Initialize matrix from array
FXMat4d::FXMat4d(const FXdouble s[]){
  m[0][0]=s[0];  m[0][1]=s[1];  m[0][2]=s[2];  m[0][3]=s[3];
  m[1][0]=s[4];  m[1][1]=s[5];  m[1][2]=s[6];  m[1][3]=s[7];
  m[2][0]=s[8];  m[2][1]=s[9];  m[2][2]=s[10]; m[2][3]=s[11];
  m[3][0]=s[12]; m[3][1]=s[13]; m[3][2]=s[14]; m[3][3]=s[15];
  }


// Initialize diagonal matrix
FXMat4d::FXMat4d(FXdouble a,FXdouble b,FXdouble c,FXdouble d){
  m[0][0]=a;   m[0][1]=0.0; m[0][2]=0.0; m[0][3]=0.0;
  m[1][0]=0.0; m[1][1]=b;   m[1][2]=0.0; m[1][3]=0.0;
  m[2][0]=0.0; m[2][1]=0.0; m[2][2]=c;   m[2][3]=0.0;
  m[3][0]=0.0; m[3][1]=0.0; m[3][2]=0.0; m[3][3]=d;
  }


// Initialize matrix from components
FXMat4d::FXMat4d(FXdouble a00,FXdouble a01,FXdouble a02,FXdouble a03,FXdouble a10,FXdouble a11,FXdouble a12,FXdouble a13,FXdouble a20,FXdouble a21,FXdouble a22,FXdouble a23,FXdouble a30,FXdouble a31,FXdouble a32,FXdouble a33){
  m[0][0]=a00; m[0][1]=a01; m[0][2]=a02; m[0][3]=a03;
  m[1][0]=a10; m[1][1]=a11; m[1][2]=a12; m[1][3]=a13;
  m[2][0]=a20; m[2][1]=a21; m[2][2]=a22; m[2][3]=a23;
  m[3][0]=a30; m[3][1]=a31; m[3][2]=a32; m[3][3]=a33;
  }


// Initialize matrix from four vectors
FXMat4d::FXMat4d(const FXVec4d& a,const FXVec4d& b,const FXVec4d& c,const FXVec4d& d){
  m[0][0]=a[0]; m[0][1]=a[1]; m[0][2]=a[2]; m[0][3]=a[3];
  m[1][0]=b[0]; m[1][1]=b[1]; m[1][2]=b[2]; m[1][3]=b[3];
  m[2][0]=c[0]; m[2][1]=c[1]; m[2][2]=c[2]; m[2][3]=c[3];
  m[3][0]=d[0]; m[3][1]=d[1]; m[3][2]=d[2]; m[3][3]=d[3];
  }


// Assign from scalar
FXMat4d& FXMat4d::operator=(FXdouble s){
  m[0][0]=s; m[0][1]=s; m[0][2]=s; m[0][3]=s;
  m[1][0]=s; m[1][1]=s; m[1][2]=s; m[1][3]=s;
  m[2][0]=s; m[2][1]=s; m[2][2]=s; m[2][3]=s;
  m[3][0]=s; m[3][1]=s; m[3][2]=s; m[3][3]=s;
  return *this;
  }


// Assign from 3x3 rotation and scaling matrix
FXMat4d& FXMat4d::operator=(const FXMat3d& s){
  m[0][0]=s[0][0]; m[0][1]=s[0][1]; m[0][2]=s[0][2]; m[0][3]=0.0;
  m[1][0]=s[1][0]; m[1][1]=s[1][1]; m[1][2]=s[1][2]; m[1][3]=0.0;
  m[2][0]=s[2][0]; m[2][1]=s[2][1]; m[2][2]=s[2][2]; m[2][3]=0.0;
  m[3][0]=0.0;     m[3][1]=0.0;     m[3][2]=0.0;     m[3][3]=1.0;
  return *this;
  }


// Assignment operator
FXMat4d& FXMat4d::operator=(const FXMat4d& s){
  m[0]=s[0];
  m[1]=s[1];
  m[2]=s[2];
  m[3]=s[3];
  return *this;
  }


// Assignment from array
FXMat4d& FXMat4d::operator=(const FXdouble s[]){
  m[0][0]=s[0];  m[0][1]=s[1];  m[0][2]=s[2];  m[0][3]=s[3];
  m[1][0]=s[4];  m[1][1]=s[5];  m[1][2]=s[6];  m[1][3]=s[7];
  m[2][0]=s[8];  m[2][1]=s[9];  m[2][2]=s[10]; m[2][3]=s[11];
  m[3][0]=s[12]; m[3][1]=s[13]; m[3][2]=s[14]; m[3][3]=s[15];
  return *this;
  }


// Set value from scalar
FXMat4d& FXMat4d::set(FXdouble s){
  m[0][0]=s; m[0][1]=s; m[0][2]=s; m[0][3]=s;
  m[1][0]=s; m[1][1]=s; m[1][2]=s; m[1][3]=s;
  m[2][0]=s; m[2][1]=s; m[2][2]=s; m[2][3]=s;
  m[3][0]=s; m[3][1]=s; m[3][2]=s; m[3][3]=s;
  return *this;
  }


// Set from 3x3 rotation and scaling matrix
FXMat4d& FXMat4d::set(const FXMat3d& s){
  m[0][0]=s[0][0]; m[0][1]=s[0][1]; m[0][2]=s[0][2]; m[0][3]=0.0;
  m[1][0]=s[1][0]; m[1][1]=s[1][1]; m[1][2]=s[1][2]; m[1][3]=0.0;
  m[2][0]=s[2][0]; m[2][1]=s[2][1]; m[2][2]=s[2][2]; m[2][3]=0.0;
  m[3][0]=0.0;     m[3][1]=0.0;     m[3][2]=0.0;     m[3][3]=1.0;
  return *this;
  }


// Set value from another matrix
FXMat4d& FXMat4d::set(const FXMat4d& s){
  m[0]=s[0];
  m[1]=s[1];
  m[2]=s[2];
  m[3]=s[3];
  return *this;
  }


// Set value from array
FXMat4d& FXMat4d::set(const FXdouble s[]){
  m[0][0]=s[0];  m[0][1]=s[1];  m[0][2]=s[2];  m[0][3]=s[3];
  m[1][0]=s[4];  m[1][1]=s[5];  m[1][2]=s[6];  m[1][3]=s[7];
  m[2][0]=s[8];  m[2][1]=s[9];  m[2][2]=s[10]; m[2][3]=s[11];
  m[3][0]=s[12]; m[3][1]=s[13]; m[3][2]=s[14]; m[3][3]=s[15];
  return *this;
  }


// Set diagonal matrix
FXMat4d& FXMat4d::set(FXdouble a,FXdouble b,FXdouble c,FXdouble d){
  m[0][0]=a;   m[0][1]=0.0; m[0][2]=0.0; m[0][3]=0.0;
  m[1][0]=0.0; m[1][1]=b;   m[1][2]=0.0; m[1][3]=0.0;
  m[2][0]=0.0; m[2][1]=0.0; m[2][2]=c;   m[2][3]=0.0;
  m[3][0]=0.0; m[3][1]=0.0; m[3][2]=0.0; m[3][3]=d;
  return *this;
  }


// Set value from components
FXMat4d& FXMat4d::set(FXdouble a00,FXdouble a01,FXdouble a02,FXdouble a03,FXdouble a10,FXdouble a11,FXdouble a12,FXdouble a13,FXdouble a20,FXdouble a21,FXdouble a22,FXdouble a23,FXdouble a30,FXdouble a31,FXdouble a32,FXdouble a33){
  m[0][0]=a00; m[0][1]=a01; m[0][2]=a02; m[0][3]=a03;
  m[1][0]=a10; m[1][1]=a11; m[1][2]=a12; m[1][3]=a13;
  m[2][0]=a20; m[2][1]=a21; m[2][2]=a22; m[2][3]=a23;
  m[3][0]=a30; m[3][1]=a31; m[3][2]=a32; m[3][3]=a33;
  return *this;
  }


// Set value from four vectors
FXMat4d& FXMat4d::set(const FXVec4d& a,const FXVec4d& b,const FXVec4d& c,const FXVec4d& d){
  m[0][0]=a[0]; m[0][1]=a[1]; m[0][2]=a[2]; m[0][3]=a[3];
  m[1][0]=b[0]; m[1][1]=b[1]; m[1][2]=b[2]; m[1][3]=b[3];
  m[2][0]=c[0]; m[2][1]=c[1]; m[2][2]=c[2]; m[2][3]=c[3];
  m[3][0]=d[0]; m[3][1]=d[1]; m[3][2]=d[2]; m[3][3]=d[3];
  return *this;
  }


// Add matrices
FXMat4d& FXMat4d::operator+=(const FXMat4d& w){
  m[0][0]+=w[0][0]; m[0][1]+=w[0][1]; m[0][2]+=w[0][2]; m[0][3]+=w[0][3];
  m[1][0]+=w[1][0]; m[1][1]+=w[1][1]; m[1][2]+=w[1][2]; m[1][3]+=w[1][3];
  m[2][0]+=w[2][0]; m[2][1]+=w[2][1]; m[2][2]+=w[2][2]; m[2][3]+=w[2][3];
  m[3][0]+=w[3][0]; m[3][1]+=w[3][1]; m[3][2]+=w[3][2]; m[3][3]+=w[3][3];
  return *this;
  }


// Substract matrices
FXMat4d& FXMat4d::operator-=(const FXMat4d& w){
  m[0][0]-=w[0][0]; m[0][1]-=w[0][1]; m[0][2]-=w[0][2]; m[0][3]-=w[0][3];
  m[1][0]-=w[1][0]; m[1][1]-=w[1][1]; m[1][2]-=w[1][2]; m[1][3]-=w[1][3];
  m[2][0]-=w[2][0]; m[2][1]-=w[2][1]; m[2][2]-=w[2][2]; m[2][3]-=w[2][3];
  m[3][0]-=w[3][0]; m[3][1]-=w[3][1]; m[3][2]-=w[3][2]; m[3][3]-=w[3][3];
  return *this;
  }


// Multiply matrix by scalar
FXMat4d& FXMat4d::operator*=(FXdouble w){
  m[0][0]*=w; m[0][1]*=w; m[0][2]*=w; m[0][3]*=w;
  m[1][0]*=w; m[1][1]*=w; m[1][2]*=w; m[2][3]*=w;
  m[2][0]*=w; m[2][1]*=w; m[2][2]*=w; m[3][3]*=w;
  m[3][0]*=w; m[3][1]*=w; m[3][2]*=w; m[3][3]*=w;
  return *this;
  }


// Multiply matrix by matrix
FXMat4d& FXMat4d::operator*=(const FXMat4d& w){
  register FXdouble x,y,z,h;
  x=m[0][0]; y=m[0][1]; z=m[0][2]; h=m[0][3];
  m[0][0]=x*w[0][0]+y*w[1][0]+z*w[2][0]+h*w[3][0];
  m[0][1]=x*w[0][1]+y*w[1][1]+z*w[2][1]+h*w[3][1];
  m[0][2]=x*w[0][2]+y*w[1][2]+z*w[2][2]+h*w[3][2];
  m[0][3]=x*w[0][3]+y*w[1][3]+z*w[2][3]+h*w[3][3];
  x=m[1][0]; y=m[1][1]; z=m[1][2]; h=m[1][3];
  m[1][0]=x*w[0][0]+y*w[1][0]+z*w[2][0]+h*w[3][0];
  m[1][1]=x*w[0][1]+y*w[1][1]+z*w[2][1]+h*w[3][1];
  m[1][2]=x*w[0][2]+y*w[1][2]+z*w[2][2]+h*w[3][2];
  m[1][3]=x*w[0][3]+y*w[1][3]+z*w[2][3]+h*w[3][3];
  x=m[2][0]; y=m[2][1]; z=m[2][2]; h=m[2][3];
  m[2][0]=x*w[0][0]+y*w[1][0]+z*w[2][0]+h*w[3][0];
  m[2][1]=x*w[0][1]+y*w[1][1]+z*w[2][1]+h*w[3][1];
  m[2][2]=x*w[0][2]+y*w[1][2]+z*w[2][2]+h*w[3][2];
  m[2][3]=x*w[0][3]+y*w[1][3]+z*w[2][3]+h*w[3][3];
  x=m[3][0]; y=m[3][1]; z=m[3][2]; h=m[3][3];
  m[3][0]=x*w[0][0]+y*w[1][0]+z*w[2][0]+h*w[3][0];
  m[3][1]=x*w[0][1]+y*w[1][1]+z*w[2][1]+h*w[3][1];
  m[3][2]=x*w[0][2]+y*w[1][2]+z*w[2][2]+h*w[3][2];
  m[3][3]=x*w[0][3]+y*w[1][3]+z*w[2][3]+h*w[3][3];
  return *this;
  }


// Divide matric by scalar
FXMat4d& FXMat4d::operator/=(FXdouble w){
  m[0][0]/=w; m[0][1]/=w; m[0][2]/=w; m[0][3]/=w;
  m[1][0]/=w; m[1][1]/=w; m[1][2]/=w; m[1][3]/=w;
  m[2][0]/=w; m[2][1]/=w; m[2][2]/=w; m[2][3]/=w;
  m[3][0]/=w; m[3][1]/=w; m[3][2]/=w; m[3][3]/=w;
  return *this;
  }


// Unary minus
FXMat4d FXMat4d::operator-() const {
  return FXMat4d(-m[0][0],-m[0][1],-m[0][2],-m[0][3],
                 -m[1][0],-m[1][1],-m[1][2],-m[1][3],
                 -m[2][0],-m[2][1],-m[2][2],-m[2][3],
                 -m[3][0],-m[3][1],-m[3][2],-m[3][3]);
  }


// Set to identity matrix
FXMat4d& FXMat4d::identity(){
  m[0][0]=1.0; m[0][1]=0.0; m[0][2]=0.0; m[0][3]=0.0;
  m[1][0]=0.0; m[1][1]=1.0; m[1][2]=0.0; m[1][3]=0.0;
  m[2][0]=0.0; m[2][1]=0.0; m[2][2]=1.0; m[2][3]=0.0;
  m[3][0]=0.0; m[3][1]=0.0; m[3][2]=0.0; m[3][3]=1.0;
  return *this;
  }


// Return true if identity matrix
FXbool FXMat4d::isIdentity() const {
  return m[0][0]==1.0 && m[0][1]==0.0 && m[0][2]==0.0 && m[0][3]==0.0 &&
         m[1][0]==0.0 && m[1][1]==1.0 && m[1][2]==0.0 && m[1][3]==0.0 &&
         m[2][0]==0.0 && m[2][1]==0.0 && m[2][2]==1.0 && m[2][3]==0.0 &&
         m[3][0]==0.0 && m[3][1]==0.0 && m[3][2]==0.0 && m[3][3]==1.0;
  }


// Set orthographic projection from view volume
FXMat4d& FXMat4d::setOrtho(FXdouble xlo,FXdouble xhi,FXdouble ylo,FXdouble yhi,FXdouble zlo,FXdouble zhi){
  register FXdouble rl=xhi-xlo;
  register FXdouble tb=yhi-ylo;
  register FXdouble yh=zhi-zlo;
  return set(2.0/rl,0.0,0.0,0.0,0.0,2.0/tb,0.0,0.0,0.0,0.0,-2.0/yh,0.0,-(xhi+xlo)/rl,-(yhi+ylo)/tb,-(zhi+zlo)/yh,1.0);
  }


// Get view volume from orthographic projection
void FXMat4d::getOrtho(FXdouble& xlo,FXdouble& xhi,FXdouble& ylo,FXdouble& yhi,FXdouble& zlo,FXdouble& zhi) const {
  zlo= (m[3][2]+1.0)/m[2][2];
  zhi= (m[3][2]-1.0)/m[2][2];
  xlo=-(1.0+m[3][0])/m[0][0];
  xhi= (1.0-m[3][0])/m[0][0];
  ylo=-(1.0+m[3][1])/m[1][1];
  yhi= (1.0-m[3][1])/m[1][1];
  }


// Set to inverse orthographic projection
FXMat4d& FXMat4d::setInverseOrtho(FXdouble xlo,FXdouble xhi,FXdouble ylo,FXdouble yhi,FXdouble zlo,FXdouble zhi){
  register FXdouble rl=xhi-xlo;
  register FXdouble tb=yhi-ylo;
  register FXdouble yh=zhi-zlo;
  return set(0.5*rl,0.0,0.0,0.0,0.0,0.5*tb,0.0,0.0,0.0,0.0,-0.5*yh,0.0,0.5*(xhi+xlo),0.5*(yhi+ylo),0.5*(zhi+zlo),1.0);
  }


// Set to perspective projection from view volume
FXMat4d& FXMat4d::setFrustum(FXdouble xlo,FXdouble xhi,FXdouble ylo,FXdouble yhi,FXdouble zlo,FXdouble zhi){
  register FXdouble rl=xhi-xlo;
  register FXdouble tb=yhi-ylo;
  register FXdouble yh=zhi-zlo;
  return set(2.0*zlo/rl,0.0,0.0,0.0,0.0,2.0*zlo/tb,0.0,0.0,(xhi+xlo)/rl,(yhi+ylo)/tb,-(zhi+zlo)/yh,-1.0,0.0,0.0,-2.0*zhi*zlo/yh,0.0);
  }


// Get view volume from perspective projection
void FXMat4d::getFrustum(FXdouble& xlo,FXdouble& xhi,FXdouble& ylo,FXdouble& yhi,FXdouble& zlo,FXdouble& zhi) const {
  zlo=m[3][2]/(m[2][2]-1.0);
  zhi=m[3][2]/(m[2][2]+1.0);
  xlo=zlo*(m[2][0]-1.0)/m[0][0];
  xhi=zlo*(m[2][0]+1.0)/m[0][0];
  yhi=zlo*(m[2][1]+1.0)/m[1][1];
  ylo=zlo*(m[2][1]-1.0)/m[1][1];
  }


// Set to inverse perspective projection from view volume
FXMat4d& FXMat4d::setInverseFrustum(FXdouble xlo,FXdouble xhi,FXdouble ylo,FXdouble yhi,FXdouble zlo,FXdouble zhi){
  register FXdouble rl=xhi-xlo;
  register FXdouble tb=yhi-ylo;
  register FXdouble yh=zhi-zlo;
  return set(0.5*rl/zlo,0.0,0.0,0.0,0.0,0.5*tb/zlo,0.0,0.0,0.0,0.0,0.0,-0.5*yh/(zhi*zlo),0.5*(xhi+xlo)/zlo,0.5*(yhi+ylo)/zlo,-1.0,0.5*(zhi+zlo)/(zhi*zlo));
  }


// Make left hand matrix
FXMat4d& FXMat4d::left(){
  m[2][0]= -m[2][0];
  m[2][1]= -m[2][1];
  m[2][2]= -m[2][2];
  m[2][3]= -m[2][3];
  return *this;
  }


// Multiply by rotation matrix
FXMat4d& FXMat4d::rot(const FXMat3d& r){
  register FXdouble x,y,z;
  x=m[0][0]; y=m[1][0]; z=m[2][0];
  m[0][0]=x*r[0][0]+y*r[0][1]+z*r[0][2];
  m[1][0]=x*r[1][0]+y*r[1][1]+z*r[1][2];
  m[2][0]=x*r[2][0]+y*r[2][1]+z*r[2][2];
  x=m[0][1]; y=m[1][1]; z=m[2][1];
  m[0][1]=x*r[0][0]+y*r[0][1]+z*r[0][2];
  m[1][1]=x*r[1][0]+y*r[1][1]+z*r[1][2];
  m[2][1]=x*r[2][0]+y*r[2][1]+z*r[2][2];
  x=m[0][2]; y=m[1][2]; z=m[2][2];
  m[0][2]=x*r[0][0]+y*r[0][1]+z*r[0][2];
  m[1][2]=x*r[1][0]+y*r[1][1]+z*r[1][2];
  m[2][2]=x*r[2][0]+y*r[2][1]+z*r[2][2];
  x=m[0][3]; y=m[1][3]; z=m[2][3];
  m[0][3]=x*r[0][0]+y*r[0][1]+z*r[0][2];
  m[1][3]=x*r[1][0]+y*r[1][1]+z*r[1][2];
  m[2][3]=x*r[2][0]+y*r[2][1]+z*r[2][2];
  return *this;
  }


// Rotate using quaternion
FXMat4d& FXMat4d::rot(const FXQuatd& q){
  return rot(FXMat3d(q));
  }


// Rotate by angle (cos,sin) about arbitrary vector
FXMat4d& FXMat4d::rot(const FXVec3d& v,FXdouble c,FXdouble s){
  register FXdouble xx,yy,zz,xy,yz,zx,xs,ys,zs,t;
  register FXdouble r00,r01,r02,r10,r11,r12,r20,r21,r22;
  register FXdouble x=v.x;
  register FXdouble y=v.y;
  register FXdouble z=v.z;
  register FXdouble mag=x*x+y*y+z*z;
  FXASSERT(-1.00001<c && c<1.00001 && -1.00001<s && s<1.00001);
  if(mag<=1.0E-30) return *this;         // Rotation about 0-length axis
  mag=sqrt(mag);
  x/=mag;
  y/=mag;
  z/=mag;
  xx=x*x;
  yy=y*y;
  zz=z*z;
  xy=x*y;
  yz=y*z;
  zx=z*x;
  xs=x*s;
  ys=y*s;
  zs=z*s;
  t=1.0-c;
  r00=t*xx+c;  r10=t*xy-zs; r20=t*zx+ys;
  r01=t*xy+zs; r11=t*yy+c;  r21=t*yz-xs;
  r02=t*zx-ys; r12=t*yz+xs; r22=t*zz+c;
  x=m[0][0];
  y=m[1][0];
  z=m[2][0];
  m[0][0]=x*r00+y*r01+z*r02;
  m[1][0]=x*r10+y*r11+z*r12;
  m[2][0]=x*r20+y*r21+z*r22;
  x=m[0][1];
  y=m[1][1];
  z=m[2][1];
  m[0][1]=x*r00+y*r01+z*r02;
  m[1][1]=x*r10+y*r11+z*r12;
  m[2][1]=x*r20+y*r21+z*r22;
  x=m[0][2];
  y=m[1][2];
  z=m[2][2];
  m[0][2]=x*r00+y*r01+z*r02;
  m[1][2]=x*r10+y*r11+z*r12;
  m[2][2]=x*r20+y*r21+z*r22;
  x=m[0][3];
  y=m[1][3];
  z=m[2][3];
  m[0][3]=x*r00+y*r01+z*r02;
  m[1][3]=x*r10+y*r11+z*r12;
  m[2][3]=x*r20+y*r21+z*r22;
  return *this;
  }


// Rotate by angle (in radians) about arbitrary vector
FXMat4d& FXMat4d::rot(const FXVec3d& v,FXdouble phi){
  return rot(v,cos(phi),sin(phi));
  }


// Rotate about x-axis
FXMat4d& FXMat4d::xrot(FXdouble c,FXdouble s){
  register FXdouble u,v;
  FXASSERT(-1.00001<c && c<1.00001 && -1.00001<s && s<1.00001);
  u=m[1][0]; v=m[2][0]; m[1][0]=c*u+s*v; m[2][0]=c*v-s*u;
  u=m[1][1]; v=m[2][1]; m[1][1]=c*u+s*v; m[2][1]=c*v-s*u;
  u=m[1][2]; v=m[2][2]; m[1][2]=c*u+s*v; m[2][2]=c*v-s*u;
  u=m[1][3]; v=m[2][3]; m[1][3]=c*u+s*v; m[2][3]=c*v-s*u;
  return *this;
  }


// Rotate by angle about x-axis
FXMat4d& FXMat4d::xrot(FXdouble phi){
  return xrot(cos(phi),sin(phi));
  }


// Rotate about y-axis
FXMat4d& FXMat4d::yrot(FXdouble c,FXdouble s){
  register FXdouble u,v;
  FXASSERT(-1.00001<c && c<1.00001 && -1.00001<s && s<1.00001);
  u=m[0][0]; v=m[2][0]; m[0][0]=c*u-s*v; m[2][0]=c*v+s*u;
  u=m[0][1]; v=m[2][1]; m[0][1]=c*u-s*v; m[2][1]=c*v+s*u;
  u=m[0][2]; v=m[2][2]; m[0][2]=c*u-s*v; m[2][2]=c*v+s*u;
  u=m[0][3]; v=m[2][3]; m[0][3]=c*u-s*v; m[2][3]=c*v+s*u;
  return *this;
  }


// Rotate by angle about y-axis
FXMat4d& FXMat4d::yrot(FXdouble phi){
  return yrot(cos(phi),sin(phi));
  }


// Rotate about z-axis
FXMat4d& FXMat4d::zrot(FXdouble c,FXdouble s){
  register FXdouble u,v;
  FXASSERT(-1.00001<c && c<1.00001 && -1.00001<s && s<1.00001);
  u=m[0][0]; v=m[1][0]; m[0][0]=c*u+s*v; m[1][0]=c*v-s*u;
  u=m[0][1]; v=m[1][1]; m[0][1]=c*u+s*v; m[1][1]=c*v-s*u;
  u=m[0][2]; v=m[1][2]; m[0][2]=c*u+s*v; m[1][2]=c*v-s*u;
  u=m[0][3]; v=m[1][3]; m[0][3]=c*u+s*v; m[1][3]=c*v-s*u;
  return *this;
  }


// Rotate by angle about z-axis
FXMat4d& FXMat4d::zrot(FXdouble phi){
  return zrot(cos(phi),sin(phi));
  }


// Look at
FXMat4d& FXMat4d::look(const FXVec3d& from,const FXVec3d& to,const FXVec3d& up){
  register FXdouble x0,x1,x2,tx,ty,tz;
  FXVec3d rz,rx,ry;
  rz=normalize(from-to);
  rx=normalize(up^rz);
  ry=normalize(rz^rx);
  tx= -from[0]*rx[0]-from[1]*rx[1]-from[2]*rx[2];
  ty= -from[0]*ry[0]-from[1]*ry[1]-from[2]*ry[2];
  tz= -from[0]*rz[0]-from[1]*rz[1]-from[2]*rz[2];
  x0=m[0][0]; x1=m[0][1]; x2=m[0][2];
  m[0][0]=rx[0]*x0+rx[1]*x1+rx[2]*x2+tx*m[0][3];
  m[0][1]=ry[0]*x0+ry[1]*x1+ry[2]*x2+ty*m[0][3];
  m[0][2]=rz[0]*x0+rz[1]*x1+rz[2]*x2+tz*m[0][3];
  x0=m[1][0]; x1=m[1][1]; x2=m[1][2];
  m[1][0]=rx[0]*x0+rx[1]*x1+rx[2]*x2+tx*m[1][3];
  m[1][1]=ry[0]*x0+ry[1]*x1+ry[2]*x2+ty*m[1][3];
  m[1][2]=rz[0]*x0+rz[1]*x1+rz[2]*x2+tz*m[1][3];
  x0=m[2][0]; x1=m[2][1]; x2=m[2][2];
  m[2][0]=rx[0]*x0+rx[1]*x1+rx[2]*x2+tx*m[2][3];
  m[2][1]=ry[0]*x0+ry[1]*x1+ry[2]*x2+ty*m[2][3];
  m[2][2]=rz[0]*x0+rz[1]*x1+rz[2]*x2+tz*m[2][3];
  x0=m[3][0]; x1=m[3][1]; x2=m[3][2];
  m[3][0]=rx[0]*x0+rx[1]*x1+rx[2]*x2+tx*m[3][3];
  m[3][1]=ry[0]*x0+ry[1]*x1+ry[2]*x2+ty*m[3][3];
  m[3][2]=rz[0]*x0+rz[1]*x1+rz[2]*x2+tz*m[3][3];
  return *this;
  }


// Translate
FXMat4d& FXMat4d::trans(FXdouble tx,FXdouble ty,FXdouble tz){
  m[3][0]=m[3][0]+tx*m[0][0]+ty*m[1][0]+tz*m[2][0];
  m[3][1]=m[3][1]+tx*m[0][1]+ty*m[1][1]+tz*m[2][1];
  m[3][2]=m[3][2]+tx*m[0][2]+ty*m[1][2]+tz*m[2][2];
  m[3][3]=m[3][3]+tx*m[0][3]+ty*m[1][3]+tz*m[2][3];
  return *this;
  }


// Translate over vector
FXMat4d& FXMat4d::trans(const FXVec3d& v){
  return trans(v[0],v[1],v[2]);
  }


// Scale unqual
FXMat4d& FXMat4d::scale(FXdouble sx,FXdouble sy,FXdouble sz){
  m[0][0]*=sx; m[0][1]*=sx; m[0][2]*=sx; m[0][3]*=sx;
  m[1][0]*=sy; m[1][1]*=sy; m[1][2]*=sy; m[1][3]*=sy;
  m[2][0]*=sz; m[2][1]*=sz; m[2][2]*=sz; m[2][3]*=sz;
  return *this;
  }


// Scale uniform
FXMat4d& FXMat4d::scale(FXdouble s){
  return scale(s,s,s);
  }


// Scale matrix
FXMat4d& FXMat4d::scale(const FXVec3d& v){
  return scale(v[0],v[1],v[2]);
  }


// Calculate determinant
FXdouble FXMat4d::det() const {
  return (m[0][0]*m[1][1]-m[0][1]*m[1][0]) * (m[2][2]*m[3][3]-m[2][3]*m[3][2]) -
         (m[0][0]*m[1][2]-m[0][2]*m[1][0]) * (m[2][1]*m[3][3]-m[2][3]*m[3][1]) +
         (m[0][0]*m[1][3]-m[0][3]*m[1][0]) * (m[2][1]*m[3][2]-m[2][2]*m[3][1]) +
         (m[0][1]*m[1][2]-m[0][2]*m[1][1]) * (m[2][0]*m[3][3]-m[2][3]*m[3][0]) -
         (m[0][1]*m[1][3]-m[0][3]*m[1][1]) * (m[2][0]*m[3][2]-m[2][2]*m[3][0]) +
         (m[0][2]*m[1][3]-m[0][3]*m[1][2]) * (m[2][0]*m[3][1]-m[2][1]*m[3][0]);
  }


// Transpose matrix
FXMat4d FXMat4d::transpose() const {
  return FXMat4d(m[0][0],m[1][0],m[2][0],m[3][0],
                 m[0][1],m[1][1],m[2][1],m[3][1],
                 m[0][2],m[1][2],m[2][2],m[3][2],
                 m[0][3],m[1][3],m[2][3],m[3][3]);
  }


// Invert matrix
FXMat4d FXMat4d::invert() const {
  FXMat4d r;
  register FXdouble A0=m[0][0]*m[1][1]-m[0][1]*m[1][0];
  register FXdouble A1=m[0][0]*m[1][2]-m[0][2]*m[1][0];
  register FXdouble A2=m[0][0]*m[1][3]-m[0][3]*m[1][0];
  register FXdouble A3=m[0][1]*m[1][2]-m[0][2]*m[1][1];
  register FXdouble A4=m[0][1]*m[1][3]-m[0][3]*m[1][1];
  register FXdouble A5=m[0][2]*m[1][3]-m[0][3]*m[1][2];
  register FXdouble B0=m[2][0]*m[3][1]-m[2][1]*m[3][0];
  register FXdouble B1=m[2][0]*m[3][2]-m[2][2]*m[3][0];
  register FXdouble B2=m[2][0]*m[3][3]-m[2][3]*m[3][0];
  register FXdouble B3=m[2][1]*m[3][2]-m[2][2]*m[3][1];
  register FXdouble B4=m[2][1]*m[3][3]-m[2][3]*m[3][1];
  register FXdouble B5=m[2][2]*m[3][3]-m[2][3]*m[3][2];
  register FXdouble dd=A0*B5-A1*B4+A2*B3+A3*B2-A4*B1+A5*B0;
  FXASSERT(dd!=0.0); 
  dd=1.0/dd;
  r[0][0]=(m[1][1]*B5-m[1][2]*B4+m[1][3]*B3)*dd;
  r[1][0]=(m[1][2]*B2-m[1][0]*B5-m[1][3]*B1)*dd;
  r[2][0]=(m[1][0]*B4-m[1][1]*B2+m[1][3]*B0)*dd;
  r[3][0]=(m[1][1]*B1-m[1][0]*B3-m[1][2]*B0)*dd;
  r[0][1]=(m[0][2]*B4-m[0][1]*B5-m[0][3]*B3)*dd;
  r[1][1]=(m[0][0]*B5-m[0][2]*B2+m[0][3]*B1)*dd;
  r[2][1]=(m[0][1]*B2-m[0][0]*B4-m[0][3]*B0)*dd;
  r[3][1]=(m[0][0]*B3-m[0][1]*B1+m[0][2]*B0)*dd;
  r[0][2]=(m[3][1]*A5-m[3][2]*A4+m[3][3]*A3)*dd;
  r[1][2]=(m[3][2]*A2-m[3][0]*A5-m[3][3]*A1)*dd;
  r[2][2]=(m[3][0]*A4-m[3][1]*A2+m[3][3]*A0)*dd;
  r[3][2]=(m[3][1]*A1-m[3][0]*A3-m[3][2]*A0)*dd;
  r[0][3]=(m[2][2]*A4-m[2][1]*A5-m[2][3]*A3)*dd;
  r[1][3]=(m[2][0]*A5-m[2][2]*A2+m[2][3]*A1)*dd;
  r[2][3]=(m[2][1]*A2-m[2][0]*A4-m[2][3]*A0)*dd;
  r[3][3]=(m[2][0]*A3-m[2][1]*A1+m[2][2]*A0)*dd;
  return r;
  }


// Invert affine matrix
FXMat4d FXMat4d::affineInvert() const {
  register FXdouble dd;
  FXMat4d r;
  r[0][0]=m[1][1]*m[2][2]-m[1][2]*m[2][1];
  r[0][1]=m[0][2]*m[2][1]-m[0][1]*m[2][2];
  r[0][2]=m[0][1]*m[1][2]-m[0][2]*m[1][1];
  r[0][3]=0.0;
  r[1][0]=m[1][2]*m[2][0]-m[1][0]*m[2][2];
  r[1][1]=m[0][0]*m[2][2]-m[0][2]*m[2][0];
  r[1][2]=m[0][2]*m[1][0]-m[0][0]*m[1][2];
  r[1][3]=0.0;
  r[2][0]=m[1][0]*m[2][1]-m[1][1]*m[2][0];
  r[2][1]=m[0][1]*m[2][0]-m[0][0]*m[2][1];
  r[2][2]=m[0][0]*m[1][1]-m[0][1]*m[1][0];
  r[2][3]=0.0;
  dd=m[0][0]*r[0][0]+m[0][1]*r[1][0]+m[0][2]*r[2][0];
  FXASSERT(dd!=0.0);
  dd=1.0/dd;
  r[0][0]*=dd;
  r[0][1]*=dd;
  r[0][2]*=dd;
  r[1][0]*=dd;
  r[1][1]*=dd;
  r[1][2]*=dd;
  r[2][0]*=dd;
  r[2][1]*=dd;
  r[2][2]*=dd;
  r[3][0]=-(r[0][0]*m[3][0]+r[1][0]*m[3][1]+r[2][0]*m[3][2]);
  r[3][1]=-(r[0][1]*m[3][0]+r[1][1]*m[3][1]+r[2][1]*m[3][2]);
  r[3][2]=-(r[0][2]*m[3][0]+r[1][2]*m[3][1]+r[2][2]*m[3][2]);
  r[3][3]=1.0;
  return r;
  }


// Invert rigid body transform matrix
FXMat4d FXMat4d::rigidInvert() const {
  register FXdouble ss;
  FXMat4d r;
  ss=1.0/(m[0][0]*m[0][0]+m[0][1]*m[0][1]+m[0][2]*m[0][2]);
  r[0][0]=m[0][0]*ss; 
  r[0][1]=m[1][0]*ss; 
  r[0][2]=m[2][0]*ss; 
  r[0][3]=0.0;
  r[1][0]=m[0][1]*ss; 
  r[1][1]=m[1][1]*ss; 
  r[1][2]=m[2][1]*ss; 
  r[1][3]=0.0;
  r[2][0]=m[0][2]*ss; 
  r[2][1]=m[1][2]*ss; 
  r[2][2]=m[2][2]*ss; 
  r[2][3]=0.0;
  r[3][0]=-(r[0][0]*m[3][0]+r[1][0]*m[3][1]+r[2][0]*m[3][2]);
  r[3][1]=-(r[0][1]*m[3][0]+r[1][1]*m[3][1]+r[2][1]*m[3][2]);
  r[3][2]=-(r[0][2]*m[3][0]+r[1][2]*m[3][1]+r[2][2]*m[3][2]);
  r[3][3]=1.0;
  return r;
  }


// Matrix times vector
FXVec3d operator*(const FXMat4d& m,const FXVec3d& v){
  return FXVec3d(m[0][0]*v[0]+m[0][1]*v[1]+m[0][2]*v[2]+m[0][3], m[1][0]*v[0]+m[1][1]*v[1]+m[1][2]*v[2]+m[1][3], m[2][0]*v[0]+m[2][1]*v[1]+m[2][2]*v[2]+m[2][3]);
  }


// Matrix times vector
FXVec4d operator*(const FXMat4d& m,const FXVec4d& v){
  return FXVec4d(m[0][0]*v[0]+m[0][1]*v[1]+m[0][2]*v[2]+m[0][3]*v[3], m[1][0]*v[0]+m[1][1]*v[1]+m[1][2]*v[2]+m[1][3]*v[3], m[2][0]*v[0]+m[2][1]*v[1]+m[2][2]*v[2]+m[2][3]*v[3], m[3][0]*v[0]+m[3][1]*v[1]+m[3][2]*v[2]+m[3][3]*v[3]);
  }


// Vector times matrix
FXVec3d operator*(const FXVec3d& v,const FXMat4d& m){
  return FXVec3d(v[0]*m[0][0]+v[1]*m[1][0]+v[2]*m[2][0]+m[3][0], v[0]*m[0][1]+v[1]*m[1][1]+v[2]*m[2][1]+m[3][1], v[0]*m[0][2]+v[1]*m[1][2]+v[2]*m[2][2]+m[3][2]);
  }


// Vector times matrix
FXVec4d operator*(const FXVec4d& v,const FXMat4d& m){
  return FXVec4d(v[0]*m[0][0]+v[1]*m[1][0]+v[2]*m[2][0]+v[3]*m[3][0], v[0]*m[0][1]+v[1]*m[1][1]+v[2]*m[2][1]+v[3]*m[3][1], v[0]*m[0][2]+v[1]*m[1][2]+v[2]*m[2][2]+v[3]*m[3][2], v[0]*m[0][3]+v[1]*m[1][3]+v[2]*m[2][3]+v[3]*m[3][3]);
  }


// Matrix and matrix add
FXMat4d operator+(const FXMat4d& a,const FXMat4d& b){
  return FXMat4d(a[0][0]+b[0][0],a[0][1]+b[0][1],a[0][2]+b[0][2],a[0][3]+b[0][3],
                 a[1][0]+b[1][0],a[1][1]+b[1][1],a[1][2]+b[1][2],a[1][3]+b[1][3],
                 a[2][0]+b[2][0],a[2][1]+b[2][1],a[2][2]+b[2][2],a[2][3]+b[2][3],
                 a[3][0]+b[3][0],a[3][1]+b[3][1],a[3][2]+b[3][2],a[3][3]+b[3][3]);
  }


// Matrix and matrix subtract
FXMat4d operator-(const FXMat4d& a,const FXMat4d& b){
  return FXMat4d(a[0][0]-b[0][0],a[0][1]-b[0][1],a[0][2]-b[0][2],a[0][3]-b[0][3],
                 a[1][0]-b[1][0],a[1][1]-b[1][1],a[1][2]-b[1][2],a[1][3]-b[1][3],
                 a[2][0]-b[2][0],a[2][1]-b[2][1],a[2][2]-b[2][2],a[2][3]-b[2][3],
                 a[3][0]-b[3][0],a[3][1]-b[3][1],a[3][2]-b[3][2],a[3][3]-b[3][3]);
  }


// Matrix and matrix multiply
FXMat4d operator*(const FXMat4d& a,const FXMat4d& b){
  register FXdouble x,y,z,w;
  FXMat4d r;
  x=a[0][0]; y=a[0][1]; z=a[0][2]; w=a[0][3];
  r[0][0]=x*b[0][0]+y*b[1][0]+z*b[2][0]+w*b[3][0];
  r[0][1]=x*b[0][1]+y*b[1][1]+z*b[2][1]+w*b[3][1];
  r[0][2]=x*b[0][2]+y*b[1][2]+z*b[2][2]+w*b[3][2];
  r[0][3]=x*b[0][3]+y*b[1][3]+z*b[2][3]+w*b[3][3];
  x=a[1][0]; y=a[1][1]; z=a[1][2]; w=a[1][3];
  r[1][0]=x*b[0][0]+y*b[1][0]+z*b[2][0]+w*b[3][0];
  r[1][1]=x*b[0][1]+y*b[1][1]+z*b[2][1]+w*b[3][1];
  r[1][2]=x*b[0][2]+y*b[1][2]+z*b[2][2]+w*b[3][2];
  r[1][3]=x*b[0][3]+y*b[1][3]+z*b[2][3]+w*b[3][3];
  x=a[2][0]; y=a[2][1]; z=a[2][2]; w=a[2][3];
  r[2][0]=x*b[0][0]+y*b[1][0]+z*b[2][0]+w*b[3][0];
  r[2][1]=x*b[0][1]+y*b[1][1]+z*b[2][1]+w*b[3][1];
  r[2][2]=x*b[0][2]+y*b[1][2]+z*b[2][2]+w*b[3][2];
  r[2][3]=x*b[0][3]+y*b[1][3]+z*b[2][3]+w*b[3][3];
  x=a[3][0]; y=a[3][1]; z=a[3][2]; w=a[3][3];
  r[3][0]=x*b[0][0]+y*b[1][0]+z*b[2][0]+w*b[3][0];
  r[3][1]=x*b[0][1]+y*b[1][1]+z*b[2][1]+w*b[3][1];
  r[3][2]=x*b[0][2]+y*b[1][2]+z*b[2][2]+w*b[3][2];
  r[3][3]=x*b[0][3]+y*b[1][3]+z*b[2][3]+w*b[3][3];
  return r;
  }


// Multiply scalar by matrix
FXMat4d operator*(FXdouble x,const FXMat4d& a){
  return FXMat4d(x*a[0][0],x*a[0][1],x*a[0][2],a[0][3],
                 x*a[1][0],x*a[1][1],x*a[1][2],a[1][3],
                 x*a[2][0],x*a[2][1],x*a[2][2],a[2][3],
                 x*a[3][0],x*a[3][1],x*a[3][2],a[3][3]);
  }


// Multiply matrix by scalar
FXMat4d operator*(const FXMat4d& a,FXdouble x){
  return FXMat4d(a[0][0]*x,a[0][1]*x,a[0][2]*x,a[0][3],
                 a[1][0]*x,a[1][1]*x,a[1][2]*x,a[1][3],
                 a[2][0]*x,a[2][1]*x,a[2][2]*x,a[2][3],
                 a[3][0]*x,a[3][1]*x,a[3][2]*x,a[3][3]);
  }


// Divide scalar by matrix
FXMat4d operator/(FXdouble x,const FXMat4d& a){
  return FXMat4d(x/a[0][0],x/a[0][1],x/a[0][2],a[0][3],
                 x/a[1][0],x/a[1][1],x/a[1][2],a[1][3],
                 x/a[2][0],x/a[2][1],x/a[2][2],a[2][3],
                 x/a[3][0],x/a[3][1],x/a[3][2],a[3][3]);
  }


// Divide matrix by scalar
FXMat4d operator/(const FXMat4d& a,FXdouble x){
  return FXMat4d(a[0][0]/x,a[0][1]/x,a[0][2]/x,a[0][3],
                 a[1][0]/x,a[1][1]/x,a[1][2]/x,a[1][3],
                 a[2][0]/x,a[2][1]/x,a[2][2]/x,a[2][3],
                 a[3][0]/x,a[3][1]/x,a[3][2]/x,a[3][3]);
  }


// Matrix and matrix equality
FXbool operator==(const FXMat4d& a,const FXMat4d& b){
  return a[0]==b[0] && a[1]==b[1] && a[2]==b[2] && a[3]==b[3];
  }


// Matrix and matrix inequality
FXbool operator!=(const FXMat4d& a,const FXMat4d& b){
  return a[0]!=b[0] || a[1]!=b[1] || a[2]!=b[2] || a[3]!=b[3];
  }


// Matrix and scalar equality
FXbool operator==(const FXMat4d& a,FXdouble n){
  return a[0]==n && a[1]==n && a[2]==n && a[3]==n;
  }


// Scalar and matrix equality
FXbool operator==(FXdouble n,const FXMat4d& a){
  return n==a[0] && n==a[1] && n==a[2] && n==a[3];
  }


// Matrix and scalar inequality
FXbool operator!=(const FXMat4d& a,FXdouble n){
  return a[0]!=n || a[1]!=n || a[2]!=n || a[3]!=n;
  }


// Scalar and matrix inequality
FXbool operator!=(FXdouble n,const FXMat4d& a){
  return n!=a[0] || n!=a[1] || n!=a[2] || n!=a[3];
  }


// Save to archive
FXStream& operator<<(FXStream& store,const FXMat4d& m){
  store << m[0] << m[1] << m[2] << m[3];
  return store;
  }


// Load from archive
FXStream& operator>>(FXStream& store,FXMat4d& m){
  store >> m[0] >> m[1] >> m[2] >> m[3];
  return store;
  }

}
