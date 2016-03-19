/********************************************************************************
*                                                                               *
*                          D i c t i o n a r y    C l a s s                     *
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
#ifndef FXDICTIONARY_H
#define FXDICTIONARY_H

namespace FX {


/**
* The dictionary class maintains a fast-access hash table of entities
* indexed by a character string.
*/
class FXAPI FXDictionary {
protected:
  struct Entry {
    FXString key;       // Lookup key
    FXptr    data;      // Data pointer
    FXuint   hash;      // Hash of key
    };
protected:
  Entry*     table;     // Hash table
protected:

  // Change size of the table
  FXbool no(FXival n);

  // Change number of used entries
  void used(FXival u){ ((FXival*)table)[-2]=u; }

  // Change number of free entries
  void free(FXival f){ ((FXival*)table)[-3]=f; }

  // Resize the table to the given size, keeping contents
  FXbool resize(FXival n);
public:

  /// Construct empty dictionary
  FXDictionary();

  /// Construct from another dictionary
  FXDictionary(const FXDictionary& other);

  /// Return the size of the table, including the empty slots
  FXival no() const { return ((FXival*)table)[-1]; }

  /// Return number of used slots in the table
  FXival used() const { return ((FXival*)table)[-2]; }

  /// Return number of free slots in the table
  FXival free() const { return ((FXival*)table)[-3]; }

  /// See if map is empty
  FXbool empty() const { return ((FXival*)table)[-1]<=1; }

  /// Assignment operator
  FXDictionary& operator=(const FXDictionary& other);

  /// Adopt dictionary from another
  FXDictionary& adopt(FXDictionary& other);

  /// Find data pointer given key
  FXptr find(const FXchar* ky) const;

  /// Find data pointer given key
  FXptr find(const FXString& ky) const { return find(ky.text()); }

  /// Return reference to slot assocated with key
  FXptr& at(const FXchar* ky);

  /// Return constant reference to slot assocated with key
  const FXptr& at(const FXchar* ky) const;

  /// Return reference to slot assocated with key
  FXptr& at(const FXString& ky){ return at(ky.text()); }

  /// Return constant reference to slot assocated with key
  const FXptr& at(const FXString& ky) const { return at(ky.text()); }

  /// Return reference to slot assocated with key
  FXptr& operator[](const FXchar* ky){ return at(ky); }

  /// Return constant reference to slot assocated with key
  const FXptr& operator[](const FXchar* ky) const { return at(ky); }

  /// Return reference to slot assocated with key
  FXptr& operator[](const FXString& ky){ return at(ky); }

  /// Return constant reference to slot assocated with key
  const FXptr& operator[](const FXString& ky) const { return at(ky); }

  /**
  * Insert data into the table with given key.
  * If there is an existing entry with the key, leave it unchanged,
  */
  FXptr insert(const FXchar* ky,FXptr ptr=NULL);
  
  /**
  * Insert data into the table with given key.
  * If there is an existing entry with the key, leave it unchanged,
  */
  FXptr insert(const FXString& ky,FXptr ptr=NULL){ return insert(ky.text(),ptr); }

  /**
  * Replace data in table with given key.
  * If there is no existing entry with the key, a new entry is inserted.
  */
  FXptr replace(const FXchar* ky,FXptr ptr=NULL);

  /**
  * Replace data in table with given key.
  * If there is no existing entry with the key, a new entry is inserted.
  */
  FXptr replace(const FXString& ky,FXptr ptr=NULL){ return replace(ky.text(),ptr); }

  /**
  * Remove data given key from table, returning old pointer.
  */
  FXptr remove(const FXchar* ky);

  /**
  * Remove data given key from table, returning old pointer.
  */
  FXptr remove(const FXString& ky){ return remove(ky.text()); }

  /// Return key at position pos
  const FXString& key(FXival pos) const { return table[pos].key; }

  /// Return data pointer at position pos
  FXptr& data(FXival pos){ return table[pos].data; }

  /// Return data pointer at position pos
  const FXptr& data(FXival pos) const { return table[pos].data; }

  /// Clear all entries
  void clear();

  /// Destructor
 ~FXDictionary();
  };

}

#endif
