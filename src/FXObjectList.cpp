/********************************************************************************
*                                                                               *
*                            O b j e c t   L i s t                              *
*                                                                               *
*********************************************************************************
* Copyright (C) 1997,2008 by Jeroen van der Zijp.   All Rights Reserved.        *
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
* $Id: FXObjectList.cpp,v 1.46 2008/01/04 15:42:26 fox Exp $                    *
********************************************************************************/
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXObject.h"
#include "FXStream.h"
#include "FXElement.h"
#include "FXObjectList.h"

/*
  Notes:
  - FXObjectList now stores only the number of items in the list, and
    stores it in front of the list.  The benefit is that an empty list
    is now only as big as a pointer; also, initialization is faster.
  - We need to be a little bit careful accessing the number field,
    it is located in front of the array; since a pointer may be
    larger than an int, there may be some unused space in between the
    first data value and the number field.
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
  if(old!=num){
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
  if(0<num && no(num)){
    copyElms(ptr,src.ptr,num);
    }
  }


// Construct and init with single object
FXObjectList::FXObjectList(FXObject* object):ptr(EMPTY){
  if(no(1)){
    ptr[0]=object;
    }
  }


// Construct and init with n copies of object
FXObjectList::FXObjectList(FXObject* object,FXint n):ptr(EMPTY){
  if(0<n && no(n)){
    fillElms(ptr,object,n);
    }
  }


// Construct and init with list of objects
FXObjectList::FXObjectList(FXObject** objects,FXint n):ptr(EMPTY){
  if(0<n && no(n)){
    copyElms(ptr,objects,n);
    }
  }


// Assignment operator
FXObjectList& FXObjectList::operator=(const FXObjectList& orig){
  if(ptr!=orig.ptr){
    register FXint num=orig.no();
    if(0<num){
      no(num);
      memcpy(ptr,orig.ptr,num*sizeof(FXObject*));
      }
    else{
      no(0);
      }
    }
  return *this;
  }


// Adopt objects from orig, leaving orig empty
FXObjectList& FXObjectList::adopt(FXObjectList& orig){
  if(ptr!=orig.ptr){
    if(ptr!=EMPTY){ free(ptr-1); }
    ptr=orig.ptr;
    orig.ptr=EMPTY;
    }
  return *this;
  }


// Assign object p to list
FXObjectList& FXObjectList::assign(FXObject* object){
  if(no(1)){ ptr[0]=object; }
  return *this;
  }


// Assign n copies of object to list
FXObjectList& FXObjectList::assign(FXObject* object,FXint n){
  if(no(n)){ fillElms(ptr,object,n); }
  return *this;
  }


// Assign n objects to list
FXObjectList& FXObjectList::assign(FXObject** objects,FXint n){
  if(no(n)){ moveElms(ptr,objects,n); }
  return *this;
  }


// Assign input string to this string
FXObjectList& FXObjectList::assign(FXObjectList& objects){
  return assign(objects.ptr,objects.no());
  }


// Insert an object
FXObjectList& FXObjectList::insert(FXint pos,FXObject* object){
  register FXint num=no();
  if(no(num+1)){
    moveElms(ptr+pos+1,ptr+pos,num-pos);
    ptr[pos]=object;
    }
  return *this;
  }


// Insert n copies of object at specified position
FXObjectList& FXObjectList::insert(FXint pos,FXObject* object,FXint n){
  register FXint num=no();
  if(no(num+n)){
    moveElms(ptr+pos+n,ptr+pos,num-pos);
    fillElms(ptr+pos,object,n);
    }
  return *this;
  }


// Insert n objects at specified position
FXObjectList& FXObjectList::insert(FXint pos,FXObject** objects,FXint n){
  register FXint num=no();
  if(no(num+n)){
    moveElms(ptr+pos+n,ptr+pos,num-pos);
    copyElms(ptr+pos,objects,n);
    }
  return *this;
  }


// Insert objects at specified position
FXObjectList& FXObjectList::insert(FXint pos,FXObjectList& objects){
  return insert(pos,objects.ptr,objects.no());
  }


// Prepend an object
FXObjectList& FXObjectList::prepend(FXObject* object){
  register FXint num=no();
  if(no(num+1)){
    moveElms(ptr+1,ptr,num);
    ptr[0]=object;
    }
  return *this;
  }


// Prepend n copies of object
FXObjectList& FXObjectList::prepend(FXObject* object,FXint n){
  register FXint num=no();
  if(no(num+n)){
    moveElms(ptr+n,ptr,num);
    fillElms(ptr,object,n);
    }
  return *this;
  }


// Prepend n objects
FXObjectList& FXObjectList::prepend(FXObject** objects,FXint n){
  register FXint num=no();
  if(no(num+n)){
    moveElms(ptr+n,ptr,num);
    copyElms(ptr,objects,n);
    }
  return *this;
  }


// Prepend objects
FXObjectList& FXObjectList::prepend(FXObjectList& objects){
  return prepend(objects.ptr,objects.no());
  }


// Append an object
FXObjectList& FXObjectList::append(FXObject* object){
  register FXint num=no();
  if(no(num+1)){
    ptr[num]=object;
    }
  return *this;
  }


// Append n copies of object
FXObjectList& FXObjectList::append(FXObject* object,FXint n){
  register FXint num=no();
  if(no(num+n)){
    fillElms(ptr+num,object,n);
    }
  return *this;
  }


// Add string to the end
FXObjectList& FXObjectList::append(FXObject** objects,FXint n){
  register FXint num=no();
  if(no(num+n)){
    copyElms(ptr+num,objects,n);
    }
  return *this;
  }


// Add string to the end
FXObjectList& FXObjectList::append(FXObjectList& objects){
  return append(objects.ptr,objects.no());
  }


// Replace element
FXObjectList& FXObjectList::replace(FXint pos,FXObject* object){
  ptr[pos]=object;
  return *this;
  }



// Replaces the m objects at pos with n copies of object
FXObjectList& FXObjectList::replace(FXint pos,FXint m,FXObject* object,FXint n){
  register FXint num=no();
  if(m<n){
    if(!no(num-m+n)) return *this;
    moveElms(ptr+pos+n,ptr+pos+m,num-pos-n);
    }
  else if(m>n){
    moveElms(ptr+pos+n,ptr+pos+m,num-pos-m);
    if(!no(num-m+n)) return *this;
    }
  fillElms(ptr+pos,object,n);
  return *this;
  }


// Replaces the m objects at pos with n objects
FXObjectList& FXObjectList::replace(FXint pos,FXint m,FXObject** objects,FXint n){
  register FXint num=no();
  if(m<n){
    if(!no(num-m+n)) return *this;
    moveElms(ptr+pos+n,ptr+pos+m,num-pos-n);
    }
  else if(m>n){
    moveElms(ptr+pos+n,ptr+pos+m,num-pos-m);
    if(!no(num-m+n)) return *this;
    }
  copyElms(ptr+pos,objects,n);
  return *this;
  }


// Replace the m objects at pos with objects
FXObjectList& FXObjectList::replace(FXint pos,FXint m,FXObjectList& objects){
  return replace(pos,m,objects.ptr,objects.no());
  }


// Remove object at pos
FXObjectList& FXObjectList::erase(FXint pos){
  register FXint num=no();
  moveElms(ptr+pos,ptr+pos+1,num-pos-1);
  no(num-1);
  return *this;
  }


// Remove n objects at pos
FXObjectList& FXObjectList::erase(FXint pos,FXint n){
  register FXint num=no();
  moveElms(ptr+pos,ptr+pos+n,num-n-pos);
  no(num-n);
  return *this;
  }


// Remove element p
FXObjectList& FXObjectList::remove(const FXObject* object){
  register FXint num=no();
  register FXint pos;
  for(pos=0; pos<num; pos++){
    if(ptr[pos]==object){
      moveElms(ptr+pos,ptr+pos+1,num-pos-1);
      no(num-1);
      break;
      }
    }
  return *this;
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
FXObjectList& FXObjectList::clear(){
  no(0);
  return *this;
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
  if(ptr!=EMPTY){
    free(ptr-1);
    }
  }

}
