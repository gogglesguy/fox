/********************************************************************************
*                                                                               *
*                       A b s t r a c t   I / O   C l a s s                     *
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



/*
  Notes:

  - An abstract class for IO.
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
FXIO::FXIO():access(NoAccess){
  }


// Construct with given mode
FXIO::FXIO(FXuint m):access(m){
  }


// Is readable
FXbool FXIO::isReadable() const {
  return ((access&ReadOnly)!=0);
  }


// Is writable
FXbool FXIO::isWritable() const {
  return ((access&WriteOnly)!=0);
  }


// Return true if open
FXbool FXIO::isOpen() const {
  return true;
  }


// Return true if serial access only
FXbool FXIO::isSerial() const {
  return true;
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
  return true;
  }


// Destroy
FXIO::~FXIO(){
  }


}

