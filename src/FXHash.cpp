/********************************************************************************
*                                                                               *
*                       H a s h   T a b l e   C l a s s                         *
*                                                                               *
*********************************************************************************
* Copyright (C) 2003,2007 by Jeroen van der Zijp.   All Rights Reserved.        *
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
*********************************************************************************
* $Id: FXHash.cpp,v 1.31 2007/07/09 16:26:56 fox Exp $                          *
********************************************************************************/
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXElement.h"


/*
  Notes:
  - The members used and free keep track of the number of slots
    in the table which are used and which are free.
  - When an item is inserted, used is incremented if the item isn't in the table
    yet, and free is decremented if a free slot is used; if an empty slot is
    used, free stays the same.  If the table exceeds the load factor, its
    size is doubled.
  - When an item is removed, used is decremented but free stays the same
    because the slot remains marked as empty instead of free; when the
    number of used items drops below some minimum, the table's size is
    halved.
  - If the table is resized, the empty slots all become free slots since
    the empty holes are not copied into the table; only used items will
    be rehashed into the new table.
*/

#define HASH1(x,m) (((FXuint)((FXuval)(x)^(((FXuval)(x))>>13)))&((m)-1))
#define HASH2(x,m) (((FXuint)((FXuval)(x)^(((FXuval)(x))>>17)|1))&((m)-1))


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
void FXHash::size(FXuint m){
  register void *name,*data;
  register FXuint q,x,i;
  FXEntry *newtable;
  callocElms(newtable,m);
  for(i=0; i<total; i++){
    name=table[i].name;
    data=table[i].data;
    if(name==NULL || name==(void*)-1L) continue;
    q=HASH1(name,m);
    x=HASH2(name,m);
    while(newtable[q].name) q=(q+x)&(m-1);
    newtable[q].name=name;
    newtable[q].data=data;
    }
  freeElms(table);
  table=newtable;
  total=m;
  free=m-used;
  }


// Insert key into the table
void* FXHash::insert(void* name,void* data){
  register FXuint p,q,x;
  if(name){
    if((free<<1)<=total) size(total<<1);
    p=HASH1(name,total);
    x=HASH2(name,total);
    q=p;
    while(table[q].name){
      if(table[q].name==name) goto y;              // Return existing
      q=(q+x)&(total-1);
      }
    q=p;
    while(table[q].name){
      if(table[q].name==(void*)-1L) goto x;      // Put it in empty slot
      q=(q+x)&(total-1);
      }
    free--;
x:  used++;
    table[q].name=name;
    table[q].data=data;
y:  return table[q].data;
    }
  return NULL;
  }


// Replace key in the table, returning old one
void* FXHash::replace(void* name,void* data){
  register FXuint p,q,x;
  register void* old;
  if(name){
    if((free<<1)<=total) size(total<<1);
    p=HASH1(name,total);
    x=HASH2(name,total);
    q=p;
    while(table[q].name){
      if(table[q].name==name) goto y;            // Replace existing
      q=(q+x)&(total-1);
      }
    q=p;
    while(table[q].name){
      if(table[q].name==(void*)-1L) goto x;      // Put it in empty slot
      q=(q+x)&(total-1);
      }
    free--;
x:  used++;
    table[q].name=name;
y:  old=table[q].data;
    table[q].data=data;
    return old;
    }
  return NULL;
  }


// Remove association from the table
void* FXHash::remove(void* name){
  register FXuint q,x;
  register void* data;
  if(name){
    q=HASH1(name,total);
    x=HASH2(name,total);
    while(table[q].name!=name){
      if(table[q].name==NULL) goto x;
      q=(q+x)&(total-1);
      }
    data=table[q].data;
    table[q].name=(void*)-1L;                    // Empty but not free
    table[q].data=NULL;
    used--;
    if(used<(total>>2)) size(total>>1);
    return data;
    }
x:return NULL;
  }


// Return true if association in table
void* FXHash::find(void* name) const {
  register FXuint q,x;
  if(name){
    q=HASH1(name,total);
    x=HASH2(name,total);
    while(table[q].name!=name){
      if(table[q].name==NULL) goto x;
      q=(q+x)&(total-1);
      }
    return table[q].data;
    }
x:return NULL;
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
  table=(FXEntry*)-1L;
  }

}

