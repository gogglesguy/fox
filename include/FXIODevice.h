/********************************************************************************
*                                                                               *
*                        I / O   D e v i c e   C l a s s                        *
*                                                                               *
*********************************************************************************
* Copyright (C) 2005,2011 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#ifndef FXIODEVICE_H
#define FXIODEVICE_H


#ifndef FXIO_H
#include "FXIO.h"
#endif


namespace FX {


/**
* FXIODevice manipulates a handle to operating system i/o device,
* such as pipes, sockets, or files.
*/
class FXAPI FXIODevice : public FXIO {
protected:
  FXInputHandle device;
private:
  FXIODevice(const FXIODevice&);
  FXIODevice &operator=(const FXIODevice&);
public:

  /// Construct
  FXIODevice();

  /// Construct with given handle and mode
  FXIODevice(FXInputHandle h,FXuint m);

  /// Open device with access mode m and handle h
  virtual FXbool open(FXInputHandle h,FXuint m);

  /// Return handle
  FXInputHandle handle() const { return device; }

  /// Return true if open
  virtual FXbool isOpen() const;

  /// Return true if serial access only
  virtual FXbool isSerial() const;

  /// Attach existing device handle, taking ownership of the handle
  virtual void attach(FXInputHandle h,FXuint m);

  /// Detach device handle, disowning the handle
  virtual void detach();

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

  /// Test if we're at the end
  virtual FXbool eof();

  /// Return size of i/o device
  virtual FXlong size();

  /// Close handle
  virtual FXbool close();

  /// Destroy and close
  virtual ~FXIODevice();
  };

}

#endif
