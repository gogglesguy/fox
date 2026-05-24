/********************************************************************************
*                                                                               *
*                  P a t h   N a m e   M a n i p u l a t i o n                  *
*                                                                               *
*********************************************************************************
* Copyright (C) 2000,2025 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "fxchar.h"
#include "fxascii.h"
#include "fxunicode.h"
#include "FXElement.h"
#include "FXArray.h"
#include "FXMetaClass.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXString.h"
#include "FXPath.h"
#include "FXSystem.h"
#include "FXIO.h"
#include "FXStat.h"
#include "FXFile.h"
#include "FXDir.h"
#if defined(WIN32)
#include <shellapi.h>
#endif


/*
  Notes:

  - Windows 95 and NT:
      -  1 to 255 character name.
      -  Complete path for a file or project name cannot exceed 259
         characters, including the separators.
      -  May not begin or end with a space.
      -  May not begin with a $
      -  May contain 1 or more file extensions (eg. MyFile.Ext1.Ext2.Ext3.Txt).
      -  Legal characters in the range of 32 - 255 but not ?"/\<>*|:
      -  Filenames may be mixed case.
      -  Filename comparisons are case insensitive (eg. ThIs.TXT = this.txt).

  - MS-DOS and Windows 3.1:
      -  1 to 11 characters in the 8.3 naming convention.
      -  Legal characters are A-Z, 0-9, Double Byte Character Set (DBCS)
         characters (128 - 255), and _^$~!#%&-{}@'()
      -  May not contain spaces, 0 - 31, and "/\[]:;|=,
      -  Must not begin with $
      -  Uppercase only filename.

  - Some examples of Windows paths:

    "C:\temp\file.txt"                   File "file.txt" in folder "temp" on drive "C:".
    "\\Server\Share\Test\Foo.txt"        File "Foo.txt" in "Test" on "Share" of "Server".
    "\\Server\C$\"                       Root directory "\" on drive "C:" on "Server".
    "\\127.0.0.1\C$\temp\file.txt"       You can see there may be dots in server name!!
    "\\LOCALHOST\C$\temp\file.txt"       Special "\\LOCALHOST\C$" maps to "C:".
    "\\.\UNC\LOCALHOST\C$\temp\file.txt" This is just nuts.
    "\\.\C:\temp\file.txt"               DOS device path.
    "\\?\C:\temp\file.txt"               DOS device path.
    "\\.\CON"                            Devices like LPT1, COM1, etc.

  - Paths "\\?\" are supposedly long pathnames on Windows.

  - On Windows environment variables:

      - Variables have a percent sign on both sides: %ThisIsAVariable%.

      - Name can include spaces, punctuation and mixed case: %_Another Ex.ample%.

      - A variable name may include any of the following characters:

          A-Z, a-z, 0-9, # $ ' ( ) * + , - . ? @ [ ] _ ` { } ~

      - The first character of the name must not be numeric.
*/

using namespace FX;

/*******************************************************************************/

namespace FX {


#if defined(WIN32)

// Return root of absolute path.
// On Windows, this is of the form:
//
//     "\",
//     "C:\",
//     "\\Server\Share\",
//
// for paths starting with these strings.
// The empty string is returned if the path is not absolute.
FXString FXPath::root(const FXString& file){
  FXint p=0,s;
  if(Ascii::isLetter(file[0]) && file[1]==':'){
    p=2;
    }
  else if(file[0]==PATHSEP && file[1]==PATHSEP && file[2]!=PATHSEP && file[2]){
    s=3;
    while(file[s] && file[s]!=PATHSEP) s++;
    if(file[s]==PATHSEP){
      s++;
      if(file[s] && file[s]!=PATHSEP){
        s++;
        while(file[s] && file[s]!=PATHSEP) s++;
        p=s;
        }
      }
    }
  if(file[p]==PATHSEP){
    return FXString(&file[0],p+1);
    }
  return FXString::null;
  }


// Return share name part from Windows UNC filename.
// For example, the share() of "\\Server\Share\Document.doc"
// yields "\\Server\Share".
// The empty string is returned if path is not a network share.
FXString FXPath::share(const FXString& file){
  if(file[0]==PATHSEP && file[1]==PATHSEP && file[2]!=PATHSEP && file[2]){
    FXint s=3;
    while(file[s] && file[s]!=PATHSEP) s++;
    if(file[s]==PATHSEP){
      s++;
      if(file[s] && file[s]!=PATHSEP){
        s++;
        while(file[s] && file[s]!=PATHSEP) s++;
        return FXString(&file[0],s);
        }
      }
    }
  return FXString::null;
  }


// Return server part from Windows UNC filename.
// For example, the server() of "\\Server\Share\Document.doc"
// yields "\\Server".
// The empty string is returned if path is not a network share.
FXString FXPath::server(const FXString& file){
  if(file[0]==PATHSEP && file[1]==PATHSEP && file[2]!=PATHSEP && file[2]){
    FXint s=3;
    while(file[s] && file[s]!=PATHSEP) s++;
    return FXString(&file[0],s);
    }
  return FXString::null;
  }


// Return the drive letter in front of file name.
// Returns empty string if no drive letter present.
FXString FXPath::drive(const FXString& file){
  if(Ascii::isLetter(file[0]) && file[1]==':'){
    return FXString(&file[0],2);
    }
  return FXString::null;
  }


// Return true if path is the form "\\Server\Share".
FXbool FXPath::isShare(const FXString& file){
  if(file[0]==PATHSEP && file[1]==PATHSEP && file[2]!=PATHSEP && file[2]){
    FXint s=3;
    while(file[s] && file[s]!=PATHSEP) s++;
    if(file[s]==PATHSEP){
      s++;
      if(file[s] && file[s]!=PATHSEP){
        s++;
        while(file[s] && file[s]!=PATHSEP) s++;
        return (file[s]=='\0');
        }
      }
    }
  return false;
  }


// Return true if path is of the form "\\Server".
FXbool FXPath::isServer(const FXString& file){
  if(file[0]==PATHSEP && file[1]==PATHSEP && file[2]!=PATHSEP && file[2]){
    FXint s=3;
    while(file[s] && file[s]!=PATHSEP) s++;
    return (file[s]=='\0');
    }
  return false;
  }

#else

// Return root of absolute path; on Unix, this is just "/".
// The empty string is returned if the path is not absolute.
FXString FXPath::root(const FXString& file){
  if(file[0]==PATHSEP){
    return PATHSEPSTRING;
    }
  return FXString::null;
  }


// Return share name part of filename.
// Always empty string on Linux.
FXString FXPath::share(const FXString&){
  return FXString::null;
  }


// Return server name part filename.
// Always empty string on Linux.
FXString FXPath::server(const FXString&){
  return FXString::null;
  }


// Return drive letter prefix "C:".
// Always empty string on Linux.
FXString FXPath::drive(const FXString&){
  return FXString::null;
  }


// Check if file represents a file share
// Always false on Linux.
FXbool FXPath::isShare(const FXString&){
  return false;
  }


// Return true if input path is a file server
// Always false on Linux.
FXbool FXPath::isServer(const FXString&){
  return false;
  }

#endif

/*******************************************************************************/

// Return directory part of pathname, assuming full pathname.
// Note that directory("/bla/bla/") is "/bla/bla" and NOT "/bla".
// However, directory("/bla/bla") is "/bla" as we expect!
// The "\\Server\Share\", "C:\" or "\" ("/" for Linux) are the
// top of that path's file system, and will not be dropped.
FXString FXPath::directory(const FXString& file){
  if(!file.empty()){
    FXint p=0,s;
#if defined(WIN32)
    if(Ascii::isLetter(file[0]) && file[1]==':'){
      p=2;
      }
    else if(file[0]==PATHSEP && file[1]==PATHSEP && file[2]!=PATHSEP && file[2]){
      s=3;
      while(file[s] && file[s]!=PATHSEP) s++;
      if(file[s]==PATHSEP){
        s++;
        if(file[s] && file[s]!=PATHSEP){
          s++;
          while(file[s] && file[s]!=PATHSEP) s++;
          p=s;
          }
        }
      }
#endif
    if(file[p]==PATHSEP) p++;
    s=p;
    while(file[p]){
      if(file[p]==PATHSEP){ s=p++; continue; }
      p++;
      }
    return FXString(&file[0],s);
    }
  return FXString::null;
  }

/*******************************************************************************/

// Return name and extension part of pathname.
// Note that name("/bla/bla/") is "" and NOT "bla".
// However, name("/bla/bla") is "bla" as we expect!
FXString FXPath::name(const FXString& file){
  if(!file.empty()){
    FXint p=0,s;
#if defined(WIN32)
    if(Ascii::isLetter(file[0]) && file[1]==':'){
      p=2;
      }
    else if(file[0]==PATHSEP && file[1]==PATHSEP && file[2]!=PATHSEP && file[2]){
      s=3;
      while(file[s] && file[s]!=PATHSEP) s++;
      if(file[s]==PATHSEP){
        s++;
        if(file[s] && file[s]!=PATHSEP){
          s++;
          while(file[s] && file[s]!=PATHSEP) s++;
          p=s;
          }
        }
      }
#endif
    s=p;
    while(file[s]){
      if(file[s]==PATHSEP){ p=++s; continue; }
      ++s;
      }
    return FXString(&file[p],s-p);
    }
  return FXString::null;
  }

/*******************************************************************************/

// Return file stem, i.e. document name only:
//
//  /path/aa        -> aa
//  /path/aa.bb     -> aa
//  /path/aa.bb.cc  -> aa.bb
//  /path/.aa       -> .aa
FXString FXPath::stem(const FXString& file){
  if(!file.empty()){
    FXint p=0,s,b,e;
#if defined(WIN32)
    if(Ascii::isLetter(file[0]) && file[1]==':'){
      p=2;
      }
    else if(file[0]==PATHSEP && file[1]==PATHSEP && file[2]!=PATHSEP && file[2]){
      s=3;
      while(file[s] && file[s]!=PATHSEP) s++;
      if(file[s]==PATHSEP){
        s++;
        if(file[s] && file[s]!=PATHSEP){
          s++;
          while(file[s] && file[s]!=PATHSEP) s++;
          p=s;
          }
        }
      }
#endif
    s=p;
    while(file[s]){
      if(file[s]==PATHSEP){ p=++s; continue; }
      ++s;
      }
    b=p;
    e=s;
    if(file[p]=='.') p++;     // Stuff following leading '.' is NOT an extension
    while(p<s){
      if(file[--s]=='.'){ e=s; break; }
      }
    return FXString(&file[b],e-b);
    }
  return FXString::null;
  }

/*******************************************************************************/

// Return extension, if there is one:
//
//  /path/aa        -> ""
//  /path/aa.bb     -> bb
//  /path/aa.bb.cc  -> cc
//  /path/.aa       -> ""
FXString FXPath::extension(const FXString& file){
  if(!file.empty()){
    FXint p=0,s,b,e;
#if defined(WIN32)
    if(Ascii::isLetter(file[0]) && file[1]==':'){
      p=2;
      }
    else if(file[0]==PATHSEP && file[1]==PATHSEP && file[2]!=PATHSEP && file[2]){
      s=3;
      while(file[s] && file[s]!=PATHSEP) s++;
      if(file[s]==PATHSEP){
        s++;
        if(file[s] && file[s]!=PATHSEP){
          s++;
          while(file[s] && file[s]!=PATHSEP) s++;
          p=s;
          }
        }
      }
#endif
    s=p;
    while(file[s]){
      if(file[s]==PATHSEP){ p=++s; continue; }
      ++s;
      }
    b=e=s;
    if(file[p]=='.') p++;     // Stuff following leading '.' is NOT an extension
    while(p<s){
      if(file[--s]=='.'){ b=s+1; break; }
      }
    return FXString(&file[b],e-b);
    }
  return FXString::null;
  }

/*******************************************************************************/

// Return file name less the extension
//
//  /path/aa        -> /path/aa
//  /path/aa.bb     -> /path/aa
//  /path/aa.bb.cc  -> /path/aa.bb
//  /path/.aa       -> /path/.aa
FXString FXPath::stripExtension(const FXString& file){
  if(!file.empty()){
    FXint p=0,s,e;
#if defined(WIN32)
    if(Ascii::isLetter(file[0]) && file[1]==':'){
      p=2;
      }
    else if(file[0]==PATHSEP && file[1]==PATHSEP && file[2]!=PATHSEP && file[2]){
      s=3;
      while(file[s] && file[s]!=PATHSEP) s++;
      if(file[s]==PATHSEP){
        s++;
        if(file[s] && file[s]!=PATHSEP){
          s++;
          while(file[s] && file[s]!=PATHSEP) s++;
          p=s;
          }
        }
      }
#endif
    s=p;
    while(file[s]){
      if(file[s]==PATHSEP){ p=++s; continue; }
      ++s;
      }
    e=s;
    if(file[p]=='.') p++;     // Stuff following leading '.' is NOT an extension
    while(p<s){
      if(file[--s]=='.'){ e=s; break; }
      }
    return FXString(&file[0],e);
    }
  return FXString::null;
  }

/*******************************************************************************/

// Expand environment variables recursively, stopping when recursion goes too deep
static FXString expandEnvironmentVariables(const FXString& file,FXint level){
  FXint s=0,r=0,b=0,e=0;
  FXString result;
#if defined(WIN32)
  while(e<file.length()){
    r=e;
    if(file[e++]=='%'){       // %VAR%
      b=e;
      while(Ascii::isWord(file[e])) e++;
      if(file[e]=='%' && b<e && 0<level){
        result.append(&file[s],r-s);
        result.append(expandEnvironmentVariables(FXSystem::getEnvironment(file.mid(b,e-b)),level-1));
        s=++e;
        }
      }
    }
  result.append(&file[s],e-s);
#else
  while(e<file.length()){
    r=e;
    if(file[e++]=='$'){
      b=e;
      if(file[e]=='{'){       // ${VAR}...
        b=++e;
        while(Ascii::isWord(file[e])) e++;
        if(file[e]=='}' && b<e && 0<level){
          result.append(&file[s],r-s);
          result.append(expandEnvironmentVariables(FXSystem::getEnvironment(file.mid(b,e-b)),level-1));
          s=++e;
          }
        }
      else{                   // $VAR...
        while(Ascii::isWord(file[e])) e++;
        if(b<e && 0<level){
          result.append(&file[s],r-s);
          result.append(expandEnvironmentVariables(FXSystem::getEnvironment(file.mid(b,e-b)),level-1));
          s=e;
          }
        }
      }
    }
  result.append(&file[s],e-s);
#endif
  return result;
  }


// Perform tilde or environment variable expansion.
// A prefix of the form "~" or "~user" is expanded to the (current) user's
// home directory.
// Environment variables of the form $VAR or ${VAR} (or %VAR% on Windows)
// are expanded by substituting the value of the variable, recusively up to
// given maximum level.
// On Windows, only environment variables are expanded.
FXString FXPath::expand(const FXString& file,FXint level){
  if(!file.empty()){
    FXString result;
#if defined(WIN32)
    result=expandEnvironmentVariables(file,level);
    return result;
#else
    if(file[0]=='~'){
      FXint e=1;
      while(file[e] && file[e]!=PATHSEP){
        if(file[e]=='\\' && file[e+1]) e++;
        e++;
        }
      if(1<e){
        result=FXSystem::getUserDirectory(file.mid(1,e-1));
        if(e<file.length()){
          result.append(expandEnvironmentVariables(file.mid(e,file.length()-e),level));
          }
        return result;
        }
      result=FXSystem::getHomeDirectory();
      if(e<file.length()){
        result.append(expandEnvironmentVariables(file.mid(e,file.length()-e),level));
        }
      return result;
      }
    result=expandEnvironmentVariables(file,level);
    return result;
#endif
    }
  return FXString::null;
  }

/*******************************************************************************/

// Convert a UNIX path to Windows conventions:
//
//  - Replace "/" with "\" for path separator.
//  - Replace "${ENVVAR}" or "$ENVVAR" with "%ENVVAR%".
//
// Also, if path starting with a single "/", prefix it by:
//
//  - Drive letter "C:" from the current working directory.
//  - Share name "\\Server\Share" from the current working directory.
//
FXString FXPath::convertToWindows(const FXString& path){
  FXString result;
  if(!path.empty()){
    FXint p=0,q=0,s=0;
    if(path[0]=='/' && path[1]!='/'){                           // Not fully qualified absolute
      result=FXSystem::getCurrentDirectory();
      if(Ascii::isLetter(result[0]) && result[1]==':'){
        p=2;                                                    // Prefix "C:"
        }
      else if(result[0]=='\\' && result[1]=='\\' && result[2]!='\\' && result[2]){
        s=3;
        while(result[s] && result[s]!='\\') s++;
        if(result[s]=='\\'){
          s++;
          if(result[s] && result[s]!='\\'){
            s++;
            while(result[s] && result[s]!='\\') s++;
            p=s;                                                // Prefix "\\Server\Share"
            }
          }
        }
      }
    if(result.length(p+path.length()+path.contains('$'))){      // Size estimate
      while(path[q]){
        if(path[q]=='/'){                                       // Path separator
          result[p++]='\\';
          q++;
          continue;
          }
        if(path[q]=='$'){                                       // Possible variable
          s=q+1;
          if(path[s]=='{'){
            s++;
            while(Ascii::isWord(path[s])) s++;
            if(path[s]=='}' && q+2<s){                          // Non-empty ${VARIABLE}
              result[p++]='%';
              q+=2;
              do{
                result[p++]=path[q++];
                }
              while(q<s);
              result[p++]='%';
              q++;
              continue;
              }
            }
          else{
            while(Ascii::isWord(path[s])) s++;
            if(q+1<s){                                          // Non-empty $VARIABLE
              result[p++]='%';
              q+=1;
              do{
                result[p++]=path[q++];
                }
              while(q<s);
              result[p++]='%';
              continue;
              }
            }
          }
        result[p++]=path[q++];                                  // Normal character
        }
      FXASSERT(p<=result.length());
      result.trunc(p);
      }
    }
  return result;
  }


// Convert Windows path to UNIX conventions:
//
//  - Replace "\" with "/" for path separator.
//  - Replace "%ENVVAR%" with "${ENVVAR}".
//
// If prefixed by drive letter "C:" or share name "\\Server\Share":
//
//  - Drop drive letter "C:" from the front.
//  - Drop share name "\\Server\Share" from the front.
//
FXString FXPath::convertFromWindows(const FXString& path){
  FXString result;
  if(!path.empty()){
    if(result.length(path.length()+path.contains('%')/2)){      // Size estimate
      FXint p=0,q=0,s;
      if(Ascii::isLetter(path[0]) && path[1]==':'){
        q=2;                                                    // Strip "C:"
        }
      else if(path[0]=='\\' && path[1]=='\\' && path[2]!='\\' && path[2]){
        s=3;
        while(path[s] && path[s]!='\\') s++;
        if(path[s]=='\\'){
          s++;
          if(path[s] && path[s]!='\\'){
            s++;
            while(path[s] && path[s]!='\\') s++;
            q=s;                                                // Strip "\\Server\Share"
            }
          }
        }
      while(path[q]){
        if(path[q]=='\\'){                                      // Path separator
          result[p++]='/';
          q++;
          continue;
          }
        if(path[q]=='%'){                                       // Environment variable
          s=q+1;
          while(Ascii::isWord(path[s])) s++;
          if(path[s]=='%' && q+1<s){                            // Non-empty %VARIABLE%
            result[p++]='$';
            result[p++]='{';
            q+=1;
            do{
              result[p++]=path[q++];
              }
            while(q<s);
            result[p++]='}';
            q++;
            continue;
            }
          }
        result[p++]=path[q++];                                  // Normal character
        }
      FXASSERT(p<=result.length());
      result.trunc(p);
      }
    }
  return result;
  }


// Convert a path to local conventions
FXString FXPath::convert(const FXString& path){
#if defined(WIN32)
  return FXPath::convertToWindows(path);
#else
  return FXPath::convertFromWindows(path);
#endif
  }


// Convert a path-list to local conventions
FXString FXPath::convertPathList(const FXString& path){
  FXString result;
  if(!path.empty()){
    FXint b=0,e=0;
    while(e<path.length()){
#if defined(WIN32)
      while(e<path.length() && path[e]!=':') e++;
      result+=FXPath::convertToWindows(path.mid(b,e-b));
      if(path[e]==':'){ result+=';'; e++; }
      b=e;
#else
      while(e<path.length() && path[e]!=';') e++;
      result+=FXPath::convertFromWindows(path.mid(b,e-b));
      if(path[e]==';'){ result+=':'; e++; }
      b=e;
#endif
      }
    }
  return result;
  }

/*******************************************************************************/

// Contract path based on an environment variable.
//
// For example, on UNIX:
//
//   "/home/jeroen/junk"                -> "~/junk"
//   "/home/someoneelse/junk"           -> "~someoneelse/junk"
//   "/usr/local/ACE_wrappers/TAO"      -> "$ACE_ROOT/TAO"
//
// On Windows:
//
//   "C:\usr\local\ACE_wrappers\TAO"    -> "%ACE_ROOT%\TAO"
//
FXString FXPath::contract(const FXString& file,const FXString& user,const FXString& var){
  const FXchar legalcharacters[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_";
  if(!file.empty()){
#if defined(WIN32)
    FXString result(file);
    if(!var.empty() && var.find_first_not_of(legalcharacters)<0){
      FXString val=FXSystem::getEnvironment(var);
      if(!val.empty()){
        FXint pos=result.find(val);
        if(0<=pos){
          if((pos==0 || result[pos-1]==PATHSEP) && (pos+val.length()==result.length() || result[pos+val.length()]==PATHSEP)){
            result.replace(pos,val.length(),"%"+var+"%");
            }
          }
        }
      }
    return result;
#else
    FXString result(file);
    if(FXPath::isAbsolute(result)){
      FXString dir=FXSystem::getUserDirectory(user);
      if(!dir.empty()){
        if(FXString::compare(result,dir,dir.length())==0 && (dir.length()==result.length() || result[dir.length()]==PATHSEP)){
          result.replace(0,dir.length(),"~"+user);
          }
        }
      }
    if(!var.empty() && var.find_first_not_of(legalcharacters)<0){
      FXString val=FXSystem::getEnvironment(var);
      if(!val.empty()){
        FXint pos=result.find(val);
        if(0<=pos){
          if((pos==0 || result[pos-1]==PATHSEP) && (pos+val.length()==result.length() || result[pos+val.length()]==PATHSEP)){
            result.replace(pos,val.length(),"$"+var);
            }
          }
        }
      }
    return result;
#endif
    }
  return FXString::null;
  }

/*******************************************************************************/

// Simplify a file path; the path will remain relative if it was relative,
// or absolute if it was absolute.  Also, a trailing "/" will be preserved
// as this is important in other functions.  Finally, returned path should
// be non-empty unless the input path was empty, and pathological paths
// will be fixed.
// The result is a shorter, canonical form of the original path.
//
// Examples:
//
//    /aa/bb/../cc      -> /aa/cc
//    /aa/bb/../cc/     -> /aa/cc/
//    /aa/bb/../..      -> /
//    /..               -> /
//    ./aa/bb/../../    -> ./
//    ./aa/bb/../.."    -> .
//    ./aa/bb/../../../ -> ../
//    ./aa/bb/../../..  -> ..
//    /aa/bb/../../../  -> /
//    /aa/bb/../../..   -> /
//    a/..              -> .
//    a/../..           -> ..
//    a/../             -> ./
//    /aa/ccc/../../bb  -> /bb
//    ./a               -> a
//    /////./././       -> /
//    /.                -> /
//    ./                -> ./
//    /a/b/./           -> /a/b/
//    /a/b/.            -> /a/b
//    /a/./b/.          -> /a/b
//    /a/./b/./         -> /a/b/
//    ./..              -> ..
//    /aa/bb/..         -> /aa
//    ../..             -> ../..
//    ../a/../b         -> ../b
//    a/../b            -> b
//    a/b/../../../     -> ../
//    /a/b/../../../    -> /
//    ../a/./b          -> ../a/b
//    /a/.              -> /a
//    /a../c/.          -> /a../c
//    /..a/c/.          -> /..a/c
//    ../../c/.         -> ../../c
//
FXString FXPath::simplify(const FXString& file){
  if(!file.empty()){
    FXString result(file);
    FXint components[64];
    FXint c=0,p=0,q=0,s;
#if defined(WIN32)
    if(Ascii::isLetter(result[q]) && result[q+1]==':'){
      result[p++]=result[q];
      result[p++]=':';
      q+=2;
      }
    else if(result[q]==PATHSEP && result[q+1]==PATHSEP){
      result[p++]=PATHSEP;
      result[p++]=PATHSEP;
      q+=2;
      if(result[q] && result[q]!=PATHSEP){
        while(result[q] && result[q]!=PATHSEP){
          result[p++]=result[q++];
          }
        if(result[q]==PATHSEP){
          result[p++]=PATHSEP;
          q++;
          if(result[q] && result[q]!=PATHSEP){
            while(result[q] && result[q]!=PATHSEP){
              result[p++]=result[q++];
              }
            }
          }
        }
      }
#endif
    if(result[q]==PATHSEP){                     // Keep root
      while(result[q]==PATHSEP) q++;            // Eat duplicate path separators
      result[p++]=PATHSEP;
      }
    s=p;                                        // We can not back up past this point
    while(result[q]){
      if(result[q]=='.'){
        if(result[q+1]=='\0'){                  // '.'
          q++;
          if(s<p && result[p-1]==PATHSEP){      // Back up over '/' if not first
            p--;
            }
          if(p==0){                             // Output '.' if it would be empty otherwise
            result[p++]='.';
            }
          continue;
          }
        if(result[q+1]==PATHSEP){               // './'
          q+=2;
          while(result[q]==PATHSEP) q++;        // Eat duplicate path separators
          if(p==0 && result[q]=='\0'){          // Output './' if it would be empty otherwise
            result[p++]='.';
            result[p++]=PATHSEP;
            }
          continue;                             // Otherwise, eat the './'
          }
        if(result[q+1]=='.'){
          if(result[q+2]=='\0'){                // '..'
            q+=2;
            if(c==0){                           // No prior path component
              if(s) continue;                   // Pathological: can't go above root
              result[p++]='.';                  // Leading '..'
              result[p++]='.';
              continue;
              }
            p=components[--c];                  // Reset to last-seen component
            if(s<p && result[p-1]==PATHSEP){    // Back up over '/' if not first
              p--;
              }
            if(p==0){                           // Output '.' if it would be empty otherwise
              result[p++]='.';
              }
            continue;
            }
          if(result[q+2]==PATHSEP){             // '../'
            q+=3;
            while(result[q]==PATHSEP) q++;      // Eat duplicate path separators
            if(c==0){                           // No prior path component
              if(s) continue;                   // Pathological: can't go above root
              result[p++]='.';                  // Leading '../'
              result[p++]='.';
              result[p++]=PATHSEP;
              continue;
              }
            p=components[--c];                  // Reset to last-seen component
            if(p==0 && result[q]=='\0'){        // Output './' if it would be empty otherwise
              result[p++]='.';
              result[p++]=PATHSEP;
              }
            continue;                           // Otherwise, eat the '../'
            }
          }
        }
      if(__unlikely(c>=64)) return file;        // Insanely many components (not simplified)
      components[c++]=p;                        // Remember backup point
      while(result[q] && result[q]!=PATHSEP){   // Advance to end of component
        result[p++]=result[q++];
        }
      if(result[q]==PATHSEP){                   // A path separator
        while(result[q]==PATHSEP) q++;          // Eat duplicate path separators
        result[p++]=PATHSEP;                    // Copy it
        }
      }
    return result.trunc(p);                     // Reached the end
    }
  return FXString::null;
  }

/*******************************************************************************/

#if defined(WIN32)

// Check if file represents absolute pathname
//
// On Windows, isAbsolute() returns true for paths of the form:
//
//   "\"
//   "C:\"
//   "\\Server\Share\"
//
FXbool FXPath::isAbsolute(const FXString& file){
  FXint p=0,s;
  if(Ascii::isLetter(file[0]) && file[1]==':'){
    p=2;
    }
  else if(file[0]==PATHSEP && file[1]==PATHSEP && file[2]!=PATHSEP && file[2]){
    s=3;
    while(file[s] && file[s]!=PATHSEP) s++;
    if(file[s]==PATHSEP){
      s++;
      if(file[s] && file[s]!=PATHSEP){
        s++;
        while(file[s] && file[s]!=PATHSEP) s++;
        p=s;
        }
      }
    }
  return (file[p]==PATHSEP);
  }


// Fully qualified absolute, i.e. has "C:\" or "\\Server\Share\"
static inline FXbool fq(const FXString& file){
  if(Ascii::isLetter(file[0]) && file[1]==':'){
    return (file[2]==PATHSEP);
    }
  if(file[0]==PATHSEP && file[1]==PATHSEP && file[2]!=PATHSEP && file[2]){
    FXint s=3;
    while(file[s] && file[s]!=PATHSEP) s++;
    if(file[s]==PATHSEP){
      s++;
      if(file[s] && file[s]!=PATHSEP){
        s++;
        while(file[s] && file[s]!=PATHSEP) s++;
        return (file[s]==PATHSEP);
        }
      }
    }
  return false;
  }


// Return absolute path name.
// If file started with "\" but is not fully qualified i.e. has
// no drive letter ["C:"] or file share prefix ["\\Server\Share"],
// then just prefix the current directory's root in front of the
// file to complete it.
// Otherwise, place the current directory in front, and simplify
// the result to obtain canonical path.
FXString FXPath::absolute(const FXString& file){
  if(!fq(file)){
    FXString result(FXSystem::getCurrentDirectory());
    if(file[0]==PATHSEP && file[1]!=PATHSEP){
      if(Ascii::isLetter(result[0]) && result[1]==':'){
        result.trunc(2);
        }
      else if(result[0]==PATHSEP && result[1]==PATHSEP && result[2]!=PATHSEP && result[2]){
        FXint p=3;
        while(result[p] && result[p]!=PATHSEP) p++;
        if(result[p]==PATHSEP){
          p++;
          if(result[p] && result[p]!=PATHSEP){
            p++;
            while(result[p] && result[p]!=PATHSEP) p++;
            result.trunc(p);
            }
          }
        }
      }
    if(!file.empty()){
      if(result.tail()!=PATHSEP) result.append(PATHSEP);
      result.append(file);
      }
    return FXPath::simplify(result);
    }
  return FXPath::simplify(file);
  }


// Return absolute path from base directory and file name.
// If file started with "\" but is not fully qualified i.e. has
// no drive letter ["C:"] or file share prefix ["\\Server\Share"],
// then just prefix the base-directory's root in front of the
// file to complete it.
// Otherwise, place the base-directory in front, and simplify
// the result to obtain canonical path.
FXString FXPath::absolute(const FXString& base,const FXString& file){
  if(!fq(file)){
    FXString result(FXPath::absolute(base));
    if(file[0]==PATHSEP && file[1]!=PATHSEP){
      if(Ascii::isLetter(result[0]) && result[1]==':'){
        result.trunc(2);
        }
      else if(result[0]==PATHSEP && result[1]==PATHSEP && result[2]!=PATHSEP && result[2]){
        FXint p=3;
        while(result[p] && result[p]!=PATHSEP) p++;
        if(result[p]==PATHSEP){
          p++;
          if(result[p] && result[p]!=PATHSEP){
            p++;
            while(result[p] && result[p]!=PATHSEP) p++;
            result.trunc(p);
            }
          }
        }
      }
    if(!file.empty()){
      if(result.tail()!=PATHSEP) result.append(PATHSEP);
      result.append(file);
      }
    return FXPath::simplify(result);
    }
  return FXPath::simplify(file);
  }

#else

// Check if file represents absolute pathname
//
// On Linux, isAbsolute() returns true for paths of the form:
//
//   "/"
//
FXbool FXPath::isAbsolute(const FXString& file){
  return file[0]==PATHSEP;
  }


// Return absolute path name
// If file is not absolute, place the current directory in
// front, and simplify the result to obtain canonical path.
FXString FXPath::absolute(const FXString& file){
  if(file[0]!=PATHSEP){
    FXString result(FXSystem::getCurrentDirectory());
    if(!file.empty()){
      if(result.tail()!=PATHSEP) result.append(PATHSEP);
      result.append(file);
      }
    return FXPath::simplify(result);
    }
  return FXPath::simplify(file);
  }


// Return absolute path from base directory and file name
// If file is not absolute, place the base-directory in
// front, and simplify the result to obtain canonical path.
FXString FXPath::absolute(const FXString& base,const FXString& file){
  if(file[0]!=PATHSEP){
    FXString result(FXPath::absolute(base));
    if(!file.empty()){
      if(result.tail()!=PATHSEP) result.append(PATHSEP);
      result.append(file);
      }
    return FXPath::simplify(result);
    }
  return FXPath::simplify(file);
  }

#endif

/*******************************************************************************/

// Return true if file name is relative
// Which means "." or "./blabla" or ".." or "../blabla".
// But ".blabla" is NOT relative.
FXbool FXPath::isRelative(const FXString& file){
  return file[0]=='.' && ((file[1]=='\0' || file[1]==PATHSEP) || (file[1]=='.' && (file[2]=='\0' || file[2]==PATHSEP)));
  }


// Return relative path of file to given absolute base directory
//
// Examples:
//
//  Base       File         Result      Comment
//  a          /b           /b          Base is relative but file is not
//  /a         b            b           Base is absolute but file is not
//
//  /a/b/c     /a/b/c/d     d           Branch point is /a/b/c
//  /a/b/c/    /a/b/c/d     d           Branch point is /a/b/c
//
//  a          b            ../b        Branch point is assumed ..
//  ./a        ./b          ../b        Branch point is assumed ..
//
//  /a/b/c     /a/b/c       ../c        Branch point is /a/b
//  /a/b/c/    /a/b/c       ../c        Branch point is /a/b
//  /a/b/c/    /a/b/c/      .           Branch point is /a/b/c
//  /a/b/c     /a/b/c/      .           Branch point is /a/b/c
//
//  ../a/b/c   ../a/b/c/d   d           Branch point is ../a/b/c
//
//  /a/b/c/d   /a/b/c       ../         Branch point is /a/b/c
//
//  /a/b/c/d   /a/b/q       ../../q     Branch point is /a/b
//
//  /          /a           a           Branch point is /
//  /a         /b           ../b        Branch point is /
//  /a/b       /c           ../../c     Branch point is /
//  /          /a/b         a/b         Branch point is /
//  /p/q       /a/b         ../../a/b   Branch point is /
//
FXString FXPath::relative(const FXString& base,const FXString& file){
  if(!base.empty() && !file.empty()){
    FXint p=0,q=0,bp=0,bq=0;

#if defined(WIN32)

    // Drive letter same?
    if(Ascii::isLetter(base[0]) && base[1]==':'){
      if(Ascii::toUpper(file[0])!=Ascii::toUpper(base[0])) goto x;
      if(file[1]!=':') goto x;
      p=2;
      q=2;
      }

    // Server and share same?
    else if(base[0]==PATHSEP && base[1]==PATHSEP){
      if(file[0]!=PATHSEP || file[1]!=PATHSEP) goto x;
      p=2;
      q=2;
      while(base[p] && base[p]!=PATHSEP){
        if(Ascii::toLower(base[p])!=Ascii::toLower(file[q])) goto x;
        p++;
        q++;
        }
      if(base[p]==PATHSEP){
        if(file[q]!=PATHSEP) goto x;
        p++;
        q++;
        while(base[p] && base[p]!=PATHSEP){
          if((Ascii::toLower(base[p])!=Ascii::toLower(file[q]))) goto x;
          p++;
          q++;
          }
        }
      }

    // Find branch point
    while(base[p] && file[q]){
      if(base[p]==PATHSEP && file[q]==PATHSEP){
        bp=p; while(base[p]==PATHSEP) p++;  // Eat multiple slashes
        bq=q; while(file[q]==PATHSEP) q++;
        continue;
        }
      if(Ascii::toLower(base[p])==Ascii::toLower(file[q])){
        p++;
        q++;
        continue;
        }
      break;
      }

#else

    // Find branch point
    while(base[p] && file[q]){
      if(base[p]==PATHSEP && file[q]==PATHSEP){
        bp=p; while(base[p]==PATHSEP) p++;  // Eat multiple slashes
        bq=q; while(file[q]==PATHSEP) q++;
        continue;
        }
      if(base[p]==file[q]){
        p++;
        q++;
        continue;
        }
      break;
      }

#endif

   // Base and file should be either both absolute or both relative.
   // The branch point MUST be at a proper head, i.e. at least a
   // "C:\" or "\\Server\Share\" on Windows ["/" on Linux] if it is
   // an absolute path.
   if((base[bq]==PATHSEP) == (file[bq]==PATHSEP)){

      // Check if common prefix extends to the end of the base-path.
      // If the file-path ends in "/", the final component is a directory,
      // in which case it is OK to return ".".  Otherwise we prefer
      // to prefix the file-part with "../".
      if((base[p]=='\0' || base[p]==PATHSEP) && file[q]==PATHSEP){
        bp=p;
        bq=q;
        }

      // Strip leading path character off, if any
      while(file[bq]==PATHSEP) bq++;

      // Non trivial
      if(file[bq]){
        FXString result;

        // Up to branch point
        while(base[bp]){
          while(base[bp]==PATHSEP) bp++;
          if(base[bp]){
            while(base[bp] && base[bp]!=PATHSEP) bp++;
            result.append(".." PATHSEPSTRING);
            }
          }

        // Append tail end
        result.append(&file[bq]);
        return result;
        }
      return ".";
      }
    }
x:return file;
  }


// Return relative path of file to the current directory
FXString FXPath::relative(const FXString& file){
  return FXPath::relative(FXSystem::getCurrentDirectory(),file);
  }

/*******************************************************************************/

// Return true if file is inside base directory
//
// Examples:
//
//  Base       File         Result      Comment
//  /a/b/c     /a/b/c/d      yes        /a/b/c/d is under directory /a/b/c
//  /a/b/c     /a/b          no         /a/b is NOT under directory /a/b/c
//  a/b        a/b/c         yes        ./a/b/c is under directory ./a/b
//  a          b             no         ./b is NOT under ./a
//  a/b        a/c           no         ./a/c is NOT under ./a/b
//  /a/b/c     c             no         ./c is NOT (necessarily) under /a/b/c
//  a          /a/b          no         /a/b is NOT under ./a
//  .          b             yes        ./b is under .
//  ..         b             yes        ./b is under ./..
//  ../a       b             no         ./b is NOT under ../a
//  ./a/b      a/b           yes        ./a/b is under ./a/b
//  a/b        ./a/b/c       yes        ./a/b/c is under ./a/b
//  ./a/b      a/b           yes        ./a/b is under ./a/b
//  .          .             yes        . is under .
//  ..         .             yes        . is under ./..
//  ..         ..            yes        .. is under ..
//  .          ..            no         ./.. is NOT under .
//  ../a/b     ../a/b/c      yes        ../a/b/c is under ../a/b
//  ../a/b     ../d          no         ../d is NOT under ../a/b
//  (empty)    (something)   no         ./something is NOT under empty
FXbool FXPath::isInside(const FXString& base,const FXString& file){
  if(!base.empty() && !file.empty()){
    FXint p=0,q=0,v=0;
#if defined(WIN32)
    // Drive letter same?
    if(Ascii::isLetter(base[0]) && base[1]==':'){
      if(Ascii::toUpper(file[0])!=Ascii::toUpper(base[0])) return false;
      if(file[1]!=':') return false;
      p=2;
      q=2;
      }

    // Server and share same?
    else if(base[0]==PATHSEP && base[1]==PATHSEP){
      if(file[0]!=PATHSEP) return false;
      if(file[1]!=PATHSEP) return false;
      p=2;
      q=2;
      while(base[p] && base[p]!=PATHSEP){
        if(Ascii::toLower(base[p])!=Ascii::toLower(file[q])) return false;
        p++;
        q++;
        }
      if(base[p]==PATHSEP){
        if(file[q]!=PATHSEP) return false;
        p++;
        q++;
        while(base[p] && base[p]!=PATHSEP){
          if((Ascii::toLower(base[p])!=Ascii::toLower(file[q]))) return false;
          p++;
          q++;
          }
        }
      }

    // Process potentially non-canonical, case-insensitive paths
    while(base[p]){
      if(base[p]==PATHSEP){
        if(file[q]!=PATHSEP) return false;
        while(base[p]==PATHSEP) p++;                            // Eat '/'
        while(file[q]==PATHSEP) q++;                            // Eat '/'
        }
a:    if(base[p]=='.'){
        if(base[p+1]=='\0'){ p+=1; goto a; }                    // Eat '.'
        if(base[p+1]==PATHSEP){ p+=2; goto a; }                 // Eat './'
        if(base[p+1]=='.'){
          if(base[p+2]=='\0'){ p+=2; v++; goto a; }             // Eat '..'
          if(base[p+2]==PATHSEP){ p+=3; v++; goto a; }          // Eat '../'
          }
        }
b:    if(file[q]=='.'){
        if(file[q+1]=='\0'){ q+=1; goto b; }                    // Eat '.'
        if(file[q+1]==PATHSEP){ q+=2; goto b; }                 // Eat './'
        if(file[q+1]=='.'){
          if(file[q+2]=='\0'){ q+=2; v--; goto b; }             // Eat '..'
          if(file[q+2]==PATHSEP){ q+=3; v--; goto b; }          // Eat '../'
          }
        }
      if(v<0) return false;
      while(base[p] && base[p]!=PATHSEP){
        if(Ascii::toUpper(base[p])!=Ascii::toUpper(file[q])) return false;
        p++;
        q++;
        }
      }
    return true;
#else
    // Process potentially non-canonical paths
    while(base[p]){
      if(base[p]==PATHSEP){
        if(file[q]!=PATHSEP) return false;
        while(base[p]==PATHSEP) p++;                            // Eat '/'
        while(file[q]==PATHSEP) q++;                            // Eat '/'
        }
a:    if(base[p]=='.'){
        if(base[p+1]=='\0'){ p+=1; goto a; }                    // Eat '.'
        if(base[p+1]==PATHSEP){ p+=2; goto a; }                 // Eat './'
        if(base[p+1]=='.'){
          if(base[p+2]=='\0'){ p+=2; v++; goto a; }             // Eat '..'
          if(base[p+2]==PATHSEP){ p+=3; v++; goto a; }          // Eat '../'
          }
        }
b:    if(file[q]=='.'){
        if(file[q+1]=='\0'){ q+=1; goto b; }                    // Eat '.'
        if(file[q+1]==PATHSEP){ q+=2; goto b; }                 // Eat './'
        if(file[q+1]=='.'){
          if(file[q+2]=='\0'){ q+=2; v--; goto b; }             // Eat '..'
          if(file[q+2]==PATHSEP){ q+=3; v--; goto b; }          // Eat '../'
          }
        }
      if(v<0) return false;
      while(base[p] && base[p]!=PATHSEP){
        if(base[p]!=file[q]) return false;
        p++;
        q++;
        }
      }
    return true;
#endif
    }
  return false;
  }

/*******************************************************************************/

// Return path to directory above input directory name
// Only append a PATHSEP if there isn't one already;
// necessary because don't want to create a UNC from
// a regular path.
FXString FXPath::upLevel(const FXString& file){
  if(!file.empty()){
    if(file.tail()==PATHSEP){
      return FXPath::simplify(file+"..");
      }
    return FXPath::simplify(file+PATHSEPSTRING "..");
    }
  return ".";
  }

/*******************************************************************************/

// Does file represent topmost directory
//
// On Windows, isTopDirectory() returns true for paths of the form:
//
//   "\"
//   "C:\"
//   "\\Server\Share\"
//
// On Linux, isTopDirectory() returns true for paths of the form:
//
//   "/"
//
FXbool FXPath::isTopDirectory(const FXString& file){
  if(!file.empty()){
#if defined(WIN32)
    FXint p=0,s;
    if(Ascii::isLetter(file[0]) && file[1]==':'){
      p=2;
      }
    else if(file[0]==PATHSEP && file[1]==PATHSEP && file[2]!=PATHSEP && file[2]){
      s=3;
      while(file[s] && file[s]!=PATHSEP) s++;
      if(file[s]==PATHSEP){
        s++;
        if(file[s] && file[s]!=PATHSEP){
          s++;
          while(file[s] && file[s]!=PATHSEP) s++;
          p=s;
          }
        }
      }
    return file[p]==PATHSEP && file[p+1]=='\0';
#else
    return file[0]==PATHSEP && file[1]=='\0';
#endif
    }
  return false;
  }

/*******************************************************************************/

#if defined(WIN32)

// Return true if input path is a hidden file or directory
FXbool FXPath::isHidden(const FXString& file){
  if(!file.empty()){
#ifdef UNICODE
    FXuint attrs;
    FXnchar unifile[MAXPATHLEN];
    utf2ncs(unifile,file.text(),MAXPATHLEN);
    if((attrs=::GetFileAttributesW(unifile))!=INVALID_FILE_ATTRIBUTES){
      return (attrs&FILE_ATTRIBUTE_HIDDEN)!=0;
      }
#else
    FXuint attrs;
    if((attrs=::GetFileAttributesA(file.text()))!=INVALID_FILE_ATTRIBUTES){
      return (attrs&FILE_ATTRIBUTE_HIDDEN)!=0;
      }
#endif
    }
  return false;
  }

#else

// Return true if input path is a hidden file or directory
FXbool FXPath::isHidden(const FXString& file){
  if(!file.empty()){
    FXint i=file.length();
    while(0<i && file[i-1]!=PATHSEP){ --i; }
    return file[i]=='.';
    }
  return false;
  }

#endif

/*******************************************************************************/

// Return valid part of absolute path
FXString FXPath::validPath(const FXString& file){
  if(FXPath::isAbsolute(file)){
    FXString result(file);
    while(!FXPath::isTopDirectory(result) && !FXStat::exists(result)){
      result=FXPath::upLevel(result);
      }
    return result;
    }
  return FXString::null;
  }

/*******************************************************************************/

// Return true if absolute path is valid
FXbool FXPath::isValidPath(const FXString& file){
  if(FXPath::isAbsolute(file)){
    return FXStat::exists(file);
    }
  return false;
  }

/*******************************************************************************/

#if defined(WIN32)           // WINDOWS

// Enquote filename to make safe for shell
//
// Enclosing with double quotes if needed:
//
//   - If force is true.
//   - If filename is empty.
//   - If any whitespace in file.
//   - If special shell characters encountered.
//
// Backslashes are doubled if:
//
//   - Followed by closing quote '"'.
//   - Followed by escaped quote '\"'.
//
// Possible alternative for '"', if '\"' proves troublesome:
//
//   - Replace by '""' if inside quotes and not preceeded by '\'.
//   - Replace by '""""' if outside quotes and not preceeded by '\'.
//
// Presence of shell characters forces quotes; this way, special
// symbols pass through cmd shell unmolested.
// We could also escape shell characters with the shell escape symbol
// '^'; however, this means other programs which only process quoted
// strings will have problems.  So quotation is best here.
FXString FXPath::enquote(const FXString& file,FXbool force){
  if(!file.empty()){
    FXint p,q,c,n;
    p=q=n=0;
    do{
      c=file[q++];
      switch(c){
      case '\\':                        // Backslash
        p++;
        n++;
        break;
      case '"':                         // Embedded '"'
        p+=n;                           // Double '\'
        p+=2;                           // Plus escaped '"'
        n=0;
        break;
      case '\0':                        // End of string
        if(!force) break;
        p+=n;                           // Double '\'
        n=0;
        break;
      case ' ':                         // White space
      case '\t':
        force=true;
        p++;
        n=0;
        break;
      case '^':                         // Escape character
      case '<':                         // Redirection
      case '>':
      case '(':
      case ')':
      case '|':                         // Pipe
      case '%':                         // Environment variables
      case '!':                         // Wildcard
      case '&':                         // Command separators
      case '*':                         // Wildcard
      case '?':
      case '@':
       force=true;                      // Must quote shell characters
        /*FALL*/
      default:                          // Normal characters
        p++;
        n=0;
        break;
        }
      }
    while(c);

    // Space for quotes or space for escapes
    if(force) p+=2;

    // File must be escaped or quoted
    if(file.length()<p){
      FXString result('\0',p);
      p=q=n=0;
      if(force) result[p++]='"';        // Opening quote
      do{
        c=file[q++];
        switch(c){
        case '\\':                      // Backslash
          result[p++]='\\';
          n++;
          break;
        case '"':                       // Embedded '"'
          while(n){
            result[p++]='\\';           // Double the '\'
            n--;
            }
          result[p++]='\\';             // Escaped '\"'
          result[p++]='"';
          n=0;
          break;
        case '\0':                      // End of string
          if(!force) break;
          while(n){
            result[p++]='\\';           // Double the '\'
            n--;
            }
          n=0;
          break;
        case ' ':                       // White space
        case '\t':
          result[p++]=c;
          n=0;
          break;
        case '^':                       // Escape character
        case '<':                       // Redirection
        case '>':
        case '(':
        case ')':
        case '|':                       // Pipe
        case '%':                       // Environment variables
        case '!':                       // Wildcard
        case '&':                       // Command separators
        case '*':                       // Wildcards
        case '?':
        case '@':
          /*FALL*/
        default:                        // Normal characters
          result[p++]=c;
          n=0;
          break;
          }
        }
      while(c);
      if(force) result[p++]='"';        // Closing quote
      FXASSERT(p==result.length());
      return result;
      }

    // Unchanged filename
    return file;
    }
  return "\"\"";
  }


// Dequote filename that was enquoted for the command line.
//
//  Input       Result          Comment
//
//  ""          (empty)         To pass empty string.
//
//  "a b"       a b             Use quotes for embedded spaces.
//  a^ b        a b             Escape for embedded spaces to cmd.exe.
//  a b         a               Only first parameter will be returned.
//
//
//  \"          "               Escape with '\'' to embed quotes.
// """"         "               Inside quotes, '""' yields one '"'.
//
//  \\\"        \"              Odd number of '\' followed by '"' yields half that
//  \\\\\"      \\"             number of '\' followed by a '"'.
//
//  a\\"b c"    a\b c           Even number of '\' followed by a '"' yields to half that
//  a\\\\"b c"  a\\b c          number of '\', with quotes serving as delimiter.
//
//  a\b         a\b             When not followed by a '"', a '\' is copied literally.
//  "a\\b"      a\\b
//
//  ^<file      <file           Caret ^ serves as escape ONLY in non-quoted string.
//  ^^          ^               Embed the escape charecter itself.
//
//  ^<LF>abc    abc             Line continuation; first character on next line
//  ^<CR><LF>"  "               is treated as having been escaped!
//
// Here we do handle special shell characters via standard shell escape '^'.
// Line-continuation is performen when '^' followed by a CR, LF, or CRLF.
// Be careful with the first character after line-continuation, it will be
// interpreted as having been escaped!
FXString FXPath::dequote(const FXString& file){
  if(!file.empty()){
    FXString result(file);
    FXint quote=0,param=0,slash=0,p=0,q=0;
    FXchar c;
    while(c=file[q++]!='\0'){
      switch(c){
      case '\\':                        // Backslashes get special handling
        result[p++]=c;
        slash++;
        param=1;
        break;
      case '"':                         // Double quoted
        p-=(slash>>1);                  // Back of over half the '\'
        if((slash&1)==0){               // Even number of '\'
          if(file[q]=='"' && quote){    // Second '"' after '"' in quote
            result[p++]='"';            // Embedded quote
            q++;
            }
          else{
            quote^=1;                   // Unescaped '"'
            }
          }
        else{                           // Odd number of '\'
          result[p-1]='"';              // Overwrite '\' with '"'
          }
        slash=0;
        param=1;
        break;
      case ' ':                         // White space
      case '\t':
        if(quote) goto nrm;             // Embedded whitespace
        if(param) goto fin;             // Finished parameter
        break;
      case '^':                         // Shell escape if not quoted
        if(quote) goto nrm;
        if(file[q]){
          c=file[q++];                  // Escaped character
          if(c=='\r' && file[q]){       // CR line continuation
            c=file[q++];                // Escape character after continuation
            }
          if(c=='\n' && file[q]){       // LF or CRLF line continuation
            c=file[q++];                // Escape character after continuation
            }
          }
        /*FALL*/
      default:                          // Normal characters
nrm:    result[p++]=c;                  // Append it
        slash=0;
        param=1;
        break;
        }
      }
    FXASSERT(p<=result.length());
fin:result.trunc(p);
    return result;
    }
  return FXString::null;
  }


#else                         // UNIX


// Enquote filename to make safe for shell
//
// Enclose with single quotes when:
//
//   - If force is true.
//   - If filename is empty.
//   - If white space before, in, or after letters.
//   - If special shell characters encountered.
//
// Escaping is done when:
//
//   - Quote character (') are encountered
//
FXString FXPath::enquote(const FXString& file,FXbool force){
  if(!file.empty()){
    FXint p,q,e,c;
    p=q=e=0;
    while((c=file[q++])!='\0'){
      switch(c){
      case '\'':                // Single quote
        p+=2;                   // Escaped as xxx\'xxx if outside quotes
        e+=2;                   // Escaped as 'xxx'\''xxx' if inside quotes
        continue;
      case '`':                 // Command substitution
      case '"':                 // Double quote
      case '\\':                // Back slash
      case '!':                 // History expansion
      case '$':                 // Variable substitution
      case '(':
      case ')':
      case '&':
      case ';':
      case '<':                 // Redirections, pipe
      case '>':
      case '|':
      case '*':                 // Wildcard characters
      case '^':
      case '+':
      case '?':
      case '[':
      case ']':
      case '{':                 // Brace expansion
      case '}':
      case '=':                 // Equals
      case '%':                 // Job control
      case '#':                 // Comments
      case '\r':
      case '\n':
      case ' ':                 // White space
      case '\t':
        force=true;             // Force quotes
        p++;
        continue;
      case '~':                 // Username substitution
        if(p==1) force=true;    // Force quotes if at beginning
        /*FALL*/
      default:                  // Normal character
        p++;
        continue;
        }
      }

    // Extra space when quoted
    if(force) p+=e+2;

    // File must be escaped or quoted
    if(file.length()<p){
      FXString result('\0',p);
      p=q=0;
      if(force) result[p++]='\'';       // Opening quote
      while((c=file[q++])!='\0'){
        if(c=='\''){                    // Quote needs to be escaped
          if(force) result[p++]='\'';   // Closing quote
          result[p++]='\\';
          result[p++]='\'';
          if(force) result[p++]='\'';   // Opening quote
          continue;
          }
        result[p++]=c;                  // Normal characters
        }
      if(force) result[p++]='\'';       // Closing quote
      FXASSERT(p<=result.length());
      return result;
      }

    // Unchanged filename
    return file;
    }
  return "''";
  }


// Dequote filename that was enquoted for the command line.
//
//  Input       Result          Comment
//
//  ""          (empty)         To pass empty string.
//
//  "a b c"     a b c           Double quotes for embedded spaces.
//  'a b c'     a b c           Single quotes for embedded spaces.
//  a\ b\ c     a b c           Escapes for embedded spaces.
//
//  \<          <               Outside of quotes, '\' escapes characters.
//  \\          \               Embed escape character itself.
//
//  '\$ENV'     \$ENV           Inside single quotes, '\' has no special meaning.
//  "\$ENV"     $ENV            Inside double quotes, '\' only escapes '$', '\', '`', and '"'.
//  "\ENV"      \ENV            No special meaning for '\', unless followed by '$', '\', '`', or '"'.
//
//  'a'\''b'    a'b             Cannot escape '\'' inside single quotes.
//
//  a b         a               Only first parameter will be returned.
//
//  '"'         "               Enquote '"' inside '..' is fine.
//  "'"         '               As is '\'' inside "...".
//
//  \<NL>       (nothing)       Line continuation for '\' followed by newline.
//
FXString FXPath::dequote(const FXString& file){
  if(!file.empty()){
    FXString result(file);
    FXint quote=0,param=0,p=0,q=0;
    FXchar c;
    while((c=file[q++])!='\0'){
      switch(c){
      case '\'':                        // Single quoted, no escape codes in here
        if(quote==2) goto nrm;          // "...'..."
        quote^=1;
        param=1;
        break;
      case '"':                         // Double quoted, escape codes only for $\`"
        if(quote==1) goto nrm;          // '..."...'
        quote^=2;
        param=1;
        break;
      case '\r':
      case '\n':
      case ' ':                         // Whitespace
      case '\t':
        if(quote) goto nrm;             // Embedded whitespace
        if(param) goto fin;             // Finished parameter
        break;
      case '\\':
        if(quote==1) goto nrm;
        if(file[q]){
          c=file[q++];                  // Escaped character
          if(c=='\n') break;            // Line continuation
          if(quote==2 && c!='$' && c!='\\' && c!='`' && c!='"'){
            result[p++]='\\';           // Backslash remains inside "..."
            }
          }
        /*FALL*/
      default:                          // Normal characters
nrm:    result[p++]=c;                  // Append it
        param=1;
        break;
        }
      }
    FXASSERT(p<=result.length());
fin:result.trunc(p);
    }
  return FXString::null;
  }

#endif

/*******************************************************************************/

#if defined(WIN32)

// Parse command to argc and argv, according to os-native rules
//
// 2N+1 backslashes + '"' : N backslashes + literal '"'
// 2N backslashes + '"'   : N backslashes and begin/end of quoted text
// N backslashes          : N backslashes
//
// '""' inside quote      : Inserts one '"'
//
FXint FXPath::parseArgs(FXchar**& argv,const FXchar* command){
  argv=nullptr;
  if(command){
    const FXchar* p;
    FXint token=0;
    FXint quote=0;
    FXint slash=0;
    FXint count=0;
    FXint space=0;
    FXchar* buffer;
    FXchar** ptr;
    FXchar* arg;
    FXchar* a;
    FXchar c;

    // First pass: measure output
    p=command;
    while((c=*p++)!='\0'){
      switch(c){
      case '\\':                        // Backslashes get special handling
        token=1;
        slash++;
        space++;
        break;
      case '"':                         // Double quotes
        token=1;                        // NO back up by slash/2
        if((slash&1)==0){               // Even number of '\'
          if(*p=='"' && quote){         // Double '"'
            space++;
            p++;
            }
          else{
            quote^=1;                   // Unescaped '"'
            }
          }
        slash=0;
        break;
      case ' ':                         // White space
      case '\t':
        if(quote) goto nrm1;            // White space in parameter
        /*FALL*/
      case '\v':
      case '\r':
      case '\f':
      case '\n':
        slash=0;
        if(!token) break;               // No preceeding token
        token=0;
        count++;
        space++;
        break;
      default:                          // Normal characters
nrm1:   token=1;
        space++;
        slash=0;
        break;
        }
      }

    // Final closeout
    count+=token;
    space+=token;

    // We got at least one argument
    if(count){

      // Allocate one buffer for the whole thing
      if(allocElms(buffer,(count+1)*sizeof(FXchar*)+space)){

        // First part of buffer contains pointers
        argv=ptr=(FXchar**)buffer;

        // Point to where the characters start
        arg=a=(FXchar*)&argv[count+1];

        quote=token=slash=0;

        // Second pass: generate output
        p=command;
        while((c=*p++)!='\0'){
          switch(c){
          case '\\':                    // Backslashes get special handling
            token=1;
            slash++;
            *a++=c;
            break;
          case '"':                     // Double quoted
            token=1;
            a-=(slash>>1);              // Back up by slash/2
            if((slash&1)==0){           // Even number of '\'
              if(*p=='"' && quote){     // Double '"'
                *a++='"';
                p++;
                }
              else{
                quote^=1;               // Unescaped '"'
                }
              }
            else{                       // Odd number of '\'
              *(a-1)='"';               // Overwrite '\' with '"'
              }
            slash=0;
            break;
          case ' ':                     // White space
          case '\t':
            if(quote) goto nrm2;        // White space in parameter
            /*FALL*/
          case '\v':
          case '\r':
          case '\f':
          case '\n':
            slash=0;
            if(!token) break;           // No preceeding token
            token=0;
            *ptr++=arg;
            *a++='\0';
            arg=a;
            break;
          default:                      // Normal characters
nrm2:       token=1;
            slash=0;
            *a++=c;
            break;
            }
          }

        // Final closeout
        if(token){
          *ptr++=arg;
          *a++='\0';
          }

        // Argv closeout
        *ptr=nullptr;

        // Make sure it all fitted
        FXASSERT(a<=buffer+(count+1)*sizeof(FXchar*)+space);
        return count;
        }
      }
    }
  return 0;
  }

#else

// Parse command to argc and argv, according to os-native rules
//
// \<nl>        : Line continuation NOT counted as a space
// \X           : Escaped character (when outside quotes)
// "......"     : Only $, \, and ' need escaping inside double-quoted text
// '......'     : No characters can be escaped inside single-quoted text
//
FXint FXPath::parseArgs(FXchar**& argv,const FXchar* command){
  argv=nullptr;
  if(command){
    const FXchar *p;
    FXint token=0;
    FXint quote=0;
    FXint count=0;
    FXint space=0;
    FXchar* buffer;
    FXchar** ptr;
    FXchar* arg;
    FXchar* a;
    FXchar c;

    // First pass: measure output
    p=command;
    while((c=*p++)!='\0'){
      switch(c){
      case '\\':                        // Escape
        if(quote!=1) c=*p++;            // No escape inside single quote
        if(c=='\n') break;              // Line continuation
        if(quote==2 && c!='$' && c!='\\' && c!='`' && c!='"'){
          space++;                      // Backslash remains inside "..."
          }
        space++;
        token=1;
        break;
      case '\'':                        // Single quoted, no escape codes in here
        if(quote==2) goto nrm1;         // "...'..."
        quote^=1;
        token=1;
        break;
      case '"':                         // Double quoted, escape codes only for $\`"
        if(quote==1) goto nrm1;         // '..."...'
        quote^=2;
        token=1;
        break;
      case ' ':                         // White space
      case '\t':
      case '\r':
      case '\n':
        if(quote) goto nrm1;            // White space in parameter
        if(!token) break;               // No preceeding token
        token=0;
        count++;                        // Another argument
        space++;
        break;
      default:                          // Normal characters
nrm1:   space++;
        token=1;
        break;
        }
      }

    // Final closeout
    count+=token;
    space+=token;

    // We got at least one argument
    if(count){

      // Allocate one buffer for the whole thing
      if(allocElms(buffer,(count+1)*sizeof(FXchar*)+space)){

        // First part of buffer contains pointers
        argv=ptr=(FXchar**)buffer;

        // Point to where the characters start
        arg=a=(FXchar*)&argv[count+1];

        quote=token=0;

        // Second pass: generate output
        p=command;
        while((c=*p++)!='\0'){
          switch(c){
          case '\\':                    // Escape
            if(quote!=1) c=*p++;        // No escape inside single quote
            if(c=='\n') break;          // Line continuation
            if(quote==2 && c!='$' && c!='\\' && c!='`' && c!='"'){
              *a++='\\';                // Backslash remains inside "..."
              }
            *a++=c;
            token=1;
            break;
          case '\'':                    // Single quoted, no escape codes in here
            if(quote==2) goto nrm2;     // "...'..."
            quote^=1;
            token=1;                    // Allow for empty argument ''
            break;
          case '"':                     // Double quoted, escape codes only for $\`"
            if(quote==1) goto nrm2;     // '..."...'
            quote^=2;
            token=1;                    // Allow for empty argument ""
            break;
          case ' ':                     // White space
          case '\t':
          case '\r':
          case '\n':
            if(quote) goto nrm2;        // White space in parameter
            if(!token) break;           // No preceeding token
            token=0;
            *ptr++=arg;                 // Another argument
            *a++='\0';
            arg=a;
            break;
          default:                      // Normal characters
nrm2:       *a++=c;
            token=1;
            break;
            }
          }

        // Final token closeout
        if(token){
          *ptr++=arg;
          *a++='\0';
          }

        // Argv closeout
        *ptr=nullptr;

        // Make sure it all fitted
        FXASSERT(a<=buffer+(count+1)*sizeof(FXchar*)+space);
        return count;
        }
      }
    }
  return 0;
  }

#endif

// Parse command to argc and argv
FXint FXPath::parseArgs(FXchar**& argv,const FXString& command){
  return parseArgs(argv,command.text());
  }

/*******************************************************************************/

// Skip over part of pattern
//
// This is a bit complex: we need to count matching parentheses, while
// also keeping track of whether we're inside a charset, as parentheses
// in a charset are not grouping parentheses.
// Charsets are delimited by brackets, but a closing bracket appearing
// as the first character in the set does not close the charset, but is
// taken as just a character.  Note, the first actual character in a
// charset may actually follow a set-negation character, '!' or '^'.
// If alt=true, we don't scan all the way to the end of the group, but
// just skip to the start of the next alternative, which immediately
// follows a '|' or ',' character.
// A last and final concern is the escape sequence (if enabled), which
// prevents interpretation of the escaped character as a special directive.
static const FXchar* skip(const FXchar* p,FXuint flags,FXbool alt){
  FXint set=0;          // Characters in set [...]
  FXint brk=0;          // Bracket set
  FXint par=0;          // Group level
  FXint neg=0;          // Negated
n:switch(*p){
    case '\0':
      goto x;
    case '[':           // Enter character set [...]
      p++;
      if(!brk){ brk=1; set=0; neg=0; goto n; }
      set++;
      goto n;
    case ']':           // Leave character set [...] (if not first character in set []...] or [!]...] or [^]...])
      p++;
      if(brk && set){ brk=0; goto n; }
      set++;
      goto n;
    case '!':
    case '^':           // Invert character set [^...] or [!...] (if in set [...])
      p++;
      if(brk && !set && !neg){ neg=1; goto n; }
      set++;
      goto n;
    case '(':           // Enter sub group (...) (if not in character set [...])
      p++;
      if(!brk){ ++par; }
      set++;
      goto n;
    case ')':           // Leave sub group (...) (if not in character set [...])
      if(!brk){ if(--par<0) goto x; }
      p++;
      set++;
      goto n;
    case ',':           // End of alternatives
    case '|':
      if(!brk && !par && alt){ goto x; }
      p++;
      set++;
      goto n;
    case '\\':          // Escape code
      if(!(flags&FXPath::NoEscape)){ if(*++p=='\0') goto x; }
      /*FALL*/
    default:            // Regular character
      p=wcinc(p);       // Next unicode character
      set++;
      goto n;
    }
x:return p;
  }


// Perform match
//
// Special pattern characters are interpreted as follows:
//
//   ?   Normally matches a single unicode character. However, when the DotFile flag is passed,
//       the ? does NOT match a '.' if it appears at the start of the string, or if it follows a
//       '/' and the PathName flag is also passed.
//
//   *   Normally matches zero or more unicode characters. However, when the DotFile flag is passed,
//       the * does NOT match a '.' if the '.' appears at the start of the string, or if it follows a
//       '/' and the PathName flag is also passed.
//
//  [ ]  Character set matches a single unicode character in the set.  However, then the DotFile flag
//       is passed, the [] does NOT match a '.' if it appears at the start of the string, or if it follows
//       a '/' and the PathName flag is also passed.
//       A set is negated (matches when the corresponding character is NOT in the set), by following the
//       opening '[' with '^' or '!'.
//
//   \   Escaped characters match the unicode character.  If \ is NOT followed by a character, the match
//       will fail.
//
// ( | ) Will start, separate, and end a group of alternatives, each of which may contain pattern
//       characters as well as sub-groups thereof; The ',' may also be used.
//
static FXbool domatch(const FXchar* string,const FXchar* s,const FXchar* p,FXuint flags){
  const FXchar* q;
  FXwchar pp,qq,ss;
  FXbool neg,ok;
  while(*p!='\0'){
    switch(*p){
      case '(':         // Start subpattern
        p++;
        q=skip(p,flags,false);                                  // End of sub group
        if(*q!=')') return false;                               // Missing ')'
        while(p<q){
          if(domatch(string,s,p,flags)) return true;
          p=skip(p,flags,true);                                 // End of alternative
          if(*p!=',' && *p!='|') continue;
          p++;
          }
        return false;
      case ')':         // End subpattern
        p++;
        break;
      case '|':         // Alternatives
      case ',':
        p=skip(p+1,flags,false);                                // Continue to match the rest
        break;
      case '?':         // Single character wildcard
        p++;                                                    // Eat '?'
        if(*s=='\0') return false;
        if((*s==PATHSEP) && (flags&FXPath::PathName)) return false;
        if((*s=='.') && (flags&FXPath::DotFile) && ((s==string) || ((*(s-1)==PATHSEP) && (flags&FXPath::PathName)))) return false;
        s=wcinc(s);
        break;
      case '*':         // Multiple character wildcard
        while(*p=='*') p++;                                     // Eat '*'
        while(*s!='\0'){
          if(domatch(string,s,p,flags)) return true;
          if((*s=='.') && (flags&FXPath::DotFile) && ((s==string) || ((*(s-1)==PATHSEP) && (flags&FXPath::PathName)))) return false;
          if((*s==PATHSEP) && (flags&FXPath::PathName)) return false;
          s=wcinc(s);
          }
        break;
      case '[':         // Single character against character-set
        p++;                                                    // Eat '['
        if(*s=='\0') return false;
        if((*s==PATHSEP) && (flags&FXPath::PathName)) return false;
        if((*s=='.') && ((s==string) || (*(s-1)==PATHSEP)) && (flags&FXPath::DotFile)) return false;
        neg=(*p=='!' || *p=='^');
        if(neg) p++;
        ss=wc(s);
        if(flags&FXPath::CaseFold){ ss=Unicode::toLower(ss); }
        s=wcinc(s);
        ok=false;
        do{
          if(*p=='\\' && !(flags&FXPath::NoEscape)) p++;
          if(*p=='\0') return false;
          pp=wc(p);
          if(flags&FXPath::CaseFold){ pp=Unicode::toLower(pp); }
          p=wcinc(p);
          if(*p=='-' && *(p+1)!=']'){                           // Range match
            p++;
            if(*p=='\\' && !(flags&FXPath::NoEscape)) p++;
            if(*p=='\0') return false;
            qq=wc(p);
            if(flags&FXPath::CaseFold){ qq=Unicode::toLower(qq); }
            p=wcinc(p);
            if(pp<=ss && ss<=qq) ok=true;
            }
          else{                                                 // Single match
            if(pp==ss) ok=true;
            }
          }
        while(*p!=']');
        p++;
        if(ok==neg) return false;
        break;
      case '\\':        // Escaped character follows
        if(!(flags&FXPath::NoEscape)){ if(*++p=='\0') return false; }
        /*FALL*/
      default:          // Match characters against pattern
        pp=wc(p);
        ss=wc(s);
        if(flags&FXPath::CaseFold){
          pp=Unicode::toLower(pp);
          ss=Unicode::toLower(ss);
          }
        if(pp!=ss) return false;
        s=wcinc(s);
        p=wcinc(p);
        break;
      }
    }
  return (*s=='\0') || ((*s==PATHSEP) && (flags&FXPath::LeadDir));
  }


// Match string against a pattern, subject to flags
FXbool FXPath::match(const FXchar* string,const FXchar* pattern,FXuint flags){
  const FXchar* end=skip(pattern,flags,false);  // Check pattern syntax
  if(*end=='\0'){
    while(*pattern){
      if(domatch(string,string,pattern,flags)) return true;
      pattern=skip(pattern,flags,true);
      if(*pattern!=',' && *pattern!='|') break;
      pattern++;
      }
    }
  return false;
  }


// Match string against pattern (like *, ?, [^a-z], and so on)
FXbool FXPath::match(const FXString& string,const FXchar* pattern,FXuint flags){
  return FXPath::match(string.text(),pattern,flags);
  }


// Match string against pattern (like *, ?, [^a-z], and so on)
FXbool FXPath::match(const FXchar* string,const FXString& pattern,FXuint flags){
  return FXPath::match(string,pattern.text(),flags);
  }


// Match string against pattern (like *, ?, [^a-z], and so on)
FXbool FXPath::match(const FXString& string,const FXString& pattern,FXuint flags){
  return FXPath::match(string.text(),pattern.text(),flags);
  }

/*******************************************************************************/

// Generate unique filename of the form pathnameXXX.ext, where pathname.ext is the
// original input file, and XXX is a number, possibly empty, that makes the file unique.
FXString FXPath::unique(const FXString& file){
  if(!FXStat::exists(file)) return file;
  FXString ext=FXPath::extension(file);
  FXString path=FXPath::stripExtension(file);   // Use the new API (Jeroen)
  FXString filename;
  FXint count=0;
  if(!ext.empty()) ext.prepend('.');            // Only add period when non-empty extension
  while(count<1000){
    filename.format("%s%i%s",path.text(),count,ext.text());
    if(!FXStat::exists(filename)) return filename;      // Return result here (Jeroen)
    count++;
    }
  return FXString::null;
  }

/*******************************************************************************/

// Search pathlist for file.
// If the file is absolute, return it.
// If the file is relative, make it absolute w.r.t. current directory, and return it.
// Otherwise, check each expanded component of pathlist and return the absolute path
// under the component of pathlist where the file is found.
// If file was empty string, isn't found, or doesn't exist, return the empty string.
FXString FXPath::search(const FXString& pathlist,const FXString& file){
  if(!file.empty()){
    FXString path;
    FXint beg=0;
    FXint end=0;
    if(FXPath::isAbsolute(file)){
      if(FXStat::exists(file)) return file;
      return FXString::null;
      }
    if(FXPath::isRelative(file)){
      path=FXPath::absolute(file);
      if(FXStat::exists(path)) return path;
      return FXString::null;
      }
    while(pathlist[end]){
      while(pathlist[end]==PATHLISTSEP) end++;
      beg=end;
      while(pathlist[end] && pathlist[end]!=PATHLISTSEP) end++;
      if(beg==end) break;
      path=FXPath::absolute(FXPath::expand(pathlist.mid(beg,end-beg)),file);
      if(FXStat::exists(path)) return path;
      }
    }
  return FXString::null;
  }

/*******************************************************************************/

// Relativize file to path list.
// If file is not absolute, its already relative, so just return file.
// Otherwise, check if file is under one of the components of pathlist.
// If there is more than one such component, keep the shortest path.
// Ensure that whatever the final relative path ends up being, it
// still references the same file when searching pathlist for it.
// If not located under any component of pathlist, then just keep the
// absolute path; it can not be relativized to the given list.
FXString FXPath::relativize(const FXString& pathlist,const FXString& file){
  if(FXPath::isAbsolute(file)){
    FXString result(file);
    FXString base;
    FXString res;
    FXint length=2147483647;
    FXint beg=0;
    FXint end=0;
    while(pathlist[end]){
      while(pathlist[end]==PATHLISTSEP) end++;
      beg=end;
      while(pathlist[end] && pathlist[end]!=PATHLISTSEP) end++;
      if(beg==end) break;
      base=FXPath::absolute(FXPath::expand(pathlist.mid(beg,end-beg)));
      if(FXPath::isInside(base,file)){
        res=FXPath::relative(base,file);
        if(res.length()<length){
          if(FXPath::search(pathlist,res)==file){
            length=res.length();
            result=res;
            }
          }
        }
      }
    return result;
    }
  return file;
  }

/*******************************************************************************/

#if defined(WIN32)

// Check if file has executable extension
FXbool FXPath::hasExecExtension(const FXString& file){
  if(!file.empty()){
    FXString pathext(FXSystem::getExecExtensions());
    FXint beg=0;
    FXint end=0;
    do{
      end=beg;
      while(end<pathext.length() && pathext[end]!=PATHLISTSEP) end++;
      if((end-beg)<=file.length()){
        if(FXString::comparecase(&file[file.length()-(end-beg)],&pathext[beg],(end-beg))==0) return true;
        }
      beg=end+1;
      }
    while(end<pathext.length());
    }
  return false;
  }


// Check if given name is controversial
FXbool FXPath::isReservedName(const FXString& file){
  if(!file.empty()){
    static const FXchar reserved3[]="CON\nPRN\nAUX\nNUL\n";
    static const FXchar reserved4[]="COM1\nCOM2\nCOM3\nCOM4\nCOM5\nCOM6\nCOM7\nCOM8\nCOM9\nLPT1\nLPT2\nLPT3\nLPT4\nLPT5\nLPT6\nLPT7\nLPT8\nLPT9\n";
    if(file.length()==3){
      return (fxstrcasestr(reserved3,file.text())!=nullptr);
      }
    if(file.length()==4){
      return (fxstrcasestr(reserved4,file.text())!=nullptr);
      }
    }
  return false;
  }

#else

// Check if file has executable extension
FXbool FXPath::hasExecExtension(const FXString&){
  return false;
  }

// Check if given name is controversial
FXbool FXPath::isReservedName(const FXString&){
  return false;
  }

#endif


}
