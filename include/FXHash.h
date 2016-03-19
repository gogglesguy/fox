/********************************************************************************
*                                                                               *
*                       H a s h   T a b l e   C l a s s                         *
*                                                                               *
*********************************************************************************
* Copyright (C) 2003,2014 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#ifndef FXHASH_H
#define FXHASH_H


namespace FX {


/**
* A hash table for mapping pointers to pointers.
* Any value may be used as a key, except 0 and -1.
*/
class FXAPI FXHash {
protected:
  struct Entry {
    FXptr name;
    FXptr data;
    };
protected:
  Entry  *table;
protected:

  // Change size of the table & hash existing contents
  FXbool no(FXival n);

  // Change number of used entries
  void used(FXival u){ ((FXival*)table)[-2]=u; }

  // Change number of free entries
  void free(FXival f){ ((FXival*)table)[-3]=f; }

  // Resize the table to the given size, keeping contents
  FXbool resize(FXival n);
public:

  /**
  * Construct empty hash table.
  */
  FXHash();

  /**
  * Construct from another table.
  */
  FXHash(const FXHash& other);

  /**
  * Return the total number of slots in the table.
  */
  FXival no() const { return ((FXival*)table)[-1]; }

  /**
  * Return number of used slots in the table.
  */
  FXival used() const { return ((FXival*)table)[-2]; }

  /**
  * Return number of free slots in the table.
  */
  FXival free() const { return ((FXival*)table)[-3]; }

  /**
  * See if hash table is empty
  */
  FXbool empty() const { return ((FXival*)table)[-1]<=1; }

  /**
  * Assign from another table.
  */
  FXHash &operator=(const FXHash& other);

  /**
  * Adopt table from another; the other table becomes empty.
  */
  FXHash& adopt(FXHash& other);

  /**
  * Return value of key, or return NULL.
  */
  FXptr find(FXptr name) const;

  /**
  * Replace key in table, overwriting the old value if the
  * given key already exists.  Returns the old value of the key.
  */
  FXptr insert(FXptr name,FXptr data=NULL);

  /**
  * Remove key from the table. Returns the old value of the key.
  */
  FXptr remove(FXptr name);

  /**
  * Erase entry from table at pos, returning old value.
  */
  FXptr erase(FXival pos);

  /**
  * Return true if slot is not occupied by a key.
  */
  FXbool empty(FXival pos) const { return (table[pos].name==(FXptr)0L)||(table[pos].name==(FXptr)-1L); }

  /**
  * Return key at position pos.
  */
  FXptr key(FXival pos) const { return table[pos].name; }

  /**
  * Return reference to data pointer at position pos.
  */
  FXptr& value(FXival pos){ return table[pos].data; }

  /**
  * Return constant reference data pointer at position pos.
  */
  const FXptr& value(FXival pos) const { return table[pos].data; }

  /**
  * Clear hash table.
  */
  void clear();

  /// Destructor
 ~FXHash();
  };

}

#endif
