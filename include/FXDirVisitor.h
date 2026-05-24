/********************************************************************************
*                                                                               *
*                     D i r e c t o r y   V i s i t o r                         *
*                                                                               *
*********************************************************************************
* Copyright (C) 2008,2026 by Jeroen van der Zijp.   All Rights Reserved.        *
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
* Directory visitor walks across all files and directories recursively,
* starting from the given path.
* If the path represents a directory, enter() is called to determine if
* the directory should be processed.  The visitor then recursively processes
* items in the directory, and concludes by calling leave() to indicate it
* has left that directory.
* If the path represents a regular file, visit() is called to process the
* file.
* Thus the functions are called in nested fashion, and each successful
* call to enter() is matched with a call to leave(), and calls to visit()
* are made for each regular file in between.
* To influence processing, the three API's can return any of the following values:
*
*   0   Skip the entry and proceed to the next item.
*   1   Continue processing the entry.
*   2   Abandon processing altogether and return from traverse().
*
* If processing is abandoned, the visitor closes off each unmatched enter()
* with a call to leave() and returns without visiting any further directories
* or files.
* The visitor automatically skips entries already visited or directories with
* unsufficient permissions.
* Recursion may be optionally limited with the limit parameter; setting limit
* equal to 1 traverses the given path only; a setting of 2 traverses the path
* and its immediate children, but stops there. A limit of 0 does nothing at
* all.
* The function info() returns information about the current file or directory
* being visited; valid only while in enter(), leave(), or visit() callbacks.
*/
class FXAPI FXDirVisitor {
private:
  struct Seen;
private:
  Seen* current;
private:
  FXDirVisitor(const FXDirVisitor&);
  FXDirVisitor& operator=(const FXDirVisitor&);
public:

  /// Initialize directory visitor
  FXDirVisitor():current(nullptr){}

  /// Start traversal at given path
  FXuint traverse(const FXString& path,FXint limit=1000);

  /// Return true if we're actively visiting directories
  FXbool visiting() const { return current!=nullptr; }

  /// Return stats on current file or directory
  const FXStat& info() const;

  /// Enter directory
  virtual FXuint enter(const FXString& path);

  /// Visit file
  virtual FXuint visit(const FXString& path);

  /// Leave directory
  virtual FXuint leave(const FXString& path);

  /// Destructor
  virtual ~FXDirVisitor();
  };

}

#endif
