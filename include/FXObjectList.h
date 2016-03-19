/********************************************************************************
*                                                                               *
*                            O b j e c t   L i s t                              *
*                                                                               *
*********************************************************************************
* Copyright (C) 1997,2012 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#ifndef FXOBJECTLIST_H
#define FXOBJECTLIST_H

#ifndef FXOBJECT_H
#include "FXObject.h"
#endif

namespace FX {

/// List of pointers to objects
class FXAPI FXObjectList {
protected:
  FXObject **ptr;
public:

  /// Default constructor
  FXObjectList();

  /// Copy constructor
  FXObjectList(const FXObjectList& src);

  /// Construct and init with single object
  FXObjectList(FXObject* object);

  /// Construct and init with n copies of object
  FXObjectList(FXObject* object,FXint n);

  /// Construct and init with list of objects
  FXObjectList(FXObject** objects,FXint n);

  /// Assignment operator
  FXObjectList& operator=(const FXObjectList& orig);

  /// Return number of objects
  FXint no() const { return *((FXint*)(ptr-1)); }

  /// Set number of objects
  FXbool no(FXint num);

  /// Indexing operator
  FXObject*& operator[](FXint i){ return ptr[i]; }
  FXObject* const& operator[](FXint i) const { return ptr[i]; }

  /// Indexing operator
  FXObject*& at(FXint i){ return ptr[i]; }
  FXObject* const& at(FXint i) const { return ptr[i]; }

  /// First element in list
  FXObject*& head(){ return ptr[0]; }
  FXObject* const& head() const { return ptr[0]; }

  /// Last element in list
  FXObject*& tail(){ return ptr[no()-1]; }
  FXObject* const& tail() const { return ptr[no()-1]; }

  /// Access to content array
  FXObject** data(){ return ptr; }
  FXObject *const * data() const { return ptr; }

  /// Adopt objects from orig, leaving orig empty
  void adopt(FXObjectList& orig);

  /// Assign object to list
  FXbool assign(FXObject* object);

  /// Assign n copies of object to list
  FXbool assign(FXObject* object,FXint n);

  /// Assign n objects to list
  FXbool assign(FXObject** objects,FXint n);

  /// Assign objects to list
  FXbool assign(const FXObjectList& objects);

  /// Insert object at certain position
  FXbool insert(FXint pos,FXObject* object);

  /// Insert n copies of object at specified position
  FXbool insert(FXint pos,FXObject* object,FXint n);

  /// Insert n objects at specified position
  FXbool insert(FXint pos,FXObject** objects,FXint n);

  /// Insert objects at specified position
  FXbool insert(FXint pos,const FXObjectList& objects);

  /// Prepend object
  FXbool prepend(FXObject* object);

  /// Prepend n copies of object
  FXbool prepend(FXObject* object,FXint n);

  /// Prepend n objects
  FXbool prepend(FXObject** objects,FXint n);

  /// Prepend objects
  FXbool prepend(const FXObjectList& objects);

  /// Append object
  FXbool append(FXObject* object);

  /// Append n copies of object
  FXbool append(FXObject* object,FXint n);

  /// Append n objects
  FXbool append(FXObject** objects,FXint n);

  /// Append objects
  FXbool append(const FXObjectList& objects);

  /// Replace object at position by given object
  FXbool replace(FXint pos,FXObject* object);

  /// Replaces the m objects at pos with n copies of object
  FXbool replace(FXint pos,FXint m,FXObject* object,FXint n);

  /// Replaces the m objects at pos with n objects
  FXbool replace(FXint pos,FXint m,FXObject** objects,FXint n);

  /// Replace the m objects at pos with objects
  FXbool replace(FXint pos,FXint m,const FXObjectList& objects);

  /// Remove object at pos
  FXbool erase(FXint pos);

  /// Remove n objects at pos
  FXbool erase(FXint pos,FXint n);

  /// Push object to end
  FXbool push(FXObject* object);

  /// Pop object from end
  FXbool pop();

  /// Remove object
  FXbool remove(const FXObject* object);

  /// Find object in list, searching forward; return position or -1
  FXint find(const FXObject *object,FXint pos=0) const;

  /// Find object in list, searching backward; return position or -1
  FXint rfind(const FXObject *object,FXint pos=2147483647) const;

  /// Remove all objects
  void clear();

  /// Save to a stream
  void save(FXStream& store) const;

  /// Load from a stream
  void load(FXStream& store);

  /// Destructor
  virtual ~FXObjectList();
  };


/// Specialize list to pointers to TYPE
template<class TYPE>
class FXObjectListOf : public FXObjectList {
public:
  /// Default constructor
  FXObjectListOf(){}

  /// Copy constructor
  FXObjectListOf(const FXObjectListOf<TYPE>& src):FXObjectList(src){ }

  /// Construct and init with single object
  FXObjectListOf(TYPE* object):FXObjectList(reinterpret_cast<FXObject*>(object)){ }

  /// Construct and init with n copies of object
  FXObjectListOf(TYPE* object,FXint n):FXObjectList(reinterpret_cast<FXObject*>(object),n){ }

  /// Construct and init with list of objects
  FXObjectListOf(TYPE** objects,FXint n):FXObjectList(reinterpret_cast<FXObject**>(objects),n){ }

  /// Indexing operator
  TYPE*& operator[](FXint i){ return reinterpret_cast<TYPE*&>(ptr[i]); }
  TYPE *const& operator[](FXint i) const { return reinterpret_cast<TYPE*const&>(ptr[i]); }

  /// Access to list
  TYPE*& at(FXint i){ return reinterpret_cast<TYPE*&>(ptr[i]); }
  TYPE *const& at(FXint i) const { return reinterpret_cast<TYPE*const&>(ptr[i]); }

  /// First element in list
  TYPE*& head(){ return reinterpret_cast<TYPE*&>(ptr[0]); }
  TYPE* const& head() const { return reinterpret_cast<TYPE*const&>(ptr[0]); }

  /// Last element in list
  TYPE*& tail(){ return reinterpret_cast<TYPE*&>(ptr[no()-1]); }
  TYPE* const& tail() const { return reinterpret_cast<TYPE* const&>(ptr[no()-1]); }

  /// Access to content array
  TYPE** data(){ return reinterpret_cast<TYPE**>(ptr); }
  TYPE *const * data() const { return reinterpret_cast<TYPE*const*>(ptr); }
  };

}

#endif
