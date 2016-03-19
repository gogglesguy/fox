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
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "FXElement.h"
#include "FXException.h"
#include "FXString.h"
#include "FXDictionary.h"
#include "FXStringMap.h"

/*
  Notes:
  - FXStringMap is a hash table from FXString to FXString. 
*/

using namespace FX;

/*******************************************************************************/

namespace FX {


// Empty string object
const FXString FXStringMap::null;



// Construct empty dictionary
FXStringMap::FXStringMap(){
  }


// Construct empty dictionary
FXStringMap::FXStringMap(const FXStringMap& other):FXDictionary(other){
  for(FXint i=0; i<no(); ++i){ 
    if(!key(i).empty()) construct(&data(i),other.data(i)); 
    }
  }


// Assignment operator
FXStringMap& FXStringMap::operator=(const FXStringMap& other){
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
FXStringMap& FXStringMap::adopt(FXStringMap& other){
  if(__likely(table!=other.table)){ 
    swap(table,other.table); 
    other.clear();
    }
  return *this;
  }


// Return reference to string assocated with key
FXString& FXStringMap::at(const FXchar* ky){
  FXptr& contents=FXDictionary::at(ky);
  if(!contents){ construct((FXString*)&contents); }
  return reinterpret_cast<FXString&>(contents);
  }



// Return constant reference to string assocated with key
const FXString& FXStringMap::at(const FXchar* ky) const {
  const FXptr& contents=FXDictionary::at(ky);
  return contents ? reinterpret_cast<const FXString&>(contents) : FXStringMap::null;
  }


// Insert string associated with given key
void FXStringMap::insert(const FXchar* ky,const FXchar* value){
  FXptr contents;
  if((contents=FXDictionary::insert(ky,construct((FXString*)&contents,value)))!=NULL){
    destruct(reinterpret_cast<FXString*>(&contents));
    }
  }


// Remove string associated with given key
void FXStringMap::remove(const FXchar* ky){
  FXptr contents;
  if((contents=FXDictionary::remove(ky))!=NULL){
    destruct(reinterpret_cast<FXString*>(&contents));
    }
  }


// Erase string at pos in the table
void FXStringMap::erase(FXival pos){
  FXptr contents;
  if((contents=FXDictionary::erase(pos))!=NULL){
    destruct(reinterpret_cast<FXString*>(&contents));
    }
  }


/// Clear entire table
void FXStringMap::clear(){
  for(FXint i=0; i<no(); ++i){
    if(!key(i).empty()){ destruct(&data(i)); }
    }
  FXDictionary::no(1);
  }


// Destroy table
FXStringMap::~FXStringMap(){
  clear();
  }

}
