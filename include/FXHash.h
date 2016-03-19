/********************************************************************************
*                                                                               *
*                       H a s h   T a b l e   C l a s s                         *
*                                                                               *
*********************************************************************************
* Copyright (C) 2003,2006 by Jeroen van der Zijp.   All Rights Reserved.        *
*********************************************************************************
* This library is free software; you can redistribute it and/or                 *
* modify it under the terms of the GNU Lesser General Public                    *
* License as published by the Free Software Foundation; either                  *
* version 2.1 of the License, or (at your option) any later version.            *
*                                                                               *
* This library is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             *
* Lesser General Public License for more details.                               *
*                                                                               *
* You should have received a copy of the GNU Lesser General Public              *
* License along with this library; if not, write to the Free Software           *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.    *
*********************************************************************************
* $Id: FXHash.h,v 1.13 2006/03/22 04:52:35 fox Exp $                            *
********************************************************************************/
#ifndef FXHASH_H
#define FXHASH_H

namespace FX {


/**
* A hash table for associating pointers to pointers.
*/
class FXAPI FXHash {
private:
  struct FXEntry {
    void* key;
    void* value;
    };
private:
  FXEntry *table;       // Hash table
  FXuint   total;       // Table size
  FXuint   used;        // Number of used entries
  FXuint   free;        // Number of free entries
private:
  FXHash(const FXHash&);
  FXHash &operator=(const FXHash&);
public:

  /**
  * Construct empty hash table.
  */
  FXHash();

  /**
  * Resize the table to the given size; the size must be
  * a power of two.
  */
  void size(FXuint m);

  /**
  * Return the total number of slots in the table.
  */
  FXint size() const { return total; }

  /**
  * Return number of non-empty slots in the table.
  */
  FXuint no() const { return used; }

  /**
  * Insert key into table, unless the key already exists.
  * Returns the current value of the key.
  */
  void* insert(void* key,void* value);

  /**
  * Replace key in table, overwriting the old value if the
  * given key already exists.  Returns the old value of the key.
  */
  void* replace(void* key,void* value);

  /**
  * Remove key from the table. Returns the old value of the key.
  */
  void* remove(void* key);

  /**
  * Return value of key, or return NULL.
  */
  void* find(void* key) const;

  /**
  * Return true if slot is not occupied by a key.
  */
  bool empty(FXint pos) const { return (table[pos].key==NULL)||(table[pos].key==(void*)-1L); }

  /**
  * Return key at position pos.
  */
  void* key(FXint pos) const { return table[pos].key; }

  /**
  * Return data pointer at position pos.
  */
  void* value(FXint pos) const { return table[pos].value; }

  /**
  * Clear hash table.
  */
  void clear();

  /// Destructor
  virtual ~FXHash();
  };


}

#endif
