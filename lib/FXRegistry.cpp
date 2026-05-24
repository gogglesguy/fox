/********************************************************************************
*                                                                               *
*                           R e g i s t r y   C l a s s                         *
*                                                                               *
*********************************************************************************
* Copyright (C) 1998,2026 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "fxmath.h"
#include "FXElement.h"
#include "FXArray.h"
#include "FXMetaClass.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXString.h"
#include "FXSystem.h"
#include "FXProcess.h"
#include "FXPath.h"
#include "FXIO.h"
#include "FXStat.h"
#include "FXFile.h"
#include "FXDir.h"
#include "FXStringDictionary.h"
#include "FXRegistry.h"

/*
  Notes:

  - The FOX settings tree stores configuration data for FOX-based applications; it
    is organized as follows:

    SettingsRoot/fox.rc                 The "foxrc" file stores common configuration
                                        settings for all FOX-based applications,
                                        such as colors and file bindings.

    SettingsRoot/Vendor/Vendor.rc       The Vendorrc file is common to all applications
                                        from a particular organization, so that common
                                        configurable attributes may be given consistent
                                        values easily.

    SettingsRoot/Vendor/Application.rc  The Applicationrc file stores configuration data
                                        unique to a specific application only.

    SettingsRoot/Application.rc         If no Vendor name is specified, the Applicationrc
                                        file is located at the toplevel of the SettingsRoot.

  - A System-Wide SettingsRoot may be located in a read-only, system adminstrator designated
    place, to be shared by all users on a particular installation.  For example, this may store
    license-keys, references to online documentation, and other installation-time parameters.

  - A Per-User SettingsRoot is typically located in a user's home directory.
    Configurations stored in these files will be merged with the System-Wide configurations.
    Parameters given different values in the Per-User files will override those in the
    System-Wide ones.

  - Rationale:

      1)    When installing an application, simply copy "seed" registry files to
            the System-Wide SettingsRoot; having a subdirectory Vendor prevents
            clobbering other people's registry files, even if their application
            has the same name.

      2)    System-Wide registry files are, as a matter of principle, read-only.

      3)    System-Wide registry files are loaded first, and Per-User registry
            files are loaded on top of that.

      4)    Registry files loaded later will take precedence over those loaded
            earlier; i.e. key/value pairs in a later file will override a key/value
            pair with the same key loaded earlier.

      5)    The exception to the rule is that a key/value pair will not be overridden
            if the value of the key was changed since it had been loaded.

      6)    An application reads files in the order:

                System-Wide:
                        fox.rc,
                        Vendor/Vendor.rc,
                        Vendor/Application.rc

                Per-User:
                        fox.rc,
                        Vendor/Vendor.rc,
                        Vendor/Application.rc

      7)    When System-Wide settings files are loaded, entries are not marked as
            changed, and thus not written into the Per-User settings file.  Only
            values changed by the Application will be written into the Per-User file.

      8)    Settings loaded from the Per-User settings file *will* however be written
            back to the Per-User settings file, regarless whether they are changed or
            not (exception is when no single entry was changed at all!).

      9)    ONLY the Per-User, Application-specific settings files ARE WRITTEN!

     10)    Special applications, such as ControlPanel, may change other settings files,
            however.

  - Locations of registry settings files:

      o System-Wide registry files are loaded from the directory defined by user-
        specified environment variable $FOXDIR, if this variable was set.

      o System-Wide registry files are searched in SystemDirectories path-list.
        The path-list may be changed by setSystemDirectories().

      o On UNIX systems, the XDG standard is followed; this means the location
        of System-Wide settings should controlled by $XDG_CONFIG_DIRS.

      o If this environment variable isn't set, then SystemDirectories with only have
        one single directory, the default value: "/etc/xdg".

      o Directories in SystemDirectories will be searched for a subdirectory "foxrc"
        which is the root of the System-Wide settings tree for all FOX applications;
        for example: "/etc/xdg/foxrc/Desktop".

      o Per-User registry files are located from a single toplevel directory in variable
        UserDirectory.  The value of this variable may be changed by setUserDirectory().

      o On UNIX systems, the XDG standard is followed; this means the location of the
        Per-User settings should controlled by $XDG_CONFIG_HOME.   If this environment
        variable set, the root for the Per-User settings tree for all FOX applications
        is $XDG_CONFIG_HOME/fox.rc.

      o Otherwise, it will have the default value: "~/.config".

  - The Freedesktop.org XDG standard is found at:

      http://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html

      $XDG_CONFIG_DIRS          Colon separated search path for configuration files.
                                Default: "/etc/xdg".
      $XDG_CONFIG_HOME          Configuration files for single user.
                                Default: "$HOME/.config".
      $XDG_DATA_DIRS            Colon separated search path for data files.
                                Default: "$HOME/.local/share:/usr/local/share/:/usr/share/"
      $XDG_DATA_HOME            Data files for single user.
                                Default: "$HOME/.local/share".

  - Somewhat equivalent information for Windows:

      %ALLUSERSPROFILE%         Configurations for all users of the system.
                                Default: "C:\ProgramData".
      %APPDATA%                 Configuration file for single user, migrated to other
                                PCs user might log on to.
                                Default: "C:\Users\UserName\AppData\Roaming".
      %LOCALAPPDATA%            Configuration files for single user on this PC only.
                                Default: "C:\Users\UserName\AppData\Local".
      %USERPROFILE%             Home directory.

    We use the order $FOXDIR, $XDG_CONFIG_DIRS, and "/etc/xdg" on Linux, and %FOXDIR%,
    %ALLUSERSPROFILE%, and %APPDATA% on Windows [an argument could be made for using
    %LOCALAPPDATA%, however].
*/

#define TOPIC_CONSTRUCT 1000
#define TOPIC_DETAIL    1001

#define MAXNAME   200
#define MAXVALUE  2000

// Default locations and names
#if defined(WIN32)
#define FOXRC           "fox.ini"
#define SYSTEMDIRS      "%FOXDIR%;%ALLUSERSPROFILE%;C:\\ProgramData"
#define USERDIR         "%APPDATA%"
#define FILEEXT         ".ini"
#else
#define FOXRC           "fox.rc"
#define SYSTEMDIRS      "$FOXDIR:$XDG_CONFIG_DIRS:/etc/xdg"
#define USERDIR         "$HOME/.config"
#define FILEEXT         ".rc"
#endif

using namespace FX;

/*******************************************************************************/

namespace FX {


// File extension for settings files
const FXchar FXRegistry::ext[]=FILEEXT;


// File name of common settings file
const FXchar FXRegistry::foxrc[]=FOXRC;


// Make registry object
FXRegistry::FXRegistry(const FXString& akey,const FXString& vkey):applicationkey(akey),vendorkey(vkey),systemdirs(SYSTEMDIRS),userdir(USERDIR){
  FXTRACE(TOPIC_CONSTRUCT,"FXRegistry::FXRegistry\n");
  }


// Read registry
FXbool FXRegistry::read(){
  FXString path;
  FXbool ok=false;

  // Read system-wide settings from systemdirs
  if(!systemdirs.empty()){

    FXTRACE(TOPIC_DETAIL,"FXRegistry::read: systemdirs=%s\n",systemdirs.text());

    // Find common settings
    path=FXPath::search(systemdirs,FOXRC);
    if(!path.empty()){
      if(parseFile(path,false)) ok=true;
      }

    // Have vendor subdirectory
    if(!vendorkey.empty()){

      // Find vendor subdirectory
      path=FXPath::search(systemdirs,vendorkey);
      if(!path.empty()){

        // Try read vendor settings
        if(parseFile(path+PATHSEPSTRING+vendorkey+ext,false)) ok=true;

        // Try read application settings
        if(!applicationkey.empty()){
          if(parseFile(path+PATHSEPSTRING+applicationkey+ext,false)) ok=true;
          }
        }
      }

    // Have application settings only
    else if(!applicationkey.empty()){

      // Find applications settings
      path=FXPath::search(systemdirs,applicationkey+ext);

      // Try read application settings
      if(!path.empty()){
        if(parseFile(path,false)) ok=true;
        }
      }
    }

  // Read per-user settings from userdir
  if(!userdir.empty()){

    FXTRACE(TOPIC_DETAIL,"FXRegistry::read: userdir=%s\n",userdir.text());

    // Path to settings data
    path=FXPath::absolute(FXPath::expand(userdir));

    // Try read common settings
    if(parseFile(path+PATHSEPSTRING FOXRC,false)) ok=true;

    // Try read vendor settings
    if(!vendorkey.empty()){
      path.append(PATHSEPSTRING+vendorkey);
      if(parseFile(path+PATHSEPSTRING+vendorkey+ext,false)) ok=true;
      }

    // Try read application settings
    if(!applicationkey.empty()){
      if(parseFile(path+PATHSEPSTRING+applicationkey+ext,true)) ok=true;
      }
    }
  return ok;
  }

/*******************************************************************************/

// Write registry
FXbool FXRegistry::write(){
  FXbool ok=false;
  if(isModified()){

    // Write per-user settings to userdir
    if(!userdir.empty()){

      FXTRACE(TOPIC_DETAIL,"FXRegistry::write: userdir=%s\n",userdir.text());

      // Have application key
      if(!applicationkey.empty()){

        // Path to settings data
        FXString path=FXPath::absolute(FXPath::expand(userdir));

        // Have vendor key
        if(!vendorkey.empty()){
          path.append(PATHSEPSTRING+vendorkey);
          }

        // Ensure parent directories exist
        if(FXDir::createDirectories(path)){

          // Final registry filename
          FXString realfile=path+PATHSEPSTRING+applicationkey+ext;

          // Temporary registry filename
          FXString tempfile=path+PATHSEPSTRING+applicationkey+"_"+FXString::value(FXProcess::current())+ext;

          // Unparse settings into temp file first
          if(unparseFile(tempfile)){

            // Rename ATOMICALLY to proper name
            if(FXFile::move(tempfile,realfile,true)){
              setModified(false);
              ok=true;
              }
            }
          }
        }
      }
    }
  return ok;
  }


// Destructor
FXRegistry::~FXRegistry(){
  FXTRACE(TOPIC_CONSTRUCT,"FXRegistry::~FXRegistry\n");
  }

}
