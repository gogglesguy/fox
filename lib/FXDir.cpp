/********************************************************************************
*                                                                               *
*                    D i r e c t o r y   E n u m e r a t o r                    *
*                                                                               *
*********************************************************************************
* Copyright (C) 2005,2026 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "fxmath.h"
#include "FXElement.h"
#include "FXString.h"
#include "FXIO.h"
#include "FXStat.h"
#include "FXFile.h"
#include "FXPath.h"
#include "FXDir.h"

/*
  Notes:
  - This class implements a way to list the files in a directory.
  - We just want to wrap directory iteration, nothing fancy.
  - Maybe add positioning for seek and tell type functions.
*/

#define TOPIC_CONSTRUCT 1000
#define TOPIC_CREATION  1001
#define TOPIC_DETAIL    1002

using namespace FX;

/*******************************************************************************/

namespace FX {


#ifdef WIN32
struct SPACE {
  HANDLE          handle;
  FXuint          first;
  WIN32_FIND_DATA result;
  };
#else
struct SPACE {
  DIR*            handle;
  struct dirent*  dp;
  };
#endif


// Construct directory enumerator
FXDir::FXDir(){
  // If this fails on your machine, determine what sizeof(SPACE) is
  // on your machine and mail it to: jeroen@fox-toolkit.net!
  //FXTRACE(TOPIC_DETAIL,"sizeof(SPACE)=%ld\n",sizeof(SPACE));
  FXASSERT(sizeof(SPACE)<=sizeof(space));
#ifdef WIN32
  alias_cast<SPACE>(space)->handle=INVALID_HANDLE_VALUE;
#else
  alias_cast<SPACE>(space)->handle=nullptr;
#endif
  }


// Construct directory enumerator
FXDir::FXDir(const FXString& path){
  // If this fails on your machine, determine what sizeof(SPACE) is
  // on your machine and mail it to: jeroen@fox-toolkit.net!
  //FXTRACE(TOPIC_DETAIL,"sizeof(SPACE)=%ld\n",sizeof(SPACE));
  FXASSERT(sizeof(SPACE)<=sizeof(space));
#ifdef WIN32
  alias_cast<SPACE>(space)->handle=INVALID_HANDLE_VALUE;
#else
  alias_cast<SPACE>(space)->handle=nullptr;
#endif
  open(path);
  }


// Open directory to path, return true if ok.
FXbool FXDir::open(const FXString& path){
  if(!path.empty()){
#ifdef WIN32
#ifdef UNICODE
    FXnchar buffer[MAXPATHLEN+2];
    utf2ncs(buffer,path.text(),MAXPATHLEN);
    wcsncat(buffer,TEXT("\\*"),MAXPATHLEN+2);
#else
    FXchar buffer[MAXPATHLEN+2];
    fxstrlcpy(buffer,path.text(),MAXPATHLEN);
    fxstrlcat(buffer,"\\*",MAXPATHLEN+2);
#endif
    alias_cast<SPACE>(space)->handle=FindFirstFile(buffer,&alias_cast<SPACE>(space)->result);
    if(alias_cast<SPACE>(space)->handle!=INVALID_HANDLE_VALUE){
      alias_cast<SPACE>(space)->first=true;
      return true;
      }
#else
    alias_cast<SPACE>(space)->handle=opendir(path.text());
    if(alias_cast<SPACE>(space)->handle){
      return true;
      }
#endif
    }
  return false;
  }


// Returns true if the directory is open
FXbool FXDir::isOpen() const {
#ifdef WIN32
  return (alias_cast<const SPACE>(space)->handle!=INVALID_HANDLE_VALUE);
#else
  return alias_cast<const SPACE>(space)->handle!=nullptr;
#endif
  }


// Go to next directory entry and return its name
FXbool FXDir::next(FXString& name){
  if(isOpen()){
#if defined(WIN32)
    if(alias_cast<SPACE>(space)->first || FindNextFile(alias_cast<SPACE>(space)->handle,&alias_cast<SPACE>(space)->result)){
      alias_cast<SPACE>(space)->first=false;
      name.assign(alias_cast<SPACE>(space)->result.cFileName);
      return true;
      }
#else
    if((alias_cast<SPACE>(space)->dp=readdir(alias_cast<SPACE>(space)->handle))!=nullptr){
      name.assign(alias_cast<SPACE>(space)->dp->d_name);
      return true;
      }
#endif
    }
  name.clear();
  return false;
  }


// Go to next directory entry and return its name and mode flags
FXbool FXDir::next(FXString& name,FXuint& mode){
  if(isOpen()){
#if defined(_WIN32)
    if(alias_cast<SPACE>(space)->first || FindNextFile(alias_cast<SPACE>(space)->handle,&alias_cast<SPACE>(space)->result)){
      alias_cast<SPACE>(space)->first=false;
      name.assign(alias_cast<SPACE>(space)->result.cFileName);
      mode=FXIO::AllFull;
      if(alias_cast<SPACE>(space)->result.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) mode|=FXIO::Directory; else mode|=FXIO::File;
      if(alias_cast<SPACE>(space)->result.dwFileAttributes&FILE_ATTRIBUTE_HIDDEN) mode|=FXIO::Hidden;
      if(alias_cast<SPACE>(space)->result.dwFileAttributes&FILE_ATTRIBUTE_DEVICE) mode|=FXIO::Device;
      if(alias_cast<SPACE>(space)->result.dwFileAttributes&FILE_ATTRIBUTE_READONLY) mode&=~FXIO::AllWrite;
      return true;
      }
#else
    if((alias_cast<SPACE>(space)->dp=readdir(alias_cast<SPACE>(space)->handle))!=nullptr){
      name.assign(alias_cast<SPACE>(space)->dp->d_name);
      mode=0;
#if defined(_DIRENT_HAVE_D_TYPE)
      switch(alias_cast<SPACE>(space)->dp->d_type){
        case DT_REG: mode|=FXIO::File; break;
        case DT_DIR: mode|=FXIO::Directory; break;
        case DT_LNK: mode|=FXIO::SymLink; break;
        case DT_FIFO:mode|=FXIO::Fifo; break;
        case DT_SOCK:mode|=FXIO::Socket; break;
        case DT_BLK: mode|=FXIO::Block; break;
        case DT_CHR: mode|=FXIO::Character; break;
        case DT_UNKNOWN: break;
        }
#endif
      return true;
      }
#endif
    }
  name=FXString::null;
  mode=0;
  return false;
  }


// Close directory
void FXDir::close(){
  if(isOpen()){
#ifdef WIN32
    FindClose(alias_cast<SPACE>(space)->handle);
    alias_cast<SPACE>(space)->handle=INVALID_HANDLE_VALUE;
#else
    closedir(alias_cast<SPACE>(space)->handle);
    alias_cast<SPACE>(space)->handle=nullptr;
#endif
    }
  }


// Create new directory
FXbool FXDir::create(const FXString& path,FXuint perm){
  if(!path.empty()){
#ifdef WIN32
#ifdef UNICODE
    FXnchar buffer[MAXPATHLEN];
    utf2ncs(buffer,path.text(),MAXPATHLEN);
    return CreateDirectoryW(buffer,nullptr)!=0;
#else
    return CreateDirectoryA(path.text(),nullptr)!=0;
#endif
#else
    return ::mkdir(path.text(),perm)==0;
#endif
    }
  return false;
  }


// Remove directory
FXbool FXDir::remove(const FXString& path){
  if(!path.empty()){
#ifdef WIN32
#ifdef UNICODE
    FXnchar buffer[MAXPATHLEN];
    utf2ncs(buffer,path.text(),MAXPATHLEN);
    return RemoveDirectoryW(buffer)!=0;
#else
    return RemoveDirectoryA(path.text())!=0;
#endif
#else
    return ::rmdir(path.text())==0;
#endif
    }
  return false;
  }


// List all the files in directory
FXint FXDir::listFiles(FXString*& filelist,const FXString& path,const FXString& pattern,FXuint flags){
  FXDir dir(path);

  // Initialize to empty
  filelist=nullptr;

  // Get directory stream pointer
  if(dir.isOpen()){
    FXuint    matching=(flags&CaseFold)?(FXPath::PathName|FXPath::NoEscape|FXPath::CaseFold):(FXPath::PathName|FXPath::NoEscape);
    FXString *newlist;
    FXint     size=0;
    FXint     count=0;
    FXString  pathname;
    FXString  name;
    FXStat    data;

    // Loop over directory entries
    while(dir.next(name)){

      // Build full pathname
      pathname=path;
      if(!ISPATHSEP(pathname.tail())) pathname+=PATHSEPSTRING;
      pathname+=name;

      // Get info on file
      if(!FXStat::statFile(pathname,data)) continue;

#ifdef WIN32

      // Filter out files; a bit tricky...
      if(!data.isDirectory() && ((flags&NoFiles) || (!(flags&HiddenFiles) && data.isHidden()) || (!(flags&AllFiles) && !FXPath::match(name,pattern,matching)))) continue;

      // Filter out directories; even more tricky!
      if(data.isDirectory() && ((flags&NoDirs) || ((flags&NoParent) && (name[0]=='.' && (name[1]==0 || (name[1]=='.' && name[2]==0)))) || (!(flags&HiddenDirs) && data.isHidden()) || (!(flags&AllDirs) && !FXPath::match(name,pattern,matching)))) continue;

#else

      // Filter out files; a bit tricky...
      if(!data.isDirectory() && ((flags&NoFiles) || (!(flags&HiddenFiles) && name[0]=='.') || (!(flags&AllFiles) && !FXPath::match(name,pattern,matching)))) continue;

      // Filter out directories; even more tricky!
      if(data.isDirectory() && ((flags&NoDirs) || ((flags&NoParent) && (name[0]=='.' && (name[1]==0 || (name[1]=='.' && name[2]==0)))) || (!(flags&HiddenDirs) && name[0]=='.') || (!(flags&AllDirs) && !FXPath::match(name,pattern,matching)))) continue;

#endif

      // Grow list
      if(count+1>=size){
        size=size?(size<<1):256;
        newlist=new FXString [size];
        for(FXint i=0; i<count; i++){
          newlist[i].adopt(filelist[i]);
          }
        delete [] filelist;
        filelist=newlist;
        }

      // Add to list
      filelist[count++].adopt(name);
      }
    return count;
    }
  return 0;
  }


// List all the files in directory
FXint FXDir::removeFiles(const FXString& path,const FXString& pattern,FXuint flags){
  FXDir dir(path);
  if(dir.isOpen()){
    FXuint    matching=(flags&CaseFold)?(FXPath::PathName|FXPath::NoEscape|FXPath::CaseFold):(FXPath::PathName|FXPath::NoEscape);
    FXint     count=0;
    FXString  pathname;
    FXString  name;
    FXStat    data;

    // Loop over directory entries
    while(dir.next(name)){

      // Build full pathname
      pathname=path;
      if(!ISPATHSEP(pathname.tail())) pathname+=PATHSEPSTRING;
      pathname+=name;

      // Get info on file
      if(!FXStat::statFile(pathname,data)) continue;

#ifdef WIN32

      // Filter out files; a bit tricky...
      if(!data.isDirectory() && ((flags&NoFiles) || (!(flags&HiddenFiles) && data.isHidden()) || (!(flags&AllFiles) && !FXPath::match(name,pattern,matching)))) continue;

      // Filter out directories; even more tricky!
      if(data.isDirectory() && ((flags&NoDirs) || ((flags&NoParent) && (name[0]=='.' && (name[1]==0 || (name[1]=='.' && name[2]==0)))) || (!(flags&HiddenDirs) && data.isHidden()) || (!(flags&AllDirs) && !FXPath::match(name,pattern,matching)))) continue;

#else

      // Filter out files; a bit tricky...
      if(!data.isDirectory() && ((flags&NoFiles) || (!(flags&HiddenFiles) && name[0]=='.') || (!(flags&AllFiles) && !FXPath::match(name,pattern,matching)))) continue;

      // Filter out directories; even more tricky!
      if(data.isDirectory() && ((flags&NoDirs) || ((flags&NoParent) && (name[0]=='.' && (name[1]==0 || (name[1]=='.' && name[2]==0)))) || (!(flags&HiddenDirs) && name[0]=='.') || (!(flags&AllDirs) && !FXPath::match(name,pattern,matching)))) continue;

#endif

      // Remove file or folder, recursively
      if(FXFile::removeFiles(pathname,true)){
        count++;
        }
      }
    return count;
    }
  return 0;
  }


// List drives, i.e. roots of directory trees.
FXint FXDir::listDrives(FXString*& drivelist){
  FXint count=0;
#ifdef WIN32
  FXnchar drives[256];
  if(GetLogicalDriveStringsW(ARRAYNUMBER(drives),drives)){
    drivelist=new FXString[33];
    for(const FXnchar* drive=drives; *drive && count<32; drive++){
      drivelist[count].assign(drive);
      while(*drive) drive++;
      count++;
      }
    }
#else
  drivelist=new FXString[2];
  drivelist[count++].assign(PATHSEP);
#endif
  return count;
  }


// List file shares
FXint FXDir::listShares(FXString*& sharelist){
#ifdef WIN32
  FXint count=0;
  HANDLE handle;
/*
  NETRESOURCEW host={
     RESOURCE_GLOBALNET,
     RESOURCETYPE_DISK,
     RESOURCEDISPLAYTYPE_SERVER,
     RESOURCEUSAGE_CONNECTABLE,
     nullptr,
     nullptr,
     nullptr,
     nullptr
     };
*/
  // Open network enumeration
  sharelist=nullptr;
  if(WNetOpenEnum(RESOURCE_CONNECTED,RESOURCETYPE_DISK,0,nullptr,&handle)==NO_ERROR){
    NETRESOURCEW  resource[16384+sizeof(NETRESOURCEW)-1/sizeof(NETRESOURCEW)];
    while(1){
      DWORD size=sizeof(resource);
      DWORD entries=-1;

      // Zero buffer
      clearElms(resource,2048);

      // Next batch of NETRESOURCE structs
      DWORD result=WNetEnumResourceW(handle,&entries,resource,&size);

      FXTRACE(TOPIC_DETAIL,"WNetEnumResourceW=%d\n",result);

      if(result==ERROR_NO_NETWORK){ break; }            // No network available
      if(result==ERROR_MORE_DATA){ break; }             // Buffer was too small; size has required size
      if(result==ERROR_NO_MORE_ITEMS){ break; }         // No more items
      if(result==ERROR_INVALID_HANDLE){ break; }        // Invalid handle
      if(result==ERROR_EXTENDED_ERROR){ break; }        // Extended error
      if(result!=NO_ERROR) break;

      // List needs to grow
      if(0<entries){
        FXString* oldlist=sharelist;
        sharelist=new FXString [count+entries+1];
        for(FXint j=0; j<count; j++){
          sharelist[j].adopt(oldlist[j]);
          }
        delete [] oldlist;
        }

      // Pile on newly found entries
      for(DWORD i=0; i<entries; i++){

        // Dump what we found
        // struct NETRESOURCEA {
        //   DWORD dwScope;           // Indicates the scope of the enumeration
        //   DWORD dwType;            // Indicates the resource type
        //   DWORD dwDisplayType;     // Set by the provider to indicate what display type a user interface
        //   DWORD dwUsage;           // A bitmask that indicates how you can enumerate information
        //   LPSTR lpLocalName;       // Contains the name of a redirected device.
        //   LPSTR lpRemoteName;      // Contains a remote network name.
        //   LPSTR lpComment;         // Provider-supplied comment associated with item
        //   LPSTR lpProvider;        // Name of the provider
        //   };
        FXTRACE(TOPIC_DETAIL,"dwScope=%s\n",resource[i].dwScope==RESOURCE_CONNECTED?"RESOURCE_CONNECTED":resource[i].dwScope==RESOURCE_GLOBALNET?"RESOURCE_GLOBALNET":resource[i].dwScope==RESOURCE_REMEMBERED?"RESOURCE_REMEMBERED":"?");
        FXTRACE(TOPIC_DETAIL,"dwType=%s\n",resource[i].dwType==RESOURCETYPE_ANY?"RESOURCETYPE_ANY":resource[i].dwType==RESOURCETYPE_DISK?"RESOURCETYPE_DISK":resource[i].dwType==RESOURCETYPE_PRINT?"RESOURCETYPE_PRINT":"?");
        FXTRACE(TOPIC_DETAIL,"dwDisplayType=%s\n",resource[i].dwDisplayType==RESOURCEDISPLAYTYPE_DOMAIN?"RESOURCEDISPLAYTYPE_DOMAIN":resource[i].dwDisplayType==RESOURCEDISPLAYTYPE_SERVER?"RESOURCEDISPLAYTYPE_SERVER":resource[i].dwDisplayType==RESOURCEDISPLAYTYPE_SHARE?"RESOURCEDISPLAYTYPE_SHARE":resource[i].dwDisplayType==RESOURCEDISPLAYTYPE_GENERIC?"RESOURCEDISPLAYTYPE_GENERIC":resource[i].dwDisplayType==6?"RESOURCEDISPLAYTYPE_NETWORK":resource[i].dwDisplayType==7?"RESOURCEDISPLAYTYPE_ROOT":resource[i].dwDisplayType==8?"RESOURCEDISPLAYTYPE_SHAREADMIN":resource[i].dwDisplayType==9?"RESOURCEDISPLAYTYPE_DIRECTORY":resource[i].dwDisplayType==10?"RESOURCEDISPLAYTYPE_TREE":resource[i].dwDisplayType==11?"RESOURCEDISPLAYTYPE_NDSCONTAINER":"?");
        FXTRACE(TOPIC_DETAIL,"dwUsage=%s\n",resource[i].dwUsage==RESOURCEUSAGE_CONNECTABLE?"RESOURCEUSAGE_CONNECTABLE":resource[i].dwUsage==RESOURCEUSAGE_CONTAINER?"RESOURCEUSAGE_CONTAINER":"?");
        FXTRACE(TOPIC_DETAIL,"lpLocalName=%s\n",resource[i].lpLocalName);
        FXTRACE(TOPIC_DETAIL,"lpRemoteName=%s\n",resource[i].lpRemoteName);
        FXTRACE(TOPIC_DETAIL,"lpComment=%s\n",resource[i].lpComment);
        FXTRACE(TOPIC_DETAIL,"lpProvider=%s\n\n",resource[i].lpProvider);

        // Add remote name to list
        sharelist[count++]=resource[i].lpRemoteName;
        }
      }
    WNetCloseEnum(handle);
    }
  return count;
#else
  sharelist=new FXString [2];
  sharelist[0].assign(PATHSEP);
  return 1;
#endif
  }


// Create a directories recursively
FXbool FXDir::createDirectories(const FXString& path,FXuint perm){
  if(FXPath::isAbsolute(path)){
    if(FXStat::isDirectory(path)) return true;
    if(createDirectories(FXPath::upLevel(path),perm)){
      if(FXDir::create(path,perm)) return true;
      }
    }
  return false;
  }


#define EXPERIMENTAL 1

// Check for subdirectories
FXbool FXDir::hasSubDirectories(const FXString& path){
  FXbool result=false;
  if(!path.empty()){
#if defined(WIN32)
#ifdef UNICODE
    FXnchar buffer[MAXPATHLEN+2];
    utf2ncs(buffer,path.text(),MAXPATHLEN);
    wcsncat(buffer,TEXT("\\*"),MAXPATHLEN+2);
#else
    FXchar buffer[MAXPATHLEN+2];
    fxstrlcpy(buffer,path.text(),MAXPATHLEN);
    fxstrlcat(buffer,"\\*",MAXPATHLEN+2);
#endif
    WIN32_FIND_DATA ffd;
    HANDLE hfnd;
    if((hfnd=FindFirstFile(buffer,&ffd))!=INVALID_HANDLE_VALUE){
      do{
        if((ffd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) && !(ffd.cFileName[0]=='.' && (ffd.cFileName[1]=='\0' || (ffd.cFileName[1]=='.' && ffd.cFileName[2]=='\0')))){
          result=true;
          break;
          }
        }
      while(FindNextFile(hfnd,&ffd)!=0);
      FindClose(hfnd);
      }
#else
#if defined(EXPERIMENTAL)
    struct stat info;
    if(stat(path.text(),&info)==0){
      // If number of hard-links is >2, then besides parent and current
      // directory (.), it also has another link (..) from a subdirectory.
      // However it may depend on type of file system so mileage may vary.
      result=S_ISDIR(info.st_mode) && (2<info.st_nlink);
      }
#else
    DIR *dir=opendir(path.text());
    if(dir){
      struct dirent *ent;
      struct stat info;
      while((ent=readdir(dir))!=nullptr){
        if(ent->d_name[0]=='.' && (ent->d_name[1]=='\0' || (ent->d_name[1]=='.' && ent->d_name[2]=='\0'))) continue;
        if(ent->d_type==DT_DIR){ result=true; break; }
        if(stat(ent->d_name,&info)==0 && S_ISDIR(info.st_mode)){ result=true; break; }
        }
      closedir(dir);
      }
#endif
#endif
    }
  return result;
  }


// Cleanup
FXDir::~FXDir(){
  close();
  }

}


