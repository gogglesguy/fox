/********************************************************************************
*                                                                               *
*                       H a s h   T a b l e   C l a s s                         *
*                                                                               *
*********************************************************************************
* Copyright (C) 2003,2013 by Jeroen van der Zijp.   All Rights Reserved.        *
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


/*
  Notes:
  - The table hashes pointers to pointers, or things which fit into a pointer to
    things which fit into a pointers.

  - The hash table does not know anything about the keys, or the values; thus, subclasses
    are responsible for managing the memory of the contents.

  - Initially the table is initialized to the empty-table pointer.  This will have only
    a single free slot; this means the table pointer is NEVER null.

  - The hash-table object requires only one pointer's worth of memory.  Thus an empty
    hash table requires very little space, and no table is allocated until at least one
    element is added.

  - Probe position p, probe increment b, and index x MUST be unsigned.

  - The table resize algorithm:

      - Table maintains count of used slots, and count of free slots; when an entry
        is removed, used slot count will be decremented, but free slot count will
        stay the same, because the slot is voided, not freed.  This is because the
        removed slot may still be part of another entry's probe sequence.
        When an entry is added, it will be placed in a voided slot if possible;
        if not, then it will be placed in a free slot.

      - When the table is resized, all entries wil be re-hashed, and voided slots be
        reclaimed as free slots.

      - Table is grown PRIOR to adding entries, and shrunk AFTER removing entries.

      - Table is grown when the number of remaining free slots is less than or
        equal to 1/4 of the table size (plus 1, because the resize occurs PRIOR to
        adding the entry).

      - Table is shrunk when the number of used slots is less than or equal to 1/4
        of the table size.

      - Table must always maintain at least one free slot to insure that probing
        sequence is always finite (all locations are eventually visited, and thus
        the probing sequence is guaranteed to be finite).

      - Thus the new implementation will be at least 1/4 used, and at most 3/4 filled
        (used and voided slots do not exceed 3/4 of table size).  The "hysteresis"
        between growing and shrinking means that adding, then removing a single item
        will not cause a table resize (except when the table is near empty, of course).

*/

#define EMPTY     ((Entry*)(__hash__empty__+3))
#define NOMEMORY  ((Entry*)(((FXival*)NULL)+3))
#define HASH(x)   ((FXival)(x)^(((FXival)(x))>>13))
#define VOID      ((FXptr)-1L)
#define LEGAL(p)  ((p)!=NULL && (p)!=VOID)
#define BSHIFT    5
#define SIZE      -1
#define USED      -2
#define FREE      -3


using namespace FX;

/*******************************************************************************/

namespace FX {


// Empty object list
extern const FXival __hash__empty__[];
const FXival __hash__empty__[7]={1,0,1,0,0};


// Make empty table
FXHash::FXHash():table(EMPTY){
  }


// Clear hash table, marking all slots as free
void FXHash::clear(){
  if(table!=EMPTY){
    free(((FXival*)table)-3);
    table=EMPTY;
    }
  }


// Resize the table to the given size; the size must be a power of two
FXbool FXHash::size(FXival num){
  FXASSERT((num&(num-1))==0);
  FXASSERT((num-no())>0);
  if(size()!=num){
    register Entry* elbat=EMPTY;
    register FXptr name,data;
    register FXuval p,b,x;
    register FXival i;
    if(1<num){
      if(__unlikely((elbat=(Entry*)(((FXival*)calloc(sizeof(FXival)*3+sizeof(Entry)*num,1))+3))==NOMEMORY)) return false;
      for(i=0; i<size(); ++i){                  // Hash existing entries into new table
        name=table[i].name;
        data=table[i].data;
        if(!LEGAL(name)) continue;              // Skip empty or voided slots
        p=b=HASH(name);
        while(elbat[x=p&(num-1)].name){         // Locate slot
          p=(p<<2)+p+b+1;
          b>>=BSHIFT;
          }
        elbat[x].name=name;
        elbat[x].data=data;
        }
      ((FXival*)elbat)[FREE]=num-no();          // All non-empty slots now free
      ((FXival*)elbat)[USED]=no();              // Used slots not changed
      ((FXival*)elbat)[SIZE]=num;               // Total number of slots in table
      }
    if(table!=EMPTY){
      free(((FXival*)table)-3);
      }
    table=elbat;
    }
  return true;
  }


// Insert entry into the table, unless it already exists
FXptr FXHash::insert(FXptr name,FXptr data){
  if(__likely(LEGAL(name))){
    register FXuval p,b,h,x;
    p=b=h=HASH(name);
    while(table[x=p&(size()-1)].name){
      if(table[x].name==name) goto y;            // Return existing
      p=(p<<2)+p+b+1;
      b>>=BSHIFT;
      }
    if(__likely(((FXival*)table)[FREE]>1+(size()>>2)) || __likely(size(size()<<1))){
      p=b=h;
      while(table[x=p&(size()-1)].name){
        if(table[x].name==VOID) goto x;         // Put into voided slot
        p=(p<<2)+p+b+1;
        b>>=BSHIFT;
        }
      ((FXival*)table)[FREE]--;                 // Put into empty slot
x:    ((FXival*)table)[USED]++;
      table[x].name=name;
      table[x].data=data;
y:    return table[x].data;
      }
    }
  return NULL;
  }


// Replace entry in the table, returning old one
FXptr FXHash::replace(FXptr name,FXptr data){
  if(__likely(LEGAL(name))){
    register FXuval p,b,h,x;
    register FXptr old;
    p=b=h=HASH(name);
    while(table[x=p&(size()-1)].name){
      if(table[x].name==name) goto y;            // Replace existing
      p=(p<<2)+p+b+1;
      b>>=BSHIFT;
      }
    if(__likely(((FXival*)table)[FREE]>1+(size()>>2)) || __likely(size(size()<<1))){
      p=b=h;
      while(table[x=p&(size()-1)].name){
        if(table[x].name==VOID) goto x;         // Put into voided slot
        p=(p<<2)+p+b+1;
        b>>=BSHIFT;
        }
      ((FXival*)table)[FREE]--;                 // Put into empty slot
x:    ((FXival*)table)[USED]++;
      table[x].name=name;
y:    old=table[x].data;
      table[x].data=data;
      return old;
      }
    }
  return NULL;
  }


// Remove association from the table
FXptr FXHash::remove(FXptr name){
  if(__likely(LEGAL(name))){
    register FXuval p,b,x;
    register FXptr old;
    p=b=HASH(name);
    while(table[x=p&(size()-1)].name!=name){
      if(table[x].name==NULL) goto x;
      p=(p<<2)+p+b+1;
      b>>=BSHIFT;
      }
    old=table[x].data;
    table[x].name=VOID;                         // Void the slot (not empty!)
    table[x].data=NULL;
    ((FXival*)table)[USED]--;
    if(__unlikely(((FXival*)table)[USED]<=(size()>>2))) size(size()>>1);
    return old;
    }
x:return NULL;
  }


// Return true if association in table
FXptr FXHash::find(FXptr name) const {
  if(__likely(LEGAL(name))){
    register FXuval p,b,x;
    p=b=HASH(name);
    while(__likely(table[x=p&(size()-1)].name)){
      if(__likely(table[x].name==name)) return table[x].data;
      p=(p<<2)+p+b+1;
      b>>=BSHIFT;
      }
    }
  return NULL;
  }


// Destroy table
FXHash::~FXHash(){
  clear();
  }

}
