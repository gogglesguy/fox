/********************************************************************************
*                                                                               *
*             D y n a m i c   L i n k   L i b r a r y   S u p p o r t           *
*                                                                               *
*********************************************************************************
* Copyright (C) 2002,2007 by Jeroen van der Zijp.   All Rights Reserved.        *
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
* $Id: FXDLL.h,v 1.23 2007/02/07 20:21:53 fox Exp $                             *
********************************************************************************/
#ifndef FXDLL_H
#define FXDLL_H

namespace FX {


/**
* Wrap library module handle to allow various operations
* on libraries to be performed.
*/
class FXAPI FXDLL {
private:
  void *hnd;
public:

  /// Default constructor
  FXDLL():hnd(NULL){}

  /// Constructor with handle
  FXDLL(void *h):hnd(h){}

  /// Copy constructor
  FXDLL(const FXDLL& d):hnd(d.hnd){}

  /// Return the name of the library module
  FXString name() const;

  /// Return library module handle
  void* handle() const { return hnd; }

  /// True if library was loaded
  FXbool loaded() const { return hnd!=NULL; }

  /// Load the library module from the given path
  FXbool load(const FXString& path);

  /// Unload the library module
  void unload();

  /// Return the address of the symbol in this library module
  void* address(const FXchar* sym) const;
  void* address(const FXString& sym) const;

  /// Return the symbol name of the given address
  static FXString symbol(void *addr);

  /// Return the name of the library module containing the address
  static FXString name(void *addr);

  /// Find DLL containing symbol
  static FXDLL dll(void* addr);

  /// Find DLL of ourselves
  static FXDLL dll();

  /// Return error message if error occurred loading the library module
  static FXString error();
  };


/**
* Wrap DLL handle but owns the handle; thus, the library module
* will automatically be unloaded when auto-dll is destroyed.
*/
class FXAPI FXAUTODLL : public FXDLL {
private:
  FXAUTODLL(const FXAUTODLL&);
  FXAUTODLL &operator=(const FXAUTODLL&);
public:

  /// Initialize by loading given library name
  FXAUTODLL(const FXString& name);

  /// Unload library if we have one
  ~FXAUTODLL();
  };


}

#endif

