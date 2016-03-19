/********************************************************************************
*                                                                               *
*          D o u b l e - P r e c i s i o n   C o m p l e x   N u m b e r        *
*                                                                               *
*********************************************************************************
* Copyright (C) 2006,2009 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#ifndef FXCOMPLEXD_H
#define FXCOMPLEXD_H


namespace FX {


/// Double-precision complex
class FXAPI FXComplexd {
public:
  FXdouble re;
  FXdouble im;
public:

  /// Default constructor; value is not initialized
  FXComplexd(){ }

  /// Construct from real
  FXComplexd(FXdouble r):re(r),im(0.0){ }

  /// Construct from components
  FXComplexd(FXdouble r,FXdouble i):re(r),im(i){ }

  /// Initialize from another complex
  FXComplexd(const FXComplexd& c):re(c.re),im(c.im){ }

  /// Set value from real
  FXComplexd& set(FXdouble r){ re=r; im=0.0; return *this; }

  /// Set value from components
  FXComplexd& set(FXdouble r,FXdouble i){ re=r; im=i; return *this;}

  /// Set value from another complex
  FXComplexd& set(const FXComplexd& c){ re=c.re; im=c.im; return *this;}

  /// Test for non-zero
  operator FXbool() const { return (re!=0.0) || (im!=0.0); }

  /// Test if zero
  FXbool operator!() const { return (re==0.0) && (im==0.0); }

  /// Squared modulus
  FXdouble modulus2() const { return re*re+im*im; }

  /// Modulus or absolute value of complex
  FXdouble modulus() const { return sqrt(modulus2()); }

  /// Argument of complex
  FXdouble argument() const { return atan2(im,re); }

  /// Return a non-const reference to the ith element
  FXdouble& operator[](FXint i){ return (&re)[i]; }

  /// Return a const reference to the ith element
  const FXdouble& operator[](FXint i) const { return (&re)[i]; }

  /// Unary
  FXComplexd operator+() const { return *this; }
  FXComplexd operator-() const { return FXComplexd(-re,-im); }

  /// Assignment from real
  FXComplexd& operator=(const FXdouble r){ return set(r); }

  /// Assignment from another complex
  FXComplexd& operator=(const FXComplexd& c){ return set(c); }

  /// Assigning operators with real
  FXComplexd& operator+=(FXdouble r){ re+=r; return *this; }
  FXComplexd& operator-=(FXdouble r){ re-=r; return *this; }
  FXComplexd& operator*=(FXdouble r){ re*=r; im*=r; return *this; }
  FXComplexd& operator/=(FXdouble r){ re/=r; im/=r; return *this; }

  /// Assigning operators with another complex
  FXComplexd& operator+=(const FXComplexd& c){ return set(re+c.re,im+c.im); }
  FXComplexd& operator-=(const FXComplexd& c){ return set(re-c.re,im-c.im); }
  FXComplexd& operator*=(const FXComplexd& c){ return set(re*c.re-im*c.im,re*c.im+im*c.re); }
  FXComplexd& operator/=(const FXComplexd& c){ FXdouble m=c.modulus2(); return set((re*c.re+im*c.im)/m,(im*c.re-re*c.im)/m); }

  /// Equality between one complex and another
  FXbool operator==(const FXComplexd& c) const { return re==c.re && im==c.im; }
  FXbool operator!=(const FXComplexd& c) const { return re!=c.re || im!=c.im; }
  };


/// Return complex complex conjugate
inline FXComplexd conjugate(const FXComplexd& c){ return FXComplexd(c.re,-c.im); }

/// Return complex number from modulus and argument
inline FXComplexd polar(FXdouble mod,FXdouble arg){ return FXComplexd(cos(arg)*mod,sin(arg)*mod); }

/// Returns the complex base e exponential of c
inline FXComplexd exponent(const FXComplexd& c){ return polar(exp(c.re),c.im); }

/// Returns the complex base e logarithm of c
inline FXComplexd logarithm(const FXComplexd& c){ return FXComplexd(log(c.modulus()),c.argument()); }


/// Equality between complex and real
inline FXbool operator==(const FXComplexd& c,FXdouble r){ return c.re==r && c.im==0.0; }
inline FXbool operator!=(const FXComplexd& c,FXdouble r){ return c.re!=r || c.im!=0.0; }

/// Equality between real and complex
inline FXbool operator==(FXdouble r,const FXComplexd& c){ return r==c.re && c.im==0.0; }
inline FXbool operator!=(FXdouble r,const FXComplexd& c){ return r!=c.re || c.im!=0.0; }


/// Operators between one complex and another
inline FXComplexd operator+(const FXComplexd& a,const FXComplexd& b){ return FXComplexd(a.re+b.re,a.im+b.im); }
inline FXComplexd operator-(const FXComplexd& a,const FXComplexd& b){ return FXComplexd(a.re-b.re,a.im-b.im); }
inline FXComplexd operator*(const FXComplexd& a,const FXComplexd& b){ return FXComplexd(a.re*b.re-a.im*b.im,a.re*b.im+a.im*b.re); }
inline FXComplexd operator/(const FXComplexd& a,const FXComplexd& b){ FXdouble m=b.modulus2(); return FXComplexd((a.re*b.re+a.im*b.im)/m,(a.im*b.re-a.re*b.im)/m); }


/// Operators between complex and real
inline FXComplexd operator+(const FXComplexd& a,FXdouble b){ return FXComplexd(a.re+b,a.im); }
inline FXComplexd operator-(const FXComplexd& a,FXdouble b){ return FXComplexd(a.re-b,a.im); }
inline FXComplexd operator*(const FXComplexd& a,FXdouble b){ return FXComplexd(a.re*b,a.im*b); }
inline FXComplexd operator/(const FXComplexd& a,FXdouble b){ return FXComplexd(a.re/b,a.im/b); }


/// Operators between real and complex
inline FXComplexd operator+(FXdouble a,const FXComplexd& b){ return FXComplexd(a+b.re,b.im); }
inline FXComplexd operator-(FXdouble a,const FXComplexd& b){ return FXComplexd(a-b.re,b.im); }
inline FXComplexd operator*(FXdouble a,const FXComplexd& b){ return FXComplexd(a*b.re,a*b.im); }
inline FXComplexd operator/(FXdouble a,const FXComplexd& b){ FXdouble m=b.modulus2(); return FXComplexd((a*b.re)/m,(-a*b.im)/m); }

/// Save to a stream
extern FXAPI FXStream& operator<<(FXStream& store,const FXComplexd& c);

/// Load from a stream
extern FXAPI FXStream& operator>>(FXStream& store,FXComplexd& c);

}

#endif
