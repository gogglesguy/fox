/********************************************************************************
*                                                                               *
*            S i n g l e - P r e c i s i o n   3 x 3   M a t r i x              *
*                                                                               *
*********************************************************************************
* Copyright (C) 2003,2008 by Jeroen van der Zijp.   All Rights Reserved.        *
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
* $Id: FXMat3f.cpp,v 1.24 2008/01/04 15:42:24 fox Exp $                         *
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
#include "FXQuatf.h"
#include "FXMat3f.h"
#include "FXMat4f.h"


/*
  Notes:
  - Transformations pre-multiply.
  - Goal is same effect as OpenGL.
*/


#define DET2(a00,a01, \
             a10,a11) ((a00)*(a11)-(a10)*(a01))

#define DET3(a00,a01,a02, \
             a10,a11,a12, \
             a20,a21,a22) ((a00)*DET2(a11,a12,a21,a22) - \
                           (a10)*DET2(a01,a02,a21,a22) + \
                           (a20)*DET2(a01,a02,a11,a12))


using namespace FX;

/*******************************************************************************/

namespace FX {


// Initialize matrix from another matrix
FXMat3f::FXMat3f(const FXMat3f& other){
  m[0]=other[0];
  m[1]=other[1];
  m[2]=other[2];
  }


// Initialize from rotation and scaling part of 4x4 matrix
FXMat3f::FXMat3f(const FXMat4f& other){
  m[0][0]=other[0][0]; m[0][1]=other[0][1]; m[0][2]=other[0][2];
  m[1][0]=other[1][0]; m[1][1]=other[1][1]; m[1][2]=other[1][2];
  m[2][0]=other[2][0]; m[2][1]=other[2][1]; m[2][2]=other[2][2];
  }


// Initialize matrix from scalar
FXMat3f::FXMat3f(FXfloat w){
  m[0][0]=w; m[0][1]=w; m[0][2]=w;
  m[1][0]=w; m[1][1]=w; m[1][2]=w;
  m[2][0]=w; m[2][1]=w; m[2][2]=w;
  }


// Initialize diagonal matrix
FXMat3f::FXMat3f(FXfloat a,FXfloat b,FXfloat c){
  m[0][0]=a;    m[0][1]=0.0f; m[0][2]=0.0f;
  m[1][0]=0.0f; m[1][1]=b;    m[1][2]=0.0f;
  m[2][0]=0.0f; m[2][1]=0.0f; m[2][2]=c;
  }


// Initialize matrix from components
FXMat3f::FXMat3f(FXfloat a00,FXfloat a01,FXfloat a02,FXfloat a10,FXfloat a11,FXfloat a12,FXfloat a20,FXfloat a21,FXfloat a22){
  m[0][0]=a00; m[0][1]=a01; m[0][2]=a02;
  m[1][0]=a10; m[1][1]=a11; m[1][2]=a12;
  m[2][0]=a20; m[2][1]=a21; m[2][2]=a22;
  }


// Initialize matrix from three vectors
FXMat3f::FXMat3f(const FXVec3f& a,const FXVec3f& b,const FXVec3f& c){
  m[0]=a;
  m[1]=b;
  m[2]=c;
  }


// Initialize matrix from quaternion
FXMat3f::FXMat3f(const FXQuatf& quat){
  quat.getAxes(m[0],m[1],m[2]);
  }


// Assignment operator
FXMat3f& FXMat3f::operator=(const FXMat3f& other){
  m[0]=other[0];
  m[1]=other[1];
  m[2]=other[2];
  return *this;
  }


// Assign from rotation and scaling part of 4x4 matrix
FXMat3f& FXMat3f::operator=(const FXMat4f& other){
  m[0][0]=other[0][0]; m[0][1]=other[0][1]; m[0][2]=other[0][2];
  m[1][0]=other[1][0]; m[1][1]=other[1][1]; m[1][2]=other[1][2];
  m[2][0]=other[2][0]; m[2][1]=other[2][1]; m[2][2]=other[2][2];
  return *this;
  }


// Assign from scalar
FXMat3f& FXMat3f::operator=(FXfloat w){
  m[0][0]=w; m[0][1]=w; m[0][2]=w;
  m[1][0]=w; m[1][1]=w; m[1][2]=w;
  m[2][0]=w; m[2][1]=w; m[2][2]=w;
  return *this;
  }


// Set value from another matrix
FXMat3f& FXMat3f::set(const FXMat3f& other){
  m[0]=other[0];
  m[1]=other[1];
  m[2]=other[2];
  return *this;
  }


// Set from rotation and scaling part of 4x4 matrix
FXMat3f& FXMat3f::set(const FXMat4f& other){
  m[0][0]=other[0][0]; m[0][1]=other[0][1]; m[0][2]=other[0][2];
  m[1][0]=other[1][0]; m[1][1]=other[1][1]; m[1][2]=other[1][2];
  m[2][0]=other[2][0]; m[2][1]=other[2][1]; m[2][2]=other[2][2];
  return *this;
  }


// Set value from scalar
FXMat3f& FXMat3f::set(FXfloat w){
  m[0][0]=w; m[0][1]=w; m[0][2]=w;
  m[1][0]=w; m[1][1]=w; m[1][2]=w;
  m[2][0]=w; m[2][1]=w; m[2][2]=w;
  return *this;
  }


// Set diagonal matrix
FXMat3f& FXMat3f::set(FXfloat a,FXfloat b,FXfloat c){
  m[0][0]=a;    m[0][1]=0.0f; m[0][2]=0.0f;
  m[1][0]=0.0f; m[1][1]=b;    m[1][2]=0.0f;
  m[2][0]=0.0f; m[2][1]=0.0f; m[2][2]=c;
  return *this;
  }


// Set value from components
FXMat3f& FXMat3f::set(FXfloat a00,FXfloat a01,FXfloat a02,FXfloat a10,FXfloat a11,FXfloat a12,FXfloat a20,FXfloat a21,FXfloat a22){
  m[0][0]=a00; m[0][1]=a01; m[0][2]=a02;
  m[1][0]=a10; m[1][1]=a11; m[1][2]=a12;
  m[2][0]=a20; m[2][1]=a21; m[2][2]=a22;
  return *this;
  }


// Set value from three vectors
FXMat3f& FXMat3f::set(const FXVec3f& a,const FXVec3f& b,const FXVec3f& c){
  m[0]=a;
  m[1]=b;
  m[2]=c;
  return *this;
  }


// Set value from quaternion
FXMat3f& FXMat3f::set(const FXQuatf& quat){
  quat.getAxes(m[0],m[1],m[2]);
  return *this;
  }


// Add matrices
FXMat3f& FXMat3f::operator+=(const FXMat3f& w){
  m[0][0]+=w[0][0]; m[0][1]+=w[0][1]; m[0][2]+=w[0][2];
  m[1][0]+=w[1][0]; m[1][1]+=w[1][1]; m[1][2]+=w[1][2];
  m[2][0]+=w[2][0]; m[2][1]+=w[2][1]; m[2][2]+=w[2][2];
  return *this;
  }


// Substract matrices
FXMat3f& FXMat3f::operator-=(const FXMat3f& w){
  m[0][0]-=w[0][0]; m[0][1]-=w[0][1]; m[0][2]-=w[0][2];
  m[1][0]-=w[1][0]; m[1][1]-=w[1][1]; m[1][2]-=w[1][2];
  m[2][0]-=w[2][0]; m[2][1]-=w[2][1]; m[2][2]-=w[2][2];
  return *this;
  }


// Multiply matrix by scalar
FXMat3f& FXMat3f::operator*=(FXfloat w){
  m[0][0]*=w; m[0][1]*=w; m[0][2]*=w;
  m[1][0]*=w; m[1][1]*=w; m[1][2]*=w;
  m[2][0]*=w; m[2][1]*=w; m[2][2]*=w;
  return *this;
  }


// Multiply matrix by matrix
FXMat3f& FXMat3f::operator*=(const FXMat3f& w){
  register FXfloat x,y,z;
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
FXMat3f& FXMat3f::operator/=(FXfloat w){
  m[0][0]/=w; m[0][1]/=w; m[0][2]/=w;
  m[1][0]/=w; m[1][1]/=w; m[1][2]/=w;
  m[2][0]/=w; m[2][1]/=w; m[2][2]/=w;
  return *this;
  }


// Negate matrix
FXMat3f FXMat3f::operator-() const {
  return FXMat3f(-m[0][0],-m[0][1],-m[0][2],
                 -m[1][0],-m[1][1],-m[1][2],
                 -m[2][0],-m[2][1],-m[2][2]);
  }


// Add matrices
FXMat3f FXMat3f::operator+(const FXMat3f& w) const {
  return FXMat3f(m[0][0]+w[0][0],m[0][1]+w[0][1],m[0][2]+w[0][2],
                 m[1][0]+w[1][0],m[1][1]+w[1][1],m[1][2]+w[1][2],
                 m[2][0]+w[2][0],m[2][1]+w[2][1],m[2][2]+w[2][2]);
  }


// Substract matrices
FXMat3f FXMat3f::operator-(const FXMat3f& w) const {
  return FXMat3f(m[0][0]-w[0][0],m[0][1]-w[0][1],m[0][2]-w[0][2],
                 m[1][0]-w[1][0],m[1][1]-w[1][1],m[1][2]-w[1][2],
                 m[2][0]-w[2][0],m[2][1]-w[2][1],m[2][2]-w[2][2]);
  }


// Multiply matrices
FXMat3f FXMat3f::operator*(const FXMat3f& w) const {
  register FXfloat x,y,z;
  FXMat3f r;
  x=m[0][0]; y=m[0][1]; z=m[0][2];
  r[0][0]=x*w[0][0]+y*w[1][0]+z*w[2][0];
  r[0][1]=x*w[0][1]+y*w[1][1]+z*w[2][1];
  r[0][2]=x*w[0][2]+y*w[1][2]+z*w[2][2];
  x=m[1][0]; y=m[1][1]; z=m[1][2];
  r[1][0]=x*w[0][0]+y*w[1][0]+z*w[2][0];
  r[1][1]=x*w[0][1]+y*w[1][1]+z*w[2][1];
  r[1][2]=x*w[0][2]+y*w[1][2]+z*w[2][2];
  x=m[2][0]; y=m[2][1]; z=m[2][2];
  r[2][0]=x*w[0][0]+y*w[1][0]+z*w[2][0];
  r[2][1]=x*w[0][1]+y*w[1][1]+z*w[2][1];
  r[2][2]=x*w[0][2]+y*w[1][2]+z*w[2][2];
  return r;
  }


// Multiply scalar by matrix
FXMat3f operator*(FXfloat x,const FXMat3f& m){
  return FXMat3f(x*m[0][0],x*m[0][1],x*m[0][2],
                 x*m[1][0],x*m[1][1],x*m[1][2],
                 x*m[2][0],x*m[2][1],x*m[2][2]);
  }


// Multiply matrix by scalar
FXMat3f operator*(const FXMat3f& m,FXfloat x){
  return FXMat3f(m[0][0]*x,m[0][1]*x,m[0][2]*x,
                 m[1][0]*x,m[1][1]*x,m[1][2]*x,
                 m[2][0]*x,m[2][1]*x,m[2][2]*x);
  }


// Divide scalar by matrix
FXMat3f operator/(FXfloat x,const FXMat3f& m){
  return FXMat3f(x/m[0][0],x/m[0][1],x/m[0][2],
                 x/m[1][0],x/m[1][1],x/m[1][2],
                 x/m[2][0],x/m[2][1],x/m[2][2]);
  }


// Divide matrix by scalar
FXMat3f operator/(const FXMat3f& m,FXfloat x){
  return FXMat3f(m[0][0]/x,m[0][1]/x,m[0][2]/x,
                 m[1][0]/x,m[1][1]/x,m[1][2]/x,
                 m[2][0]/x,m[2][1]/x,m[2][2]/x);
  }


// Matrix times vector
FXVec3f FXMat3f::operator*(const FXVec3f& v) const {
  register FXfloat x=v.x,y=v.y,z=v.z;
  return FXVec3f(x*m[0][0]+y*m[0][1]+z*m[0][2], x*m[1][0]+y*m[1][1]+z*m[1][2], x*m[2][0]+y*m[2][1]+z*m[2][2]);
  }


// Matrix times vector
FXVec2f FXMat3f::operator*(const FXVec2f& v) const {
  register FXfloat x=v.x,y=v.y;
  FXASSERT(m[0][2]==0.0f && m[1][2]==0.0f && m[2][2]==1.0f);
  return FXVec2f(x*m[0][0]+y*m[0][1]+m[0][2], x*m[1][0]+y*m[1][1]+m[1][2]);
  }


// Equality
FXbool FXMat3f::operator==(const FXMat3f& a) const { return m[0]==a.m[0] && m[1]==a.m[1] && m[2]==a.m[2]; }


// Inequality
FXbool FXMat3f::operator!=(const FXMat3f& a) const { return m[0]!=a.m[0] || m[1]!=a.m[1] || m[2]!=a.m[2]; }


// Equality
FXbool operator==(const FXMat3f& a,FXfloat n){ return a[0]==n && a[1]==n && a[2]==n; }
FXbool operator==(FXfloat n,const FXMat3f& a){ return n==a[0] && n==a[1] && n==a[2]; }


// Inequality
FXbool operator!=(const FXMat3f& a,FXfloat n){ return a[0]!=n || a[1]!=n || a[2]!=n; }
FXbool operator!=(FXfloat n,const FXMat3f& a){ return n!=a[0] || n!=a[1] || n!=a[2]; }


// Set to identity matrix
FXMat3f& FXMat3f::identity(){
  m[0][0]=1.0f; m[0][1]=0.0f; m[0][2]=0.0f;
  m[1][0]=0.0f; m[1][1]=1.0f; m[1][2]=0.0f;
  m[2][0]=0.0f; m[2][1]=0.0f; m[2][2]=1.0f;
  return *this;
  }


// Return true if identity matrix
FXbool FXMat3f::isIdentity() const {
  return m[0][0]==1.0f && m[0][1]==0.0f && m[0][2]==0.0f &&
         m[1][0]==0.0f && m[1][1]==1.0f && m[1][2]==0.0f &&
         m[2][0]==0.0f && m[2][1]==0.0f && m[2][2]==1.0f;
  }


// Rotate by cosine, sine
FXMat3f& FXMat3f::rot(FXfloat c,FXfloat s){
  register FXfloat u,v;
  FXASSERT(-1.00001f<c && c<1.00001f && -1.00001f<s && s<1.00001f);
  u=m[0][0]; v=m[1][0]; m[0][0]=c*u+s*v; m[1][0]=c*v-s*u;
  u=m[0][1]; v=m[1][1]; m[0][1]=c*u+s*v; m[1][1]=c*v-s*u;
  u=m[0][2]; v=m[1][2]; m[0][2]=c*u+s*v; m[1][2]=c*v-s*u;
  return *this;
  }


// Rotate by angle
FXMat3f& FXMat3f::rot(FXfloat phi){
  return rot(cosf(phi),sinf(phi));
  }


// Translate
FXMat3f& FXMat3f::trans(FXfloat tx,FXfloat ty){
  m[2][0]=m[2][0]+tx*m[0][0]+ty*m[1][0];
  m[2][1]=m[2][1]+tx*m[0][1]+ty*m[1][1];
  m[2][2]=m[2][2]+tx*m[0][2]+ty*m[1][2];
  return *this;
  }


// Scale unqual
FXMat3f& FXMat3f::scale(FXfloat sx,FXfloat sy){
  m[0][0]*=sx; m[0][1]*=sx; m[0][2]*=sx;
  m[1][0]*=sy; m[1][1]*=sy; m[1][2]*=sy;
  return *this;
  }


// Scale uniform
FXMat3f& FXMat3f::scale(FXfloat s){
  return scale(s,s);
  }


// Calculate determinant
FXfloat FXMat3f::det() const {
  return DET3(m[0][0],m[0][1],m[0][2],
              m[1][0],m[1][1],m[1][2],
              m[2][0],m[2][1],m[2][2]);
  }


// Transpose matrix
FXMat3f FXMat3f::transpose() const {
  return FXMat3f(m[0][0],m[1][0],m[2][0],
                 m[0][1],m[1][1],m[2][1],
                 m[0][2],m[1][2],m[2][2]);
  }


// Invert matrix
FXMat3f FXMat3f::invert() const {
  register FXfloat dt;
  FXMat3f res;
  res[0][0]=m[1][1]*m[2][2]-m[1][2]*m[2][1];
  res[0][1]=m[0][2]*m[2][1]-m[0][1]*m[2][2];
  res[0][2]=m[0][1]*m[1][2]-m[0][2]*m[1][1];

  res[1][0]=m[1][2]*m[2][0]-m[1][0]*m[2][2];
  res[1][1]=m[0][0]*m[2][2]-m[0][2]*m[2][0];
  res[1][2]=m[0][2]*m[1][0]-m[0][0]*m[1][2];

  res[2][0]=m[1][0]*m[2][1]-m[1][1]*m[2][0];
  res[2][1]=m[0][1]*m[2][0]-m[0][0]*m[2][1];
  res[2][2]=m[0][0]*m[1][1]-m[0][1]*m[1][0];

  dt=m[0][0]*res[0][0]+m[0][1]*res[1][0]+m[0][2]*res[2][0];
  //if(det==0.0f) throw FXException("FXMat3f is singular.");
  FXASSERT(dt!=0.0f);

  res[0][0]/=dt;
  res[0][1]/=dt;
  res[0][2]/=dt;
  res[1][0]/=dt;
  res[1][1]/=dt;
  res[1][2]/=dt;
  res[2][0]/=dt;
  res[2][1]/=dt;
  res[2][2]/=dt;
  return res;
  }


// Save to archive
FXStream& operator<<(FXStream& store,const FXMat3f& m){
  store << m[0] << m[1] << m[2];
  return store;
  }


// Load from archive
FXStream& operator>>(FXStream& store,FXMat3f& m){
  store >> m[0] >> m[1] >> m[2];
  return store;
  }

}
