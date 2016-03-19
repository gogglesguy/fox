/********************************************************************************
*                                                                               *
*            D o u b l e - P r e c i s i o n   3 x 3   M a t r i x              *
*                                                                               *
*********************************************************************************
* Copyright (C) 2003,2010 by Jeroen van der Zijp.   All Rights Reserved.        *
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
FXMat3d::FXMat3d(FXdouble s){
  m[0][0]=s; m[0][1]=s; m[0][2]=s;
  m[1][0]=s; m[1][1]=s; m[1][2]=s;
  m[2][0]=s; m[2][1]=s; m[2][2]=s;
  }


// Initialize matrix from another matrix
FXMat3d::FXMat3d(const FXMat3d& s){
  m[0][0]=s[0][0]; m[0][1]=s[0][1]; m[0][2]=s[0][2];
  m[1][0]=s[1][0]; m[1][1]=s[1][1]; m[1][2]=s[1][2];
  m[2][0]=s[2][0]; m[2][1]=s[2][1]; m[2][2]=s[2][2];
  }


// Initialize from rotation and scaling part of 4x4 matrix
FXMat3d::FXMat3d(const FXMat4d& s){
  m[0][0]=s[0][0]; m[0][1]=s[0][1]; m[0][2]=s[0][2];
  m[1][0]=s[1][0]; m[1][1]=s[1][1]; m[1][2]=s[1][2];
  m[2][0]=s[2][0]; m[2][1]=s[2][1]; m[2][2]=s[2][2];
  }


// Initialize matrix from array
FXMat3d::FXMat3d(const FXdouble s[]){
  m[0][0]=s[0]; m[0][1]=s[1]; m[0][2]=s[2];
  m[1][0]=s[3]; m[1][1]=s[4]; m[1][2]=s[5];
  m[2][0]=s[6]; m[2][1]=s[7]; m[2][2]=s[8];
  }


// Initialize diagonal matrix
FXMat3d::FXMat3d(FXdouble a,FXdouble b,FXdouble c){
  m[0][0]=a;   m[0][1]=0.0; m[0][2]=0.0;
  m[1][0]=0.0; m[1][1]=b;   m[1][2]=0.0;
  m[2][0]=0.0; m[2][1]=0.0; m[2][2]=c;
  }


// Initialize matrix from components
FXMat3d::FXMat3d(FXdouble a00,FXdouble a01,FXdouble a02,FXdouble a10,FXdouble a11,FXdouble a12,FXdouble a20,FXdouble a21,FXdouble a22){
  m[0][0]=a00; m[0][1]=a01; m[0][2]=a02;
  m[1][0]=a10; m[1][1]=a11; m[1][2]=a12;
  m[2][0]=a20; m[2][1]=a21; m[2][2]=a22;
  }


// Initialize matrix from three vectors
FXMat3d::FXMat3d(const FXVec3d& a,const FXVec3d& b,const FXVec3d& c){
  m[0]=a;
  m[1]=b;
  m[2]=c;
  }


// Initialize matrix from quaternion
FXMat3d::FXMat3d(const FXQuatd& quat){
  quat.getAxes(m[0],m[1],m[2]);
  }


// Assign from scalar
FXMat3d& FXMat3d::operator=(FXdouble s){
  m[0][0]=s; m[0][1]=s; m[0][2]=s;
  m[1][0]=s; m[1][1]=s; m[1][2]=s;
  m[2][0]=s; m[2][1]=s; m[2][2]=s;
  return *this;
  }


// Assignment operator
FXMat3d& FXMat3d::operator=(const FXMat3d& s){
  m[0][0]=s[0][0]; m[0][1]=s[0][1]; m[0][2]=s[0][2];
  m[1][0]=s[1][0]; m[1][1]=s[1][1]; m[1][2]=s[1][2];
  m[2][0]=s[2][0]; m[2][1]=s[2][1]; m[2][2]=s[2][2];
  return *this;
  }


// Assign from rotation and scaling part of 4x4 matrix
FXMat3d& FXMat3d::operator=(const FXMat4d& s){
  m[0][0]=s[0][0]; m[0][1]=s[0][1]; m[0][2]=s[0][2];
  m[1][0]=s[1][0]; m[1][1]=s[1][1]; m[1][2]=s[1][2];
  m[2][0]=s[2][0]; m[2][1]=s[2][1]; m[2][2]=s[2][2];
  return *this;
  }


// Assignment from array
FXMat3d& FXMat3d::operator=(const FXdouble s[]){
  m[0][0]=s[0]; m[0][1]=s[1]; m[0][2]=s[2];
  m[1][0]=s[3]; m[1][1]=s[4]; m[1][2]=s[5];
  m[2][0]=s[6]; m[2][1]=s[7]; m[2][2]=s[8];
  return *this;
  }


// Set value from scalar
FXMat3d& FXMat3d::set(FXdouble s){
  m[0][0]=s; m[0][1]=s; m[0][2]=s;
  m[1][0]=s; m[1][1]=s; m[1][2]=s;
  m[2][0]=s; m[2][1]=s; m[2][2]=s;
  return *this;
  }


// Set value from another matrix
FXMat3d& FXMat3d::set(const FXMat3d& s){
  m[0][0]=s[0][0]; m[0][1]=s[0][1]; m[0][2]=s[0][2];
  m[1][0]=s[1][0]; m[1][1]=s[1][1]; m[1][2]=s[1][2];
  m[2][0]=s[2][0]; m[2][1]=s[2][1]; m[2][2]=s[2][2];
  return *this;
  }


// Set from rotation and scaling part of 4x4 matrix
FXMat3d& FXMat3d::set(const FXMat4d& s){
  m[0][0]=s[0][0]; m[0][1]=s[0][1]; m[0][2]=s[0][2];
  m[1][0]=s[1][0]; m[1][1]=s[1][1]; m[1][2]=s[1][2];
  m[2][0]=s[2][0]; m[2][1]=s[2][1]; m[2][2]=s[2][2];
  return *this;
  }


// Set value from array
FXMat3d& FXMat3d::set(const FXdouble s[]){
  m[0][0]=s[0]; m[0][1]=s[1]; m[0][2]=s[2];
  m[1][0]=s[3]; m[1][1]=s[4]; m[1][2]=s[5];
  m[2][0]=s[6]; m[2][1]=s[7]; m[2][2]=s[8];
  return *this;
  }


// Set diagonal matrix
FXMat3d& FXMat3d::set(FXdouble a,FXdouble b,FXdouble c){
  m[0][0]=a;   m[0][1]=0.0; m[0][2]=0.0;
  m[1][0]=0.0; m[1][1]=b;   m[1][2]=0.0;
  m[2][0]=0.0; m[2][1]=0.0; m[2][2]=c;
  return *this;
  }


// Set value from components
FXMat3d& FXMat3d::set(FXdouble a00,FXdouble a01,FXdouble a02,FXdouble a10,FXdouble a11,FXdouble a12,FXdouble a20,FXdouble a21,FXdouble a22){
  m[0][0]=a00; m[0][1]=a01; m[0][2]=a02;
  m[1][0]=a10; m[1][1]=a11; m[1][2]=a12;
  m[2][0]=a20; m[2][1]=a21; m[2][2]=a22;
  return *this;
  }


// Set value from three vectors
FXMat3d& FXMat3d::set(const FXVec3d& a,const FXVec3d& b,const FXVec3d& c){
  m[0]=a;
  m[1]=b;
  m[2]=c;
  return *this;
  }


// Set value from quaternion
FXMat3d& FXMat3d::set(const FXQuatd& quat){
  quat.getAxes(m[0],m[1],m[2]);
  return *this;
  }


// Add matrices
FXMat3d& FXMat3d::operator+=(const FXMat3d& w){
  m[0][0]+=w[0][0]; m[0][1]+=w[0][1]; m[0][2]+=w[0][2];
  m[1][0]+=w[1][0]; m[1][1]+=w[1][1]; m[1][2]+=w[1][2];
  m[2][0]+=w[2][0]; m[2][1]+=w[2][1]; m[2][2]+=w[2][2];
  return *this;
  }


// Substract matrices
FXMat3d& FXMat3d::operator-=(const FXMat3d& w){
  m[0][0]-=w[0][0]; m[0][1]-=w[0][1]; m[0][2]-=w[0][2];
  m[1][0]-=w[1][0]; m[1][1]-=w[1][1]; m[1][2]-=w[1][2];
  m[2][0]-=w[2][0]; m[2][1]-=w[2][1]; m[2][2]-=w[2][2];
  return *this;
  }


// Multiply matrix by scalar
FXMat3d& FXMat3d::operator*=(FXdouble w){
  m[0][0]*=w; m[0][1]*=w; m[0][2]*=w;
  m[1][0]*=w; m[1][1]*=w; m[1][2]*=w;
  m[2][0]*=w; m[2][1]*=w; m[2][2]*=w;
  return *this;
  }


// Multiply matrix by matrix
FXMat3d& FXMat3d::operator*=(const FXMat3d& w){
  register FXdouble x,y,z;
  x=m[0][0]; y=m[0][1]; z=m[0][2];
  m[0][0]=x*w[0][0]+y*w[1][0]+z*w[2][0];
  m[0][1]=x*w[0][1]+y*w[1][1]+z*w[2][1];
  m[0][2]=x*w[0][2]+y*w[1][2]+z*w[2][2];
  x=m[1][0]; y=m[1][1]; z=m[1][2];
  m[1][0]=x*w[0][0]+y*w[1][0]+z*w[2][0];
  m[1][1]=x*w[0][1]+y*w[1][1]+z*w[2][1];
  m[1][2]=x*w[0][2]+y*w[1][2]+z*w[2][2];
  x=m[2][0]; y=m[2][1]; z=m[2][2];
  m[2][0]=x*w[0][0]+y*w[1][0]+z*w[2][0];
  m[2][1]=x*w[0][1]+y*w[1][1]+z*w[2][1];
  m[2][2]=x*w[0][2]+y*w[1][2]+z*w[2][2];
  return *this;
  }


// Divide matrix by scalar
FXMat3d& FXMat3d::operator/=(FXdouble w){
  m[0][0]/=w; m[0][1]/=w; m[0][2]/=w;
  m[1][0]/=w; m[1][1]/=w; m[1][2]/=w;
  m[2][0]/=w; m[2][1]/=w; m[2][2]/=w;
  return *this;
  }


// Negate matrix
FXMat3d FXMat3d::operator-() const {
  return FXMat3d(-m[0][0],-m[0][1],-m[0][2],
                 -m[1][0],-m[1][1],-m[1][2],
                 -m[2][0],-m[2][1],-m[2][2]);
  }


// Set to identity matrix
FXMat3d& FXMat3d::identity(){
  m[0][0]=1.0; m[0][1]=0.0; m[0][2]=0.0;
  m[1][0]=0.0; m[1][1]=1.0; m[1][2]=0.0;
  m[2][0]=0.0; m[2][1]=0.0; m[2][2]=1.0;
  return *this;
  }


// Return true if identity matrix
FXbool FXMat3d::isIdentity() const {
  return m[0][0]==1.0 && m[0][1]==0.0 && m[0][2]==0.0 &&
         m[1][0]==0.0 && m[1][1]==1.0 && m[1][2]==0.0 &&
         m[2][0]==0.0 && m[2][1]==0.0 && m[2][2]==1.0;
  }


// Rotate by cosine, sine
FXMat3d& FXMat3d::rot(FXdouble c,FXdouble s){
  register FXdouble u,v;
  FXASSERT(-1.00001<c && c<1.00001 && -1.00001<s && s<1.00001);
  u=m[0][0]; v=m[1][0]; m[0][0]=c*u+s*v; m[1][0]=c*v-s*u;
  u=m[0][1]; v=m[1][1]; m[0][1]=c*u+s*v; m[1][1]=c*v-s*u;
  u=m[0][2]; v=m[1][2]; m[0][2]=c*u+s*v; m[1][2]=c*v-s*u;
  return *this;
  }


// Rotate by angle
FXMat3d& FXMat3d::rot(FXdouble phi){
  return rot(cos(phi),sin(phi));
  }


// Translate
FXMat3d& FXMat3d::trans(FXdouble tx,FXdouble ty){
  m[2][0]=m[2][0]+tx*m[0][0]+ty*m[1][0];
  m[2][1]=m[2][1]+tx*m[0][1]+ty*m[1][1];
  m[2][2]=m[2][2]+tx*m[0][2]+ty*m[1][2];
  return *this;
  }


// Scale unqual
FXMat3d& FXMat3d::scale(FXdouble sx,FXdouble sy){
  m[0][0]*=sx; m[0][1]*=sx; m[0][2]*=sx;
  m[1][0]*=sy; m[1][1]*=sy; m[1][2]*=sy;
  return *this;
  }


// Scale uniform
FXMat3d& FXMat3d::scale(FXdouble s){
  return scale(s,s);
  }


// Calculate determinant
FXdouble FXMat3d::det() const {
  return m[0][0]*(m[1][1]*m[2][2]-m[2][1]*m[1][2])+
         m[0][1]*(m[2][0]*m[1][2]-m[1][0]*m[2][2])+
         m[0][2]*(m[1][0]*m[2][1]-m[2][0]*m[1][1]);
  }


// Transpose matrix
FXMat3d FXMat3d::transpose() const {
  return FXMat3d(m[0][0],m[1][0],m[2][0],
                 m[0][1],m[1][1],m[2][1],
                 m[0][2],m[1][2],m[2][2]);
  }


// Invert matrix
FXMat3d FXMat3d::invert() const {
  register FXdouble dd;
  FXMat3d res;
  res[0][0]=m[1][1]*m[2][2]-m[1][2]*m[2][1];
  res[0][1]=m[0][2]*m[2][1]-m[0][1]*m[2][2];
  res[0][2]=m[0][1]*m[1][2]-m[0][2]*m[1][1];
  res[1][0]=m[1][2]*m[2][0]-m[1][0]*m[2][2];
  res[1][1]=m[0][0]*m[2][2]-m[0][2]*m[2][0];
  res[1][2]=m[0][2]*m[1][0]-m[0][0]*m[1][2];
  res[2][0]=m[1][0]*m[2][1]-m[1][1]*m[2][0];
  res[2][1]=m[0][1]*m[2][0]-m[0][0]*m[2][1];
  res[2][2]=m[0][0]*m[1][1]-m[0][1]*m[1][0];
  dd=m[0][0]*res[0][0]+m[0][1]*res[1][0]+m[0][2]*res[2][0];
  FXASSERT(dd!=0.0);
  dd=1.0/dd;
  res[0][0]*=dd;
  res[0][1]*=dd;
  res[0][2]*=dd;
  res[1][0]*=dd;
  res[1][1]*=dd;
  res[1][2]*=dd;
  res[2][0]*=dd;
  res[2][1]*=dd;
  res[2][2]*=dd;
  return res;
  }


// Matrix times vector
FXVec2d operator*(const FXMat3d& m,const FXVec2d& v){
  return FXVec2d(m[0][0]*v[0]+m[0][1]*v[1]+m[0][2], m[1][0]*v[0]+m[1][1]*v[1]+m[1][2]);
  }


// Matrix times vector
FXVec3d operator*(const FXMat3d& m,const FXVec3d& v){
  return FXVec3d(m[0][0]*v[0]+m[0][1]*v[1]+m[0][2]*v[2], m[1][0]*v[0]+m[1][1]*v[1]+m[1][2]*v[2], m[2][0]*v[0]+m[2][1]*v[1]+m[2][2]*v[2]);
  }


// Vector times matrix
FXVec2d operator*(const FXVec2d& v,const FXMat3d& m){
  return FXVec2d(v[0]*m[0][0]+v[1]*m[1][0]+m[2][0],v[0]*m[0][1]+v[1]*m[1][1]+m[2][1]);
  }


// Vector times matrix
FXVec3d operator*(const FXVec3d& v,const FXMat3d& m){
  return FXVec3d(v[0]*m[0][0]+v[1]*m[1][0]+v[2]*m[2][0], v[0]*m[0][1]+v[1]*m[1][1]+v[2]*m[2][1], v[0]*m[0][2]+v[1]*m[1][2]+v[2]*m[2][2]);
  }


// Matrix and matrix add
FXMat3d operator+(const FXMat3d& a,const FXMat3d& b){
  return FXMat3d(a[0][0]+b[0][0],a[0][1]+b[0][1],a[0][2]+b[0][2],
                 a[1][0]+b[1][0],a[1][1]+b[1][1],a[1][2]+b[1][2],
                 a[2][0]+b[2][0],a[2][1]+b[2][1],a[2][2]+b[2][2]);
  }


// Matrix and matrix subtract
FXMat3d operator-(const FXMat3d& a,const FXMat3d& b){
  return FXMat3d(a[0][0]-b[0][0],a[0][1]-b[0][1],a[0][2]-b[0][2],
                 a[1][0]-b[1][0],a[1][1]-b[1][1],a[1][2]-b[1][2],
                 a[2][0]-b[2][0],a[2][1]-b[2][1],a[2][2]-b[2][2]);
  }


// Matrix and matrix multiply
FXMat3d operator*(const FXMat3d& a,const FXMat3d& b){
  register FXdouble x,y,z;
  FXMat3d r;
  x=a[0][0]; y=a[0][1]; z=a[0][2];
  r[0][0]=x*b[0][0]+y*b[1][0]+z*b[2][0];
  r[0][1]=x*b[0][1]+y*b[1][1]+z*b[2][1];
  r[0][2]=x*b[0][2]+y*b[1][2]+z*b[2][2];
  x=a[1][0]; y=a[1][1]; z=a[1][2];
  r[1][0]=x*b[0][0]+y*b[1][0]+z*b[2][0];
  r[1][1]=x*b[0][1]+y*b[1][1]+z*b[2][1];
  r[1][2]=x*b[0][2]+y*b[1][2]+z*b[2][2];
  x=a[2][0]; y=a[2][1]; z=a[2][2];
  r[2][0]=x*b[0][0]+y*b[1][0]+z*b[2][0];
  r[2][1]=x*b[0][1]+y*b[1][1]+z*b[2][1];
  r[2][2]=x*b[0][2]+y*b[1][2]+z*b[2][2];
  return r;
  }


// Multiply scalar by matrix
FXMat3d operator*(FXdouble x,const FXMat3d& m){
  return FXMat3d(x*m[0][0],x*m[0][1],x*m[0][2],
                 x*m[1][0],x*m[1][1],x*m[1][2],
                 x*m[2][0],x*m[2][1],x*m[2][2]);
  }


// Multiply matrix by scalar
FXMat3d operator*(const FXMat3d& m,FXdouble x){
  return FXMat3d(m[0][0]*x,m[0][1]*x,m[0][2]*x,
                 m[1][0]*x,m[1][1]*x,m[1][2]*x,
                 m[2][0]*x,m[2][1]*x,m[2][2]*x);
  }


// Divide scalar by matrix
FXMat3d operator/(FXdouble x,const FXMat3d& m){
  return FXMat3d(x/m[0][0],x/m[0][1],x/m[0][2],
                 x/m[1][0],x/m[1][1],x/m[1][2],
                 x/m[2][0],x/m[2][1],x/m[2][2]);
  }


// Divide matrix by scalar
FXMat3d operator/(const FXMat3d& m,FXdouble x){
  return FXMat3d(m[0][0]/x,m[0][1]/x,m[0][2]/x,
                 m[1][0]/x,m[1][1]/x,m[1][2]/x,
                 m[2][0]/x,m[2][1]/x,m[2][2]/x);
  }


// Matrix and matrix equality
FXbool operator==(const FXMat3d& a,const FXMat3d& b){
  return a[0]==b[0] && a[1]==b[1] && a[2]==b[2];
  }


// Matrix and matrix inequality
FXbool operator!=(const FXMat3d& a,const FXMat3d& b){
  return a[0]!=b[0] || a[1]!=b[1] || a[2]!=b[2];
  }


// Matrix and scalar equality
FXbool operator==(const FXMat3d& a,FXdouble n){
  return a[0]==n && a[1]==n && a[2]==n;
  }


// Matrix and scalar inequality
FXbool operator!=(const FXMat3d& a,FXdouble n){
  return a[0]!=n || a[1]!=n || a[2]!=n;
  }


// Scalar and matrix equality
FXbool operator==(FXdouble n,const FXMat3d& a){
  return n==a[0] && n==a[1] && n==a[2];
  }


// Scalar and matrix inequality
FXbool operator!=(FXdouble n,const FXMat3d& a){
  return n!=a[0] || n!=a[1] || n!=a[2];
  }


// Save to archive
FXStream& operator<<(FXStream& store,const FXMat3d& m){
  store << m[0] << m[1] << m[2];
  return store;
  }


// Load from archive
FXStream& operator>>(FXStream& store,FXMat3d& m){
  store >> m[0] >> m[1] >> m[2];
  return store;
  }

}
