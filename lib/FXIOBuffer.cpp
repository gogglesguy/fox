/********************************************************************************
*                                                                               *
*                        I / O   B u f f e r   C l a s s                        *
*                                                                               *
*********************************************************************************
* Copyright (C) 2005,2013 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "fxascii.h"
#include "FXArray.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXString.h"
#include "FXIO.h"
#include "FXIOBuffer.h"


/*
  Notes:
  - FIXME need option "owned" vs non-owned buffer.
  - Etc.
*/


using namespace FX;

/*******************************************************************************/

namespace FX {



// Construct
FXIOBuffer::FXIOBuffer():begptr(NULL),endptr(NULL),ptr(NULL){
  }


// Construct and open
FXIOBuffer::FXIOBuffer(FXuchar* data,FXuval size,FXuint m):begptr(NULL),endptr(NULL),ptr(NULL){
  open(data,size,m);
  }



// Open buffer 
FXbool FXIOBuffer::open(FXuchar* data,FXuval size,FXuint m){
  begptr=data;
  endptr=begptr+size;
  access=m;
  return true;
  }
  

// Return true if open
FXbool FXIOBuffer::isOpen() const {
  return endptr!=begptr;
  }


// Return true if serial access only
FXbool FXIOBuffer::isSerial() const {
  return false;
  }


// Get position
FXlong FXIOBuffer::position() const {
  return ptr-begptr;
  }


// Move to position
FXlong FXIOBuffer::position(FXlong off,FXuint from){
  if(access&ReadWrite){
    if(from==Current) off+=(ptr-begptr);
    else if(from==End) off+=(endptr-begptr);
    ptr=begptr+off;
    return ptr-begptr;
    }
  return -1;
  }


// Read block
FXival FXIOBuffer::readBlock(void* data,FXival count){
  if(access&ReadOnly){
    if(count>(endptr-ptr)) count=(endptr-ptr);
    memmove(data,ptr,count);
    ptr+=count;
    return count;
    }
  return 0;
  }


// Write block
FXival FXIOBuffer::writeBlock(const void* data,FXival count){
  if(access&WriteOnly){
    if(count>(endptr-ptr)) count=(endptr-ptr);
    memmove(ptr,data,count);
    ptr+=count;
    return count;
    }
  return 0;
  }


// Truncate file
FXlong FXIOBuffer::truncate(FXlong){
  return -1;
  }


// Synchronize disk with cached data
FXbool FXIOBuffer::flush(){
  return false;
  }


// Test if we're at the end; -1 if error
FXint FXIOBuffer::eof(){
  return ptr>=endptr;
  }


// Return file size
FXlong FXIOBuffer::size(){
  return endptr-begptr;
  }


// Close file
FXbool FXIOBuffer::close(){
  begptr=endptr=ptr=NULL;
  return true;
  }


// Destroy
FXIOBuffer::~FXIOBuffer(){
  close();
  }


}

