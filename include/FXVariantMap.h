/********************************************************************************
*                                                                               *
*                              V a r i a n t - M a p                            *
*                                                                               *
*********************************************************************************
* Copyright (C) 1998,2013 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#ifndef FXVARIANTMAP_H
#define FXVARIANTMAP_H

namespace FX {


/**
* Variant map associates strings to variants using fast hash
* table.
*/
class FXAPI FXVariantMap {
protected:
  struct Entry {
    FXString  key;      // Lookup key
    FXVariant data;     // Variant data
    FXuint    hash;     // Hash of key
    };
protected:
  Entry      *table;    // Hash table
protected:
  FXbool no(FXival n);
  FXbool resize(FXival n);
  void used(FXival u){ ((FXival*)table)[-2]=u; }
  void free(FXival f){ ((FXival*)table)[-3]=f; }
public:

  /// Construct an empty map
  FXVariantMap();

  /// Construct from another map
  FXVariantMap(const FXVariantMap& other);

  /// Return the size of the table, including the empty slots
  FXival no() const { return ((const FXival*)table)[-1]; }

  /// Return number of used slots in the table
  FXival used() const { return ((const FXival*)table)[-2]; }

  /// Return number of free slots in the table
  FXival free() const { return ((const FXival*)table)[-3]; }

  /// Adopt map from another
  FXVariantMap& adopt(FXVariantMap& other);

  /// Find slot index for key; return -1 if not found
  FXival find(const FXchar* ky) const;

  /// Find slot index for key; return -1 if not found
  FXival find(const FXString& ky) const { return find(ky.text()); }

  /// Return reference to variant assocated with key
  FXVariant& at(const FXchar* ky);

  /// Return constant reference to variant assocated with key
  const FXVariant& at(const FXchar* ky) const;

  /// Return reference to variant assocated with key
  FXVariant& at(const FXString& ky){ return at(ky.text()); }

  /// Return constant reference to variant assocated with key
  const FXVariant& at(const FXString& ky) const { return at(ky.text()); }

  /// Return reference to variant assocated with key
  FXVariant& operator[](const FXchar* ky){ return at(ky); }

  /// Return constant reference to variant assocated with key
  const FXVariant& operator[](const FXchar* ky) const { return at(ky); }

  /// Return reference to variant assocated with key
  FXVariant& operator[](const FXString& ky){ return at(ky); }

  /// Return constant reference to variant assocated with key
  const FXVariant& operator[](const FXString& ky) const { return at(ky); }

  /// Assignment operator
  FXVariantMap& operator=(const FXVariantMap& other);

  /// Remove entry from the table
  void remove(const FXchar* ky);

  /// Remove entry from the table
  void remove(const FXString& ky){ remove(ky.text()); }

  /// Equality operator
  FXbool operator==(const FXVariantMap& other) const;

  /// Inequality operator
  FXbool operator!=(const FXVariantMap& other) const { return !operator==(other); }

  /// Return key value at slot s; may be empty!
  const FXString& key(FXival s) const { return table[s].key; }

  /// Return value at slot s; may be empty!
  const FXVariant& data(FXival s) const { return table[s].data; }

  /// Clear the table
  void clear();

  /// Destructor
 ~FXVariantMap();
  };

}

#endif
