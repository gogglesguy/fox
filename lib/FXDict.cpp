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
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "FXArray.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXElement.h"
#include "FXDict.h"


/*
  Notes:
  - We store the hash key, so that 99.999% of the time we can compare hash numbers;
    only when hash numbers match do we need to compare keys.
    Thus, with a good hash function, the number of calls to strcmp() should be
    roughly the same as the number of successful lookups.
  - Maybe dict[].key should be FXString? Think of the possibilities!
  - FIXME store FXString instead of char* for key.
  - FIXME hash() like FXString.
*/

#define BSHIFT 5

using namespace FX;

/*******************************************************************************/

namespace FX {


// Object implementation
FXIMPLEMENT(FXDict,FXObject,NULL,0)


// Hash function for string
FXuint FXDict::hash(const FXchar* str){
  register FXuint h=0;
  register FXuint c;
  while((c=(FXuchar)*str++)!='\0'){
    h = ((h << 5) + h) ^ c;
    }
  return h&0x7fffffff;
  }


// Initial value for slot
const FXDict::Entry FXDict::init={NULL,NULL,0xFFFFFFFF,false};


// Construct empty dictionary
FXDict::FXDict():table(init,2),used(0),free(2){
  }


// Default implementation
void *FXDict::createData(void* ptr){ return ptr; }


// Default implementation
void FXDict::deleteData(void*){ }               // FIXME maybe should return arg by default


// Resize table, must be power of 2
FXbool FXDict::size(FXival m){
  FXArray<Entry> elbat;
  if(elbat.assign(init,m)){
    register FXuval p,b,x;
    register FXival i;
    for(i=0; i<size(); ++i){
      if(table[i].key){
        p=b=table[i].hash;
        while(elbat[x=p&(m-1)].hash!=0xFFFFFFFF){
          p=(p<<2)+p+b+1;
          b>>=BSHIFT;
          }
        elbat[x]=table[i];
        }
      }
    table.adopt(elbat);
    free=m-used;
    return true;
    }
  return false;
  }


// Insert a new entry, leave it alone if already existing
void* FXDict::insert(const FXchar* ky,void* ptr,FXbool mrk){
  if(__likely(ky)){
    register FXuval p,b,h,x;
    p=b=h=hash(ky);
    while(table[x=p&(size()-1)].hash!=0xFFFFFFFF){
      if(table[x].hash==h && strcmp(table[x].key,ky)==0){ goto y; }
      p=(p<<2)+p+b+1;
      b>>=BSHIFT;
      }
    if(__likely((free<<1)>size()) || __likely(size(size()<<1))){
      p=b=h;
      while(table[x=p&(size()-1)].hash!=0xFFFFFFFF){
        if(table[x].hash==0xFFFFFFFE) goto x;
        p=(p<<2)+p+b+1;
        b>>=BSHIFT;
        }
      free--;
x:    used++;
      table[x].key=strdup(ky);
      table[x].hash=h;
      table[x].mark=mrk;
      table[x].data=createData(ptr);
y:    return table[x].data;
      }
    }
  return NULL;
  }


// Insert entry or replace existing entry
void* FXDict::replace(const FXchar* ky,void* ptr,FXbool mrk){
  if(__likely(ky)){
    register FXuval p,b,h,x;
    p=b=h=hash(ky);
    while(table[x=p&(size()-1)].hash!=0xFFFFFFFF){
      if(table[x].hash==h && strcmp(table[x].key,ky)==0){ deleteData(table[x].data); goto y; }
      p=(p<<2)+p+b+1;
      b>>=BSHIFT;
      }
    if(__likely((free<<1)>size()) || __likely(size(size()<<1))){
      p=b=h;
      while(table[x=p&(size()-1)].hash!=0xFFFFFFFF){
        if(table[x].hash==0xFFFFFFFE) goto x;
        p=(p<<2)+p+b+1;
        b>>=BSHIFT;
        }
      free--;
x:    used++;
      table[x].key=strdup(ky);
      table[x].hash=h;
y:    table[x].mark=mrk;
      table[x].data=createData(ptr);
      return table[x].data;
      }
    }
  return NULL;
  }


// Remove entry
void* FXDict::remove(const FXchar* ky){
  if(__likely(ky)){
    register FXuval p,b,h,x;
    p=b=h=hash(ky);
    while(table[x=p&(size()-1)].hash!=h || strcmp(table[x].key,ky)!=0){
      if(table[x].hash==0xFFFFFFFF) return NULL;
      p=(p<<2)+p+b+1;
      b>>=BSHIFT;
      }
    ::free(table[x].key);
    deleteData(table[x].data);
    table[x].key=NULL;
    table[x].data=NULL;
    table[x].hash=0xFFFFFFFE;
    table[x].mark=false;
    used--;
    if(__unlikely(used<(size()>>2))) size(size()>>1);
    }
  return NULL;          // FIXME maybe should return result of deleteData()
  }


// Find entry
void* FXDict::find(const FXchar* ky) const {
  if(__likely(ky)){
    register FXuval p,b,x,h;
    p=b=h=hash(ky);
    while(__likely(table[x=p&(size()-1)].hash!=0xFFFFFFFF)){
      if(__likely(table[x].hash==h && strcmp(table[x].key,ky)==0)){
        return table[x].data;
        }
      p=(p<<2)+p+b+1;
      b>>=BSHIFT;
      }
    }
  return NULL;
  }


// Get first non-empty entry
FXival FXDict::first() const {
  register FXival pos=0;
  while(pos<size()){ if(table[pos].key) break; pos++; }
  FXASSERT(size()<=pos || table[pos].key);
  return pos;
  }


// Get last non-empty entry
FXival FXDict::last() const {
  register FXival pos=size()-1;
  while(0<=pos){ if(table[pos].key) break; pos--; }
  FXASSERT(pos<0 || table[pos].key);
  return pos;
  }


// Find next entry
FXival FXDict::next(FXival pos) const {
  FXASSERT(0<=pos && pos<size());
  while(++pos <= size()-1){ if(table[pos].key) break; }
  FXASSERT(size()<=pos || table[pos].key);
  return pos;
  }


// Find previous entry
FXival FXDict::prev(FXival pos) const {
  FXASSERT(0<=pos && pos<size());
  while(--pos >= 0){ if(table[pos].key) break; }
  FXASSERT(pos<0 || table[pos].key);
  return pos;
  }


// Remove all
void FXDict::clear(){
  for(FXival x=0; x<size(); ++x){
    if(table[x].key){
      ::free(table[x].key);
      deleteData(table[x].data);
      }
    table[x].key=NULL;
    table[x].data=NULL;
    table[x].hash=-1;
    table[x].mark=false;
    }
  used=0;
  free=size();
  }


// Destroy table
FXDict::~FXDict(){
  clear();
  }

}
