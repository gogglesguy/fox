/********************************************************************************
*                                                                               *
*              D o u b l e - P r e c i s i o n  Q u a t e r n i o n             *
*                                                                               *
*********************************************************************************
* Copyright (C) 1994,2024 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "FXArray.h"
#include "FXMetaClass.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXVec2d.h"
#include "FXVec3d.h"
#include "FXVec4d.h"
#include "FXQuatd.h"
#include "FXMat3d.h"
#include "FXMat4d.h"

/*
  Notes:

  - Quaternion represents a rotation as follows:

                   phi       axis            phi
     Q =  ( sin ( ----- ) * ------  , cos ( ----- ) )
                    2       |axis|            2

  - Typically, |Q| == 1.  But this is not always a given.
  - Repeated operations should periodically fix Q to maintain |Q| == 1, using
    the adjust() API.
  - FIXME maybe refine exp() and log() as non-members.
*/

using namespace FX;

/*******************************************************************************/

namespace FX {

// Construct from rotation axis and angle in radians
FXQuatd::FXQuatd(const FXVec3d& axis,FXdouble phi){
  setAxisAngle(axis,phi);
  }


// Construct quaternion from arc between two unit vectors fm and to
FXQuatd::FXQuatd(const FXVec3d& fr,const FXVec3d& to){
  set(arc(fr,to));
  }


// Construct from euler angles yaw (z), pitch (y), and roll (x)
FXQuatd::FXQuatd(FXdouble roll,FXdouble pitch,FXdouble yaw){
  setRollPitchYaw(roll,pitch,yaw);
  }


// Construct quaternion from three orthogonal unit vectors
FXQuatd::FXQuatd(const FXVec3d& ex,const FXVec3d& ey,const FXVec3d& ez){
  setAxes(ex,ey,ez);
  }


// Construct quaternion from rotation vector rot, representing a rotation
// by |rot| radians about a unit vector rot/|rot|.
FXQuatd::FXQuatd(const FXVec3d& rot){
  setRotation(rot);
  }


// Set axis and angle
void FXQuatd::setAxisAngle(const FXVec3d& axis,FXdouble phi){
  FXdouble mag2(axis.length2());
  if(__likely(0.0<mag2)){
    FXdouble arg(0.5*phi);
    FXdouble mag(Math::sqrt(mag2));
    FXdouble s(Math::sin(arg)/mag);
    FXdouble c(Math::cos(arg));
    x=axis.x*s;
    y=axis.y*s;
    z=axis.z*s;
    w=c;
    }
  else{
    x=0.0;
    y=0.0;
    z=0.0;
    w=1.0;
    }
  }


// Obtain axis and angle
// Remeber that: q = sin(A/2)*(x*i+y*j+z*k)+cos(A/2)
void FXQuatd::getAxisAngle(FXVec3d& axis,FXdouble& phi) const {
  FXdouble mag2(x*x+y*y+z*z);
  if(0.0<mag2){
    FXdouble mag(Math::sqrt(mag2));
    axis.x=x/mag;
    axis.y=y/mag;
    axis.z=z/mag;
    phi=2.0*Math::atan2(mag,w);
    }
  else{
    axis.x=1.0;
    axis.y=0.0;
    axis.z=0.0;
    phi=0.0;
    }
  }


// Set quaternion from rotation vector rot
//
//                 |rot|         rot              |rot|
//   Q =  ( sin ( ------- ) *  -------  , cos (  ------- ) )
//                   2          |rot|               2
//
void FXQuatd::setRotation(const FXVec3d& rot){
  FXdouble mag2(rot.length2());
  if(0.0<mag2){
    FXdouble mag(Math::sqrt(mag2));
    FXdouble arg(mag*0.5);
    FXdouble s(Math::sin(arg)/mag);
    FXdouble c(Math::cos(arg));
    x=rot.x*s;
    y=rot.y*s;
    z=rot.z*s;
    w=c;
    }
  else{
    x=0.0;
    y=0.0;
    z=0.0;
    w=1.0;
    }
  }


// Get rotation vector from quaternion
FXVec3d FXQuatd::getRotation() const {
  FXVec3d rot(0.0,0.0,0.0);
  FXdouble mag2(x*x+y*y+z*z);
  if(0.0<mag2){
    FXdouble mag(Math::sqrt(mag2));
    FXdouble phi(2.0*Math::atan2(mag,w)/mag);
    rot.x=x*phi*mag;
    rot.y=y*phi*mag;
    rot.z=z*phi*mag;
    }
  return rot;
  }


// Set unit quaternion to modified rodrigues parameters.
// Modified Rodrigues parameters are defined as MRP = tan(theta/4)*E,
// where theta is rotation angle (radians), and E is unit axis of rotation.
// Reference: "A survey of Attitude Representations", Malcolm D. Shuster,
// Journal of Astronautical sciences, Vol. 41, No. 4, Oct-Dec. 1993, pp. 476,
// Equations (253).
void FXQuatd::setMRP(const FXVec3d& m){
  FXdouble mm=m[0]*m[0]+m[1]*m[1]+m[2]*m[2];
  FXdouble D=1.0/(1.0+mm);
  x=m[0]*2.0*D;
  y=m[1]*2.0*D;
  z=m[2]*2.0*D;
  w=(1.0-mm)*D;
  }


// Return modified rodrigues parameters from unit quaternion.
// Reference: "A survey of Attitude Representations", Malcolm D. Shuster,
// Journal of Astronautical sciences, Vol. 41, No. 4, Oct-Dec. 1993, pp. 475,
// Equations (249). (250).
FXVec3d FXQuatd::getMRP() const {
  FXdouble m=(0.0<w)?1.0/(1.0+w):-1.0/(1.0-w);
  return FXVec3d(x*m,y*m,z*m);
  }


// Set quaternion from roll (x), pitch (y), yaw (z)
void FXQuatd::setRollPitchYaw(FXdouble roll,FXdouble pitch,FXdouble yaw){
  FXdouble sr,cr,sp,cp,sy,cy;
  FXdouble rr=0.5*roll;
  FXdouble pp=0.5*pitch;
  FXdouble yy=0.5*yaw;
  sr=Math::sin(rr); cr=Math::cos(rr);
  sp=Math::sin(pp); cp=Math::cos(pp);
  sy=Math::sin(yy); cy=Math::cos(yy);
  x=sr*cp*cy-cr*sp*sy;
  y=cr*sp*cy+sr*cp*sy;
  z=cr*cp*sy-sr*sp*cy;
  w=cr*cp*cy+sr*sp*sy;
  }


// Set quaternion from yaw (z), pitch (y), roll (x)
void FXQuatd::setYawPitchRoll(FXdouble yaw,FXdouble pitch,FXdouble roll){
  FXdouble sr,cr,sp,cp,sy,cy;
  FXdouble rr=0.5*roll;
  FXdouble pp=0.5*pitch;
  FXdouble yy=0.5*yaw;
  sr=Math::sin(rr); cr=Math::cos(rr);
  sp=Math::sin(pp); cp=Math::cos(pp);
  sy=Math::sin(yy); cy=Math::cos(yy);
  x=sr*cp*cy+cr*sp*sy;
  y=cr*sp*cy-sr*cp*sy;
  z=cr*cp*sy+sr*sp*cy;
  w=cr*cp*cy-sr*sp*sy;
  }


// Set quaternion from roll (x), yaw (z), pitch (y)
void FXQuatd::setRollYawPitch(FXdouble roll,FXdouble yaw,FXdouble pitch){
  FXdouble sr,cr,sp,cp,sy,cy;
  FXdouble rr=0.5*roll;
  FXdouble pp=0.5*pitch;
  FXdouble yy=0.5*yaw;
  sr=Math::sin(rr); cr=Math::cos(rr);
  sp=Math::sin(pp); cp=Math::cos(pp);
  sy=Math::sin(yy); cy=Math::cos(yy);
  x=cp*cy*sr+sp*sy*cr;
  y=sp*cy*cr+cp*sy*sr;
  z=cp*sy*cr-sp*cy*sr;
  w=cp*cy*cr-sp*sy*sr;
  }


// Set quaternion from pitch (y), roll (x),yaw (z)
void FXQuatd::setPitchRollYaw(FXdouble pitch,FXdouble roll,FXdouble yaw){
  FXdouble sr,cr,sp,cp,sy,cy;
  FXdouble rr=0.5*roll;
  FXdouble pp=0.5*pitch;
  FXdouble yy=0.5*yaw;
  sr=Math::sin(rr); cr=Math::cos(rr);
  sp=Math::sin(pp); cp=Math::cos(pp);
  sy=Math::sin(yy); cy=Math::cos(yy);
  x=cy*sr*cp-sy*cr*sp;
  y=cy*cr*sp+sy*sr*cp;
  z=cy*sr*sp+sy*cr*cp;
  w=cy*cr*cp-sy*sr*sp;
  }


// Set quaternion from pitch (y), yaw (z), roll (x)
void FXQuatd::setPitchYawRoll(FXdouble pitch,FXdouble yaw,FXdouble roll){
  FXdouble sr,cr,sp,cp,sy,cy;
  FXdouble rr=0.5*roll;
  FXdouble pp=0.5*pitch;
  FXdouble yy=0.5*yaw;
  sr=Math::sin(rr); cr=Math::cos(rr);
  sp=Math::sin(pp); cp=Math::cos(pp);
  sy=Math::sin(yy); cy=Math::cos(yy);
  x=sr*cy*cp-cr*sy*sp;
  y=cr*cy*sp-sr*sy*cp;
  z=sr*cy*sp+cr*sy*cp;
  w=cr*cy*cp+sr*sy*sp;
  }


// Set quaternion from yaw (z), roll (x), pitch (y)
void FXQuatd::setYawRollPitch(FXdouble yaw,FXdouble roll,FXdouble pitch){
  FXdouble sr,cr,sp,cp,sy,cy;
  FXdouble rr=0.5*roll;
  FXdouble pp=0.5*pitch;
  FXdouble yy=0.5*yaw;
  sr=Math::sin(rr); cr=Math::cos(rr);
  sp=Math::sin(pp); cp=Math::cos(pp);
  sy=Math::sin(yy); cy=Math::cos(yy);
  x=cp*sr*cy+sp*cr*sy;
  y=sp*cr*cy-cp*sr*sy;
  z=cp*cr*sy-sp*sr*cy;
  w=cp*cr*cy+sp*sr*sy;
  }


// Obtain roll, pitch, yaw
void FXQuatd::getRollPitchYaw(FXdouble& roll,FXdouble& pitch,FXdouble& yaw) const {
  roll=Math::atan2(2.0*(y*z+w*x),1.0-2.0*(x*x+y*y));
  pitch=Math::asin(Math::fclamp(-1.0,-2.0*(x*z-w*y),1.0));
  yaw=Math::atan2(2.0*(x*y+w*z),1.0-2.0*(y*y+z*z));
  }


// Obtain yaw, pitch, and roll
void FXQuatd::getYawPitchRoll(FXdouble& yaw,FXdouble& pitch,FXdouble& roll) const {
  yaw=Math::atan2(-2.0*(x*y-w*z),1.0-2.0*(y*y+z*z));
  pitch=Math::asin(Math::fclamp(-1.0,2.0*(x*z+w*y),1.0));
  roll=Math::atan2(-2.0*(y*z-w*x),1.0-2.0*(x*x+y*y));
  }


// Obtain roll, yaw, pitch
void FXQuatd::getRollYawPitch(FXdouble& roll,FXdouble& yaw,FXdouble& pitch) const {
  roll=Math::atan2(-2.0*(y*z-w*x),1.0-2.0*(x*x+z*z));
  yaw=Math::asin(Math::fclamp(-1.0,2.0*(x*y+w*z),1.0));
  pitch=Math::atan2(-2.0*(x*z-w*y),1.0-2.0*(y*y+z*z));
  }


// Obtain pitch, roll, yaw
void FXQuatd::getPitchRollYaw(FXdouble& pitch,FXdouble& roll,FXdouble& yaw) const {
  pitch=Math::atan2(-2.0*(x*z-w*y),1.0-2.0*(x*x+y*y));
  roll=Math::asin(Math::fclamp(-1.0,2.0*(y*z+w*x),1.0));
  yaw=Math::atan2(-2.0*(x*y-w*z),1.0-2.0*(x*x+z*z));
  }


// Obtain pitch, yaw, roll
void FXQuatd::getPitchYawRoll(FXdouble& pitch,FXdouble& yaw,FXdouble& roll) const {
  pitch=Math::atan2(2.0*(x*z+w*y),1.0-2.0*(y*y+z*z));
  yaw=Math::asin(Math::fclamp(-1.0,-2.0*(x*y-w*z),1.0));
  roll=Math::atan2(2.0*(y*z+w*x),1.0-2.0*(x*x+z*z));
  }


// Obtain yaw, roll, pitch
void FXQuatd::getYawRollPitch(FXdouble& yaw,FXdouble& roll,FXdouble& pitch) const {
  yaw=Math::atan2(2.0*(x*y+w*z),1.0-2.0*(x*x+z*z));
  roll=Math::asin(Math::fclamp(-1.0,-2.0*(y*z-w*x),1.0));
  pitch=Math::atan2(2.0*(x*z+w*y),1.0-2.0*(x*x+y*y));
  }


// Set quaternion from axes
// Singularity-free matrix to quaternion using Cayley’s method.
// "A Survey on the Computation of Quaternions from Rotation Matrices", S. Sarabandi
// and F. Thomas, pg. 14, eq. (76)..(79).
// "Singularity-Free Computation of Quaternions From Rotation Matrices in E4 and E3"
// S. Sarabandi, A. Perez-Gracia and F. Thomas, pg. 5, eq. (23)...(26).
void FXQuatd::setAxes(const FXVec3d& ex,const FXVec3d& ey,const FXVec3d& ez){
  FXdouble opxpypz=1.0+ex.x+ey.y+ez.z;
  FXdouble opxmymz=1.0+ex.x-ey.y-ez.z;
  FXdouble omxpymz=1.0-ex.x+ey.y-ez.z;
  FXdouble omxmypz=1.0-ex.x-ey.y+ez.z;
  FXdouble xymyx=ex.y-ey.x;
  FXdouble xypyx=ex.y+ey.x;
  FXdouble yzmzy=ey.z-ez.y;
  FXdouble yzpzy=ey.z+ez.y;
  FXdouble zxmxz=ez.x-ex.z;
  FXdouble zxpxz=ez.x+ex.z;
  FXdouble x0=Math::sqr(opxmymz);
  FXdouble y0=Math::sqr(omxpymz);
  FXdouble z0=Math::sqr(omxmypz);
  FXdouble w0=Math::sqr(opxpypz);
  FXdouble x1=Math::sqr(xypyx);
  FXdouble z1=Math::sqr(xymyx);
  FXdouble x2=Math::sqr(yzmzy);
  FXdouble y2=Math::sqr(yzpzy);
  FXdouble x3=Math::sqr(zxpxz);
  FXdouble y3=Math::sqr(zxmxz);
  x=0.25*Math::sqrt(x0+x1+x2+x3);
  y=0.25*Math::sqrt(y0+x1+y2+y3);
  z=0.25*Math::sqrt(z0+z1+y2+x3);
  w=0.25*Math::sqrt(w0+z1+x2+y3);
  x=Math::copysign(x,yzmzy);
  y=Math::copysign(y,zxmxz);
  z=Math::copysign(z,xymyx);
  }


// Get quaternion axes
void FXQuatd::getAxes(FXVec3d& ex,FXVec3d& ey,FXVec3d& ez) const {
  FXdouble tx=2.0*x;
  FXdouble ty=2.0*y;
  FXdouble tz=2.0*z;
  FXdouble twx=tx*w;
  FXdouble twy=ty*w;
  FXdouble twz=tz*w;
  FXdouble txx=tx*x;
  FXdouble txy=ty*x;
  FXdouble txz=tz*x;
  FXdouble tyy=ty*y;
  FXdouble tyz=tz*y;
  FXdouble tzz=tz*z;
  ex.x=1.0-tyy-tzz;
  ex.y=txy+twz;
  ex.z=txz-twy;
  ey.x=txy-twz;
  ey.y=1.0-txx-tzz;
  ey.z=tyz+twx;
  ez.x=txz+twy;
  ez.y=tyz-twx;
  ez.z=1.0-txx-tyy;
  }


// Obtain local x axis
FXVec3d FXQuatd::getXAxis() const {
  FXdouble ty=2.0*y;
  FXdouble tz=2.0*z;
  return FXVec3d(1.0-ty*y-tz*z,ty*x+tz*w,tz*x-ty*w);
  }


// Obtain local y axis
FXVec3d FXQuatd::getYAxis() const {
  FXdouble tx=2.0*x;
  FXdouble tz=2.0*z;
  return FXVec3d(tx*y-tz*w,1.0-tx*x-tz*z,tz*y+tx*w);
  }


// Obtain local z axis
FXVec3d FXQuatd::getZAxis() const {
  FXdouble tx=2.0*x;
  FXdouble ty=2.0*y;
  return FXVec3d(tx*z+ty*w,ty*z-tx*w,1.0-tx*x-ty*y);
  }


// Exponentiate quaternion.
// Given:
//
//   q = theta*(x*i+y*j+z*k),
//
// then:
//
//   exp(q) = sin(theta)*(x*i+y*j+z*k)+cos(theta)
//
// with length of (x,y,z) = 1.
FXQuatd FXQuatd::exp() const {
  FXQuatd result(0.0,0.0,0.0,1.0);
  FXdouble mag2(x*x+y*y+z*z);
  if(0.0<mag2){
    FXdouble mag(Math::sqrt(mag2));
    FXdouble s(Math::sin(mag)/mag);
    FXdouble c(Math::cos(mag));
    result.x=x*s;
    result.y=y*s;
    result.z=z*s;
    result.w=c;
    }
  return result;
  }


// Take logarithm of quaternion.
// Given:
//
//   q = sin(theta)*(x*i+y*j+z*k)+cos(theta)
//
// with length of (x,y,z) = 1. then :
//
//   log(q) = theta*(x*i+y*j+z*k)
//
FXQuatd FXQuatd::log() const {
  FXQuatd result(0.0,0.0,0.0,0.0);
  FXdouble mag2(x*x+y*y+z*z);
  if(0.0<mag2){
    FXdouble mag(Math::sqrt(mag2));
    FXdouble phi(Math::atan2(mag,w)/mag);
    result.x=x*phi;
    result.y=y*phi;
    result.z=z*phi;
    }
  return result;
  }


// Power of quaternion is formally defined as:
//
//   q.pow(t) := (t*q.log()).exp()
//
// We can short-circuit some calculations by noting the rotation axis
// (i.e. the imaginary part) need not be normalized more than once;
// thus, we save 1 division, 1 square root, and a dot-product.
FXQuatd FXQuatd::pow(FXdouble t) const {
  FXQuatd result(0.0,0.0,0.0,1.0);
  FXdouble mag2(x*x+y*y+z*z);
  if(0.0<mag2){
    FXdouble mag(Math::sqrt(mag2));
    FXdouble phi(Math::atan2(mag,w)*t);
    FXdouble s(Math::sin(phi)/mag);
    FXdouble c(Math::cos(phi));
    result.x=x*s;
    result.y=y*s;
    result.z=z*s;
    result.w=c;
    }
  return result;
  }



// Rotation unit-quaternion and vector v . q = (q . v . q*) where q* is
// the conjugate of q.
//
// The Rodriques Formula for rotating a vector V over angle A about a unit-vector K:
//
//    V' = K (K . V) + (K x V) sin(A) - K x (K x V) cos(A)
//
// Given UNIT quaternion q=(S,c), i.e. |q|=1, defined as a imaginary part S with
// |S|=K sin(A/2), where K is a unit vector, and a real part c=cos(A/2), we can obtain,
// after some (well, a LOT of) manipulation:
//
//    V' = S (S . V) + c (c V + S x V) + S x (c V + S x V)
//
// Using:
//
//    A x (B x C) = B (A . C) - C (A . B)
//
// We can further simplify:
//
//    V' = V + 2 S x (S x V + c V)
//
FXVec3d operator*(const FXVec3d& v,const FXQuatd& q){
  FXVec3d s(q.x,q.y,q.z);
  return v+(s^((s^v)+(v*q.w)))*2.0;
  }


// Rotation unit-quaternion and vector q . v = (q* . v . q)
FXVec3d operator*(const FXQuatd& q,const FXVec3d& v){
  FXVec3d s(q.x,q.y,q.z);
  return v+(((v^s)+(v*q.w))^s)*2.0;     // Yes, -a^b is b^a!
  }

/*******************************************************************************/

// Adjust quaternion length
FXQuatd& FXQuatd::adjust(){
  FXdouble s(length());
  if(__likely(s)){
    return *this /= s;
    }
  return *this;
  }


/*******************************************************************************/

// Construct quaternion from arc f->t, described by two vectors f and t on
// a unit sphere.
//
// A quaternion which rotates by an angle theta about a unit axis A is specified as:
//
//   q = (A * sin(theta/2), cos(theta/2)).
//
// Assuming is f and t are unit length, construct half-way vector:
//
//   h = (f + t)
//
// Then:
//                        f . h
//  cos(theta/2)     =   -------
//                       |f|*|h|
//
// and:
//
//                        f x h
//  A * sin(theta/2) =   -------
//                       |f|*|h|
//
// So generate normalized quaternion as follows:
//
//         f x h     f . h        (f x h, f . h)     (f x h, f . h)
//  Q = ( ------- , ------- )  = ---------------- = ----------------
//        |f|*|h|   |f|*|h|          |f|*|h|        |(f x h, f . h)|
//
// NOTE1: Technically, input vectors f and t do not actually have to
// be unit length in this formulation.  However, they do need to be
// the same lengths.
//
// NOTE2: A problem exists when |h|=0.  This only happens when rotating
// 180 degrees, i.e. f = -t.  In this case, the dot-product (f . h) will
// be zero.  Pick a vector v orthogonal to f, then set Q:
//
//  Q = (v, 0)
//
FXQuatd arc(const FXVec3d& f,const FXVec3d& t){
  FXQuatd result;
  FXVec3d h(f+t);
  FXdouble w(f.x*h.x+f.y*h.y+f.z*h.z);
  if(Math::fabs(w)<0.00000000000000001){ // |f.h| is small
    FXdouble ax=Math::fabs(f.x);
    FXdouble ay=Math::fabs(f.y);
    FXdouble az=Math::fabs(f.z);
    if(ax<ay){
      if(ax<az){                        // |f.x| smallest
        result.x=-f.y*f.y-f.z*f.z;
        result.y= f.x*f.y;
        result.z= f.x*f.z;
        result.w= 0.0;
        }
      else{                             // |f.z| smallest
        result.x= f.x*f.z;
        result.y= f.y*f.z;
        result.z=-f.x*f.x-f.y*f.y;
        result.w= 0.0;
        }
      }
    else{
      if(ay<az){                        // |f.y| smallest
        result.x= f.y*f.x;
        result.y=-f.x*f.x-f.z*f.z;
        result.z= f.y*f.z;
        result.w= 0.0;
        }
      else{                             // |f.z| smallest
        result.x= f.x*f.z;
        result.y= f.y*f.z;
        result.z=-f.y*f.y-f.x*f.x;
        result.w= 0.0;
        }
      }
    }
  else{
    result.x=f.y*h.z-f.z*h.y;           // fxh
    result.y=f.z*h.x-f.x*h.z;
    result.z=f.x*h.y-f.y*h.x;
    result.w=w;                         // f.h
    }
  result*=(1.0/result.length());
  return result;
  }

/*******************************************************************************/

// Spherical lerp of unit quaternions u,v
// This is equivalent to: u * (u.unitinvert()*v).pow(f)
FXQuatd lerp(const FXQuatd& u,const FXQuatd& v,FXdouble f){
  FXdouble dot=u.x*v.x+u.y*v.y+u.z*v.z+u.w*v.w;
  FXdouble to=Math::fblend(dot,0.0,-f,f);
  FXdouble fr=1.0-f;
  FXdouble cost=Math::fabs(dot);
  FXdouble sint;
  FXdouble theta;
  FXQuatd result;
  if(__likely(cost<0.999999999999999)){
    sint=Math::sqrt(1.0-cost*cost);
    theta=Math::atan2(sint,cost);
    fr=Math::sin(fr*theta)/sint;
    to=Math::sin(to*theta)/sint;
    }
  result.x=fr*u.x+to*v.x;
  result.y=fr*u.y+to*v.y;
  result.z=fr*u.z+to*v.z;
  result.w=fr*u.w+to*v.w;
  return result;
  }


// Derivative of spherical lerp of unit quaternions u,v
// This is equivalent to: u * (u.unitinvert()*v).pow(f) * (u.unitinvert()*v).log(),
// which is itself equivalent to: lerp(u,v,f) * (u.unitinvert()*v).log()
FXQuatd lerpdot(const FXQuatd& u,const FXQuatd& v,FXdouble f){
  FXdouble dot=u.x*v.x+u.y*v.y+u.z*v.z+u.w*v.w;
  FXdouble cost=Math::fabs(dot);
  FXdouble sint;
  FXdouble fr=1.0-f;
  FXdouble to=f;
  FXdouble theta;
  FXQuatd result;
  if(__likely(cost<0.999999999999999)){
    sint=Math::sqrt(1.0-cost*cost);
    theta=Math::atan2(sint,cost);
    fr=-theta*Math::cos(fr*theta)/sint;
    to=theta*Math::cos(to*theta)/sint;
    }
  result.x=fr*u.x+to*v.x;
  result.y=fr*u.y+to*v.y;
  result.z=fr*u.z+to*v.z;
  result.w=fr*u.w+to*v.w;
  return result;
  }


/*******************************************************************************/

// 1/(i*(2*i+1)) for i>=1
const FXdouble u10_0=0.333333333333333333333333;
const FXdouble u10_1=0.1;
const FXdouble u10_2=0.047619047619047619047619;
const FXdouble u10_3=0.027777777777777777777778;
const FXdouble u10_4=0.018181818181818181818182;
const FXdouble u10_5=0.012820512820512820512820;
const FXdouble u10_6=0.009523809523809523809524;
const FXdouble u10_7=0.007352941176470588235294;
const FXdouble u10_8=0.005847953216374269005848;
const FXdouble u10_9=0.004761904761904761904762*1.87666328810155;

// i/(2*i+1) for i>=1
const FXdouble v10_0=0.333333333333333333333333;
const FXdouble v10_1=0.4;
const FXdouble v10_2=0.428571428571428571428571;
const FXdouble v10_3=0.444444444444444444444444;
const FXdouble v10_4=0.454545454545454545454545;
const FXdouble v10_5=0.461538461538461538461538;
const FXdouble v10_6=0.466666666666666666666667;
const FXdouble v10_7=0.470588235294117647058824;
const FXdouble v10_8=0.473684210526315789473684;
const FXdouble v10_9=0.476190476190476190476190*1.87666328810155;


// Fast approximate spherical lerp of unit quaternions u,v (with angle between u,v < pi/2)
// Based on "A Fast and Accurate Algorithm for Computing SLERP", by David Eberly.
FXQuatd fastlerp(const FXQuatd& u,const FXQuatd& v,FXdouble t){
  FXdouble xm1=u.x*v.x+u.y*v.y+u.z*v.z+u.w*v.w-1.0;     // x-1 = cos(theta)-1
  FXdouble f=1.0-t;
  FXdouble tt=t*t;
  FXdouble ff=f*f;
  FXdouble F0=(u10_0*ff-v10_0)*xm1;
  FXdouble F1=(u10_1*ff-v10_1)*xm1;
  FXdouble F2=(u10_2*ff-v10_2)*xm1;
  FXdouble F3=(u10_3*ff-v10_3)*xm1;
  FXdouble F4=(u10_4*ff-v10_4)*xm1;
  FXdouble F5=(u10_5*ff-v10_5)*xm1;
  FXdouble F6=(u10_6*ff-v10_6)*xm1;
  FXdouble F7=(u10_7*ff-v10_7)*xm1;
  FXdouble F8=(u10_8*ff-v10_8)*xm1;
  FXdouble F9=(u10_9*ff-v10_9)*xm1;
  FXdouble T0=(u10_0*tt-v10_0)*xm1;
  FXdouble T1=(u10_1*tt-v10_1)*xm1;
  FXdouble T2=(u10_2*tt-v10_2)*xm1;
  FXdouble T3=(u10_3*tt-v10_3)*xm1;
  FXdouble T4=(u10_4*tt-v10_4)*xm1;
  FXdouble T5=(u10_5*tt-v10_5)*xm1;
  FXdouble T6=(u10_6*tt-v10_6)*xm1;
  FXdouble T7=(u10_7*tt-v10_7)*xm1;
  FXdouble T8=(u10_8*tt-v10_8)*xm1;
  FXdouble T9=(u10_9*tt-v10_9)*xm1;
  FXdouble F=f*(1.0+F0*(1.0+F1*(1.0+F2*(1.0+F3*(1.0+F4*(1.0+F5*(1.0+F6*(1.0+F7*(1.0+F8*(1.0+F9))))))))));
  FXdouble T=t*(1.0+T0*(1.0+T1*(1.0+T2*(1.0+T3*(1.0+T4*(1.0+T5*(1.0+T6*(1.0+T7*(1.0+T8*(1.0+T9))))))))));
  return u*F+v*T;
  }

#if 0

// 1/(i*(2*i+1)) for i>=1
const FXdouble u12_0=0.333333333333333333333333;
const FXdouble u12_1=0.1;
const FXdouble u12_2=0.047619047619047619047619;
const FXdouble u12_3=0.027777777777777777777778;
const FXdouble u12_4=0.018181818181818181818182;
const FXdouble u12_5=0.012820512820512820512820;
const FXdouble u12_6=0.009523809523809523809524;
const FXdouble u12_7=0.007352941176470588235294;
const FXdouble u12_8=0.005847953216374269005848;
const FXdouble u12_9=0.004761904761904761904762;
const FXdouble u12_10=0.003952569169960474308300;
const FXdouble u12_11=0.00333333333333333333333333*1.89371240325272;

// i/(2*i+1) for i>=1
const FXdouble v12_0=0.333333333333333333333333;
const FXdouble v12_1=0.4;
const FXdouble v12_2=0.428571428571428571428571;
const FXdouble v12_3=0.444444444444444444444444;
const FXdouble v12_4=0.454545454545454545454545;
const FXdouble v12_5=0.461538461538461538461538;
const FXdouble v12_6=0.466666666666666666666667;
const FXdouble v12_7=0.470588235294117647058824;
const FXdouble v12_8=0.473684210526315789473684;
const FXdouble v12_9=0.476190476190476190476190;
const FXdouble v12_10=0.478260869565217391304348;
const FXdouble v12_11=0.48*1.89371240325272;


// About 26 clocks, err = ~1E-6
FXQuatd fastlerp12(const FXQuatd& u,const FXQuatd& v,FXdouble t){
  FXdouble xm1=u.x*v.x+u.y*v.y+u.z*v.z+u.w*v.w-1.0;      // x-1 = cos(theta)-1
  FXdouble f=1.0-t;
  FXdouble tt=t*t;
  FXdouble ff=f*f;
  FXdouble F0=(u12_0*ff-v12_0)*xm1;
  FXdouble F1=(u12_1*ff-v12_1)*xm1;
  FXdouble F2=(u12_2*ff-v12_2)*xm1;
  FXdouble F3=(u12_3*ff-v12_3)*xm1;
  FXdouble F4=(u12_4*ff-v12_4)*xm1;
  FXdouble F5=(u12_5*ff-v12_5)*xm1;
  FXdouble F6=(u12_6*ff-v12_6)*xm1;
  FXdouble F7=(u12_7*ff-v12_7)*xm1;
  FXdouble F8=(u12_8*ff-v12_8)*xm1;
  FXdouble F9=(u12_9*ff-v12_9)*xm1;
  FXdouble F10=(u12_10*ff-v12_10)*xm1;
  FXdouble F11=(u12_11*ff-v12_11)*xm1;
  FXdouble T0=(u12_0*tt-v12_0)*xm1;
  FXdouble T1=(u12_1*tt-v12_1)*xm1;
  FXdouble T2=(u12_2*tt-v12_2)*xm1;
  FXdouble T3=(u12_3*tt-v12_3)*xm1;
  FXdouble T4=(u12_4*tt-v12_4)*xm1;
  FXdouble T5=(u12_5*tt-v12_5)*xm1;
  FXdouble T6=(u12_6*tt-v12_6)*xm1;
  FXdouble T7=(u12_7*tt-v12_7)*xm1;
  FXdouble T8=(u12_8*tt-v12_8)*xm1;
  FXdouble T9=(u12_9*tt-v12_9)*xm1;
  FXdouble T10=(u12_10*tt-v12_10)*xm1;
  FXdouble T11=(u12_11*tt-v12_11)*xm1;
  FXdouble F=f*(1.0+F0*(1.0+F1*(1.0+F2*(1.0+F3*(1.0+F4*(1.0+F5*(1.0+F6*(1.0+F7*(1.0+F8*(1.0+F9*(1.0+F10*(1.0+F11))))))))))));
  FXdouble T=t*(1.0+T0*(1.0+T1*(1.0+T2*(1.0+T3*(1.0+T4*(1.0+T5*(1.0+T6*(1.0+T7*(1.0+T8*(1.0+T9*(1.0+T10*(1.0+T11))))))))))));
  return u*F+v*T;
  }


// 1/(i*(2*i+1)) for i>=1
const FXdouble u16_0=0.333333333333333333333333;
const FXdouble u16_1=0.1;
const FXdouble u16_2=0.047619047619047619047619;
const FXdouble u16_3=0.027777777777777777777778;
const FXdouble u16_4=0.018181818181818181818182;
const FXdouble u16_5=0.012820512820512820512820;
const FXdouble u16_6=0.009523809523809523809524;
const FXdouble u16_7=0.007352941176470588235294;
const FXdouble u16_8=0.005847953216374269005848;
const FXdouble u16_9=0.004761904761904761904762;
const FXdouble u16_10=0.003952569169960474308300;
const FXdouble u16_11=0.00333333333333333333333333;
const FXdouble u16_12=0.002849002849002849002849;
const FXdouble u16_13=0.00246305418719211822660099;
const FXdouble u16_14=0.00215053763440860215053763;
const FXdouble u16_15=0.00189393939393939393939394*1.91666919924319;     // Best value if angle <pi/2
//const FXdouble u16_15=0.00189393939393939393939394*1.06647791713476;   // Best value if angle <pi/4

// i/(2*i+1) for i>=1
const FXdouble v16_0=0.333333333333333333333333;
const FXdouble v16_1=0.4;
const FXdouble v16_2=0.428571428571428571428571;
const FXdouble v16_3=0.444444444444444444444444;
const FXdouble v16_4=0.454545454545454545454545;
const FXdouble v16_5=0.461538461538461538461538;
const FXdouble v16_6=0.466666666666666666666667;
const FXdouble v16_7=0.470588235294117647058824;
const FXdouble v16_8=0.473684210526315789473684;
const FXdouble v16_9=0.476190476190476190476190;
const FXdouble v16_10=0.478260869565217391304348;
const FXdouble v16_11=0.48;
const FXdouble v16_12=0.481481481481481481481481;
const FXdouble v16_13=0.482758620689655172413793;
const FXdouble v16_14=0.483870967741935483870968;
const FXdouble v16_15=0.484848484848484848484848*1.91666919924319;       // Best value if angle <pi/2
//const FXdouble v16_15=0.484848484848484848484848*1.06647791713476;     // Best value if angle <pi/4


// About 39 clocks, err = ~3.5E-8
FXQuatd fastlerp16(const FXQuatd& u,const FXQuatd& v,FXdouble t){
  FXdouble xm1=u.x*v.x+u.y*v.y+u.z*v.z+u.w*v.w-1.0;      // x-1 = cos(theta)-1
  FXdouble f=1.0-t;
  FXdouble tt=t*t;
  FXdouble ff=f*f;
  FXdouble F0=(u16_0*ff-v16_0)*xm1;
  FXdouble F1=(u16_1*ff-v16_1)*xm1;
  FXdouble F2=(u16_2*ff-v16_2)*xm1;
  FXdouble F3=(u16_3*ff-v16_3)*xm1;
  FXdouble F4=(u16_4*ff-v16_4)*xm1;
  FXdouble F5=(u16_5*ff-v16_5)*xm1;
  FXdouble F6=(u16_6*ff-v16_6)*xm1;
  FXdouble F7=(u16_7*ff-v16_7)*xm1;
  FXdouble F8=(u16_8*ff-v16_8)*xm1;
  FXdouble F9=(u16_9*ff-v16_9)*xm1;
  FXdouble F10=(u16_10*ff-v16_10)*xm1;
  FXdouble F11=(u16_11*ff-v16_11)*xm1;
  FXdouble F12=(u16_12*ff-v16_12)*xm1;
  FXdouble F13=(u16_13*ff-v16_13)*xm1;
  FXdouble F14=(u16_14*ff-v16_14)*xm1;
  FXdouble F15=(u16_15*ff-v16_15)*xm1;
  FXdouble T0=(u16_0*tt-v16_0)*xm1;
  FXdouble T1=(u16_1*tt-v16_1)*xm1;
  FXdouble T2=(u16_2*tt-v16_2)*xm1;
  FXdouble T3=(u16_3*tt-v16_3)*xm1;
  FXdouble T4=(u16_4*tt-v16_4)*xm1;
  FXdouble T5=(u16_5*tt-v16_5)*xm1;
  FXdouble T6=(u16_6*tt-v16_6)*xm1;
  FXdouble T7=(u16_7*tt-v16_7)*xm1;
  FXdouble T8=(u16_8*tt-v16_8)*xm1;
  FXdouble T9=(u16_9*tt-v16_9)*xm1;
  FXdouble T10=(u16_10*tt-v16_10)*xm1;
  FXdouble T11=(u16_11*tt-v16_11)*xm1;
  FXdouble T12=(u16_12*tt-v16_12)*xm1;
  FXdouble T13=(u16_13*tt-v16_13)*xm1;
  FXdouble T14=(u16_14*tt-v16_14)*xm1;
  FXdouble T15=(u16_15*t-v16_15)*xm1;
  FXdouble F=f*(1.0+F0*(1.0+F1*(1.0+F2*(1.0+F3*(1.0+F4*(1.0+F5*(1.0+F6*(1.0+F7*(1.0+F8*(1.0+F9*(1.0+F10*(1.0+F11*(1.0+F12*(1.0+F13*(1.0+F14*(1.0+F15))))))))))))))));
  FXdouble T=t*(1.0+T0*(1.0+T1*(1.0+T2*(1.0+T3*(1.0+T4*(1.0+T5*(1.0+T6*(1.0+T7*(1.0+T8*(1.0+T9*(1.0+T10*(1.0+T11*(1.0+T12*(1.0+T13*(1.0+T14*(1.0+T15))))))))))))))));
  return u*F+v*T;
  }

#endif

/*******************************************************************************/

// Cubic hermite quaternion interpolation
FXQuatd hermite(const FXQuatd& q0,const FXVec3d& r0,const FXQuatd& q1,const FXVec3d& r1,FXdouble t){
  FXQuatd w1(r0[0]*0.333333333333333333,r0[1]*0.333333333333333333,r0[2]*0.333333333333333333,0.0);
  FXQuatd w3(r1[0]*0.333333333333333333,r1[1]*0.333333333333333333,r1[2]*0.333333333333333333,0.0);
  FXQuatd w2((w1.exp().unitinvert()*q0.unitinvert()*q1*w3.exp().unitinvert()).log());
  FXdouble t2=t*t;
  FXdouble beta3=t2*t;
  FXdouble beta1=3.0*t-3.0*t2+beta3;
  FXdouble beta2=3.0*t2-2.0*beta3;
  return q0*(w1*beta1).exp()*(w2*beta2).exp()*(w3*beta3).exp();
  }

/*******************************************************************************/

// Estimate angular body rates omega from unit quaternions Q0 and Q1 separated by time dt
//
//                      -1
//          2 * log ( Q0   * Q1 )
//   w   =  ---------------------
//    b              dt
//
// Be aware that we don't know how many full revolutions happened between q0 and q1;
// there may be aliasing!
FXVec3d omegaBody(const FXQuatd& q0,const FXQuatd& q1,FXdouble dt){
  FXVec3d omega(0.0,0.0,0.0);
  FXQuatd diff(q0.unitinvert()*q1);
  FXdouble mag2(diff.x*diff.x+diff.y*diff.y+diff.z*diff.z);
  if(0.0<mag2){
    FXdouble mag(Math::sqrt(mag2));
    FXdouble phi(2.0*Math::atan2(mag,diff.w));
    FXdouble s(Math::wrap(phi)/(mag*dt));       // Wrap angle -PI*2...PI*2 to -PI...PI range
    omega.x=diff.x*s;
    omega.y=diff.y*s;
    omega.z=diff.z*s;
    }
  return omega;
  }

/*******************************************************************************/

// Derivative q' of orientation quaternion q with angular body rates omega (rad/s)
//
//   .
//   Q = 0.5 * Q * w
//
FXQuatd quatDot(const FXQuatd& q,const FXVec3d& omega){
  return FXQuatd( 0.5*(omega.x*q.w-omega.y*q.z+omega.z*q.y),
                  0.5*(omega.x*q.z+omega.y*q.w-omega.z*q.x),
                  0.5*(omega.y*q.x+omega.z*q.w-omega.x*q.y),
                 -0.5*(omega.x*q.x+omega.y*q.y+omega.z*q.z));
  }

/*******************************************************************************/

// Calculate angular acceleration of a body with inertial moments tensor M
// Rotationg about its axes with angular rates omega, under a torque torq.
// Formula is:
//
//   .         -1
//   w    =   M   * ( T   -   w  x  ( M * w )
//    b                        b           b
//
// Where M is inertia tensor:
//
//      ( Ixx   Ixy   Ixz )                                                              T
//  M = ( Iyx   Iyy   Iyz )     , with Ixy == Iyz,  Ixz == Izx,  Iyz == Izy, i.e. M == M
//      ( Izx   Izy   Izz )
//
// In the unlikely case that M is singular, return angular acceleration of 0.
FXVec3d omegaDot(const FXMat3d& M,const FXVec3d& omega,const FXVec3d& torq){
  FXVec3d result(0.0,0.0,0.0);
  FXdouble Ixx=M[0][0];
  FXdouble Ixy=M[0][1];
  FXdouble Ixz=M[0][2];
  FXdouble Iyy=M[1][1];
  FXdouble Iyz=M[1][2];
  FXdouble Izz=M[2][2];
  FXdouble m00=Iyy*Izz-Iyz*Iyz;                 // Cofactors of M
  FXdouble m01=Ixz*Iyz-Ixy*Izz;
  FXdouble m02=Ixy*Iyz-Ixz*Iyy;
  FXdouble m11=Ixx*Izz-Ixz*Ixz;
  FXdouble m12=Ixy*Ixz-Ixx*Iyz;
  FXdouble m22=Ixx*Iyy-Ixy*Ixy;
  FXdouble det=Ixx*m00+Ixy*m01+Ixz*m02;
  FXASSERT(M[0][1]==M[1][0]);
  FXASSERT(M[0][2]==M[2][0]);
  FXASSERT(M[1][2]==M[2][1]);
  if(__likely(det!=0.0)){                       // Check if M is singular
    FXdouble ox=omega.x;
    FXdouble oy=omega.y;
    FXdouble oz=omega.z;
    FXdouble mox=Ixx*ox+Ixy*oy+Ixz*oz;          // M * w
    FXdouble moy=Ixy*ox+Iyy*oy+Iyz*oz;
    FXdouble moz=Ixz*ox+Iyz*oy+Izz*oz;
    FXdouble rhx=torq.x-(oy*moz-oz*moy);        // R = T - w x (M * w)
    FXdouble rhy=torq.y-(oz*mox-ox*moz);
    FXdouble rhz=torq.z-(ox*moy-oy*mox);
    result.x=(m00*rhx+m01*rhy+m02*rhz)/det;     //  -1
    result.y=(m01*rhx+m11*rhy+m12*rhz)/det;     // M   * R
    result.z=(m02*rhx+m12*rhy+m22*rhz)/det;     //
    }
  return result;
  }


// Save vector to stream
FXStream& operator<<(FXStream& store,const FXQuatd& v){
  store << v.x << v.y << v.z << v.w;
  return store;
  }

// Load vector from stream
FXStream& operator>>(FXStream& store,FXQuatd& v){
  store >> v.x >> v.y >> v.z >> v.w;
  return store;
  }

}
