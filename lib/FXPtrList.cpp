/********************************************************************************
*                                                                               *
*                            P o i n t e r   L i s t                            *
*                                                                               *
*********************************************************************************
* Copyright (C) 1997,2014 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "FXPtrList.h"

/*
  Notes:
  - FXPtrList now stores only the number of items in the list, and
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

// Special empty pointer list value
#define EMPTY       ((FXptr*)(__ptrlist__empty__+1))

using namespace FX;

/*******************************************************************************/

namespace FX {


// Empty pointer list value
extern const FXival __ptrlist__empty__[];
const FXival __ptrlist__empty__[2]={0,0};


// Change number of items in list
FXbool FXPtrList::no(FXint num){
  register FXint old=no();
  register FXptr* p;
  if(__likely(old!=num)){
    if(0<num){
      if(ptr==EMPTY){
        if((p=(FXptr*)::malloc(ROUNDUP(num)*sizeof(FXptr)+sizeof(FXptr)))==NULL) return false;
        }
      else{
        if((p=(FXptr*)::realloc(ptr-1,ROUNDUP(num)*sizeof(FXptr)+sizeof(FXptr)))==NULL) return false;
        }
      ptr=p+1;
      *((FXint*)(ptr-1))=num;
      }
    else if(ptr!=EMPTY){
      ::free(ptr-1);
      ptr=EMPTY;
      }
    }
  return true;
  }


// Default constructor
FXPtrList::FXPtrList():ptr(EMPTY){
  }


// Copy constructor
FXPtrList::FXPtrList(const FXPtrList& src):ptr(EMPTY){
  register FXint num=src.no();
  if(__likely(0<num && no(num))){
    copyElms(ptr,src.ptr,num);
    }
  }


// Construct and init with single object
FXPtrList::FXPtrList(FXptr object):ptr(EMPTY){
  if(__likely(no(1))){
    ptr[0]=object;
    }
  }


// Construct and init with n copies of object
FXPtrList::FXPtrList(FXptr object,FXint n):ptr(EMPTY){
  if(__likely(0<n && no(n))){
    fillElms(ptr,object,n);
    }
  }


// Construct and init with list of objects
FXPtrList::FXPtrList(FXptr* objects,FXint n):ptr(EMPTY){
  if(__likely(0<n && no(n))){
    copyElms(ptr,objects,n);
    }
  }


// Assignment operator
FXPtrList& FXPtrList::operator=(const FXPtrList& orig){
  if(__likely(ptr!=orig.ptr && no(orig.no()))){
    copyElms(ptr,(FXptr*)orig.ptr,orig.no());
    }
  return *this;
  }


// Adopt objects from orig, leaving orig empty
FXPtrList& FXPtrList::adopt(FXPtrList& orig){
  if(__likely(ptr!=orig.ptr)){
    swap(ptr,orig.ptr);
    orig.clear();
    }
  return *this;
  }


// Assign object p to list
FXbool FXPtrList::assign(FXptr object){
  if(__likely(no(1))){
    ptr[0]=object;
    return true;
    }
  return false;
  }


// Assign n copies of object to list
FXbool FXPtrList::assign(FXptr object,FXint n){
  if(__likely(no(n))){
    fillElms(ptr,object,n);
    return true;
    }
  return false;
  }


// Assign n objects to list
FXbool FXPtrList::assign(FXptr* objects,FXint n){
  if(__likely(no(n))){
    moveElms(ptr,objects,n);
    return true;
    }
  return false;
  }


// Assign input string to this string
FXbool FXPtrList::assign(const FXPtrList& objects){
  return assign((FXptr*)objects.ptr,objects.no());
  }


// Insert an object
FXbool FXPtrList::insert(FXint pos,FXptr object){
  register FXint num=no();
  if(__likely(no(num+1))){
    moveElms(ptr+pos+1,ptr+pos,num-pos);
    ptr[pos]=object;
    return true;
    }
  return false;
  }


// Insert n copies of object at specified position
FXbool FXPtrList::insert(FXint pos,FXptr object,FXint n){
  register FXint num=no();
  if(__likely(no(num+n))){
    moveElms(ptr+pos+n,ptr+pos,num-pos);
    fillElms(ptr+pos,object,n);
    return true;
    }
  return false;
  }


// Insert n objects at specified position
FXbool FXPtrList::insert(FXint pos,FXptr* objects,FXint n){
  register FXint num=no();
  if(__likely(no(num+n))){
    moveElms(ptr+pos+n,ptr+pos,num-pos);
    copyElms(ptr+pos,objects,n);
    return true;
    }
  return false;
  }


// Insert objects at specified position
FXbool FXPtrList::insert(FXint pos,const FXPtrList& objects){
  return insert(pos,(FXptr*)objects.ptr,objects.no());
  }


// Prepend an object
FXbool FXPtrList::prepend(FXptr object){
  register FXint num=no();
  if(__likely(no(num+1))){
    moveElms(ptr+1,ptr,num);
    ptr[0]=object;
    return true;
    }
  return false;
  }


// Prepend n copies of object
FXbool FXPtrList::prepend(FXptr object,FXint n){
  register FXint num=no();
  if(__likely(no(num+n))){
    moveElms(ptr+n,ptr,num);
    fillElms(ptr,object,n);
    return true;
    }
  return false;
  }


// Prepend n objects
FXbool FXPtrList::prepend(FXptr* objects,FXint n){
  register FXint num=no();
  if(__likely(no(num+n))){
    moveElms(ptr+n,ptr,num);
    copyElms(ptr,objects,n);
    return true;
    }
  return false;
  }


// Prepend objects
FXbool FXPtrList::prepend(const FXPtrList& objects){
  return prepend((FXptr*)objects.ptr,objects.no());
  }


// Append an object
FXbool FXPtrList::append(FXptr object){
  register FXint num=no();
  if(__likely(no(num+1))){
    ptr[num]=object;
    return true;
    }
  return false;
  }


// Append n copies of object
FXbool FXPtrList::append(FXptr object,FXint n){
  register FXint num=no();
  if(__likely(no(num+n))){
    fillElms(ptr+num,object,n);
    return true;
    }
  return false;
  }


// Add string to the end
FXbool FXPtrList::append(FXptr* objects,FXint n){
  register FXint num=no();
  if(__likely(no(num+n))){
    copyElms(ptr+num,objects,n);
    return true;
    }
  return false;
  }


// Add string to the end
FXbool FXPtrList::append(const FXPtrList& objects){
  return append((FXptr*)objects.ptr,objects.no());
  }


// Replace element
FXbool FXPtrList::replace(FXint pos,FXptr object){
  ptr[pos]=object;
  return true;
  }


// Replaces the m objects at pos with n copies of object
FXbool FXPtrList::replace(FXint pos,FXint m,FXptr object,FXint n){
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
FXbool FXPtrList::replace(FXint pos,FXint m,FXptr* objects,FXint n){
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
FXbool FXPtrList::replace(FXint pos,FXint m,const FXPtrList& objects){
  return replace(pos,m,(FXptr*)objects.ptr,objects.no());
  }


// Remove object at pos
FXbool FXPtrList::erase(FXint pos){
  register FXint num=no();
  moveElms(ptr+pos,ptr+pos+1,num-pos-1);
  return no(num-1);
  }


// Remove n objects at pos
FXbool FXPtrList::erase(FXint pos,FXint n){
  register FXint num=no();
  moveElms(ptr+pos,ptr+pos+n,num-n-pos);
  return no(num-n);
  }


// Push object to end
FXbool FXPtrList::push(FXptr object){
  register FXint num=no();
  if(__likely(no(num+1))){
    ptr[num]=object;
    return true;
    }
  return false;
  }


// Pop object from end
FXbool FXPtrList::pop(){
  return no(no()-1);
  }


// Remove object
FXbool FXPtrList::remove(FXptr object){
  register FXint pos;
  if(0<=(pos=find(object))){
    return erase(pos);
    }
  return false;
  }


// Find object in list, searching forward; return position or -1
FXint FXPtrList::find(FXptr object,FXint pos) const {
  register FXint p=FXMAX(0,pos);
  while(p<no()){
    if(ptr[p]==object){ return p; }
    ++p;
    }
  return -1;
  }


// Find object in list, searching backward; return position or -1
FXint FXPtrList::rfind(FXptr object,FXint pos) const {
  register FXint p=FXMIN(pos,no()-1);
  while(0<=p){
    if(ptr[p]==object){ return p; }
    --p;
    }
  return -1;
  }


// Clear the list
void FXPtrList::clear(){
  if(__likely(ptr!=EMPTY)){
    ::free(ptr-1);
    ptr=EMPTY;
    }
  }


// Free up nicely
FXPtrList::~FXPtrList(){
  clear();
  }

}
