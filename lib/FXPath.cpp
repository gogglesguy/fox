/********************************************************************************
*                                                                               *
*                  P a t h   N a m e   M a n i p u l a t i o n                  *
*                                                                               *
*********************************************************************************
* Copyright (C) 2000,2009 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "fxascii.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXString.h"
#include "FXPath.h"
#include "FXSystem.h"
#include "FXStat.h"
#include "FXFile.h"
#include "FXDir.h"
#ifdef WIN32
#include <shellapi.h>
#endif



/*
  Notes:

  - On FXPath::match():

    - This is "upward compatible" from the standard fnmatch function in glibc,
      in addition to the basic matching, FXPath::match can also handle alternatives.

    - Match patterns are as follows:

      ?           Matches single character.
      *           Matches zero or more characters.
      [abc]       Matches a single character, which must be a, b, or c.
      [^abc]      Matches a single character, which must be anything other than a, b, or c.
      [!abc]      Ditto.
      [a-zA-Z]    Matches single character, which must be one of a-z or A-Z.
      [^a-zA-Z]   Matches single character, which must be anything other than a-z or A-Z.
      [!a-zA-Z]   Ditto.
      pat1|pat2   Matches either pat1 or pat2.
      pat1,pat2   Ditto.
      (pat1|pat2) Matches either pat1 or pat2; patterns may be nested.
      (pat1,pat2) Ditto.

    - Examples:

      *.cpp|*.cc|*.cxx|*.C  Matches some common extensions for C++ source files.

      image.(bmp,gif,jpg)   Matches a file called image given as either bmp, gif, or jpg.

      *.[^o]                Matches any file except object files.

    - You can escape meta characters like '?', '*', '(', ')', '|', '^', '!', and ','
      with the backslash '\'.

    - Match modes:

      MatchFilename     No wildcard can ever match "/" (or "\","/" under Windows).
      MatchNoEscape     Backslashes don't quote special chars ("\" is treated as "\").
      MatchPeriod       Leading "." is matched only explicitly (Useful to match hidden files on Unix).
      MatchLeadingDir   Ignore "/..." after a match.
      MatchCaseFold     Compare without regard to case.

    - Note that under Windows, MatchNoEscape must be passed!

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

  - Perhaps also taking into account certain environment variables in the
    contraction function?

  - Deal with Windows paths "\\?\" long pathname convention.
*/


using namespace FX;

/*******************************************************************************/

namespace FX {


// Return root of given path, including share name or drive letter
FXString FXPath::root(const FXString& file){
  if(!file.empty()){
#ifdef WIN32
    FXString result=file;
    FXint p=0;
    FXint q=0;
    if(ISPATHSEP(result[q])){                                   // UNC
      result[p++]=PATHSEP; q++;
      if(ISPATHSEP(result[q])){
        result[p++]=PATHSEP; q++;
        while(ISPATHSEP(result[q])) q++;
        while(result[q]){
          if(ISPATHSEP(result[q])){ result[p++]=PATHSEP; break; }
          result[p++]=result[q++];
          }
        }
      return result.trunc(p);
      }
    if(Ascii::isLetter(result[q]) && result[q+1]==':'){         // C:
      result[p++]=result[q++]; result[p++]=':'; q++;
      if(ISPATHSEP(result[q])){
        result[p++]=PATHSEP;
        }
      return result.trunc(p);
      }
#else
    if(ISPATHSEP(file[0])){
      return PATHSEPSTRING;
      }
#endif
    }
  return FXString::null;
  }


// Return share name from Windows UNC filename
FXString FXPath::share(const FXString& file){
#ifdef WIN32
  register FXint f,n;
  if(!file.empty()){
    if(ISPATHSEP(file[0])){                                   // UNC
      if(ISPATHSEP(file[1])){
        n=2;
        while(ISPATHSEP(file[n])) n++;
        f=n;
        while(file[n]){
          if(ISPATHSEP(file[n])) break;
          n++;
          }
        return FXString(&file[f],n-f);
        }
      }
    }
#endif
  return FXString::null;
  }


// Return directory part of pathname, assuming full pathname.
// Note that directory("/bla/bla/") is "/bla/bla" and NOT "/bla".
// However, directory("/bla/bla") is "/bla" as we expect!
FXString FXPath::directory(const FXString& file){
  if(!file.empty()){
    FXString result=file;
    FXint p=0,q=0,s;
#ifdef WIN32
    if(ISPATHSEP(result[q])){                                   // UNC
      result[p++]=PATHSEP; q++;
      if(ISPATHSEP(result[q])){
        result[p++]=PATHSEP; q++;
        while(ISPATHSEP(result[q])) q++;
        }
      }
    else if(Ascii::isLetter(result[q]) && result[q+1]==':'){    // C:
      result[p++]=result[q++]; result[p++]=':'; q++;
      if(ISPATHSEP(result[q])){
        result[p++]=PATHSEP; q++;
        while(ISPATHSEP(result[q])) q++;
        }
      }
#else
    if(ISPATHSEP(result[q])){
      result[p++]=PATHSEP; q++;
      while(ISPATHSEP(result[q])) q++;
      }
#endif
    s=p;
    while(result[q]){
      if(ISPATHSEP(result[q])){
        result[s=p++]=PATHSEP;
        while(ISPATHSEP(result[q])) q++;
        continue;
        }
      result[p++]=result[q++];
      }
    return result.trunc(s);
    }
  return FXString::null;
  }


// Return name and extension part of pathname.
// Note that name("/bla/bla/") is "" and NOT "bla".
// However, name("/bla/bla") is "bla" as we expect!
FXString FXPath::name(const FXString& file){
  register FXint f,n;
  if(!file.empty()){
    n=0;
#ifdef WIN32
    if(Ascii::isLetter(file[0]) && file[1]==':') n=2;
#endif
    f=n;
    while(file[n]){
      if(ISPATHSEP(file[n])) f=n+1;
      n++;
      }
    return FXString(&file[f],n-f);
    }
  return FXString::null;
  }


// Return file title, i.e. document name only:
//
//  /path/aa        -> aa
//  /path/aa.bb     -> aa
//  /path/aa.bb.cc  -> aa.bb
//  /path/.aa       -> .aa
FXString FXPath::title(const FXString& file){
  register FXint f,e,b,i;
  if(!file.empty()){
    i=0;
#ifdef WIN32
    if(Ascii::isLetter(file[0]) && file[1]==':') i=2;
#endif
    f=i;
    while(file[i]){
      if(ISPATHSEP(file[i])) f=i+1;
      i++;
      }
    b=f;
    if(file[b]=='.') b++;     // Leading '.'
    e=i;
    while(b<i){
      if(file[--i]=='.'){ e=i; break; }
      }
    return FXString(&file[f],e-f);
    }
  return FXString::null;
  }


// Return extension, if there is one:
//
//  /path/aa        -> ""
//  /path/aa.bb     -> bb
//  /path/aa.bb.cc  -> cc
//  /path/.aa       -> ""
FXString FXPath::extension(const FXString& file){
  register FXint f,e,i,n;
  if(!file.empty()){
    n=0;
#ifdef WIN32
    if(Ascii::isLetter(file[0]) && file[1]==':') n=2;
#endif
    f=n;
    while(file[n]){
      if(ISPATHSEP(file[n])) f=n+1;
      n++;
      }
    if(file[f]=='.') f++;     // Leading '.'
    e=i=n;
    while(f<i){
      if(file[--i]=='.'){ e=i+1; break; }
      }
    return FXString(&file[e],n-e);
    }
  return FXString::null;
  }


// Return file name less the extension
//
//  /path/aa        -> /path/aa
//  /path/aa.bb     -> /path/aa
//  /path/aa.bb.cc  -> /path/aa.bb
//  /path/.aa       -> /path/.aa
FXString FXPath::stripExtension(const FXString& file){
  if(!file.empty()){
    FXString result=file;
    FXint p=0,q=0,s,e;
#ifdef WIN32
    if(ISPATHSEP(result[q])){                                   // UNC
      result[p++]=PATHSEP; q++;
      if(ISPATHSEP(result[q])){
        result[p++]=PATHSEP; q++;
        while(ISPATHSEP(result[q])) q++;
        }
      }
    else if(Ascii::isLetter(result[q]) && result[q+1]==':'){    // C:
      result[p++]=result[q++];
      result[p++]=':'; q++;
      if(ISPATHSEP(result[q])){
        result[p++]=PATHSEP; q++;
        while(ISPATHSEP(result[q])) q++;
        }
      }
#else
    if(ISPATHSEP(result[q])){
      result[p++]=PATHSEP; q++;
      while(ISPATHSEP(result[q])) q++;
      }
#endif
    s=p;
    while(result[q]){
      if(ISPATHSEP(result[q])){
        result[p++]=PATHSEP; s=p;
        while(ISPATHSEP(result[q])) q++;
        continue;
        }
      result[p++]=result[q++];
      }
    if(result[s]=='.') s++;     // Leading '.'
    e=p;
    while(s<p){
      if(result[--p]=='.'){ e=p; break; }
      }
    return result.trunc(e);
    }
  return FXString::null;
  }


#ifdef WIN32

// Return drive letter prefix "c:"
FXString FXPath::drive(const FXString& file){
  FXchar buffer[3];
  if(Ascii::isLetter(file[0]) && file[1]==':'){
    buffer[0]=Ascii::toLower(file[0]);
    buffer[1]=':';
    buffer[2]='\0';
    return FXString(buffer,2);
    }
  return FXString::null;
  }

#else

// Return drive letter prefix "c:"
FXString FXPath::drive(const FXString&){
  return FXString::null;
  }

#endif


// Perform tilde or environment variable expansion
FXString FXPath::expand(const FXString& file){
#ifdef WIN32
  FXString result;
  if(!file.empty()){
    FXString var,val;
    FXint b=0,e;
    while(file[b]){
      if(file[b]=='%'){
        e=file.find('%',b+1);
        if(b<e){
          var=file.mid(b+1,e-b-1);
          val=FXSystem::getEnvironment(var);
          if(!val.empty()){                             // Value found, replace the %VARIABLE% with it
            result.append(val);
            }
          else{                                         // No value; leave %VARIABLE% in place
            result.append(&file[b],e-b+1);
            }
          b=e+1;
          }
        else{                                           // Unmatched '%'; just copy the rest
          result.append(&file[b],file.length()-b);
          b=file.length();
          }
        }
      else{
        e=file.find('%',b+1);
        if(b<e){
          result.append(&file[b],e-b);                  // Copy string up till first '%'
          b=e;
          }
        else{
          result.append(&file[b],file.length()-b);      // Just copy the rest
          b=file.length();
          }
        }
      }
    }
  return result;
#else
  FXString result;
  if(!file.empty()){
    register FXint b,e,n;

    // Expand leading tilde of the form ~/filename or ~user/filename
    n=0;
    if(file[n]=='~'){
      n++;
      b=n;
      while(file[n] && !ISPATHSEP(file[n])) n++;
      e=n;
      result.append(FXSystem::getUserDirectory(file.mid(b,e-b)));
      }

    // Expand environment variables of the form $HOME or ${HOME}
    while(file[n]){
      if(file[n]=='$'){
        n++;
        if(file[n]=='{') n++;
        b=n;
        while(Ascii::isAlphaNumeric(file[n]) || file[n]=='_') n++;
        e=n;
        if(file[n]=='}') n++;
        result.append(FXSystem::getEnvironment(file.mid(b,e-b)));
        continue;
        }
      result.append(file[n]);
      n++;
      }
    }
  return result;
#endif
  }


// Contract path based on environment variables
//
//      /home/jeroen/junk
//      /home/someoneelse/junk
//      /usr/local/ACE_wrappers/TAO
//
//    to:
//
//      ~/junk
//      ~someoneelse/junk
//      $ACE_ROOT/TAO
//
FXString FXPath::contract(const FXString& file,const FXString& user,const FXString& var){
  FXString result=file;
  if(!result.empty()){
    FXString dir=FXSystem::getUserDirectory(user);
    if(compare(result,dir,dir.length())==0){
      result.replace(0,dir.length(),"~"+user);
      }
    dir=FXSystem::getEnvironment(var);
    result.substitute(dir,"$"+var);
    }
  return result;
  }


// Simplify a file path; the path will remain relative if it was relative,
// or absolute if it was absolute.  Also, a trailing "/" will be preserved
// as this is important in other functions.
//
// Examples:
//
//  /aa/bb/../cc     -> /aa/cc
//  /aa/bb/../cc/    -> /aa/cc/
//  /aa/bb/../..     -> /
//  ../../bb         -> ../../bb
//  ../../bb/        -> ../../bb/
//  /../             -> /
//  ./aa/bb/../../   -> ./
//  a/..             -> .
//  a/../            -> ./
//  ./a              -> ./a
//  /////./././      -> /
//  c:/../           -> c:/
//  c:a/..           -> c:
//  /.               -> /
//  /a/b/./          -> /a/b/
//  /a/b/.           -> /a/b
//  /aa/ccc.../../bb -> /aa/bb
FXString FXPath::simplify(const FXString& file){
  if(!file.empty()){
    FXString result=file;
    register FXint p=0;
    register FXint q=0;
    register FXint s;
#ifdef WIN32
    if(ISPATHSEP(result[q])){                                   // UNC
      result[p++]=PATHSEP; q++;
      if(ISPATHSEP(result[q])){
        result[p++]=PATHSEP; q++;
        while(ISPATHSEP(result[q])) q++;
        }
      }
    else if(Ascii::isLetter(result[q]) && result[q+1]==':'){    // C:
      result[p++]=result[q++];
      result[p++]=':'; q++;
      if(ISPATHSEP(result[q])){
        result[p++]=PATHSEP; q++;
        while(ISPATHSEP(result[q])) q++;
        }
      }
#else
    if(ISPATHSEP(result[q])){
      result[p++]=PATHSEP; q++;
      while(ISPATHSEP(result[q])) q++;
      }
#endif
    s=p;
    while(result[q]){
      while(result[q] && !ISPATHSEP(result[q])){
        result[p++]=result[q++];
        }
      if(ISPATHSEP(result[q])){
        result[p++]=PATHSEP; q++;
        while(ISPATHSEP(result[q])) q++;
        }
      if(2<=p && ISPATHSEP(result[p-2]) && result[p-1]=='.'){   // Case "xxx/."
        p--;
        if(s<p) p--;
        }
      else if(3<=p && ISPATHSEP(result[p-3]) && result[p-2]=='.' && ISPATHSEP(result[p-1])){    // Case "xxx/./"
        p-=2;
        }
      else if(3<=p && ISPATHSEP(result[p-3]) && result[p-2]=='.' && result[p-1]=='.' && !(((6<=p && ISPATHSEP(result[p-6])) || 5==p) && result[p-5]=='.' && result[p-4]=='.')){ // Case "xxx/.."
        p-=2;
        if(s<p){                // Pathological case "/.." will become "/"
          p--;
          while(s<p && !ISPATHSEP(result[p-1])) p--;
          if(s<p && ISPATHSEP(result[p-1])) p--;
          if(p==0){                             // Don't allow empty path
            result[p++]='.';
            }
          }
        }
      else if(4<=p && ISPATHSEP(result[p-4]) && result[p-3]=='.' && result[p-2]=='.' && ISPATHSEP(result[p-1]) && !(((7<=p && ISPATHSEP(result[p-7])) || 6==p) && result[p-6]=='.' && result[p-5]=='.')){       // Case "xxx/../"
        p-=3;
        if(s<p){                // Pathological case "/../" will become "/"
          p--;
          while(s<p && !ISPATHSEP(result[p-1])) p--;
          if(p==0){                             // Don't allow empty path
            result[p++]='.';
            result[p++]=PATHSEP;                // Keep trailing "/" around
            }
          }
        }
      }
    return result.trunc(p);
    }
  return FXString::null;
  }


// Build absolute pathname
FXString FXPath::absolute(const FXString& file){
  if(file.empty()) return FXSystem::getCurrentDirectory();
#ifdef WIN32
  if(ISPATHSEP(file[0])){
    if(ISPATHSEP(file[1])) return FXPath::simplify(file);       // UNC
    return FXPath::simplify(FXSystem::getCurrentDrive()+file);
    }
  if(Ascii::isLetter(file[0]) && file[1]==':'){                 // C:
    if(ISPATHSEP(file[2])) return FXPath::simplify(file);
    return FXPath::simplify(file.mid(0,2)+PATHSEPSTRING+file.mid(2,2147483647));
    }
#else
  if(ISPATHSEP(file[0])) return FXPath::simplify(file);
#endif
  return FXPath::simplify(FXSystem::getCurrentDirectory()+PATHSEPSTRING+file);
  }


// Build absolute pathname from parts
FXString FXPath::absolute(const FXString& base,const FXString& file){
  if(file.empty()) return FXPath::absolute(base);
#ifdef WIN32
  if(ISPATHSEP(file[0])){
    if(ISPATHSEP(file[1])) return FXPath::simplify(file);       // UNC
    return FXPath::simplify(FXSystem::getCurrentDrive()+file);
    }
  if(Ascii::isLetter(file[0]) && file[1]==':'){                 // C:
    if(ISPATHSEP(file[2])) return FXPath::simplify(file);
    return FXPath::simplify(file.mid(0,2)+PATHSEPSTRING+file.mid(2,2147483647));
    }
#else
  if(ISPATHSEP(file[0])) return FXPath::simplify(file);
#endif
  return FXPath::simplify(FXPath::absolute(base)+PATHSEPSTRING+file);
  }


// Return relative path of file to given base directory
//
// Examples:
//
//  Base       File         Result      Comment
//  /          /a/b         /a/b        Branch point is /
//  /p/q/r     /a/b/c       /a/b/c      Branch point is /
//  /a/b/c     /a/b/c/d     d           Branch point is /a/b/c
//  /a/b/c/    /a/b/c/d     d           Branch point is /a/b/c
//  /a/b/c/d   /a/b/c       ../         Branch point is /a/b/c
//  /a/b/c/d   /a/b/q       ../../q     Branch point is /a/b
//  /a/b/c     /a/b/c       .           Equal
//  /a/b/c/    /a/b/c/      .           Equal
//  ../a/b/c   ../a/b/c/d   d           Branch point is ../a/b/c
//  ./a        ./b          ../b        Branch point assumed to be ..
//  a          b            ../b        Branch point assumed to be ..
FXString FXPath::relative(const FXString& base,const FXString& file){
  if(!base.empty() && !FXPath::isTopDirectory(base)){
    register FXint p=0,q=0,bp=0,bq=0;

    // Find branch point
#ifdef WIN32
    while(base[p] && ((Ascii::toLower(base[p])==Ascii::toLower(file[q])) || (ISPATHSEP(base[p]) && ISPATHSEP(file[q])))){
      if(ISPATHSEP(base[p])){
        bp=p;
        bq=q;
        while(0<p && ISPATHSEP(base[p+1])) p++;           // Eat multiple slashes, but not the UNC "\\" at the start
        while(0<q && ISPATHSEP(file[q+1])) q++;
        }
      p++;
      q++;
      }
#else
    while(base[p] && (base[p]==file[q])){
      if(ISPATHSEP(base[p])){
        bp=p;
        bq=q;
        while(ISPATHSEP(base[p+1])) p++;                  // Eat multiple slashes
        while(ISPATHSEP(file[q+1])) q++;
        }
      p++;
      q++;
      }
#endif

    // Common prefix except for trailing path separator
    if((base[p]=='\0' || ISPATHSEP(base[p])) && (file[q]=='\0' || ISPATHSEP(file[q]))){
      bp=p;
      bq=q;
      }

    // If branch point is not root
#ifdef WIN32
    if(!((ISPATHSEP(base[0]) && (bp==0 || (ISPATHSEP(base[1]) && bp==1))) || (Ascii::isLetter(base[0]) && base[1]==':' && (bp==1 || (ISPATHSEP(base[2]) && bp==2))))){
#else
    if(!(ISPATHSEP(base[0]) && bp==0)){
#endif

      // Strip leading path character off, if any
      while(ISPATHSEP(file[bq])) bq++;

      // Non trivial
      if(file[bq]){
        FXString result;

        // Up to branch point
        while(base[bp]){
          while(ISPATHSEP(base[bp])) bp++;
          if(base[bp]){
            result.append(".." PATHSEPSTRING);
            while(base[bp] && !ISPATHSEP(base[bp])) bp++;
            }
          }

        // Append tail end
        result.append(&file[bq]);
        return result;
        }
      return ".";
      }
    }
  return file;
  }


// Return relative path of file to the current directory
FXString FXPath::relative(const FXString& file){
  return FXPath::relative(FXSystem::getCurrentDirectory(),file);
  }


// Return true if file is inside base directory
FXbool FXPath::isInside(const FXString& base,const FXString& file){
  register FXint p=0,q=0;
#ifdef WIN32
  while(base[p] && ((Ascii::toLower(base[p])==Ascii::toLower(file[q])) || (ISPATHSEP(base[p]) && ISPATHSEP(file[q])))){
    if(ISPATHSEP(base[p])){
      while(0<p && ISPATHSEP(base[p+1])) p++;           // Eat multiple slashes, but not the UNC "\\" at the start
      while(0<q && ISPATHSEP(file[q+1])) q++;
      }
    p++;
    q++;
    }
#else
  while(base[p] && (base[p]==file[q])){
    if(ISPATHSEP(base[p])){
      while(ISPATHSEP(base[p+1])) p++;                  // Eat multiple slashes
      while(ISPATHSEP(file[q+1])) q++;
      }
    p++;
    q++;
    }
#endif
  return base[p]=='\0' && (ISPATHSEP(file[q]) || file[q]=='\0');
  }


// Convert path from using 'sepfm' to use 'septo' path-separators
FXString FXPath::convert(const FXString& file,FXchar septo,FXchar sepfm){
  if(!file.empty()){
    FXString result(file);
    FXint p=0,q=0;
#ifdef WIN32
    if(result[q]==sepfm || result[q]==septo){                   // UNC
      result[p++]=septo; q++;
      if(result[q]==sepfm || result[q]==septo){
        result[p++]=septo; q++;
        while(result[q]==sepfm || result[q]==septo) q++;
        }
      }
    else if(Ascii::isLetter(result[q]) && result[q+1]==':'){    // C:
      result[p++]=result[q++];
      result[p++]=':'; q++;
      if(result[q]==sepfm || result[q]==septo){
        result[p++]=septo; q++;
        while(result[q]==sepfm || result[q]==septo) q++;
        }
      }
#else
    if(result[q]==sepfm || result[q]==septo){
      result[p++]=septo; q++;
      while(result[q]==sepfm || result[q]==septo) q++;
      }
#endif
    while(result[q]){
      if(result[q]==sepfm || result[q]==septo){
        result[p++]=septo; q++;
        while(result[q]==sepfm || result[q]==septo) q++;
        continue;
        }
      result[p++]=result[q++];
      }
    return result.trunc(p);
    }
  return FXString::null;
  }


// Up one level, given absolute path
FXString FXPath::upLevel(const FXString& file){
  if(!file.empty()){
    FXString result=file;
    FXint p=0,q=0,s;
#ifdef WIN32
    if(ISPATHSEP(result[q])){                                   // UNC
      result[p++]=PATHSEP; q++;
      if(ISPATHSEP(result[q])){
        result[p++]=PATHSEP; q++;
        while(ISPATHSEP(result[q])) q++;
        }
      }
    else if(Ascii::isLetter(result[q]) && result[q+1]==':'){    // C:
      result[p++]=result[q++];
      result[p++]=':'; q++;
      if(ISPATHSEP(result[q])){
        result[p++]=PATHSEP; q++;
        while(ISPATHSEP(result[q])) q++;
        }
      }
#else
    if(ISPATHSEP(result[q])){
      result[p++]=PATHSEP; q++;
      while(ISPATHSEP(result[q])) q++;
      }
#endif
    s=p;
    while(result[q]){
      if(ISPATHSEP(result[q])){
        result[p++]=PATHSEP; q++;
        while(ISPATHSEP(result[q])) q++;
        if(result[q]) s=p-1;
        continue;
        }
      result[p++]=result[q++];
      }
    return result.trunc(s);
    }
  return PATHSEPSTRING;
  }


// Check if file represents absolute pathname
FXbool FXPath::isAbsolute(const FXString& file){
#ifdef WIN32
  return ISPATHSEP(file[0]) || (Ascii::isLetter(file[0]) && file[1]==':');
#else
  return ISPATHSEP(file[0]);
#endif
  }


// Does file represent topmost directory
FXbool FXPath::isTopDirectory(const FXString& file){
#ifdef WIN32
  return (ISPATHSEP(file[0]) && (file[1]=='\0' || (ISPATHSEP(file[1]) && file[2]=='\0'))) || (Ascii::isLetter(file[0]) && file[1]==':' && (file[2]=='\0' || (ISPATHSEP(file[2]) && file[3]=='\0')));
#else
  return ISPATHSEP(file[0]) && file[1]=='\0';
#endif
  }


#ifdef WIN32

// Check if file represents a file share
FXbool FXPath::isShare(const FXString& file){
  return ISPATHSEP(file[0]) && ISPATHSEP(file[1]) && file.find(PATHSEP,2)<0;
  }

#else

// Check if file represents a file share
FXbool FXPath::isShare(const FXString&){
  return false;
  }

#endif


#ifdef WIN32                 // WINDOWS

// Enquote filename to make safe for shell
FXString FXPath::enquote(const FXString& file,FXbool forcequotes){
  FXString result;
  register FXint i,c;
  for(i=0; (c=file[i])!='\0'; i++){
    switch(c){
      case '<':               // Redirections
      case '>':
      case '|':
      case '$':
      case ':':
      case '*':               // Wildcards
      case '?':
      case ' ':               // White space
        forcequotes=true;
      default:                // Normal characters just added
        result+=c;
        break;
      }
    }
  if(forcequotes) return "\""+result+"\"";
  return result;
  }


// Decode filename to get original again
FXString FXPath::dequote(const FXString& file){
  register FXint i,c;
  FXString result;
  i=0;
  while((c=file[i])!='\0' && Ascii::isSpace(c)) i++;
  if(file[i]=='"'){
    i++;
    while((c=file[i])!='\0' && c!='"'){
      result+=c;
      i++;
      }
    }
  else{
    while((c=file[i])!='\0' && !Ascii::isSpace(c)){
      result+=c;
      i++;
      }
    }
  return result;
  }


#else                         // UNIX


// Enquote filename to make safe for shell
FXString FXPath::enquote(const FXString& file,FXbool forcequotes){
  FXString result;
  register FXint i,c;
  for(i=0; (c=file[i])!='\0'; i++){
    switch(c){
      case '\'':              // Quote needs to be escaped
        result+="\\\'";
        break;
      case '\\':              // Backspace needs to be escaped, of course
        result+="\\\\";
        break;
      case '#':
      case '~':
        if(i) goto noquote;   // Only quote if at begin of filename
      case '!':               // Special in csh
      case '"':
      case '$':               // Variable substitution
      case '&':
      case '(':
      case ')':
      case ';':
      case '<':               // Redirections, pipe
      case '>':
      case '|':
      case '`':               // Command substitution
      case '^':               // Special in sh
      case '*':               // Wildcard characters
      case '?':
      case '[':
      case ']':
      case '\t':              // White space
      case '\n':
      case ' ':
        forcequotes=true;
      default:                // Normal characters just added
noquote:result+=c;
        break;
      }
    }
  if(forcequotes) return "'"+result+"'";
  return result;
  }


// Decode filename to get original again
FXString FXPath::dequote(const FXString& file){
  FXString result;
  register FXint i,c;
  i=0;
  while((c=file[i])!='\0' && Ascii::isSpace(c)) i++;
  if(file[i]=='\''){
    i++;
    while((c=file[i])!='\0' && c!='\''){
      if(c=='\\' && file[i+1]!='\0') c=file[++i];
      result+=c;
      i++;
      }
    }
  else{
    while((c=file[i])!='\0' && !Ascii::isSpace(c)){
      if(c=='\\' && file[i+1]!='\0') c=file[++i];
      result+=c;
      i++;
      }
    }
  return result;
  }

#endif


/*
FIXME
  NameOnly
  NoEscape
  CaseFold
  DotFile
  DirName
  
    PathName   = 1,        /// No wildcard can ever match "/'
    NoEscape   = 2,        /// Backslashes don't quote special chars
    DotFile    = 4,        /// Leading "." is matched only explicitly
    LeadDir    = 8,        /// Ignore "/..." after a match
    CaseFold   = 16        /// Compare without regard to case

Need to update to match UTF-8
*/

// If folding case, make lower case
#define FOLD(c) ((flags&FXPath::CaseFold)?Ascii::toLower(c):(c))


// Perform match
static FXbool domatch(const FXchar *string,const FXchar *pattern,FXuint flags){
  register const FXchar *s=string;
  register const FXchar *p=pattern;
  register const FXchar *r;
  register FXchar c,cs,ce,cc,neg;
  register FXint level;
  while((c=*p++)!='\0'){
    switch(c){
      case '?':
        if(*s=='\0') return false;
        if((flags&FXPath::PathName) && ISPATHSEP(*s)) return false;
        if((flags&FXPath::DotFile) && (*s=='.') && ((s==string) || ((flags&FXPath::PathName) && ISPATHSEP(*(s-1))))) return false;
        s++;
        break;
      case '*':
        c=*p;
        while(c=='*') c=*++p;
        if((flags&FXPath::DotFile) && (*s=='.') && ((s==string) || ((flags&FXPath::PathName) && ISPATHSEP(*(s-1))))) return false;
        if(c=='\0'){    // Optimize for case of trailing '*'
          if(flags&FXPath::PathName){ for(r=s; *r; r++){ if(ISPATHSEP(*r)) return false; } }
          return true;
          }
        while(!domatch(s,p,flags&~FXPath::DotFile)){
          if((flags&FXPath::PathName) && ISPATHSEP(*s)) return false;
          if(*s++=='\0') return false;
          }
        return true;
      case '[':
        if(*s=='\0') return false;
        if((flags&FXPath::DotFile) && (*s=='.') && ((s==string) || ((flags&FXPath::PathName) && ISPATHSEP(*(s-1))))) return false;
        cc=FOLD(*s);
        neg=((*p=='!') || (*p=='^'));
        if(neg) p++;
        c=*p++;
        do{
          if(c=='\\' && !(flags&FXPath::NoEscape)) c=*p++;
          cs=ce=FOLD(c);
          if(c=='\0') return false;
          c=*p++;
          c=FOLD(c);
          if((flags&FXPath::PathName) && ISPATHSEP(c)) return false;
          if(c=='-' && *p!=']'){
            c = *p++;
            if(c=='\\' && !(flags&FXPath::NoEscape)) c=*p++;
            if(c=='\0') return false;
            ce=FOLD(c);
            c=*p++;
            }
          if(((FXuchar)cs)<=((FXuchar)cc) && ((FXuchar)cc)<=((FXuchar)ce)) goto match;
          }
        while(c!=']');
        if(!neg) return false;
        s++;
        break;
match:  while(c!=']'){
          if(c=='\0') return false;
          c=*p++;
          if(c=='\\' && !(flags&FXPath::NoEscape)) p++;
          }
        if(neg) return false;
        s++;
        break;
      case '(':
nxt:    if(domatch(s,p,flags)) return true;
        for(level=0; *p && 0<=level; ){
          switch(*p++){
            case '\\': if(*p) p++; break;
            case '(': level++; break;
            case ')': level--; break;
            case '|':
            case ',': if (level==0) goto nxt;
            }
          }
        return false;
      case ')':
        break;
      case '|':
      case ',':
        for(level=0; *p && 0<=level; ){
          switch(*p++){
            case '\\': if(*p) p++; break;
            case '(': level++; break;
            case ')': level--; break;
            }
          }
        break;
      case '\\':
        if(*p && !(flags&FXPath::NoEscape)) c=*p++;   // Trailing escape represents itself
      default:
        if(FOLD(c)!=FOLD(*s)) return false;
        s++;
        break;
      }
    }
  return (*s=='\0') || (ISPATHSEP(*s) && (flags&FXPath::LeadDir));
  }


// Match filename against pattern (like *, ?, [^a-z], and so on)
FXbool FXPath::match(const FXchar *string,const FXchar *pattern,FXuint flags){
  register const FXchar *s=string;
  register const FXchar *p=pattern;
  register FXint level;
  if(s && p){
nxt:if(domatch(s,p,flags)) return true;
    for(level=0; *p && 0<=level; ){
      switch(*p++){
        case '\\': if(*p) p++; break;
        case '(': level++; break;
        case ')': level--; break;
        case '|':
        case ',': if (level==0) goto nxt;
        }
      }
    }
  return false;
  }


// Match filename against pattern (like *, ?, [^a-z], and so on)
FXbool FXPath::match(const FXString& file,const FXchar *pattern,FXuint flags){
  return FXPath::match(file.text(),pattern,flags);
  }


// Match filename against pattern (like *, ?, [^a-z], and so on)
FXbool FXPath::match(const FXString& file,const FXString& pattern,FXuint flags){
  return FXPath::match(file.text(),pattern.text(),flags);
  }


// Generate unique filename of the form pathnameXXX.ext, where
// pathname.ext is the original input file, and XXX is a number,
// possibly empty, that makes the file unique.
// (From: Mathew Robertson <mathew.robertson@mi-services.com>)
FXString FXPath::unique(const FXString& file){
  if(!FXStat::exists(file)) return file;
  FXString ext=FXPath::extension(file);
  FXString path=FXPath::stripExtension(file);           // Use the new API (Jeroen)
  FXString filename;
  register FXint count=0;
  if(!ext.empty()) ext.prepend('.');            // Only add period when non-empty extension
  while(count<1000){
    filename.format("%s%i%s",path.text(),count,ext.text());
    if(!FXStat::exists(filename)) return filename;      // Return result here (Jeroen)
    count++;
    }
  return FXString::null;
  }


// Search pathlist for file
FXString FXPath::search(const FXString& pathlist,const FXString& file){
  if(!file.empty()){
    FXString path;
    FXint beg,end;
#ifdef WIN32
    if(ISPATHSEP(file[0])){
      if(ISPATHSEP(file[1])){
        if(FXStat::exists(file)) return file;           // UNC
        return FXString::null;
        }
      path=FXSystem::getCurrentDrive()+file;
      if(FXStat::exists(path)) return path;
      return FXString::null;
      }
    if(Ascii::isLetter(file[0]) && file[1]==':'){       // C:
      if(FXStat::exists(file)) return file;
      return FXString::null;
      }
#else
    if(ISPATHSEP(file[0])){
      if(FXStat::exists(file)) return file;
      return FXString::null;
      }
#endif
    for(beg=0; pathlist[beg]; beg=end){
      while(pathlist[beg]==PATHLISTSEP) beg++;
      for(end=beg; pathlist[end] && pathlist[end]!=PATHLISTSEP; end++){}
      if(beg==end) break;
      path=FXPath::absolute(FXPath::expand(pathlist.mid(beg,end-beg)),file);
      if(FXStat::exists(path)) return path;
      }
    }
  return FXString::null;
  }


}

