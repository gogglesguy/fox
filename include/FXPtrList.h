/********************************************************************************
*                                                                               *
*                            P o i n t e r   L i s t                            *
*                                                                               *
*********************************************************************************
* Copyright (C) 1997,2013 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#ifndef FXPTRLIST_H
#define FXPTRLIST_H


namespace FX {


/// List of void pointers
class FXAPI FXPtrList {
protected:
  FXptr* ptr;
public:

  /// Default constructor
  FXPtrList();

  /// Copy constructor
  FXPtrList(const FXPtrList& src);

  /// Construct and init with single object
  FXPtrList(FXptr object);

  /// Construct and init with n copies of object
  FXPtrList(FXptr object,FXint n);

  /// Construct and init with list of objects
  FXPtrList(FXptr* objects,FXint n);

  /// Assignment operator
  FXPtrList& operator=(const FXPtrList& orig);

  /// Return number of objects
  FXint no() const { return *((FXint*)(ptr-1)); }

  /// Set number of objects
  FXbool no(FXint num);

  /// Indexing operator
  FXptr& operator[](FXint i){ return ptr[i]; }
  FXptr const& operator[](FXint i) const { return ptr[i]; }

  /// Indexing operator
  FXptr& at(FXint i){ return ptr[i]; }
  FXptr const& at(FXint i) const { return ptr[i]; }

  /// First element in list
  FXptr& head(){ return ptr[0]; }
  FXptr const& head() const { return ptr[0]; }

  /// Last element in list
  FXptr& tail(){ return ptr[no()-1]; }
  FXptr const& tail() const { return ptr[no()-1]; }

  /// Access to content array
  FXptr* data(){ return ptr; }
  const FXptr* data() const { return ptr; }

  /// Adopt objects from orig, leaving orig empty
  FXPtrList& adopt(FXPtrList& orig);

  /// Assign object to list
  FXbool assign(FXptr object);

  /// Assign n copies of object to list
  FXbool assign(FXptr object,FXint n);

  /// Assign n objects to list
  FXbool assign(FXptr* objects,FXint n);

  /// Assign objects to list
  FXbool assign(const FXPtrList& objects);

  /// Insert object at certain position
  FXbool insert(FXint pos,FXptr object);

  /// Insert n copies of object at specified position
  FXbool insert(FXint pos,FXptr object,FXint n);

  /// Insert n objects at specified position
  FXbool insert(FXint pos,FXptr* objects,FXint n);

  /// Insert objects at specified position
  FXbool insert(FXint pos,const FXPtrList& objects);

  /// Prepend object
  FXbool prepend(FXptr object);

  /// Prepend n copies of object
  FXbool prepend(FXptr object,FXint n);

  /// Prepend n objects
  FXbool prepend(FXptr* objects,FXint n);

  /// Prepend objects
  FXbool prepend(const FXPtrList& objects);

  /// Append object
  FXbool append(FXptr object);

  /// Append n copies of object
  FXbool append(FXptr object,FXint n);

  /// Append n objects
  FXbool append(FXptr* objects,FXint n);

  /// Append objects
  FXbool append(const FXPtrList& objects);

  /// Replace object at position by given object
  FXbool replace(FXint pos,FXptr object);

  /// Replaces the m objects at pos with n copies of object
  FXbool replace(FXint pos,FXint m,FXptr object,FXint n);

  /// Replaces the m objects at pos with n objects
  FXbool replace(FXint pos,FXint m,FXptr* objects,FXint n);

  /// Replace the m objects at pos with objects
  FXbool replace(FXint pos,FXint m,const FXPtrList& objects);

  /// Remove object at pos
  FXbool erase(FXint pos);

  /// Remove n objects at pos
  FXbool erase(FXint pos,FXint n);

  /// Push object to end
  FXbool push(FXptr object);

  /// Pop object from end
  FXbool pop();

  /// Remove object
  FXbool remove(FXptr object);

  /// Find object in list, searching forward; return position or -1
  FXint find(FXptr object,FXint pos=0) const;

  /// Find object in list, searching backward; return position or -1
  FXint rfind(FXptr object,FXint pos=2147483647) const;

  /// Remove all objects
  void clear();

  /// Destructor
 ~FXPtrList();
  };



/// List to pointers to TYPE
template<class TYPE>
class FXPtrListOf : public FXPtrList {
public:

  /// Default constructor
  FXPtrListOf(){}

  /// Copy constructor
  FXPtrListOf(const FXPtrListOf<TYPE>& src):FXPtrList(src){ }

  /// Construct and init with single object
  FXPtrListOf(TYPE* object):FXPtrList(reinterpret_cast<FXptr>(object)){ }

  /// Construct and init with n copies of object
  FXPtrListOf(TYPE* object,FXint n):FXPtrList(reinterpret_cast<FXptr>(object),n){ }

  /// Construct and init with list of objects
  FXPtrListOf(TYPE** objects,FXint n):FXPtrList(reinterpret_cast<FXptr*>(objects),n){ }

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

  /// Adopt objects from orig, leaving orig empty
  FXPtrListOf<TYPE>& adopt(FXPtrList& orig){ return reinterpret_cast<FXPtrListOf<TYPE>&>(FXPtrList::adopt(orig)); }
  };

}

#endif
