/********************************************************************************
*                                                                               *
*                     D i r e c t o r y   V i s i t o r                         *
*                                                                               *
*********************************************************************************
* Copyright (C) 2008,2012 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "FXArray.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXString.h"
#include "FXStat.h"
#include "FXFile.h"
#include "FXPath.h"
#include "FXDir.h"
#include "FXDirVisitor.h"

/*
  Notes:
  - There should be three return value categories:
      rv > 0      Proceed scanning.
      rv = 0      Stop with scanning and return with 0.
      rv < 0      Skip scanning current directory or file.
  - Not all traversals are top-down.  Some might be upward.
*/


using namespace FX;

/*******************************************************************************/

namespace FX {


/*
FIXME
  three options:
  - continue traversing.
  - stop traversing.
  - skip file or directory during traversal.

  pass pattern, flags.  Test pattern first, then
  as below.

  - Need glob function.

  FXDirVisitor vis;
  fxTraceLevel=10;
  vis.traverse(argv[1]);
  exit(0);

  what if we want to see '.' and '..'??
*/

// Keep track of visited directories
struct FXDirVisitor::Seen {
  Seen   *next;
  FXlong  node;
  };


// Recurse, keeping track of where we've been already
FXuint FXDirVisitor::recurse(const FXString& path,Seen *seen){
  FXStat data;
  if(FXStat::statLink(path,data)){
    if(data.isDirectory()){
      for(Seen *s=seen; s; s=s->next){
        if(data.index()==s->node) return 1;
        }
      if(enter(path)){
        Seen here={seen,data.index()};
        FXDir directory(path);
        FXString name;
        while(directory.next(name)){
          if(!(name[0]=='.' && (name[1]==0 || (name[1]=='.' && name[2]==0)))){
            if(!recurse(path+(ISPATHSEP(path.tail())?"":PATHSEPSTRING)+name,&here)) break;
            }
          }
        return leave(path);
        }
      }
    else{
      return visit(path);
      }
    }
  return 0;
  }


// Recursively traverse starting from path
FXuint FXDirVisitor::traverse(const FXString& path){
  return recurse(path,NULL);
  }


// Handle directory
FXuint FXDirVisitor::enter(const FXString& path){
  FXTRACE((1,"enter(%s)\n",path.text()));
  return 1;
  }


// Handle file
FXuint FXDirVisitor::visit(const FXString& path){
  FXTRACE((1,"visit(%s)\n",path.text()));
  return 1;
  }


// Handle directory
FXuint FXDirVisitor::leave(const FXString& path){
  FXTRACE((1,"leave(%s)\n",path.text()));
  return 1;
  }


/*******************************************************************************/


// Recursively traverse starting from path
FXuint FXGlobVisitor::traverse(const FXString& path,const FXString& pat,FXuint flg){
  mode=(flg&FXDir::CaseFold)?(FXPath::PathName|FXPath::NoEscape|FXPath::CaseFold):(FXPath::PathName|FXPath::NoEscape);
  pattern=pat;
  flags=flg;
  return recurse(path,NULL);
  }


// Handle directory
FXuint FXGlobVisitor::enter(const FXString& path){
#ifdef WIN32
  return !(flags&FXDir::NoDirs) && ((flags&FXDir::HiddenDirs) || !FXStat::isHidden(path)) && ((flags&FXDir::AllDirs) || FXPath::match(path,pattern,mode));
#else
  return !(flags&FXDir::NoDirs) && ((flags&FXDir::HiddenDirs) || !FXPath::isHidden(path)) && ((flags&FXDir::AllDirs) || FXPath::match(path,pattern,mode));
#endif
  }


// Handle file
FXuint FXGlobVisitor::visit(const FXString& path){
#ifdef WIN32
  return !(flags&FXDir::NoFiles) && ((flags&FXDir::HiddenFiles) || !FXStat::isHidden(path)) && ((flags&FXDir::AllFiles) || FXPath::match(path,pattern,mode));
#else
  return !(flags&FXDir::NoFiles) && ((flags&FXDir::HiddenFiles) || !FXPath::isHidden(path)) && ((flags&FXDir::AllFiles) || FXPath::match(path,pattern,mode));
#endif
  }


}
