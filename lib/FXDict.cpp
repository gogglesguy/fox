/********************************************************************************
*                                                                               *
*                          D i c t i o n a r y    C l a s s                     *
*                                                                               *
*********************************************************************************
* Copyright (C) 1998,2011 by Jeroen van der Zijp.   All Rights Reserved.        *
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
*/

#define BSHIFT 5

using namespace FX;

/*******************************************************************************/

namespace FX {


// Hash function for string
FXint FXDict::hash(const FXchar* str){
  register const FXuchar *s=(const FXuchar*)str;
  register FXint h=0;
  register FXint c;
  while((c=*s++)!='\0'){
    h = ((h << 5) + h) ^ c;
    }
  return h&0x7fffffff;
  }


// Object implementation
FXIMPLEMENT(FXDict,FXObject,NULL,0)


// Construct empty dictionary
FXDict::FXDict(){
  allocElms(dict,2);
  dict[0].key=NULL;
  dict[0].data=NULL;
  dict[0].hash=-1;
  dict[0].mark=false;
  dict[1].key=NULL;
  dict[1].data=NULL;
  dict[1].hash=-1;
  dict[1].mark=false;
  used=0;
  free=2;
  total=2;
  }


// Copy constructor
FXDict::FXDict(const FXDict& orig):FXObject(orig){
  allocElms(dict,orig.total);
  for(FXint x=0; x<orig.total; ++x){
    if(0<=orig.dict[x].hash){
      dict[x].key=::strdup(orig.dict[x].key);
      dict[x].data=createData(orig.dict[x].data);
      dict[x].hash=orig.dict[x].hash;
      dict[x].mark=orig.dict[x].mark;
      continue;
      }
    dict[x].key=NULL;
    dict[x].data=NULL;
    dict[x].hash=-1;
    dict[x].mark=false;
    }
  used=orig.used;
  free=orig.free;
  total=orig.total;
  }


// Assignment operator
FXDict& FXDict::operator=(const FXDict& orig){
  if(dict!=orig.dict){
    clear();
    resizeElms(dict,orig.total);
    for(FXint x=0; x<orig.total; ++x){
      if(0<=orig.dict[x].hash){
        dict[x].key=::strdup(orig.dict[x].key);
        dict[x].data=createData(orig.dict[x].data);
        dict[x].hash=orig.dict[x].hash;
        dict[x].mark=orig.dict[x].mark;
        continue;
        }
      dict[x].key=NULL;
      dict[x].data=NULL;
      dict[x].hash=-1;
      dict[x].mark=false;
      }
    used=orig.used;
    free=orig.free;
    total=orig.total;
    }
  return *this;
  }


// Default implementation
void *FXDict::createData(void* ptr){ return ptr; }


// Default implementation
void FXDict::deleteData(void*){ }


// Resize table, must be power of 2
FXbool FXDict::size(FXint m){
  FXEntry* ndict;
  if(allocElms(ndict,m)){
    register FXint p,b,x,i;
    for(i=0; i<m; ++i){
      ndict[i].key=NULL;
      ndict[i].data=NULL;
      ndict[i].hash=-1;
      ndict[i].mark=false;
      }
    for(i=0; i<total; ++i){
      p=b=dict[i].hash;
      if(0<=p){
        while(ndict[x=p&(m-1)].hash!=-1){
          p=(p<<2)+p+b+1;
          b>>=BSHIFT;
          }
        ndict[x].key=dict[i].key;
        ndict[x].data=dict[i].data;
        ndict[x].hash=dict[i].hash;
        ndict[x].mark=dict[i].mark;
        }
      }
    freeElms(dict);
    dict=ndict;
    free=m-used;
    total=m;
    return true;
    }
  return false;
  }


// Insert a new entry, leave it alone if already existing
void* FXDict::insert(const FXchar* ky,void* ptr,FXbool mrk){
  if(__likely(ky)){
    if((free<<1)>total || size(total<<1)){
      register FXint p,b,h,x;
      p=b=h=hash(ky);
      while(dict[x=p&(total-1)].hash!=-1){
        if(dict[x].hash==h && strcmp(dict[x].key,ky)==0){ goto y; }
        p=(p<<2)+p+b+1;
        b>>=BSHIFT;
        }
      p=b=h;
      while(dict[x=p&(total-1)].hash!=-1){
        if(dict[x].hash==-2) goto x;
        p=(p<<2)+p+b+1;
        b>>=BSHIFT;
        }
      free--;
x:    used++;
      dict[x].key=strdup(ky);
      dict[x].hash=h;
      dict[x].mark=mrk;
      dict[x].data=createData(ptr);
y:    return dict[x].data;
      }
    }
  return NULL;
  }


// Add or replace entry
void* FXDict::replace(const FXchar* ky,void* ptr,FXbool mrk){
  if(__likely(ky)){
    if((free<<1)>total || size(total<<1)){
      register FXint p,b,h,x;
      p=b=h=hash(ky);
      while(dict[x=p&(total-1)].hash!=-1){
        if(dict[x].hash==h && strcmp(dict[x].key,ky)==0){ deleteData(dict[x].data); goto y; }
        p=(p<<2)+p+b+1;
        b>>=BSHIFT;
        }
      p=b=h;
      while(dict[x=p&(total-1)].hash!=-1){
        if(dict[x].hash==-2) goto x;
        p=(p<<2)+p+b+1;
        b>>=BSHIFT;
        }
      free--;
x:    used++;
      dict[x].key=strdup(ky);
      dict[x].hash=h;
y:    dict[x].mark=mrk;
      dict[x].data=createData(ptr);
      return dict[x].data;
      }
    }
  return NULL;
  }


// Remove entry
void* FXDict::remove(const FXchar* ky){
  if(__likely(ky)){
    register FXint p,b,h,x;
    p=b=h=hash(ky);
    while(dict[x=p&(total-1)].hash!=h || strcmp(dict[x].key,ky)!=0){
      if(dict[x].hash==-1) return NULL;
      p=(p<<2)+p+b+1;
      b>>=BSHIFT;
      }
    ::free(dict[x].key);
    deleteData(dict[x].data);
    dict[x].key=NULL;
    dict[x].data=NULL;
    dict[x].hash=-2;
    dict[x].mark=false;
    used--;
    if(used<(total>>2)) size(total>>1);
    }
  return NULL;
  }


// Find entry
void* FXDict::find(const FXchar* ky) const {
  if(__likely(ky)){
    register FXint p,b,x,h;
    p=b=h=hash(ky);
    while(__likely(dict[x=p&(total-1)].hash!=-1)){
      if(__likely(dict[x].hash==h && strcmp(dict[x].key,ky)==0)){
        return dict[x].data;
        }
      p=(p<<2)+p+b+1;
      b>>=BSHIFT;
      }
    }
  return NULL;
  }


// Get first non-empty entry
FXint FXDict::first() const {
  register FXint pos=0;
  while(pos<total){ if(0<=dict[pos].hash) break; pos++; }
  FXASSERT(total<=pos || 0<=dict[pos].hash);
  return pos;
  }


// Get last non-empty entry
FXint FXDict::last() const {
  register FXint pos=total-1;
  while(0<=pos){ if(0<=dict[pos].hash) break; pos--; }
  FXASSERT(pos<0 || 0<=dict[pos].hash);
  return pos;
  }


// Find next entry
FXint FXDict::next(FXint pos) const {
  FXASSERT(0<=pos && pos<total);
  while(++pos <= total-1){ if(0<=dict[pos].hash) break; }
  FXASSERT(total<=pos || 0<=dict[pos].hash);
  return pos;
  }


// Find previous entry
FXint FXDict::prev(FXint pos) const {
  FXASSERT(0<=pos && pos<total);
  while(--pos >= 0){ if(0<=dict[pos].hash) break; }
  FXASSERT(pos<0 || 0<=dict[pos].hash);
  return pos;
  }


// Remove all
void FXDict::clear(){
  for(FXint x=0; x<total; ++x){
    if(0<=dict[x].hash){
      ::free(dict[x].key);
      deleteData(dict[x].data);
      }
    dict[x].key=NULL;
    dict[x].data=NULL;
    dict[x].hash=-1;
    dict[x].mark=false;
    }
  used=0;
  free=total;
  }


// Destroy table
FXDict::~FXDict(){
  clear();
  freeElms(dict);
  dict=(FXEntry*)-1L;
  }

}

