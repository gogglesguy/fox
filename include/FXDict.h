/********************************************************************************
*                                                                               *
*                  S t r i n g   D i c t i o n a r y    C l a s s               *
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
#ifndef FXDICT_H
#define FXDICT_H

#ifndef FXOBJECT_H
#include "FXObject.h"
#endif

namespace FX {


/**
* The dictionary class maintains a fast-access hash table of entities
* indexed by a character string.
* It is typically used to map strings to pointers; however, overloading
* the createData() and deleteData() members allows any type of data to
* be indexed by strings.
*/
class FXAPI FXDict : public FXObject {
  FXDECLARE(FXDict)
protected:
  struct Entry {
    FXchar *key;                // Key string
    void   *data;               // Data
    FXuint  hash;               // Hash value of key
    FXbool  mark;               // Entry is marked
    };
protected:
  FXArray<Entry> table;         // Hash table
  FXival         used;          // Used entries
  FXival         free;          // Free entries
protected:
  static const Entry init;      // Initialization value
protected:
  static FXuint hash(const FXchar* str);
private:
  FXDict(const FXDict&);
  FXDict &operator=(const FXDict&);
protected:

  /**
  * Overload this function in a derived class to return the
  * data pointer given an input pointer; the default implementation
  * just returns the input pointer.
  */
  virtual void *createData(void*);

  /**
  * Overload this function in a derived class to delete the pointer
  * previously returned by createData(); the default implementation
  * does nothing.
  */
  virtual void deleteData(void*);
public:

  /**
  * Construct an empty dictionary.
  */
  FXDict();

  /**
  * Resize the table to the given size.
  */
  FXbool size(FXival m);

  /**
  * Return the size of the table, including the empty slots.
  */
  FXival size() const { return table.no(); }

  /**
  * Return the total number of entries in the table.
  */
  FXival no() const { return used; }

  /**
  * Insert a new entry into the table given key and mark.
  * If there is already an entry with that key, leave it unchanged,
  * otherwise insert the new entry.
  */
  void* insert(const FXchar* ky,void* ptr=NULL,FXbool mrk=false);

  /**
  * Replace data at key, if the entry's mark is less than
  * or equal to the given mark.  If there was no existing entry,
  * a new entry is inserted with the given mark.
  */
  void* replace(const FXchar* ky,void* ptr=NULL,FXbool mrk=false);

  /**
  * Remove data given key.
  */
  void* remove(const FXchar* ky);

  /**
  * Find data pointer given key.
  */
  void* find(const FXchar* ky) const;

  /**
  * Return true if slot is empty.
  */
  FXbool empty(FXival pos) const { return !table[pos].key; }

  /**
  * Return key at position pos.
  */
  const FXchar* key(FXival pos) const { return table[pos].key; }

  /**
  * Return data pointer at position pos.
  */
  void* data(FXival pos) const { return table[pos].data; }

  /**
  * Return mark flag of entry at position pos.
  */
  FXbool mark(FXival pos) const { return table[pos].mark; }

  /**
  * Return position of first filled slot, or >= total
  */
  FXival first() const;

  /**
  * Return position of last filled slot or -1
  */
  FXival last() const;


  /**
  * Return position of next filled slot in hash table
  * or a value greater than or equal to total if no filled
  * slot was found
  */
  FXival next(FXival pos) const;

  /**
  * Return position of previous filled slot in hash table
  * or a -1 if no filled slot was found
  */
  FXival prev(FXival pos) const;

  /// Clear all entries
  void clear();

  /// Destructor
  virtual ~FXDict();
  };

}

#endif
