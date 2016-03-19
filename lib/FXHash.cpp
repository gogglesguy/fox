/********************************************************************************
*                                                                               *
*                       H a s h   T a b l e   C l a s s                         *
*                                                                               *
*********************************************************************************
* Copyright (C) 2003,2010 by Jeroen van der Zijp.   All Rights Reserved.        *
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


/*
  Notes:
  - The table hashes pointers to pointers, or things which fit into a pointer to
    things which fit into a pointer.  This covers a large number of hashing cases.
  - We distinguish between used slots, free slots, and formerly used empty slots.
    The members "used" and "free" keep track of the number of slots in the table which
    are occupied, and the number of slots which are free.  These numbers do not always
    sum to the total number of slots!
  - An occupied slot which is removed will be marked as empty; it can not be counted
    as free, since the slot may be part of the probe-chain of some other slot's probe-
    sequence.
  - Only when the table is resized will the empty slots be reclaimed.
  - The probe sequence will eventually visit each table location, thus guaranteeing that
    items will be found if they're in the table.
  - When items are added, the load factor is kept below half full; any additional items
    will cause the table to be resized to double the previous size.
  - When items are removed, the load factor is kept above one quarter full, and the
    table will be resized to half the original size.
  - The calculations are tweaked such that the minimal table size is exactly 2.
  - Use of __likely() and __unlikely() causes very optimal code generation on the
    compilers; it makes a noticeable difference.
*/

#define BSHIFT 5

using namespace FX;

/*******************************************************************************/

namespace FX {

// Make empty table
FXHash::FXHash(){
  allocElms(table,2);
  table[0].name=NULL;
  table[0].data=NULL;
  table[1].name=NULL;
  table[1].data=NULL;
  total=2;
  used=0;
  free=2;
  }


// Resize hash table, and rehash old stuff into it
FXbool FXHash::size(FXuint m){
  FXEntry *newtable;
  if(__likely(callocElms(newtable,m))){
    register FXuval p,b,x,i;
    register void *name;
    register void *data;
    for(i=0; i<total; ++i){
      name=table[i].name;
      data=table[i].data;
      if(name==NULL || name==(void*)-1L) continue;
      p=b=(FXuval)name;
      while(newtable[x=p&(m-1)].name){
        p=(p<<2)+p+b+1;
        b>>=BSHIFT;
        }
      newtable[x].name=name;
      newtable[x].data=data;
      }
    freeElms(table);
    table=newtable;
    total=m;
    free=m-used;
    return true;
    }
  return false;
  }


// Insert key into the table
void* FXHash::insert(void* name,void* data){
  register FXuval p,b,x;
  if(__likely(name)){
    p=b=(FXuval)name;
    while(table[x=p&(total-1)].name){
      if(table[x].name==name) goto y;            // Return existing
      p=(p<<2)+p+b+1;
      b>>=BSHIFT;
      }
    if(__unlikely((free<<1)<=total)) size(total<<1);
    p=b=(FXuval)name;
    while(table[x=p&(total-1)].name){
      if(table[x].name==(void*)-1L) goto x;      // Put it in empty slot
      p=(p<<2)+p+b+1;
      b>>=BSHIFT;
      }
    free--;
x:  used++;
    table[x].name=name;
    table[x].data=data;
y:  return table[x].data;
    }
  return NULL;
  }


// Replace key in the table, returning old one
void* FXHash::replace(void* name,void* data){
  register FXuval p,b,x;
  register void* old;
  if(__likely(name)){
    p=b=(FXuval)name;
    while(table[x=p&(total-1)].name){
      if(table[x].name==name) goto y;            // Replace existing
      p=(p<<2)+p+b+1;
      b>>=BSHIFT;
      }
    if(__unlikely((free<<1)<=total)) size(total<<1);
    p=b=(FXuval)name;
    while(table[x=p&(total-1)].name){
      if(table[x].name==(void*)-1L) goto x;      // Put it in empty slot
      p=(p<<2)+p+b+1;
      b>>=BSHIFT;
      }
    free--;
x:  used++;
    table[x].name=name;
y:  old=table[x].data;
    table[x].data=data;
    return old;
    }
  return NULL;
  }


// Remove association from the table
void* FXHash::remove(void* name){
  register FXuval p,b,x;
  register void* old;
  if(__likely(name)){
    p=b=(FXuval)name;
    while(table[x=p&(total-1)].name!=name){
      if(table[x].name==NULL) goto x;
      p=(p<<2)+p+b+1;
      b>>=BSHIFT;
      }
    old=table[x].data;
    table[x].name=(void*)-1L;                    // Empty but not free
    table[x].data=NULL;
    used--;
    if(__unlikely(used<(total>>2))) size(total>>1);
    return old;
    }
x:return NULL;
  }


// Return true if association in table
void* FXHash::find(void* name) const {
  register FXuval p,b,x;
  if(__likely(name)){
    p=b=(FXuval)name;
    while(__likely(table[x=p&(total-1)].name)){
      if(__likely(table[x].name==name)) return table[x].data;
      p=(p<<2)+p+b+1;
      b>>=BSHIFT;
      }
   }
  return NULL;
  }


// Clear hash table
void FXHash::clear(){
  resizeElms(table,2);
  table[0].name=NULL;
  table[0].data=NULL;
  table[1].name=NULL;
  table[1].data=NULL;
  total=2;
  used=0;
  free=2;
  }


// Destroy table
FXHash::~FXHash(){
  freeElms(table);
  }

}

