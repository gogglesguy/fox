/********************************************************************************
*                                                                               *
*                          V a r i a n t   T y p e                              *
*                                                                               *
*********************************************************************************
* Copyright (C) 2013 by Jeroen van der Zijp.   All Rights Reserved.             *
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
#ifndef FXVARIANT_H
#define FXVARIANT_H

namespace FX {


// Variant Type
class FXAPI FXVariant {
public:
  enum VType {
    VNull=0,
    VBool,
    VChar,
    VInt,
    VUInt,
    VLong,
    VULong,
    VFloat,
    VDouble,
    VPointer,
    VString,
    VArray,
    VMap
    };
private:
  union VValue {
    FXlong      i;      // Signed integral types
    FXulong     u;      // Unsigned integral types
    FXdouble    d;      // Floating point types
    FXchar*     s;      // Character string
    FXptr       p;      // Pointer types
    };
private:
  VValue        value;  // Current value
  VType         type;   // Type of value
private:
  FXVariant& copy(const FXVariant& other);
  FXVariant& init(VType t);
  FXVariant& reset();
public:

  /// Default constructor makes Null type
  FXVariant();

  /// Copy constructor
  FXVariant(const FXVariant& other);

  /// Construct and initialize with bool
  explicit FXVariant(FXbool val);

  /// Construct and initialize with char
  explicit FXVariant(FXchar val);

  /// Construct and initialize with int
  explicit FXVariant(FXint val);

  /// Construct and initialize with unsigned int
  explicit FXVariant(FXuint val);

  /// Construct and initialize with long
  explicit FXVariant(FXlong val);

  /// Construct and initialize with unsigned long
  explicit FXVariant(FXulong val);

  /// Construct and initialize with float
  explicit FXVariant(FXfloat val);

  /// Construct and initialize with double
  explicit FXVariant(FXdouble val);

  /// Construct and initialize with pointer
  explicit FXVariant(FXptr val);

  /// Construct and initialize with constant string
  explicit FXVariant(const FXchar *val);

  /// Construct and initialize with string
  explicit FXVariant(const FXString& val);

  /// Convert to bool
  FXbool toBool(FXbool* ok=NULL) const;

  /// Convert to char
  FXchar toChar(FXbool* ok=NULL) const;

  /// Convert to int
  FXint toInt(FXbool* ok=NULL) const;

  /// Convert to unsigned int
  FXint toUInt(FXbool* ok=NULL) const;

  /// Convert to long
  FXlong toLong(FXbool* ok=NULL) const;

  /// Convert to unsigned long
  FXulong toULong(FXbool* ok=NULL) const;

  /// Convert to float
  FXfloat toFloat(FXbool* ok=NULL) const;

  /// Convert to double
  FXdouble toDouble(FXbool* ok=NULL) const;

  /// Convert to pointer
  FXptr toPointer(FXbool* ok=NULL) const;

  /// Convert to string
  FXString toString(FXbool* ok=NULL) const;

  /// Convert to bool
  operator FXbool() const { return toBool(); }

  /// Convert to char
  operator FXchar() const { return toChar(); }

  /// Convert to int
  operator FXint() const { return toInt(); }

  /// Convert to unsigned int
  operator FXuint() const { return toUInt(); }

  /// Convert to long
  operator FXlong() const { return toLong(); }

  /// Convert to unsigned long
  operator FXulong() const { return toULong(); }

  /// Convert to float
  operator FXfloat() const { return toFloat(); }

  /// Convert to double
  operator FXdouble() const { return toDouble(); }

  /// Convert to double
  operator FXptr() const { return toPointer(); }

  /// Convert to string
  operator FXString() const { return toString(); }

  /// Assign with bool
  FXVariant& operator=(FXbool val);

  /// Assign with char
  FXVariant& operator=(FXchar val);

  /// Assign with int
  FXVariant& operator=(FXint val);

  /// Assign with unsigned int
  FXVariant& operator=(FXuint val);

  /// Assign with long
  FXVariant& operator=(FXlong val);

  /// Assign with unsigned long
  FXVariant& operator=(FXulong val);

  /// Assign with float
  FXVariant& operator=(FXfloat val);

  /// Assign with double
  FXVariant& operator=(FXdouble val);

  /// Assign with pointer
  FXVariant& operator=(FXptr val);

  /// Assign with constant string
  FXVariant& operator=(const FXchar* val);

  /// Assign with string
  FXVariant& operator=(const FXString& val);

  /// Assign with variant
  FXVariant& operator=(const FXVariant& val);

  /// Return value of object member
  FXVariant& operator[](const FXchar* key);

  /// Return value of object member
  const FXVariant& operator[](const FXchar* key) const;

  /// Return value of object member
  FXVariant& operator[](const FXString& key);

  /// Return value of object member
  const FXVariant& operator[](const FXString& key) const;

  /// Return value of array member
  FXVariant& operator[](FXint idx);

  /// Return value of array member
  const FXVariant& operator[](FXint idx) const;

  /// Equality operator
  FXbool operator==(const FXVariant& other) const;

  /// Inequality operator
  FXbool operator!=(const FXVariant& other) const { return !operator==(other); }

  /// Return type
  VType getType() const { return type; }

  /// Change type
  void setType(VType t);

  /// Check if key is mapped
  FXbool has(const FXchar* key) const;

  /// Check if key is mapped
  FXbool has(const FXString& key) const;

  /// Return size of array
  FXival no() const;

  /// Adopt variant from another
  FXVariant& adopt(FXVariant& other);

  /// Clear the data
  void clear();

  /// Default constant variant
  static const FXVariant null;

  /// Destroy
 ~FXVariant();
  };

}

#endif
