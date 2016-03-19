/********************************************************************************
*                                                                               *
*          S i n g l e - P r e c i s i o n   C o m p l e x   N u m b e r        *
*                                                                               *
*********************************************************************************
* Copyright (C) 2006,2008 by Jeroen van der Zijp.   All Rights Reserved.        *
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
* $Id: FXComplexf.h,v 1.16 2008/01/04 15:18:15 fox Exp $                        *
********************************************************************************/
#ifndef FXCOMPLEXF_H
#define FXCOMPLEXF_H


namespace FX {


/// Single-precision complex
class FXAPI FXComplexf {
public:
  FXfloat re;
  FXfloat im;
public:

  /// Default constructor
  FXComplexf(){ }

  /// Construct from real
  FXComplexf(FXfloat r):re(r),im(0.0f){ }

  /// Construct from components
  FXComplexf(FXfloat r,FXfloat i):re(r),im(i){ }

  /// Initialize from another complex
  FXComplexf(const FXComplexf& c):re(c.re),im(c.im){ }

  /// Set value from real
  FXComplexf& set(FXfloat r){ re=r; im=0.0f; return *this; }

  /// Set value from components
  FXComplexf& set(FXfloat r,FXfloat i){ re=r; im=i; return *this;}

  /// Set value from another complex
  FXComplexf& set(const FXComplexf& c){ re=c.re; im=c.im; return *this;}

  /// Test for non-zero
  operator FXbool() const { return (re!=0.0f) || (im!=0.0f); }

  /// Test if zero
  FXbool operator!() const { return (re==0.0f) && (im==0.0f); }

  /// Squared modulus
  FXfloat modulus2() const { return re*re+im*im; }

  /// Modulus or absolute value of complex
  FXfloat modulus() const { return sqrtf(modulus2()); }

  /// Argument of complex
  FXfloat argument() const { return atan2f(im,re); }

  /// Return a non-const reference to the ith element
  FXfloat& operator[](FXint i){ return (&re)[i]; }

  /// Return a const reference to the ith element
  const FXfloat& operator[](FXint i) const { return (&re)[i]; }

  /// Unary
  FXComplexf operator+() const { return *this; }
  FXComplexf operator-() const { return FXComplexf(-re,-im); }

  /// Assignment from real
  FXComplexf& operator=(const FXfloat r){ return set(r); }

  /// Assignment from another complex
  FXComplexf& operator=(const FXComplexf& c){ return set(c); }

  /// Assigning operators with real
  FXComplexf& operator+=(FXfloat r){ re+=r; return *this; }
  FXComplexf& operator-=(FXfloat r){ re-=r; return *this; }
  FXComplexf& operator*=(FXfloat r){ re*=r; im*=r; return *this; }
  FXComplexf& operator/=(FXfloat r){ re/=r; im/=r; return *this; }

  /// Assigning operators with another complex
  FXComplexf& operator+=(const FXComplexf& c){ return set(re+c.re,im+c.im); }
  FXComplexf& operator-=(const FXComplexf& c){ return set(re-c.re,im-c.im); }
  FXComplexf& operator*=(const FXComplexf& c){ return set(re*c.re-im*c.im,re*c.im+im*c.re); }
  FXComplexf& operator/=(const FXComplexf& c){ FXfloat m=c.modulus2(); return set((re*c.re+im*c.im)/m,(im*c.re-re*c.im)/m); }

  /// Equality between one complex and another
  FXbool operator==(const FXComplexf& c) const { return re==c.re && im==c.im; }
  FXbool operator!=(const FXComplexf& c) const { return re!=c.re || im!=c.im; }

  /// Return complex complex conjugate
  friend inline FXComplexf conjugate(const FXComplexf& c);

  /// Return complex number from modulus and argument
  friend inline FXComplexf polar(FXfloat mod,FXfloat arg);

  /// Returns the complex base e exponential of c
  friend inline FXComplexf exponent(const FXComplexf& c);

  /// Returns the complex base e logarithm of c
  friend inline FXComplexf logarithm(const FXComplexf& c);

  /// Equality between one complex and real
  friend inline FXbool operator==(const FXComplexf& c,FXfloat r);
  friend inline FXbool operator!=(const FXComplexf& c,FXfloat r);

  /// Equality between one real and complex
  friend inline FXbool operator==(FXfloat r,const FXComplexf& c);
  friend inline FXbool operator!=(FXfloat r,const FXComplexf& c);

  /// Operators between one complex and another
  friend inline FXComplexf operator+(const FXComplexf& a,const FXComplexf& b);
  friend inline FXComplexf operator-(const FXComplexf& a,const FXComplexf& b);
  friend inline FXComplexf operator*(const FXComplexf& a,const FXComplexf& b);
  friend inline FXComplexf operator/(const FXComplexf& a,const FXComplexf& b);

  /// Operators between complex and real
  friend inline FXComplexf operator+(const FXComplexf& a,FXfloat b);
  friend inline FXComplexf operator-(const FXComplexf& a,FXfloat b);
  friend inline FXComplexf operator*(const FXComplexf& a,FXfloat b);
  friend inline FXComplexf operator/(const FXComplexf& a,FXfloat b);

  /// Operators between real and complex
  friend inline FXComplexf operator+(FXfloat a,const FXComplexf& b);
  friend inline FXComplexf operator-(FXfloat a,const FXComplexf& b);
  friend inline FXComplexf operator*(FXfloat a,const FXComplexf& b);
  friend inline FXComplexf operator/(FXfloat a,const FXComplexf& b);

  /// Save to a stream
  friend FXAPI FXStream& operator<<(FXStream& store,const FXComplexf& c);

  /// Load from a stream
  friend FXAPI FXStream& operator>>(FXStream& store,FXComplexf& c);
  };



inline FXComplexf conjugate(const FXComplexf& c){ return FXComplexf(c.re,-c.im); }
inline FXComplexf polar(FXfloat mod,FXfloat arg){ return FXComplexf(cosf(arg)*mod,sinf(arg)*mod); }
inline FXComplexf exponent(const FXComplexf& c){ return polar(expf(c.re),c.im); }
inline FXComplexf logarithm(const FXComplexf& c){ return FXComplexf(logf(c.modulus()),c.argument()); }


inline FXbool operator==(const FXComplexf& c,FXfloat r){ return c.re==r && c.im==0.0f; }
inline FXbool operator!=(const FXComplexf& c,FXfloat r){ return c.re!=r || c.im!=0.0f; }

inline FXbool operator==(FXfloat r,const FXComplexf& c){ return r==c.re && c.im==0.0f; }
inline FXbool operator!=(FXfloat r,const FXComplexf& c){ return r!=c.re || c.im!=0.0f; }


inline FXComplexf operator+(const FXComplexf& a,const FXComplexf& b){ return FXComplexf(a.re+b.re,a.im+b.im); }
inline FXComplexf operator-(const FXComplexf& a,const FXComplexf& b){ return FXComplexf(a.re-b.re,a.im-b.im); }
inline FXComplexf operator*(const FXComplexf& a,const FXComplexf& b){ return FXComplexf(a.re*b.re-a.im*b.im,a.re*b.im+a.im*b.re); }
inline FXComplexf operator/(const FXComplexf& a,const FXComplexf& b){ FXfloat m=b.modulus2(); return FXComplexf((a.re*b.re+a.im*b.im)/m,(a.im*b.re-a.re*b.im)/m); }


inline FXComplexf operator+(const FXComplexf& a,FXfloat b){ return FXComplexf(a.re+b,a.im); }
inline FXComplexf operator-(const FXComplexf& a,FXfloat b){ return FXComplexf(a.re-b,a.im); }
inline FXComplexf operator*(const FXComplexf& a,FXfloat b){ return FXComplexf(a.re*b,a.im*b); }
inline FXComplexf operator/(const FXComplexf& a,FXfloat b){ return FXComplexf(a.re/b,a.im/b); }


inline FXComplexf operator+(FXfloat a,const FXComplexf& b){ return FXComplexf(a+b.re,b.im); }
inline FXComplexf operator-(FXfloat a,const FXComplexf& b){ return FXComplexf(a-b.re,b.im); }
inline FXComplexf operator*(FXfloat a,const FXComplexf& b){ return FXComplexf(a*b.re,a*b.im); }
inline FXComplexf operator/(FXfloat a,const FXComplexf& b){ FXfloat m=b.modulus2(); return FXComplexf((a*b.re)/m,(-a*b.im)/m); }

extern FXAPI FXStream& operator<<(FXStream& store,const FXComplexf& c);
extern FXAPI FXStream& operator>>(FXStream& store,FXComplexf& c);

}

#endif
