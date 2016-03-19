/********************************************************************************
*                                                                               *
*                           S e t t i n g s   C l a s s                         *
*                                                                               *
*********************************************************************************
* Copyright (C) 1998,2009 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#ifndef FXSETTINGS_H
#define FXSETTINGS_H

#ifndef FXDICT_H
#include "FXDict.h"
#endif

namespace FX {


class FXStringDict;


/**
* The Settings class manages a key-value database.  This is normally used as
* part of Registry, but can also be used separately in applications that need
* to maintain a key-value database in a file of their own.
* String values can contain any character, and will be escaped when written
* to the file.
*/
class FXAPI FXSettings : public FXDict {
  FXDECLARE(FXSettings)
protected:
  FXbool modified;
protected:
  virtual void *createData(void*);
  virtual void deleteData(void*);
protected:
  FXStringDict* insert(const FXchar* ky){ return (FXStringDict*)FXDict::insert(ky,NULL); }
  FXStringDict* replace(const FXchar* ky,FXStringDict* section){ return (FXStringDict*)FXDict::replace(ky,section,true); }
  FXStringDict* remove(const FXchar* ky){ return (FXStringDict*)FXDict::remove(ky); }
public:

  /// Construct settings database.
  FXSettings();

  /// Construct copy of existing database.
  FXSettings(const FXSettings& orig);

  /// Assignment operator
  FXSettings &operator=(const FXSettings& orig);

  /// Is it modified
  FXbool isModified() const { return modified; }

  /// Mark as changed
  void setModified(FXbool mdfy=true){ modified=mdfy; }

  /// Parse a file containing a settings database.
  FXbool parseFile(const FXString& filename,FXbool mark);

  /// Unparse settings database into given file.
  FXbool unparseFile(const FXString& filename);

  /// Find string dictionary for the given section; may be NULL
  FXStringDict* find(const FXchar *section) const { return (FXStringDict*)FXDict::find(section); }

  /// Obtain the string dictionary for the given section
  FXStringDict* data(FXint pos) const { return (FXStringDict*)FXDict::data(pos); }

  /// Read a formatted registry entry, using scanf-style format
  FXint readFormatEntry(const FXchar *section,const FXchar *name,const FXchar *fmt,...) const FX_SCANF(4,5) ;
  FXint readFormatEntry(const FXString& section,const FXchar *name,const FXchar *fmt,...) const FX_SCANF(4,5) ;
  FXint readFormatEntry(const FXString& section,const FXString& name,const FXchar *fmt,...) const FX_SCANF(4,5) ;

  /// Write a formatted registry entry, using printf-style format
  FXint writeFormatEntry(const FXchar *section,const FXchar *name,const FXchar *fmt,...) FX_PRINTF(4,5) ;
  FXint writeFormatEntry(const FXString& section,const FXchar *name,const FXchar *fmt,...) FX_PRINTF(4,5) ;
  FXint writeFormatEntry(const FXString& section,const FXString& name,const FXchar *fmt,...) FX_PRINTF(4,5) ;

  /// Read a string registry entry; if no value is found, the default value def is returned
  const FXchar *readStringEntry(const FXchar *section,const FXchar *name,const FXchar *def=NULL) const;
  const FXchar *readStringEntry(const FXString& section,const FXchar *name,const FXchar *def=NULL) const;
  const FXchar *readStringEntry(const FXString& section,const FXString& name,const FXchar *def=NULL) const;

  /// Write a string registry entry
  FXbool writeStringEntry(const FXchar *section,const FXchar *name,const FXchar *val);
  FXbool writeStringEntry(const FXString& section,const FXchar *name,const FXchar *val);
  FXbool writeStringEntry(const FXString& section,const FXString& name,const FXchar *val);

  /// Read a integer registry entry; if no value is found, the default value def is returned
  FXint readIntEntry(const FXchar *section,const FXchar *name,FXint def=0) const;
  FXint readIntEntry(const FXString& section,const FXchar *name,FXint def=0) const;
  FXint readIntEntry(const FXString& section,const FXString& name,FXint def=0) const;

  /// Write a integer registry entry
  FXbool writeIntEntry(const FXchar *section,const FXchar *name,FXint val);
  FXbool writeIntEntry(const FXString& section,const FXchar *name,FXint val);
  FXbool writeIntEntry(const FXString& section,const FXString& name,FXint val);

  /// Read a unsigned integer registry entry; if no value is found, the default value def is returned
  FXuint readUIntEntry(const FXchar *section,const FXchar *name,FXuint def=0) const;
  FXuint readUIntEntry(const FXString& section,const FXchar *name,FXuint def=0) const;
  FXuint readUIntEntry(const FXString& section,const FXString& name,FXuint def=0) const;

  /// Write a unsigned integer registry entry
  FXbool writeUIntEntry(const FXchar *section,const FXchar *name,FXuint val);
  FXbool writeUIntEntry(const FXString& section,const FXchar *name,FXuint val);
  FXbool writeUIntEntry(const FXString& section,const FXString& name,FXuint val);

  /// Read a 64-bit long integer registry entry; if no value is found, the default value def is returned
  FXlong readLongEntry(const FXchar *section,const FXchar *name,FXlong def=0) const;
  FXlong readLongEntry(const FXString& section,const FXchar *name,FXlong def=0) const;
  FXlong readLongEntry(const FXString& section,const FXString& name,FXlong def=0) const;

  /// Write a 64-bit long integer registry entry
  FXbool writeLongEntry(const FXchar *section,const FXchar *name,FXlong val);
  FXbool writeLongEntry(const FXString& section,const FXchar *name,FXlong val);
  FXbool writeLongEntry(const FXString& section,const FXString& name,FXlong val);

  /// Read a 64-bit unsigned long integer registry entry; if no value is found, the default value def is returned
  FXulong readULongEntry(const FXchar *section,const FXchar *name,FXulong def=0) const;
  FXulong readULongEntry(const FXString& section,const FXchar *name,FXulong def=0) const;
  FXulong readULongEntry(const FXString& section,const FXString& name,FXulong def=0) const;

  /// Write a 64-bit unsigned long integer registry entry
  FXbool writeULongEntry(const FXchar *section,const FXchar *name,FXulong val);
  FXbool writeULongEntry(const FXString& section,const FXchar *name,FXulong val);
  FXbool writeULongEntry(const FXString& section,const FXString& name,FXulong val);

  /// Read a double-precision floating point registry entry; if no value is found, the default value def is returned
  FXdouble readRealEntry(const FXchar *section,const FXchar *name,FXdouble def=0.0) const;
  FXdouble readRealEntry(const FXString& section,const FXchar *name,FXdouble def=0.0) const;
  FXdouble readRealEntry(const FXString& section,const FXString& name,FXdouble def=0.0) const;

  /// Write a double-precision floating point registry entry
  FXbool writeRealEntry(const FXchar *section,const FXchar *name,FXdouble val);
  FXbool writeRealEntry(const FXString& section,const FXchar *name,FXdouble val);
  FXbool writeRealEntry(const FXString& section,const FXString& name,FXdouble val);

  /// Read a color value registry entry; if no value is found, the default value def is returned
  FXColor readColorEntry(const FXchar *section,const FXchar *name,FXColor def=0) const;
  FXColor readColorEntry(const FXString& section,const FXchar *name,FXColor def=0) const;
  FXColor readColorEntry(const FXString& section,const FXString& name,FXColor def=0) const;

  /// Write a color value entry
  FXbool writeColorEntry(const FXchar *section,const FXchar *name,FXColor val);
  FXbool writeColorEntry(const FXString& section,const FXchar *name,FXColor val);
  FXbool writeColorEntry(const FXString& section,const FXString& name,FXColor val);

  /// Read a boolean registry entry
  FXbool readBoolEntry(const FXchar *section,const FXchar *name,FXbool def=false) const;
  FXbool readBoolEntry(const FXString& section,const FXchar *name,FXbool def=false) const;
  FXbool readBoolEntry(const FXString& section,const FXString& name,FXbool def=false) const;

  /// Write a boolean value entry
  FXbool writeBoolEntry(const FXchar *section,const FXchar *name,FXbool val);
  FXbool writeBoolEntry(const FXString& section,const FXchar *name,FXbool val);
  FXbool writeBoolEntry(const FXString& section,const FXString& name,FXbool val);

  /// See if entry exists
  FXbool existingEntry(const FXchar *section,const FXchar *name) const;
  FXbool existingEntry(const FXString& section,const FXchar *name) const;
  FXbool existingEntry(const FXString& section,const FXString& name) const;

  /// Delete a registry entry
  FXbool deleteEntry(const FXchar *section,const FXchar *name);
  FXbool deleteEntry(const FXString& section,const FXchar *name);
  FXbool deleteEntry(const FXString& section,const FXString& name);

  /// See if section exists
  FXbool existingSection(const FXchar *section) const;
  FXbool existingSection(const FXString& section) const;

  /// Delete section
  FXbool deleteSection(const FXchar *section);
  FXbool deleteSection(const FXString& section);

  /// Clear all sections
  FXbool clear();

  /// Cleanup
  virtual ~FXSettings();
  };

}

#endif
