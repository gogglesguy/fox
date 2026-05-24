/********************************************************************************
*                                                                               *
*                        I / O   D e v i c e   C l a s s                        *
*                                                                               *
*********************************************************************************
* Copyright (C) 2005,2026 by Jeroen van der Zijp.   All Rights Reserved.        *
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

  /// Construct device and attach existing handle h
  FXIODevice(FXInputHandle h);

  /// Return handle
  FXInputHandle handle() const { return device; }

  /// Return true if open
  virtual FXbool isOpen() const override;

  /// Return access mode
  virtual FXuint mode() const override;

  /// Change access mode of open device
  virtual FXbool mode(FXuint m) override;

  /// Return permissions
  virtual FXuint perms() const override;

  /// Set permissions
  virtual FXbool perms(FXuint p) override;

  /// Attach existing device handle, taking ownership of the handle
  virtual FXbool attach(FXInputHandle h);

  /// Detach device handle
  virtual FXbool detach();

  /// Read block of bytes, returning number of bytes read
  virtual FXival readBlock(void* ptr,FXival count) override;

  /// Write block of bytes, returning number of bytes written
  virtual FXival writeBlock(const void* ptr,FXival count) override;

  /// Truncate file
  virtual FXlong truncate(FXlong sz) override;

  /// Flush to disk
  virtual FXbool flush() override;

  /// Test if we're at the end; -1 if error
  virtual FXint eof() override;

  /// Return size
  virtual FXlong size() override;

  /// Close handle
  virtual FXbool close() override;

  /// Check handle validity
  static FXbool valid(FXInputHandle hnd);

  /// Destroy and close
  virtual ~FXIODevice();
  };

}

#endif
