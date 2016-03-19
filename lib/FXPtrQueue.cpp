/********************************************************************************
*                                                                               *
*                         Q u e u e   O f   P o i n t e r s                     *
*                                                                               *
*********************************************************************************
* Copyright (C) 2006,2012 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "fxdefs.h"
#include "FXElement.h"
#include "FXPtrQueue.h"


/*
  Notes:
  - Its safe to write with only one thread, and read with only one thread.
*/

using namespace FX;


namespace FX {

/*******************************************************************************/


// Create queue with initial size
FXPtrQueue::FXPtrQueue(FXuint sz):list(NULL),size(0),head(0),tail(0){
  if(callocElms(list,sz)){ size=sz; }
  }


// Change size of queue; return true if success
FXbool FXPtrQueue::setSize(FXuint sz){
  if(resizeElms(list,sz)){
    size=sz;
    head=0;
    tail=0;
    return true;
    }
  return false;
  }


// Return size
FXuint FXPtrQueue::getSize() const {
  return size;
  }


// Return head
FXuint FXPtrQueue::getHead() const {
  return head;
  }


// Return tail
FXuint FXPtrQueue::getTail() const {
  return tail;
  }


// Return used slots
FXuint FXPtrQueue::getUsed() const {
  return (head-tail+size)%size;
  }


// Return free slots
FXuint FXPtrQueue::getFree() const {
  return (size-1+tail-head)%size;
  }


// Check if queue is full
FXbool FXPtrQueue::isFull() const {
  return ((size+head-tail+1)%size)==0;
  }


// Check if queue is empty
FXbool FXPtrQueue::isEmpty() const {
  return (head-tail)==0;
  }


// Peek for item
FXbool FXPtrQueue::peek(void*& ptr){
  if(__likely(head!=tail)){
    ptr=list[tail];
    return true;
    }
  return false;
  }


// Add item to queue, return true if success
FXbool FXPtrQueue::push(void* ptr){
  FXuint next=(head+1)%size;
  if(__likely(next!=tail)){
    list[head]=ptr;
    head=next;
    return true;
    }
  return false;
  }


// Remove item from queue, return true if success
FXbool FXPtrQueue::pop(void*& ptr){
  if(__likely(head!=tail)){
    FXuint next=(tail+1)%size;
    ptr=list[tail];
    tail=next;
    return true;
    }
  return false;
  }


// Destroy job queue
FXPtrQueue::~FXPtrQueue(){
  freeElms(list);
  }

}
