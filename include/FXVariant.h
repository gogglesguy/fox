/********************************************************************************
*                                                                               *
*                          V a r i a n t   T y p e                              *
*                                                                               *
*********************************************************************************
* Copyright (C) 2013,2014 by Jeroen van der Zijp.   All Rights Reserved.        *
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


class FXString;
class FXVariantMap;
class FXVariantArray;


/**
* A Variant type can hold any kind of object, be it a boolean,
* integer, real, string, or even array of variants or mapping
* from strings to variants.
*/
class FXAPI FXVariant {
public:
  enum VType {
    VNull=0,            // Simple types
    VBool,
    VChar,
    VInt,
    VUInt,
    VLong,
    VULong,
    VFloat,
    VDouble,
    VPointer,
    VString,            // Complex types
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

  /// Construct and initialize with array
  explicit FXVariant(const FXVariantArray& val);

  /// Construct and initialize with map
  explicit FXVariant(const FXVariantMap& val);

  /// Change type
  void setType(VType t);

  /// Return type
  VType getType() const { return type; }

  /// Return size of array
  FXint no() const;

  /// Change number of elements in array
  FXbool no(FXint n);

  /// Convert to bool; always OK
  FXbool toBool() const;

  /// Convert to pointer
  FXptr toPtr() const;

  /// Convert to char pointer
  const FXchar* toChars() const;

  /// Convert to int
  FXint toInt(FXbool* ok=NULL) const;

  /// Convert to unsigned int
  FXuint toUInt(FXbool* ok=NULL) const;

  /// Convert to long
  FXlong toLong(FXbool* ok=NULL) const;

  /// Convert to unsigned long
  FXulong toULong(FXbool* ok=NULL) const;

  /// Convert to float
  FXfloat toFloat(FXbool* ok=NULL) const;

  /// Convert to double
  FXdouble toDouble(FXbool* ok=NULL) const;

  /// Convert to string
  FXString toString(FXbool* ok=NULL) const;

  /// Convert to bool
  operator FXbool() const { return toBool(); }

  /// Convert to pointer
  operator FXptr() const { return toPtr(); }

  /// Convert to char
  operator FXchar() const { return toInt(); }

  /// Convert to char
  operator FXuchar() const { return toUInt(); }

  /// Convert to short
  operator FXshort() const { return toInt(); }

  /// Convert to unsigned short
  operator FXushort() const { return toUInt(); }

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

  /// Assign with array
  FXVariant& operator=(const FXVariantArray& val);

  /// Assign with map
  FXVariant& operator=(const FXVariantMap& val);

  /// Assign with variant
  FXVariant& operator=(const FXVariant& val);

  /// Adopt variant from another
  FXVariant& adopt(FXVariant& other);

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

  /// Check if key is mapped
  FXbool has(const FXchar* key) const;

  /// Check if key is mapped
  FXbool has(const FXString& key) const { return has(key.text()); }

  /// Return the value of the variant as a pointer
  const FXptr& asPtr() const { return value.p; }

  /// Return the value of the variant as a char pointer; variant MUST be a string
  const FXchar* asChars() const { return value.s; }

  /// Return the value of the variant as a long
  const FXlong& asLong() const { return value.i; }

  /// Return the value of the variant as an unsigned long
  const FXulong& asULong() const { return value.u; }

  /// Return the value of the variant as a double
  const FXdouble& asDouble() const { return value.d; }

  /// Return the value of the variant as a string-reference; variant MUST be a string
  const FXString& asString() const { return *reinterpret_cast<const FXString*>(&value.p); }

  /// Return the value of the variant as an array-reference; variant MUST be a array
  const FXVariantArray& asArray() const { return *reinterpret_cast<const FXVariantArray*>(&value.p); }

  /// Return the value of the variant as an map-reference; variant MUST be a map
  const FXVariantMap& asMap() const { return *reinterpret_cast<const FXVariantMap*>(&value.p); }

  /// Clear the data
  void clear();

  /// Reset to null
  void reset();

  /// Default constant variant
  static const FXVariant null;

  /// Destroy
 ~FXVariant();
  };

}

#endif
