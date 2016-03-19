/********************************************************************************
*                                                                               *
*                     D i r e c t o r y   V i s i t o r                         *
*                                                                               *
*********************************************************************************
* Copyright (C) 2008,2010 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#ifndef FXDIRVISITOR_H
#define FXDIRVISITOR_H

namespace FX {


/**
* Directory visitor frequents all files and directories
* recursively, calling visit() for every file, and enter()/leave()
* for every directory.
*/
class FXAPI FXDirVisitor {
private:
  struct Seen;
protected:
  virtual FXuint recurse(const FXString& path,Seen *seen);
public:
  FXuint traverse(const FXString& path);
  virtual FXuint enter(const FXString& path);
  virtual FXuint visit(const FXString& path);
  virtual FXuint leave(const FXString& path);
  virtual ~FXDirVisitor(){}
  };

}

#endif
