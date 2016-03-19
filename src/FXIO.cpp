/********************************************************************************
*                                                                               *
*                        I / O   D e v i c e   C l a s s                        *
*                                                                               *
*********************************************************************************
* Copyright (C) 2005,2007 by Jeroen van der Zijp.   All Rights Reserved.        *
*********************************************************************************
* This library is free software; you can redistribute it and/or                 *
* modify it under the terms of the GNU Lesser General Public                    *
* License as published by the Free Software Foundation; either                  *
* version 2.1 of the License, or (at your option) any later version.            *
*                                                                               *
* This library is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             *
* Lesser General Public License for more details.                               *
*                                                                               *
* You should have received a copy of the GNU Lesser General Public              *
* License along with this library; if not, write to the Free Software           *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.    *
*********************************************************************************
* $Id: FXIO.cpp,v 1.19 2007/04/04 14:45:42 fox Exp $                            *
********************************************************************************/
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "fxascii.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXString.h"
#include "FXIO.h"



/*
  Notes:

  - An abstract class for low-level IO.
*/

// Bad handle value
#ifdef WIN32
#define BadHandle INVALID_HANDLE_VALUE
#else
#define BadHandle -1
#endif

using namespace FX;

/*******************************************************************************/

namespace FX {



// Construct
FXIO::FXIO():device(BadHandle),access(NoAccess){
  }


// Is readable
FXbool FXIO::isReadable() const {
  return ((access&ReadOnly)!=0);
  }


// Is writable
FXbool FXIO::isWritable() const {
  return ((access&WriteOnly)!=0);
  }


// Open file
FXbool FXIO::open(FXInputHandle h,FXuint m){
  device=h;
  access=m;
  return true;
  }


// Return true if open
FXbool FXIO::isOpen() const {
  return device!=BadHandle;
  }


// Return true if serial access only
FXbool FXIO::isSerial() const {
  return true;
  }


// Attach existing file handle
void FXIO::attach(FXInputHandle h,FXuint m){
  close();
  device=h;
  access=(m|OwnHandle);
  }


// Detach existing file handle
void FXIO::detach(){
  access&=~OwnHandle;
  }


// Get position
FXlong FXIO::position() const {
  return -1;
  }


// Move to position
FXlong FXIO::position(FXlong,FXuint){
  return -1;
  }


// Read block
FXival FXIO::readBlock(void*,FXival){
  return 0;
  }


// Write block
FXival FXIO::writeBlock(const void*,FXival count){
  return count;
  }


// Truncate file
FXlong FXIO::truncate(FXlong){
  return -1;
  }


// Synchronize disk with cached data
FXbool FXIO::flush(){
  return false;
  }


// Test if we're at the end
FXbool FXIO::eof(){
  return true;
  }


// Return file size
FXlong FXIO::size(){
  return 0;
  }


// Close file
FXbool FXIO::close(){
  device=BadHandle;
  access=NoAccess;
  return true;
  }


// Destroy
FXIO::~FXIO(){
  close();
  }


}

