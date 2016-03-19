/********************************************************************************
*                                                                               *
*                            O b j e c t   L i s t                              *
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
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "FXArray.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXObject.h"
#include "FXStream.h"
#include "FXElement.h"
#include "FXObjectList.h"

/*
  Notes:
  - A list of pointers to objects.
  - The list may be serialized; this means contents of all objects referenced
    from the list may be saved or loaded through the serialization mechanism.
*/


// Size rounded up to nearest multiple of ROUNDVAL
#define ROUNDVAL    16

// Round up to nearest ROUNDVAL
#define ROUNDUP(n)  (((n)+ROUNDVAL-1)&-ROUNDVAL)

// Empty list
#define EMPTY       ((FXObject**)(emptylist+1))

using namespace FX;

/*******************************************************************************/

namespace FX {


// Empty object list
static const FXObject* emptylist[2]={0,0};


// Change number of items in list
FXbool FXObjectList::no(FXint num){
  register FXint old=no();
  register FXObject **p;
  if(__likely(old!=num)){
    if(0<num){
      if(ptr==EMPTY){
        if((p=(FXObject**)malloc(ROUNDUP(num)*sizeof(FXObject*)+sizeof(FXObject*)))==NULL) return false;
        }
      else{
        if((p=(FXObject**)realloc(ptr-1,ROUNDUP(num)*sizeof(FXObject*)+sizeof(FXObject*)))==NULL) return false;
        }
      ptr=p+1;
      *((FXint*)(ptr-1))=num;
      }
    else if(ptr!=EMPTY){
      free(ptr-1);
      ptr=EMPTY;
      }
    }
  return true;
  }


// Default constructor
FXObjectList::FXObjectList():ptr(EMPTY){
  }


// Copy constructor
FXObjectList::FXObjectList(const FXObjectList& src):ptr(EMPTY){
  register FXint num=src.no();
  if(__likely(0<num && no(num))){
    copyElms(ptr,src.ptr,num);
    }
  }


// Construct and init with single object
FXObjectList::FXObjectList(FXObject* object):ptr(EMPTY){
  if(__likely(no(1))){
    ptr[0]=object;
    }
  }


// Construct and init with n copies of object
FXObjectList::FXObjectList(FXObject* object,FXint n):ptr(EMPTY){
  if(__likely(0<n && no(n))){
    fillElms(ptr,object,n);
    }
  }


// Construct and init with list of objects
FXObjectList::FXObjectList(FXObject** objects,FXint n):ptr(EMPTY){
  if(__likely(0<n && no(n))){
    copyElms(ptr,objects,n);
    }
  }


// Assignment operator
FXObjectList& FXObjectList::operator=(const FXObjectList& orig){
  if(__likely(ptr!=orig.ptr && no(orig.no()))){
    copyElms(ptr,(FXObject**)orig.ptr,orig.no());
    }
  return *this;
  }


// Adopt objects from orig, leaving orig empty
void FXObjectList::adopt(FXObjectList& orig){
  if(__likely(ptr!=orig.ptr)){
    if(ptr!=EMPTY){ free(ptr-1); }
    ptr=orig.ptr;
    orig.ptr=EMPTY;
    }
  }


// Assign object p to list
FXbool FXObjectList::assign(FXObject* object){
  if(__likely(no(1))){
    ptr[0]=object;
    return true;
    }
  return false;
  }


// Assign n copies of object to list
FXbool FXObjectList::assign(FXObject* object,FXint n){
  if(__likely(no(n))){
    fillElms(ptr,object,n);
    return true;
    }
  return false;
  }


// Assign n objects to list
FXbool FXObjectList::assign(FXObject** objects,FXint n){
  if(__likely(no(n))){
    moveElms(ptr,objects,n);
    return true;
    }
  return false;
  }


// Assign input string to this string
FXbool FXObjectList::assign(const FXObjectList& objects){
  return assign((FXObject**)objects.ptr,objects.no());
  }


// Insert an object
FXbool FXObjectList::insert(FXint pos,FXObject* object){
  register FXint num=no();
  if(__likely(no(num+1))){
    moveElms(ptr+pos+1,ptr+pos,num-pos);
    ptr[pos]=object;
    return true;
    }
  return false;
  }


// Insert n copies of object at specified position
FXbool FXObjectList::insert(FXint pos,FXObject* object,FXint n){
  register FXint num=no();
  if(__likely(no(num+n))){
    moveElms(ptr+pos+n,ptr+pos,num-pos);
    fillElms(ptr+pos,object,n);
    return true;
    }
  return false;
  }


// Insert n objects at specified position
FXbool FXObjectList::insert(FXint pos,FXObject** objects,FXint n){
  register FXint num=no();
  if(__likely(no(num+n))){
    moveElms(ptr+pos+n,ptr+pos,num-pos);
    copyElms(ptr+pos,objects,n);
    return true;
    }
  return false;
  }


// Insert objects at specified position
FXbool FXObjectList::insert(FXint pos,const FXObjectList& objects){
  return insert(pos,(FXObject**)objects.ptr,objects.no());
  }


// Prepend an object
FXbool FXObjectList::prepend(FXObject* object){
  register FXint num=no();
  if(__likely(no(num+1))){
    moveElms(ptr+1,ptr,num);
    ptr[0]=object;
    return true;
    }
  return false;
  }


// Prepend n copies of object
FXbool FXObjectList::prepend(FXObject* object,FXint n){
  register FXint num=no();
  if(__likely(no(num+n))){
    moveElms(ptr+n,ptr,num);
    fillElms(ptr,object,n);
    return true;
    }
  return false;
  }


// Prepend n objects
FXbool FXObjectList::prepend(FXObject** objects,FXint n){
  register FXint num=no();
  if(__likely(no(num+n))){
    moveElms(ptr+n,ptr,num);
    copyElms(ptr,objects,n);
    return true;
    }
  return false;
  }


// Prepend objects
FXbool FXObjectList::prepend(const FXObjectList& objects){
  return prepend((FXObject**)objects.ptr,objects.no());
  }


// Append an object
FXbool FXObjectList::append(FXObject* object){
  register FXint num=no();
  if(__likely(no(num+1))){
    ptr[num]=object;
    return true;
    }
  return false;
  }


// Append n copies of object
FXbool FXObjectList::append(FXObject* object,FXint n){
  register FXint num=no();
  if(__likely(no(num+n))){
    fillElms(ptr+num,object,n);
    return true;
    }
  return false;
  }


// Add string to the end
FXbool FXObjectList::append(FXObject** objects,FXint n){
  register FXint num=no();
  if(__likely(no(num+n))){
    copyElms(ptr+num,objects,n);
    return true;
    }
  return false;
  }


// Add string to the end
FXbool FXObjectList::append(const FXObjectList& objects){
  return append((FXObject**)objects.ptr,objects.no());
  }


// Replace element
FXbool FXObjectList::replace(FXint pos,FXObject* object){
  ptr[pos]=object;
  return true;
  }


// Replaces the m objects at pos with n copies of object
FXbool FXObjectList::replace(FXint pos,FXint m,FXObject* object,FXint n){
  register FXint num=no();
  if(__unlikely(m<n)){
    if(__unlikely(!no(num-m+n))) return false;
    moveElms(ptr+pos+n,ptr+pos+m,num-pos-n);
    }
  else if(__unlikely(m>n)){
    moveElms(ptr+pos+n,ptr+pos+m,num-pos-m);
    if(__unlikely(!no(num-m+n))) return false;
    }
  fillElms(ptr+pos,object,n);
  return true;
  }


// Replaces the m objects at pos with n objects
FXbool FXObjectList::replace(FXint pos,FXint m,FXObject** objects,FXint n){
  register FXint num=no();
  if(__unlikely(m<n)){
    if(__unlikely(!no(num-m+n))) return false;
    moveElms(ptr+pos+n,ptr+pos+m,num-pos-n);
    }
  else if(__unlikely(m>n)){
    moveElms(ptr+pos+n,ptr+pos+m,num-pos-m);
    if(__unlikely(!no(num-m+n))) return false;
    }
  copyElms(ptr+pos,objects,n);
  return true;
  }


// Replace the m objects at pos with objects
FXbool FXObjectList::replace(FXint pos,FXint m,const FXObjectList& objects){
  return replace(pos,m,(FXObject**)objects.ptr,objects.no());
  }


// Remove object at pos
FXbool FXObjectList::erase(FXint pos){
  register FXint num=no();
  moveElms(ptr+pos,ptr+pos+1,num-pos-1);
  return no(num-1);
  }


// Remove n objects at pos
FXbool FXObjectList::erase(FXint pos,FXint n){
  register FXint num=no();
  moveElms(ptr+pos,ptr+pos+n,num-n-pos);
  return no(num-n);
  }


// Push object to end
FXbool FXObjectList::push(FXObject* object){
  register FXint num=no();
  if(__likely(no(num+1))){
    ptr[num]=object;
    return true;
    }
  return false;
  }


// Pop object from end
FXbool FXObjectList::pop(){
  return no(no()-1);
  }


// Remove element p
FXbool FXObjectList::remove(const FXObject* object){
  register FXint pos;
  if(0<=(pos=find(object))){
    return erase(pos);
    }
  return false;
  }


// Find object in list, searching forward; return position or -1
FXint FXObjectList::find(const FXObject* object,FXint pos) const {
  register FXint p=pos;
  while(p<no()){
    if(ptr[p]==object){ return p; }
    ++p;
    }
  return -1;
  }


// Find object in list, searching backward; return position or -1
FXint FXObjectList::rfind(const FXObject* object,FXint pos) const {
  register FXint p=pos;
  while(0<=p){
    if(ptr[p]==object){ return p; }
    --p;
    }
  return -1;
  }


// Clear the list
void FXObjectList::clear(){
  if(__likely(ptr!=EMPTY)){
    free(ptr-1);
    ptr=EMPTY;
    }
  }


// Save to stream; children may be NULL
void FXObjectList::save(FXStream& store) const {
  FXint num,i;
  num=no();
  store << num;
  for(i=0; i<num; i++){
    store << ptr[i];
    }
  }


// Load from stream; children may be NULL
void FXObjectList::load(FXStream& store){
  FXint num,i;
  store >> num;
  if(!no(num)) return;
  for(i=0; i<num; i++){
    store >> ptr[i];
    }
  }


// Free up nicely
FXObjectList::~FXObjectList(){
  if(__likely(ptr!=EMPTY)){
    free(ptr-1);
    }
  }

}
