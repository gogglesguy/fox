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
#ifndef FXIOBUFFER_H
#define FXIOBUFFER_H

#ifndef FXIO_H
#include "FXIO.h"
#endif


namespace FX {


/**
* IOBuffer is a file-interface to a chunk of memory, permitting
* file-operations to be performed on memory-resident data.
*/
class FXAPI FXIOBuffer : public FXIO {
protected:
  FXuchar *begptr;      // Begin of buffer
  FXuchar *endptr;      // End of buffer
  FXuchar *ptr;         // Pointer
private:
  FXIOBuffer(const FXIOBuffer&);
  FXIOBuffer &operator=(const FXIOBuffer&);
public:

  /// Construct
  FXIOBuffer();

  /// Construct and open
  FXIOBuffer(FXuchar* data=NULL,FXuval size=0L,FXuint m=FXIO::Reading);

  /// Open buffer 
  virtual FXbool open(FXuchar* data=NULL,FXuval size=0L,FXuint m=FXIO::Reading);

  /// Return true if open
  virtual FXbool isOpen() const;

  /// Return true if serial access only
  virtual FXbool isSerial() const;

  /// Get current file position
  virtual FXlong position() const;

  /// Change file position, returning new position from start
  virtual FXlong position(FXlong offset,FXuint from=FXIO::Begin);

  /// Read block of bytes, returning number of bytes read
  virtual FXival readBlock(void* data,FXival count);

  /// Write block of bytes, returning number of bytes written
  virtual FXival writeBlock(const void* data,FXival count);

  /// Truncate file
  virtual FXlong truncate(FXlong size);

  /// Flush to disk
  virtual FXbool flush();

  /// Test if we're at the end; -1 if error
  virtual FXint eof();

  /// Return size
  virtual FXlong size();

  /// Close handle
  virtual FXbool close();

  /// Destroy and close
  virtual ~FXIOBuffer();
  };

}

#endif
