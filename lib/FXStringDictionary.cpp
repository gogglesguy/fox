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
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "FXElement.h"
#include "FXException.h"
#include "FXString.h"
#include "FXDictionary.h"
#include "FXStringDictionary.h"

/*
  Notes:
  - FXStringDictionary is a hash table from FXString to FXString.
*/

using namespace FX;

/*******************************************************************************/

namespace FX {


// Construct empty dictionary
FXStringDictionary::FXStringDictionary(){
  }


// Construct empty dictionary
FXStringDictionary::FXStringDictionary(const FXStringDictionary& other):FXDictionary(other){
  for(FXint i=0; i<no(); ++i){
    if(!key(i).empty()) construct(&data(i),other.data(i));
    }
  }


// Assignment operator
FXStringDictionary& FXStringDictionary::operator=(const FXStringDictionary& other){
  if(__likely(table!=other.table)){
    clear();
    FXDictionary::operator=(other);
    for(FXint i=0; i<no(); ++i){
      if(!key(i).empty()) construct(&data(i),other.data(i));
      }
    }
  return *this;
  }


// Adopt dictionary from another
FXStringDictionary& FXStringDictionary::adopt(FXStringDictionary& other){
  if(__likely(table!=other.table)){
    swap(table,other.table);
    other.clear();
    }
  return *this;
  }


// Dirty tricks
extern const FXint __string__empty__[];


// Return reference to string assocated with key
FXString& FXStringDictionary::at(const FXchar* ky){
  FXptr& contents(FXDictionary::at(ky));
  if(!contents){ contents=(FXptr)(__string__empty__+1); }
  return reinterpret_cast<FXString&>(contents);
  }



// Return constant reference to string assocated with key
const FXString& FXStringDictionary::at(const FXchar* ky) const {
  const FXptr& contents(FXDictionary::at(ky));
  if(!contents){ return *((const FXString*)(const FXptr)(__string__empty__+1)); }
  return reinterpret_cast<const FXString&>(contents);
  }


// Insert string associated with given key
void FXStringDictionary::insert(const FXchar* ky,const FXchar* value){
  FXptr contents;
  if((contents=FXDictionary::insert(ky,construct(reinterpret_cast<FXString*>(&contents),value)))!=NULL){
    destruct(reinterpret_cast<FXString*>(&contents));
    }
  }


// Remove string associated with given key
void FXStringDictionary::remove(const FXchar* ky){
  FXptr contents;
  if((contents=FXDictionary::remove(ky))!=NULL){
    destruct(reinterpret_cast<FXString*>(&contents));
    }
  }


// Erase string at pos in the table
void FXStringDictionary::erase(FXival pos){
  FXptr contents;
  if((contents=FXDictionary::erase(pos))!=NULL){
    destruct(reinterpret_cast<FXString*>(&contents));
    }
  }


/// Clear entire table
void FXStringDictionary::clear(){
  for(FXint i=0; i<no(); ++i){
    if(!key(i).empty()){ destruct(&data(i)); }
    }
  FXDictionary::no(1);
  }


// Destroy table
FXStringDictionary::~FXStringDictionary(){
  clear();
  }

}
