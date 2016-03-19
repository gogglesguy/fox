/********************************************************************************
*                                                                               *
*            S i n g l e - P r e c i s i o n   4 x 4   M a t r i x              *
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


using namespace FX;

/*******************************************************************************/

namespace FX {


// Initialize matrix from scalar
FXMat4f::FXMat4f(FXfloat s){
  m[0][0]=s; m[0][1]=s; m[0][2]=s; m[0][3]=s;
  m[1][0]=s; m[1][1]=s; m[1][2]=s; m[1][3]=s;
  m[2][0]=s; m[2][1]=s; m[2][2]=s; m[2][3]=s;
  m[3][0]=s; m[3][1]=s; m[3][2]=s; m[3][3]=s;
  }


// Initialize with 3x3 rotation and scaling matrix
FXMat4f::FXMat4f(const FXMat3f& s){
  m[0][0]=s[0][0]; m[0][1]=s[0][1]; m[0][2]=s[0][2]; m[0][3]=0.0f;
  m[1][0]=s[1][0]; m[1][1]=s[1][1]; m[1][2]=s[1][2]; m[1][3]=0.0f;
  m[2][0]=s[2][0]; m[2][1]=s[2][1]; m[2][2]=s[2][2]; m[2][3]=0.0f;
  m[3][0]=0.0f;    m[3][1]=0.0f;    m[3][2]=0.0f;    m[3][3]=1.0f;
  }


// Initialize matrix from another matrix
FXMat4f::FXMat4f(const FXMat4f& other){
  m[0]=other[0];
  m[1]=other[1];
  m[2]=other[2];
  m[3]=other[3];
  }


// Initialize matrix from array
FXMat4f::FXMat4f(const FXfloat s[]){
  m[0][0]=s[0];  m[0][1]=s[1];  m[0][2]=s[2];  m[0][3]=s[3];
  m[1][0]=s[4];  m[1][1]=s[5];  m[1][2]=s[6];  m[1][3]=s[7];
  m[2][0]=s[8];  m[2][1]=s[9];  m[2][2]=s[10]; m[2][3]=s[11];
  m[3][0]=s[12]; m[3][1]=s[13]; m[3][2]=s[14]; m[3][3]=s[15];
  }


// Initialize diagonal matrix
FXMat4f::FXMat4f(FXfloat a,FXfloat b,FXfloat c,FXfloat d){
  m[0][0]=a;    m[0][1]=0.0f; m[0][2]=0.0f; m[0][3]=0.0f;
  m[1][0]=0.0f; m[1][1]=b;    m[1][2]=0.0f; m[1][3]=0.0f;
  m[2][0]=0.0f; m[2][1]=0.0f; m[2][2]=c;    m[2][3]=0.0f;
  m[3][0]=0.0f; m[3][1]=0.0f; m[3][2]=0.0f; m[3][3]=d;
  }


// Initialize matrix from components
FXMat4f::FXMat4f(FXfloat a00,FXfloat a01,FXfloat a02,FXfloat a03,FXfloat a10,FXfloat a11,FXfloat a12,FXfloat a13,FXfloat a20,FXfloat a21,FXfloat a22,FXfloat a23,FXfloat a30,FXfloat a31,FXfloat a32,FXfloat a33){
  m[0][0]=a00; m[0][1]=a01; m[0][2]=a02; m[0][3]=a03;
  m[1][0]=a10; m[1][1]=a11; m[1][2]=a12; m[1][3]=a13;
  m[2][0]=a20; m[2][1]=a21; m[2][2]=a22; m[2][3]=a23;
  m[3][0]=a30; m[3][1]=a31; m[3][2]=a32; m[3][3]=a33;
  }


// Initialize matrix from four vectors
FXMat4f::FXMat4f(const FXVec4f& a,const FXVec4f& b,const FXVec4f& c,const FXVec4f& d){
  m[0][0]=a[0]; m[0][1]=a[1]; m[0][2]=a[2]; m[0][3]=a[3];
  m[1][0]=b[0]; m[1][1]=b[1]; m[1][2]=b[2]; m[1][3]=b[3];
  m[2][0]=c[0]; m[2][1]=c[1]; m[2][2]=c[2]; m[2][3]=c[3];
  m[3][0]=d[0]; m[3][1]=d[1]; m[3][2]=d[2]; m[3][3]=d[3];
  }


// Assign from scalar
FXMat4f& FXMat4f::operator=(FXfloat s){
  m[0][0]=s; m[0][1]=s; m[0][2]=s; m[0][3]=s;
  m[1][0]=s; m[1][1]=s; m[1][2]=s; m[1][3]=s;
  m[2][0]=s; m[2][1]=s; m[2][2]=s; m[2][3]=s;
  m[3][0]=s; m[3][1]=s; m[3][2]=s; m[3][3]=s;
  return *this;
  }


// Assign from 3x3 rotation and scaling matrix
FXMat4f& FXMat4f::operator=(const FXMat3f& s){
  m[0][0]=s[0][0]; m[0][1]=s[0][1]; m[0][2]=s[0][2]; m[0][3]=0.0f;
  m[1][0]=s[1][0]; m[1][1]=s[1][1]; m[1][2]=s[1][2]; m[1][3]=0.0f;
  m[2][0]=s[2][0]; m[2][1]=s[2][1]; m[2][2]=s[2][2]; m[2][3]=0.0f;
  m[3][0]=0.0f;    m[3][1]=0.0f;    m[3][2]=0.0f;    m[3][3]=1.0f;
  return *this;
  }


// Assignment operator
FXMat4f& FXMat4f::operator=(const FXMat4f& s){
  m[0]=s[0];
  m[1]=s[1];
  m[2]=s[2];
  m[3]=s[3];
  return *this;
  }


// Assignment from array
FXMat4f& FXMat4f::operator=(const FXfloat s[]){
  m[0][0]=s[0];  m[0][1]=s[1];  m[0][2]=s[2];  m[0][3]=s[3];
  m[1][0]=s[4];  m[1][1]=s[5];  m[1][2]=s[6];  m[1][3]=s[7];
  m[2][0]=s[8];  m[2][1]=s[9];  m[2][2]=s[10]; m[2][3]=s[11];
  m[3][0]=s[12]; m[3][1]=s[13]; m[3][2]=s[14]; m[3][3]=s[15];
  return *this;
  }


// Set value from scalar
FXMat4f& FXMat4f::set(FXfloat s){
  m[0][0]=s; m[0][1]=s; m[0][2]=s; m[0][3]=s;
  m[1][0]=s; m[1][1]=s; m[1][2]=s; m[1][3]=s;
  m[2][0]=s; m[2][1]=s; m[2][2]=s; m[2][3]=s;
  m[3][0]=s; m[3][1]=s; m[3][2]=s; m[3][3]=s;
  return *this;
  }


// Set from 3x3 rotation and scaling matrix
FXMat4f& FXMat4f::set(const FXMat3f& s){
  m[0][0]=s[0][0]; m[0][1]=s[0][1]; m[0][2]=s[0][2]; m[0][3]=0.0f;
  m[1][0]=s[1][0]; m[1][1]=s[1][1]; m[1][2]=s[1][2]; m[1][3]=0.0f;
  m[2][0]=s[2][0]; m[2][1]=s[2][1]; m[2][2]=s[2][2]; m[2][3]=0.0f;
  m[3][0]=0.0f;    m[3][1]=0.0f;    m[3][2]=0.0f;    m[3][3]=1.0f;
  return *this;
  }


// Set value from another matrix
FXMat4f& FXMat4f::set(const FXMat4f& s){
  m[0]=s[0];
  m[1]=s[1];
  m[2]=s[2];
  m[3]=s[3];
  return *this;
  }


// Set value from array
FXMat4f& FXMat4f::set(const FXfloat s[]){
  m[0][0]=s[0];  m[0][1]=s[1];  m[0][2]=s[2];  m[0][3]=s[3];
  m[1][0]=s[4];  m[1][1]=s[5];  m[1][2]=s[6];  m[1][3]=s[7];
  m[2][0]=s[8];  m[2][1]=s[9];  m[2][2]=s[10]; m[2][3]=s[11];
  m[3][0]=s[12]; m[3][1]=s[13]; m[3][2]=s[14]; m[3][3]=s[15];
  return *this;
  }


// Set diagonal matrix
FXMat4f& FXMat4f::set(FXfloat a,FXfloat b,FXfloat c,FXfloat d){
  m[0][0]=a;    m[0][1]=0.0f; m[0][2]=0.0f; m[0][3]=0.0f;
  m[1][0]=0.0f; m[1][1]=b;    m[1][2]=0.0f; m[1][3]=0.0f;
  m[2][0]=0.0f; m[2][1]=0.0f; m[2][2]=c;    m[2][3]=0.0f;
  m[3][0]=0.0f; m[3][1]=0.0f; m[3][2]=0.0f; m[3][3]=d;
  return *this;
  }


// Set value from components
FXMat4f& FXMat4f::set(FXfloat a00,FXfloat a01,FXfloat a02,FXfloat a03,FXfloat a10,FXfloat a11,FXfloat a12,FXfloat a13,FXfloat a20,FXfloat a21,FXfloat a22,FXfloat a23,FXfloat a30,FXfloat a31,FXfloat a32,FXfloat a33){
  m[0][0]=a00; m[0][1]=a01; m[0][2]=a02; m[0][3]=a03;
  m[1][0]=a10; m[1][1]=a11; m[1][2]=a12; m[1][3]=a13;
  m[2][0]=a20; m[2][1]=a21; m[2][2]=a22; m[2][3]=a23;
  m[3][0]=a30; m[3][1]=a31; m[3][2]=a32; m[3][3]=a33;
  return *this;
  }


// Set value from four vectors
FXMat4f& FXMat4f::set(const FXVec4f& a,const FXVec4f& b,const FXVec4f& c,const FXVec4f& d){
  m[0][0]=a[0]; m[0][1]=a[1]; m[0][2]=a[2]; m[0][3]=a[3];
  m[1][0]=b[0]; m[1][1]=b[1]; m[1][2]=b[2]; m[1][3]=b[3];
  m[2][0]=c[0]; m[2][1]=c[1]; m[2][2]=c[2]; m[2][3]=c[3];
  m[3][0]=d[0]; m[3][1]=d[1]; m[3][2]=d[2]; m[3][3]=d[3];
  return *this;
  }


// Add matrices
FXMat4f& FXMat4f::operator+=(const FXMat4f& w){
  m[0][0]+=w[0][0]; m[0][1]+=w[0][1]; m[0][2]+=w[0][2]; m[0][3]+=w[0][3];
  m[1][0]+=w[1][0]; m[1][1]+=w[1][1]; m[1][2]+=w[1][2]; m[1][3]+=w[1][3];
  m[2][0]+=w[2][0]; m[2][1]+=w[2][1]; m[2][2]+=w[2][2]; m[2][3]+=w[2][3];
  m[3][0]+=w[3][0]; m[3][1]+=w[3][1]; m[3][2]+=w[3][2]; m[3][3]+=w[3][3];
  return *this;
  }


// Substract matrices
FXMat4f& FXMat4f::operator-=(const FXMat4f& w){
  m[0][0]-=w[0][0]; m[0][1]-=w[0][1]; m[0][2]-=w[0][2]; m[0][3]-=w[0][3];
  m[1][0]-=w[1][0]; m[1][1]-=w[1][1]; m[1][2]-=w[1][2]; m[1][3]-=w[1][3];
  m[2][0]-=w[2][0]; m[2][1]-=w[2][1]; m[2][2]-=w[2][2]; m[2][3]-=w[2][3];
  m[3][0]-=w[3][0]; m[3][1]-=w[3][1]; m[3][2]-=w[3][2]; m[3][3]-=w[3][3];
  return *this;
  }


// Multiply matrix by scalar
FXMat4f& FXMat4f::operator*=(FXfloat w){
  m[0][0]*=w; m[0][1]*=w; m[0][2]*=w; m[0][3]*=w;
  m[1][0]*=w; m[1][1]*=w; m[1][2]*=w; m[2][3]*=w;
  m[2][0]*=w; m[2][1]*=w; m[2][2]*=w; m[3][3]*=w;
  m[3][0]*=w; m[3][1]*=w; m[3][2]*=w; m[3][3]*=w;
  return *this;
  }


// Multiply matrix by matrix
FXMat4f& FXMat4f::operator*=(const FXMat4f& w){
  register FXfloat x,y,z,h;
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
FXMat4f& FXMat4f::operator/=(FXfloat w){
  m[0][0]/=w; m[0][1]/=w; m[0][2]/=w; m[0][3]/=w;
  m[1][0]/=w; m[1][1]/=w; m[1][2]/=w; m[1][3]/=w;
  m[2][0]/=w; m[2][1]/=w; m[2][2]/=w; m[2][3]/=w;
  m[3][0]/=w; m[3][1]/=w; m[3][2]/=w; m[3][3]/=w;
  return *this;
  }


// Unary minus
FXMat4f FXMat4f::operator-() const {
  return FXMat4f(-m[0][0],-m[0][1],-m[0][2],-m[0][3],
                 -m[1][0],-m[1][1],-m[1][2],-m[1][3],
                 -m[2][0],-m[2][1],-m[2][2],-m[2][3],
                 -m[3][0],-m[3][1],-m[3][2],-m[3][3]);
  }


// Set to identity matrix
FXMat4f& FXMat4f::identity(){
  m[0][0]=1.0f; m[0][1]=0.0f; m[0][2]=0.0f; m[0][3]=0.0f;
  m[1][0]=0.0f; m[1][1]=1.0f; m[1][2]=0.0f; m[1][3]=0.0f;
  m[2][0]=0.0f; m[2][1]=0.0f; m[2][2]=1.0f; m[2][3]=0.0f;
  m[3][0]=0.0f; m[3][1]=0.0f; m[3][2]=0.0f; m[3][3]=1.0f;
  return *this;
  }


// Return true if identity matrix
FXbool FXMat4f::isIdentity() const {
  return m[0][0]==1.0f && m[0][1]==0.0f && m[0][2]==0.0f && m[0][3]==0.0f &&
         m[1][0]==0.0f && m[1][1]==1.0f && m[1][2]==0.0f && m[1][3]==0.0f &&
         m[2][0]==0.0f && m[2][1]==0.0f && m[2][2]==1.0f && m[2][3]==0.0f &&
         m[3][0]==0.0f && m[3][1]==0.0f && m[3][2]==0.0f && m[3][3]==1.0f;
  }


// Set orthographic projection from view volume
FXMat4f& FXMat4f::setOrtho(FXfloat xlo,FXfloat xhi,FXfloat ylo,FXfloat yhi,FXfloat zlo,FXfloat zhi){
  register FXfloat rl=xhi-xlo;
  register FXfloat tb=yhi-ylo;
  register FXfloat yh=zhi-zlo;
  return set(2.0f/rl,0.0f,0.0f,0.0f,0.0f,2.0f/tb,0.0f,0.0f,0.0f,0.0f,-2.0f/yh,0.0f,-(xhi+xlo)/rl,-(yhi+ylo)/tb,-(zhi+zlo)/yh,1.0f);
  }


// Get view volume from orthographic projection
void FXMat4f::getOrtho(FXfloat& xlo,FXfloat& xhi,FXfloat& ylo,FXfloat& yhi,FXfloat& zlo,FXfloat& zhi) const {
  zlo= (m[3][2]+1.0f)/m[2][2];
  zhi= (m[3][2]-1.0f)/m[2][2];
  xlo=-(1.0f+m[3][0])/m[0][0];
  xhi= (1.0f-m[3][0])/m[0][0];
  ylo=-(1.0f+m[3][1])/m[1][1];
  yhi= (1.0f-m[3][1])/m[1][1];
  }


// Set to inverse orthographic projection
FXMat4f& FXMat4f::setInverseOrtho(FXfloat xlo,FXfloat xhi,FXfloat ylo,FXfloat yhi,FXfloat zlo,FXfloat zhi){
  register FXfloat rl=xhi-xlo;
  register FXfloat tb=yhi-ylo;
  register FXfloat yh=zhi-zlo;
  return set(0.5f*rl,0.0f,0.0f,0.0f,0.0f,0.5f*tb,0.0f,0.0f,0.0f,0.0f,-0.5f*yh,0.0f,0.5f*(xhi+xlo),0.5f*(yhi+ylo),0.5f*(zhi+zlo),1.0f);
  }


// Set to perspective projection from view volume
FXMat4f& FXMat4f::setFrustum(FXfloat xlo,FXfloat xhi,FXfloat ylo,FXfloat yhi,FXfloat zlo,FXfloat zhi){
  register FXfloat rl=xhi-xlo;
  register FXfloat tb=yhi-ylo;
  register FXfloat yh=zhi-zlo;
  return set(2.0f*zlo/rl,0.0f,0.0f,0.0f,0.0f,2.0f*zlo/tb,0.0f,0.0f,(xhi+xlo)/rl,(yhi+ylo)/tb,-(zhi+zlo)/yh,-1.0f,0.0f,0.0f,-2.0f*zhi*zlo/yh,0.0f);
  }


// Get view volume from perspective projection
void FXMat4f::getFrustum(FXfloat& xlo,FXfloat& xhi,FXfloat& ylo,FXfloat& yhi,FXfloat& zlo,FXfloat& zhi) const {
  zlo=m[3][2]/(m[2][2]-1.0f);
  zhi=m[3][2]/(m[2][2]+1.0f);
  xlo=zlo*(m[2][0]-1.0f)/m[0][0];
  xhi=zlo*(m[2][0]+1.0f)/m[0][0];
  yhi=zlo*(m[2][1]+1.0f)/m[1][1];
  ylo=zlo*(m[2][1]-1.0f)/m[1][1];
  }


// Set to inverse perspective projection from view volume
FXMat4f& FXMat4f::setInverseFrustum(FXfloat xlo,FXfloat xhi,FXfloat ylo,FXfloat yhi,FXfloat zlo,FXfloat zhi){
  register FXfloat rl=xhi-xlo;
  register FXfloat tb=yhi-ylo;
  register FXfloat yh=zhi-zlo;
  return set(0.5f*rl/zlo,0.0f,0.0f,0.0f,0.0f,0.5f*tb/zlo,0.0f,0.0f,0.0f,0.0f,0.0f,-0.5f*yh/(zhi*zlo),0.5f*(xhi+xlo)/zlo,0.5f*(yhi+ylo)/zlo,-1.0f,0.5f*(zhi+zlo)/(zhi*zlo));
  }


// Make left hand matrix
FXMat4f& FXMat4f::left(){
  m[2][0]= -m[2][0];
  m[2][1]= -m[2][1];
  m[2][2]= -m[2][2];
  m[2][3]= -m[2][3];
  return *this;
  }


// Multiply by rotation matrix
FXMat4f& FXMat4f::rot(const FXMat3f& r){
  register FXfloat x,y,z;
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
FXMat4f& FXMat4f::rot(const FXQuatf& q){
  return rot(FXMat3f(q));
  }


// Rotate by angle (cos,sin) about arbitrary vector
FXMat4f& FXMat4f::rot(const FXVec3f& v,FXfloat c,FXfloat s){
  register FXfloat xx,yy,zz,xy,yz,zx,xs,ys,zs,t;
  register FXfloat r00,r01,r02,r10,r11,r12,r20,r21,r22;
  register FXfloat x=v.x;
  register FXfloat y=v.y;
  register FXfloat z=v.z;
  register FXfloat mag=x*x+y*y+z*z;
  FXASSERT(-1.00001f<c && c<1.00001f && -1.00001f<s && s<1.00001f);
  if(mag<=1.0E-30f) return *this;         // Rotation about 0-length axis
  mag=sqrtf(mag);
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
  t=1.0f-c;
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
FXMat4f& FXMat4f::rot(const FXVec3f& v,FXfloat phi){
  return rot(v,cosf(phi),sinf(phi));
  }


// Rotate about x-axis
FXMat4f& FXMat4f::xrot(FXfloat c,FXfloat s){
  register FXfloat u,v;
  FXASSERT(-1.00001f<c && c<1.00001f && -1.00001f<s && s<1.00001f);
  u=m[1][0]; v=m[2][0]; m[1][0]=c*u+s*v; m[2][0]=c*v-s*u;
  u=m[1][1]; v=m[2][1]; m[1][1]=c*u+s*v; m[2][1]=c*v-s*u;
  u=m[1][2]; v=m[2][2]; m[1][2]=c*u+s*v; m[2][2]=c*v-s*u;
  u=m[1][3]; v=m[2][3]; m[1][3]=c*u+s*v; m[2][3]=c*v-s*u;
  return *this;
  }


// Rotate by angle about x-axis
FXMat4f& FXMat4f::xrot(FXfloat phi){
  return xrot(cosf(phi),sinf(phi));
  }


// Rotate about y-axis
FXMat4f& FXMat4f::yrot(FXfloat c,FXfloat s){
  register FXfloat u,v;
  FXASSERT(-1.00001f<c && c<1.00001f && -1.00001f<s && s<1.00001f);
  u=m[0][0]; v=m[2][0]; m[0][0]=c*u-s*v; m[2][0]=c*v+s*u;
  u=m[0][1]; v=m[2][1]; m[0][1]=c*u-s*v; m[2][1]=c*v+s*u;
  u=m[0][2]; v=m[2][2]; m[0][2]=c*u-s*v; m[2][2]=c*v+s*u;
  u=m[0][3]; v=m[2][3]; m[0][3]=c*u-s*v; m[2][3]=c*v+s*u;
  return *this;
  }


// Rotate by angle about y-axis
FXMat4f& FXMat4f::yrot(FXfloat phi){
  return yrot(cosf(phi),sinf(phi));
  }


// Rotate about z-axis
FXMat4f& FXMat4f::zrot(FXfloat c,FXfloat s){
  register FXfloat u,v;
  FXASSERT(-1.00001f<c && c<1.00001f && -1.00001f<s && s<1.00001f);
  u=m[0][0]; v=m[1][0]; m[0][0]=c*u+s*v; m[1][0]=c*v-s*u;
  u=m[0][1]; v=m[1][1]; m[0][1]=c*u+s*v; m[1][1]=c*v-s*u;
  u=m[0][2]; v=m[1][2]; m[0][2]=c*u+s*v; m[1][2]=c*v-s*u;
  u=m[0][3]; v=m[1][3]; m[0][3]=c*u+s*v; m[1][3]=c*v-s*u;
  return *this;
  }


// Rotate by angle about z-axis
FXMat4f& FXMat4f::zrot(FXfloat phi){
  return zrot(cosf(phi),sinf(phi));
  }


// Look at
FXMat4f& FXMat4f::look(const FXVec3f& from,const FXVec3f& to,const FXVec3f& up){
  register FXfloat x0,x1,x2,tx,ty,tz;
  FXVec3f rz,rx,ry;
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
FXMat4f& FXMat4f::trans(FXfloat tx,FXfloat ty,FXfloat tz){
  m[3][0]=m[3][0]+tx*m[0][0]+ty*m[1][0]+tz*m[2][0];
  m[3][1]=m[3][1]+tx*m[0][1]+ty*m[1][1]+tz*m[2][1];
  m[3][2]=m[3][2]+tx*m[0][2]+ty*m[1][2]+tz*m[2][2];
  m[3][3]=m[3][3]+tx*m[0][3]+ty*m[1][3]+tz*m[2][3];
  return *this;
  }


// Translate over vector
FXMat4f& FXMat4f::trans(const FXVec3f& v){
  return trans(v[0],v[1],v[2]);
  }


// Scale unqual
FXMat4f& FXMat4f::scale(FXfloat sx,FXfloat sy,FXfloat sz){
  m[0][0]*=sx; m[0][1]*=sx; m[0][2]*=sx; m[0][3]*=sx;
  m[1][0]*=sy; m[1][1]*=sy; m[1][2]*=sy; m[1][3]*=sy;
  m[2][0]*=sz; m[2][1]*=sz; m[2][2]*=sz; m[2][3]*=sz;
  return *this;
  }


// Scale uniform
FXMat4f& FXMat4f::scale(FXfloat s){
  return scale(s,s,s);
  }


// Scale matrix
FXMat4f& FXMat4f::scale(const FXVec3f& v){
  return scale(v[0],v[1],v[2]);
  }


// Calculate determinant
FXfloat FXMat4f::det() const {
  return (m[0][0]*m[1][1]-m[0][1]*m[1][0]) * (m[2][2]*m[3][3]-m[2][3]*m[3][2]) -
         (m[0][0]*m[1][2]-m[0][2]*m[1][0]) * (m[2][1]*m[3][3]-m[2][3]*m[3][1]) +
         (m[0][0]*m[1][3]-m[0][3]*m[1][0]) * (m[2][1]*m[3][2]-m[2][2]*m[3][1]) +
         (m[0][1]*m[1][2]-m[0][2]*m[1][1]) * (m[2][0]*m[3][3]-m[2][3]*m[3][0]) -
         (m[0][1]*m[1][3]-m[0][3]*m[1][1]) * (m[2][0]*m[3][2]-m[2][2]*m[3][0]) +
         (m[0][2]*m[1][3]-m[0][3]*m[1][2]) * (m[2][0]*m[3][1]-m[2][1]*m[3][0]);
  }


// Transpose matrix
FXMat4f FXMat4f::transpose() const {
  return FXMat4f(m[0][0],m[1][0],m[2][0],m[3][0],
                 m[0][1],m[1][1],m[2][1],m[3][1],
                 m[0][2],m[1][2],m[2][2],m[3][2],
                 m[0][3],m[1][3],m[2][3],m[3][3]);
  }


// Invert matrix
FXMat4f FXMat4f::invert() const {
  FXMat4f r;
  register FXfloat A0=m[0][0]*m[1][1]-m[0][1]*m[1][0];
  register FXfloat A1=m[0][0]*m[1][2]-m[0][2]*m[1][0];
  register FXfloat A2=m[0][0]*m[1][3]-m[0][3]*m[1][0];
  register FXfloat A3=m[0][1]*m[1][2]-m[0][2]*m[1][1];
  register FXfloat A4=m[0][1]*m[1][3]-m[0][3]*m[1][1];
  register FXfloat A5=m[0][2]*m[1][3]-m[0][3]*m[1][2];
  register FXfloat B0=m[2][0]*m[3][1]-m[2][1]*m[3][0];
  register FXfloat B1=m[2][0]*m[3][2]-m[2][2]*m[3][0];
  register FXfloat B2=m[2][0]*m[3][3]-m[2][3]*m[3][0];
  register FXfloat B3=m[2][1]*m[3][2]-m[2][2]*m[3][1];
  register FXfloat B4=m[2][1]*m[3][3]-m[2][3]*m[3][1];
  register FXfloat B5=m[2][2]*m[3][3]-m[2][3]*m[3][2];
  register FXfloat dd=A0*B5-A1*B4+A2*B3+A3*B2-A4*B1+A5*B0;
  FXASSERT(dd!=0.0f);                       
  dd=1.0f/dd;
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
FXMat4f FXMat4f::affineInvert() const {
  register FXfloat dd;
  FXMat4f r;
  r[0][0]=m[1][1]*m[2][2]-m[1][2]*m[2][1];
  r[0][1]=m[0][2]*m[2][1]-m[0][1]*m[2][2];
  r[0][2]=m[0][1]*m[1][2]-m[0][2]*m[1][1];
  r[0][3]=0.0f;
  r[1][0]=m[1][2]*m[2][0]-m[1][0]*m[2][2];
  r[1][1]=m[0][0]*m[2][2]-m[0][2]*m[2][0];
  r[1][2]=m[0][2]*m[1][0]-m[0][0]*m[1][2];
  r[1][3]=0.0f;
  r[2][0]=m[1][0]*m[2][1]-m[1][1]*m[2][0];
  r[2][1]=m[0][1]*m[2][0]-m[0][0]*m[2][1];
  r[2][2]=m[0][0]*m[1][1]-m[0][1]*m[1][0];
  r[2][3]=0.0f;
  dd=m[0][0]*r[0][0]+m[0][1]*r[1][0]+m[0][2]*r[2][0];
  FXASSERT(dd!=0.0f);
  dd=1.0f/dd;
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
  r[3][3]=1.0f;
  return r;
  }


// Invert rigid body transform matrix
FXMat4f FXMat4f::rigidInvert() const {
  register FXfloat ss;
  FXMat4f r;
  ss=1.0f/(m[0][0]*m[0][0]+m[0][1]*m[0][1]+m[0][2]*m[0][2]);
  r[0][0]=m[0][0]*ss; 
  r[0][1]=m[1][0]*ss; 
  r[0][2]=m[2][0]*ss; 
  r[0][3]=0.0f;
  r[1][0]=m[0][1]*ss; 
  r[1][1]=m[1][1]*ss; 
  r[1][2]=m[2][1]*ss; 
  r[1][3]=0.0f;
  r[2][0]=m[0][2]*ss; 
  r[2][1]=m[1][2]*ss; 
  r[2][2]=m[2][2]*ss; 
  r[2][3]=0.0f;
  r[3][0]=-(r[0][0]*m[3][0]+r[1][0]*m[3][1]+r[2][0]*m[3][2]);
  r[3][1]=-(r[0][1]*m[3][0]+r[1][1]*m[3][1]+r[2][1]*m[3][2]);
  r[3][2]=-(r[0][2]*m[3][0]+r[1][2]*m[3][1]+r[2][2]*m[3][2]);
  r[3][3]=1.0f;
  return r;
  }


// Matrix times vector
FXVec3f operator*(const FXMat4f& m,const FXVec3f& v){
  return FXVec3f(m[0][0]*v[0]+m[0][1]*v[1]+m[0][2]*v[2]+m[0][3], m[1][0]*v[0]+m[1][1]*v[1]+m[1][2]*v[2]+m[1][3], m[2][0]*v[0]+m[2][1]*v[1]+m[2][2]*v[2]+m[2][3]);
  }


// Matrix times vector
FXVec4f operator*(const FXMat4f& m,const FXVec4f& v){
  return FXVec4f(m[0][0]*v[0]+m[0][1]*v[1]+m[0][2]*v[2]+m[0][3]*v[3], m[1][0]*v[0]+m[1][1]*v[1]+m[1][2]*v[2]+m[1][3]*v[3], m[2][0]*v[0]+m[2][1]*v[1]+m[2][2]*v[2]+m[2][3]*v[3], m[3][0]*v[0]+m[3][1]*v[1]+m[3][2]*v[2]+m[3][3]*v[3]);
  }


// Vector times matrix
FXVec3f operator*(const FXVec3f& v,const FXMat4f& m){
  return FXVec3f(v[0]*m[0][0]+v[1]*m[1][0]+v[2]*m[2][0]+m[3][0], v[0]*m[0][1]+v[1]*m[1][1]+v[2]*m[2][1]+m[3][1], v[0]*m[0][2]+v[1]*m[1][2]+v[2]*m[2][2]+m[3][2]);
  }


// Vector times matrix
FXVec4f operator*(const FXVec4f& v,const FXMat4f& m){
  return FXVec4f(v[0]*m[0][0]+v[1]*m[1][0]+v[2]*m[2][0]+v[3]*m[3][0], v[0]*m[0][1]+v[1]*m[1][1]+v[2]*m[2][1]+v[3]*m[3][1], v[0]*m[0][2]+v[1]*m[1][2]+v[2]*m[2][2]+v[3]*m[3][2], v[0]*m[0][3]+v[1]*m[1][3]+v[2]*m[2][3]+v[3]*m[3][3]);
  }


// Matrix and matrix add
FXMat4f operator+(const FXMat4f& a,const FXMat4f& b){
  return FXMat4f(a[0][0]+b[0][0],a[0][1]+b[0][1],a[0][2]+b[0][2],a[0][3]+b[0][3],
                 a[1][0]+b[1][0],a[1][1]+b[1][1],a[1][2]+b[1][2],a[1][3]+b[1][3],
                 a[2][0]+b[2][0],a[2][1]+b[2][1],a[2][2]+b[2][2],a[2][3]+b[2][3],
                 a[3][0]+b[3][0],a[3][1]+b[3][1],a[3][2]+b[3][2],a[3][3]+b[3][3]);
  }


// Matrix and matrix subtract
FXMat4f operator-(const FXMat4f& a,const FXMat4f& b){
  return FXMat4f(a[0][0]-b[0][0],a[0][1]-b[0][1],a[0][2]-b[0][2],a[0][3]-b[0][3],
                 a[1][0]-b[1][0],a[1][1]-b[1][1],a[1][2]-b[1][2],a[1][3]-b[1][3],
                 a[2][0]-b[2][0],a[2][1]-b[2][1],a[2][2]-b[2][2],a[2][3]-b[2][3],
                 a[3][0]-b[3][0],a[3][1]-b[3][1],a[3][2]-b[3][2],a[3][3]-b[3][3]);
  }


// Matrix and matrix multiply
FXMat4f operator*(const FXMat4f& a,const FXMat4f& b){
  register FXfloat x,y,z,w;
  FXMat4f r;
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
FXMat4f operator*(FXfloat x,const FXMat4f& a){
  return FXMat4f(x*a[0][0],x*a[0][1],x*a[0][2],a[0][3],
                 x*a[1][0],x*a[1][1],x*a[1][2],a[1][3],
                 x*a[2][0],x*a[2][1],x*a[2][2],a[2][3],
                 x*a[3][0],x*a[3][1],x*a[3][2],a[3][3]);
  }


// Multiply matrix by scalar
FXMat4f operator*(const FXMat4f& a,FXfloat x){
  return FXMat4f(a[0][0]*x,a[0][1]*x,a[0][2]*x,a[0][3],
                 a[1][0]*x,a[1][1]*x,a[1][2]*x,a[1][3],
                 a[2][0]*x,a[2][1]*x,a[2][2]*x,a[2][3],
                 a[3][0]*x,a[3][1]*x,a[3][2]*x,a[3][3]);
  }


// Divide scalar by matrix
FXMat4f operator/(FXfloat x,const FXMat4f& a){
  return FXMat4f(x/a[0][0],x/a[0][1],x/a[0][2],a[0][3],
                 x/a[1][0],x/a[1][1],x/a[1][2],a[1][3],
                 x/a[2][0],x/a[2][1],x/a[2][2],a[2][3],
                 x/a[3][0],x/a[3][1],x/a[3][2],a[3][3]);
  }


// Divide matrix by scalar
FXMat4f operator/(const FXMat4f& a,FXfloat x){
  return FXMat4f(a[0][0]/x,a[0][1]/x,a[0][2]/x,a[0][3],
                 a[1][0]/x,a[1][1]/x,a[1][2]/x,a[1][3],
                 a[2][0]/x,a[2][1]/x,a[2][2]/x,a[2][3],
                 a[3][0]/x,a[3][1]/x,a[3][2]/x,a[3][3]);
  }


// Matrix and matrix equality
FXbool operator==(const FXMat4f& a,const FXMat4f& b){
  return a[0]==b[0] && a[1]==b[1] && a[2]==b[2] && a[3]==b[3];
  }


// Matrix and matrix inequality
FXbool operator!=(const FXMat4f& a,const FXMat4f& b){
  return a[0]!=b[0] || a[1]!=b[1] || a[2]!=b[2] || a[3]!=b[3];
  }


// Matrix and scalar equality
FXbool operator==(const FXMat4f& a,FXfloat n){
  return a[0]==n && a[1]==n && a[2]==n && a[3]==n;
  }


// Scalar and matrix equality
FXbool operator==(FXfloat n,const FXMat4f& a){
  return n==a[0] && n==a[1] && n==a[2] && n==a[3];
  }


// Matrix and scalar inequality
FXbool operator!=(const FXMat4f& a,FXfloat n){
  return a[0]!=n || a[1]!=n || a[2]!=n || a[3]!=n;
  }


// Scalar and matrix inequality
FXbool operator!=(FXfloat n,const FXMat4f& a){
  return n!=a[0] || n!=a[1] || n!=a[2] || n!=a[3];
  }


// Save to archive
FXStream& operator<<(FXStream& store,const FXMat4f& m){
  store << m[0] << m[1] << m[2] << m[3];
  return store;
  }


// Load from archive
FXStream& operator>>(FXStream& store,FXMat4f& m){
  store >> m[0] >> m[1] >> m[2] >> m[3];
  return store;
  }

}
