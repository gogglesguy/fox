/********************************************************************************
*                                                                               *
*                         S t r i n g   T a b l e   C l a s s                   *
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
#ifndef FXSTRINGMAP_H
#define FXSTRINGMAP_H

namespace FX {


/**
* The dictionary class maintains a fast-access hash table of entities
* indexed by a character string.
*/
class FXAPI FXStringMap : public FXDictionary {
public:

  /// Construct empty dictionary
  FXStringMap();

  /// Construct from another dictionary
  FXStringMap(const FXStringMap& other);

  /// Assignment operator
  FXStringMap& operator=(const FXStringMap& other);

  /// Adopt dictionary from another
  FXStringMap& adopt(FXStringMap& other);

  /// Return reference to string assocated with key
  FXString& at(const FXchar* ky);

  /// Return constant reference to string assocated with key
  const FXString& at(const FXchar* ky) const;

  /// Return reference to string assocated with key
  FXString& at(const FXString& ky){ return at(ky.text()); }

  /// Return constant reference to string assocated with key
  const FXString& at(const FXString& ky) const { return at(ky.text()); }

  /// Return reference to string assocated with key
  FXString& operator[](const FXchar* ky){ return at(ky); }

  /// Return constant reference to string assocated with key
  const FXString& operator[](const FXchar* ky) const { return at(ky); }

  /// Return reference to string assocated with key
  FXString& operator[](const FXString& ky){ return at(ky); }

  /// Return constant reference to string assocated with key
  const FXString& operator[](const FXString& ky) const { return at(ky); }

  /// Insert string associated with given key
  void insert(const FXchar* ky,const FXchar* value);

  /// Insert string associated with given key
  void insert(const FXchar* ky,const FXString& value){ return insert(ky,value.text()); }

  /// Insert string associated with given key
  void insert(const FXString& ky,const FXchar* value){ return insert(ky.text(),value); }

  /// Insert string associated with given key
  void insert(const FXString& ky,const FXString& value){ return insert(ky.text(),value.text()); }

  /// Remove association with given key
  void remove(const FXchar* ky);

  /// Remove string associated with given key
  void remove(const FXString& ky){ return remove(ky.text()); }

  /// Erase string at pos in the table
  void erase(FXival pos);

  /// Return string reference to slot at position pos (if key is non-empty!)
  FXString& data(FXival pos){ return reinterpret_cast<FXString&>(FXDictionary::data(pos)); }

  /// Return constant string reference to slot at position pos (if key is non-empty!)
  const FXString& data(FXival pos) const { return reinterpret_cast<const FXString&>(FXDictionary::data(pos)); }

  /// Clear entire table
  void clear();

  /// Default constant string value
  static const FXString null;

  /// Destroy table
 ~FXStringMap();
  };

}

#endif
