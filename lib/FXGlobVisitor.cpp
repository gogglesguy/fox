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
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "fxmath.h"
#include "FXElement.h"
#include "FXMetaClass.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXString.h"
#include "FXIO.h"
#include "FXStat.h"
#include "FXPath.h"
#include "FXDir.h"
#include "FXDirVisitor.h"
#include "FXGlobVisitor.h"

/*
  Notes:

  - Recursively visit files and directories according to flags and wildcard
    pattern.

  - You typically override visit(), enter(), or leave() to perform some
    desired action, calling base FXGlobVisitor's version of these functions
    to determine if the file [visit(), or folder enter()/leave()] should
    be included.

  - Returning 0 from enter() causes FXGlobVisitor to skip the sub-directory;
    returning 2 causes FXGlobVisitor to return from all further processing.
    However, leave() will still be called while unwinding from the recursion.

  - A maximum recursion depth can be set to limit the amount of recursion
    to a certain number of levels; any folders deeper than this maximum
    will be skipped.
*/


using namespace FX;

/*******************************************************************************/

namespace FX {


// Recursively traverse starting from path
FXuint FXGlobVisitor::traverse(const FXString& path,const FXString& wild,FXuint opts,FXint depth){
  wildcard=wild;
  options=opts;
  return FXDirVisitor::traverse(path,depth);
  }


// Enter directory; returns 1 if path matches criteria
FXuint FXGlobVisitor::enter(const FXString& path){
  FXuint matching=(options&FXDir::CaseFold)?(FXPath::NoEscape|FXPath::CaseFold):(FXPath::NoEscape);
#ifdef WIN32
  return !(options&FXDir::NoDirs) && ((options&FXDir::HiddenDirs) || !FXStat::isHidden(path)) && ((options&FXDir::AllDirs) || FXPath::match(path,wildcard,matching));
#else
  return !(options&FXDir::NoDirs) && ((options&FXDir::HiddenDirs) || !FXPath::isHidden(path)) && ((options&FXDir::AllDirs) || FXPath::match(path,wildcard,matching));
#endif
  }


// Handle file; returns 1 if path matches criteria
FXuint FXGlobVisitor::visit(const FXString& path){
  FXuint matching=(options&FXDir::CaseFold)?(FXPath::NoEscape|FXPath::CaseFold):(FXPath::NoEscape);
#ifdef WIN32
  return !(options&FXDir::NoFiles) && ((options&FXDir::HiddenFiles) || !FXStat::isHidden(path)) && ((options&FXDir::AllFiles) || FXPath::match(path,wildcard,matching));
#else
  return !(options&FXDir::NoFiles) && ((options&FXDir::HiddenFiles) || !FXPath::isHidden(path)) && ((options&FXDir::AllFiles) || FXPath::match(path,wildcard,matching));
#endif
  }


// Leave directory; always returns 1
FXuint FXGlobVisitor::leave(const FXString&){
  return 1;
  }


// Destructor
FXGlobVisitor::~FXGlobVisitor(){
  }


/*******************************************************************************/


// Create new glob counting visitor
FXGlobCountVisitor::FXGlobCountVisitor():countFolders(0),countFiles(0),countBytes(0),maxDepth(0),depth(0){
  }


// Start traversal of path
FXuint FXGlobCountVisitor::traverse(const FXString& path,const FXString& wild,FXuint opts,FXint limit){
  countFolders=countFiles=countBytes=maxDepth=depth=0;
  return FXGlobVisitor::traverse(path,wild,opts,limit);
  }


// Enter directory
FXuint FXGlobCountVisitor::enter(const FXString& path){
  if(FXGlobVisitor::enter(path)){
    countFolders++;
    depth++;
    return 1;
    }
  return 0;
  }


// He mister tally man, tally me banana...
FXuint FXGlobCountVisitor::visit(const FXString& path){
  if(FXGlobVisitor::visit(path)){
    countBytes+=info().size();
    countFiles++;
    return 1;
    }
  return 0;
  }


// Leave directory
FXuint FXGlobCountVisitor::leave(const FXString& path){
  if(FXGlobVisitor::leave(path)){
    maxDepth=FXMAX(maxDepth,depth);
    depth--;
    return 1;
    }
  return 0;
  }


// Destructor
FXGlobCountVisitor::~FXGlobCountVisitor(){
  }

}
