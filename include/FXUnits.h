/********************************************************************************
*                                                                               *
*                        U n i t - C o n v e r s i o n s                        *
*                                                                               *
*********************************************************************************
* Copyright (C) 2026 by Jeroen van der Zijp.   All Rights Reserved.             *
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
#ifndef FXUNITS_H
#define FXUNITS_H

namespace FX {


/**
* Unit conversion class.
* Convert from any unit to any other unit, via intermediate S.I.
* unit representation.
* There are 9 basic units in the S.I. system (kg,m,A,cd,mol,rad,sr,s,K).
* We have a lot of derived units, which can be expressed in either other
* derived units or, eventually, into a collection of the base units.
* The derived units are described in terms of unit sub-expressions.
* To ensure unit-dimensions are correct when setting up a conversion,
* this class supports a so-called dimension-vector; currently, implemented
* as a single 64-bit integer number which encodes a combination of the
* base units.  This dimension-vector is calculated for both source-unit
* and destination-unit, and then compared; if there is no match, then
* an error is detected (the FXUnits object returned is empty).
*
* In order to bulk-transform numbers from one unit to another, the
* operator needs to perform only a single multiply and add (hopefully,
* your compiler will turn this into a FMA (Fused-Multiply-Add), making
* it a single instruction only).
*
* Temperature scale is the only one that needs an offset; this offset
* only applies for temperature units that are "pure temperature", i.e.
* no other units in the mix, and no other powers than 1.
*
* Scaling prefixes (y,z,a,f,p,n,μ,u,m,c,d,h,k,M,G,T,P,E,Z,Y), these are
* incorporated into the conversion automatically, like "1.21GW".
*/
class FXAPI FXUnits {
private:
  FXdouble m;           // Scale
  FXdouble a;           // Offset
public:
  enum {
    GRAM,               // Mass
    METER,              // Length
    AMPERE,             // Current
    CANDELA,            // Luminous flux
    MOLE,               // Mole
    RADIAN,             // Angles
    STERADIAN,          // Solid angles
    SECOND,             // Time
    KELVIN,             // Temperature / Kelvin
    CELSIUS,            // Temperature / Celsius
    FAHRENHEIT,         // Temperature / Fahrenheit
    RANKINE,            // Temperature / Rankine
    };
  enum {
    NumUnits=133        // Total number of units
    };
public:

  /// List of unit names.
  static const FXchar symbol[NumUnits][8];

  /// List of conversion factors.
  static const FXdouble factor[NumUnits];

  /// Unit sub-expression for non-basic units; for
  /// basic units, this would be empty string.
  static const FXchar expression[NumUnits][16];
private:
  struct Conv;
  static const char* divex(const FXchar* unit,Conv& u);
  static const FXchar* unitex(const FXchar* unit,Conv& u);
  static const FXchar* scalex(const FXchar* unit,Conv& u);
  static const FXchar* powex(const FXchar* unit,Conv& u);
  static const FXchar* mulex(const FXchar* unit,Conv& u);
public:

  /// Initialize to identity
  FXUnits():m(1.0),a(0.0){ }

  /// Initialize only with scale
  FXUnits(FXdouble mm,FXdouble aa=0.0):m(mm),a(aa){ }

  /// Copy from other units
  FXUnits(const FXUnits& org):m(org.m),a(org.a){ }

  /// Check if non-empty
  operator FXbool() const { return (m || a); }

  /// Assign from other units
  FXUnits& operator=(const FXUnits& org){m=org.m;a=org.a;return *this;}

  /// Convert number x according to transformation
  FXdouble operator()(FXdouble x) const { return x*m+a; }

  /// Return the transformation
  FXUnits invert() const { return FXUnits(1.0/m,-a/m); }

  /// Look up unit; return -1 if not found
  static FXint lookup(const FXchar* unit);
  static FXint lookup(const FXString& unit);

  /// Return number of characters consumed from unit-string
  static FXint span(const FXchar* unit);
  static FXint span(const FXString& unit);

  /// Return dimension-description from a unit-string
  static FXulong dimensions(const FXchar* unit);
  static FXulong dimensions(const FXString& unit);

  /// Return canonical (S.I.) unit-string from dimension-description
  static FXString canonical(FXulong dims);

  /// Convert from source unit to destination unit; default is convert to S.I.
  static FXUnits convertFromTo(const FXchar* srcUnit,const FXchar* dstUnit=nullptr);
  static FXUnits convertFromTo(const FXString& srcUnit,const FXchar* dstUnit=nullptr);
  static FXUnits convertFromTo(const FXchar* srcUnit,const FXString& dstUnit);
  static FXUnits convertFromTo(const FXString& srcUnit,const FXString& dstUnit);

  /// Convert to destination unit from source unit; default is convert from S.I.
  static FXUnits convertToFrom(const FXchar* dstUnit,const FXchar* srcUnit=nullptr);
  static FXUnits convertToFrom(const FXString& dstUnit,const FXchar* srcUnit=nullptr);
  static FXUnits convertToFrom(const FXchar* dstUnit,const FXString& srcUnit);
  static FXUnits convertToFrom(const FXString& dstUnit,const FXString& srcUnit);

  /// Convert from source units to S.I., checking dimension-description
  static FXUnits convertFromToDims(const FXchar* srcUnit,FXulong dims);
  static FXUnits convertFromToDims(const FXString& srcUnit,FXulong dims);

  /// Convert to destimation units from S.I., checking dimension-description
  static FXUnits convertToFromDims(const FXchar* dstUnit,FXulong dims);
  static FXUnits convertToFromDims(const FXString& dstUnit,FXulong dims);

  };

}

#endif
