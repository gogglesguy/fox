/********************************************************************************
*                                                                               *
*              G l o b b i n g   D i r e c t o r y   V i s i t o r              *
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
#ifndef FXGLOBVISITOR_H
#define FXGLOBVISITOR_H

namespace FX {


/**
* Visit directory entries according to flags and matching with wild card pattern,
* with similar matching conditions as FXDir::listFiles().
*/
class FXAPI FXGlobVisitor : public FXDirVisitor {
private:
  FXString wildcard;            // Match files against this wild card
  FXuint   options;             // Matching options
private:
  FXGlobVisitor(const FXGlobVisitor&);
  FXGlobVisitor& operator=(const FXGlobVisitor&);
public:

  /// Construct directory visitor
  FXGlobVisitor():options(0){}

  /// Start traversal at given path
  FXuint traverse(const FXString& path,const FXString& wild="*",FXuint opts=FXDir::MatchAll,FXint depth=1000);

  /// Enter directory; returns 1 if path matches criteria
  virtual FXuint enter(const FXString& path) override;

  /// Visit file; returns 1 if path matches criteria
  virtual FXuint visit(const FXString& path) override;

  /// Leave directory; always returns 1
  virtual FXuint leave(const FXString& path) override;

  /// Destructor
  virtual ~FXGlobVisitor();
  };


/**
* Visit directory entries according to flags and matching with wild card pattern,
* as per FXGlobVisitor above.
* In addition, count numbers of items visited, with an eye toward setting progress
* bar boundaries.
*/
class FXAPI FXGlobCountVisitor : public FXGlobVisitor {
private:
  FXlong countFolders;          // Count of folders seen
  FXlong countFiles;            // Count of files seen
  FXlong countBytes;            // Total number of bytes in files
  FXlong maxDepth;              // Maximum depth
  FXlong depth;                 // Current depth during traversal
private:
  FXGlobCountVisitor(const FXGlobCountVisitor&);
  FXGlobCountVisitor& operator=(const FXGlobCountVisitor&);
public:

  /// Create new glob counting visitor
  FXGlobCountVisitor();

  /// Start traversal of path
  FXuint traverse(const FXString& path,const FXString& wild="*",FXuint opts=FXDir::MatchAll,FXint limit=1000);

  /// Return total number of folders found
  FXlong getTotalFolders() const { return countFolders; }

  /// Return total number of files matched
  FXlong getTotalFiles() const { return countFiles; }

  /// Return total number of bytes in matching files
  FXlong getTotalBytes() const { return countBytes; }

  /// Return maximum depth of directory tree
  FXlong getMaximumDepth() const { return maxDepth; }

  /// Count directories
  virtual FXuint enter(const FXString& path) override;

  /// Count files
  virtual FXuint visit(const FXString& path) override;

  /// Count depth
  virtual FXuint leave(const FXString& path) override;

  /// Destructor
  virtual ~FXGlobCountVisitor();
  };

}

#endif
