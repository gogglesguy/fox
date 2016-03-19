/********************************************************************************
*                                                                               *
*          S i n g l e - P r e c i s i o n   C o m p l e x   N u m b e r        *
*                                                                               *
*********************************************************************************
* Copyright (C) 2006,2014 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#ifndef FXCOMPLEXF_H
#define FXCOMPLEXF_H


namespace FX {


/// Single-precision complex
class FXAPI FXComplexf {
public:
  FXfloat re;
  FXfloat im;
public:

  /// Default constructor; value is not initialized
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

  /// Destructor
 ~FXComplexf(){}
  };


/// Return complex complex conjugate
inline FXComplexf conjugate(const FXComplexf& c){ return FXComplexf(c.re,-c.im); }

/// Return complex number from modulus and argument
inline FXComplexf polar(FXfloat mod,FXfloat arg){ return FXComplexf(cosf(arg)*mod,sinf(arg)*mod); }

/// Returns the complex base e exponential of c
inline FXComplexf exponent(const FXComplexf& c){ return polar(expf(c.re),c.im); }

/// Returns the complex base e logarithm of c
inline FXComplexf logarithm(const FXComplexf& c){ return FXComplexf(logf(c.modulus()),c.argument()); }


/// Equality between complex and real
inline FXbool operator==(const FXComplexf& c,FXfloat r){ return c.re==r && c.im==0.0f; }
inline FXbool operator!=(const FXComplexf& c,FXfloat r){ return c.re!=r || c.im!=0.0f; }

/// Equality between real and complex
inline FXbool operator==(FXfloat r,const FXComplexf& c){ return r==c.re && c.im==0.0f; }
inline FXbool operator!=(FXfloat r,const FXComplexf& c){ return r!=c.re || c.im!=0.0f; }

/// Equality between one complex and another
inline FXbool operator==(const FXComplexf& a,const FXComplexf& b){ return a.re==b.re && a.im==b.im; }
inline FXbool operator!=(const FXComplexf& a,const FXComplexf& b){ return a.re!=b.re || a.im!=b.im; }

/// Operators between complex and real
inline FXComplexf operator+(const FXComplexf& a,FXfloat b){ return FXComplexf(a.re+b,a.im); }
inline FXComplexf operator-(const FXComplexf& a,FXfloat b){ return FXComplexf(a.re-b,a.im); }
inline FXComplexf operator*(const FXComplexf& a,FXfloat b){ return FXComplexf(a.re*b,a.im*b); }
inline FXComplexf operator/(const FXComplexf& a,FXfloat b){ return FXComplexf(a.re/b,a.im/b); }

/// Operators between real and complex
inline FXComplexf operator+(FXfloat a,const FXComplexf& b){ return FXComplexf(a+b.re,b.im); }
inline FXComplexf operator-(FXfloat a,const FXComplexf& b){ return FXComplexf(a-b.re,-b.im); }
inline FXComplexf operator*(FXfloat a,const FXComplexf& b){ return FXComplexf(a*b.re,a*b.im); }
inline FXComplexf operator/(FXfloat a,const FXComplexf& b){ FXfloat m=b.modulus2(); return FXComplexf((a*b.re)/m,(-a*b.im)/m); }

/// Operators between one complex and another
inline FXComplexf operator+(const FXComplexf& a,const FXComplexf& b){ return FXComplexf(a.re+b.re,a.im+b.im); }
inline FXComplexf operator-(const FXComplexf& a,const FXComplexf& b){ return FXComplexf(a.re-b.re,a.im-b.im); }
inline FXComplexf operator*(const FXComplexf& a,const FXComplexf& b){ return FXComplexf(a.re*b.re-a.im*b.im,a.re*b.im+a.im*b.re); }
inline FXComplexf operator/(const FXComplexf& a,const FXComplexf& b){ FXfloat m=b.modulus2(); return FXComplexf((a.re*b.re+a.im*b.im)/m,(a.im*b.re-a.re*b.im)/m); }

/// Complex square root
extern FXAPI FXComplexf csqrt(const FXComplexf& c);

/// Complex sine
extern FXAPI FXComplexf csin(const FXComplexf& c);

/// Complex cosine
extern FXAPI FXComplexf ccos(const FXComplexf& c);

/// Complex tangent
extern FXAPI FXComplexf ctan(const FXComplexf& c);

/// Complex hyperbolic sine 
extern FXAPI FXComplexf csinh(const FXComplexf& c);

/// Complex hyperbolic cosine
extern FXAPI FXComplexf ccosh(const FXComplexf& c);

/// Complex hyperbolic tangent
extern FXAPI FXComplexf ctanh(const FXComplexf& c);

/// Save to a stream
extern FXAPI FXStream& operator<<(FXStream& store,const FXComplexf& c);

/// Load from a stream
extern FXAPI FXStream& operator>>(FXStream& store,FXComplexf& c);

}

#endif
