/********************************************************************************
*                                                                               *
*                           R e g i s t r y   C l a s s                         *
*                                                                               *
*********************************************************************************
* Copyright (C) 1998,2010 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#ifndef FXREGISTRY_H
#define FXREGISTRY_H

#ifndef FXSETTINGS_H
#include "FXSettings.h"
#endif

namespace FX {


/**
* The registry maintains a database of persistent settings for an application.
* The settings database is organized in two groups of three layers each.  The
* system-wide settings group contains settings information pertaining to all
* users on a system.  The per-user settings group contains settings affecting
* that user only.
* Each settings group contains a desktop layer, which comprises the settings
* which affect all FOX programs, a vendor layer which holds settings that
* affect all applications from that vendor (e.g. a application-suite), and
* an application layer which holds settings only for a single application.
* The vendor-key and application-key determine which files these layers come
* from, while the "Desktop" key is used for all FOX applications.
* Settings in the system-wide group are overwritten by the per-user group,
* and settings from the "Desktop" layer are overwritten by the vendor-layer;
* vendor-layer settings are overwritten by the application-layer settings.
* Only the per-user, per-application settings ever gets written; the layers
* in the system-group only get written during installation and configuration
* of the application.
* The registry is read when FXApp::init() is called, and written back to the
* system when FXApp::exit() is called.
*/
class FXAPI FXRegistry : public FXSettings {
  FXDECLARE(FXRegistry)
protected:
  FXString applicationkey;  // Application key
  FXString vendorkey;       // Vendor key
  FXString systemdirs;      // System-wide settings directories
  FXString userdir;         // User settings directory
  FXbool   ascii;           // ASCII file-based registry
protected:
#if defined(WIN32)
  FXbool readFromRegistry(void* hRootKey,FXbool mark);
  FXbool readFromRegistryGroup(void* org,const char* groupname,FXbool mark=false);
  FXbool writeToRegistry(void* hRootKey);
  FXbool writeToRegistryGroup(void* org,const char* groupname);
#endif
private:
  FXRegistry(const FXRegistry&);
  FXRegistry &operator=(const FXRegistry&);
public:
  static const FXchar foxrc[];          // Name of common settings file
  static const FXchar ext[];            // File extension for settings files
public:

  /**
  * Construct registry object; akey and vkey must be string constants.
  * Regular applications SHOULD set a vendor key!
  */
  FXRegistry(const FXString& akey=FXString::null,const FXString& vkey=FXString::null);

  /**
  * Set ASCII mode; under MS-Windows, this will switch the system to a
  * file-based registry system, instead of using the System Registry API.
  */
  void setAsciiMode(FXbool asciiMode){ ascii=asciiMode; }

  /// Get ASCII mode
  FXbool getAsciiMode() const { return ascii; }

  /// Change application key name
  void setAppKey(const FXString& name){ applicationkey=name; }

  /// Return application key name
  const FXString& getAppKey() const { return applicationkey; }

  /// Change vendor key name
  void setVendorKey(const FXString& name){ vendorkey=name; }

  /// Return vendor key name
  const FXString& getVendorKey() const { return vendorkey; }

  /// Change search path for system-wide settings
  void setSystemDirectories(const FXString& dirs){ systemdirs=dirs; }

  /// Return search path for system-wide settings
  const FXString& getSystemDirectories() const { return systemdirs; }

  /// Change directory root for per-user settings tree
  void setUserDirectory(const FXString& dir){ userdir=dir; }

  /// Return directory root of per-user settings tree
  const FXString& getUserDirectory() const { return userdir; }

  /// Read registry
  virtual FXbool read();

  /// Write registry
  virtual FXbool write();

  /// Destructor
  virtual ~FXRegistry();
  };

}

#endif
