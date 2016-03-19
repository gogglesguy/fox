/********************************************************************************
*                                                                               *
*                      M e m o r y   M a p p e d   F i l e                      *
*                                                                               *
*********************************************************************************
* Copyright (C) 2004,2008 by Jeroen van der Zijp.   All Rights Reserved.        *
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
*********************************************************************************
* $Id: FXMemMap.cpp,v 1.37 2008/03/29 03:08:29 fox Exp $                        *
********************************************************************************/
#include "xincs.h"
#include "fxdefs.h"
#include "FXString.h"
#include "FXMemMap.h"

/*
  Notes:
  - A memory mapped region of a file, or anonymous memory map.
  - Maybe use long sz = sysconf(_SC_PAGESIZE);
  - msync().
  - Need to bring in line with FXIO esp. with interpretation of options and so on.
  - Need API's to open with existing file handles.
*/


#ifdef WIN32
#define BadHandle INVALID_HANDLE_VALUE
#else
#define BadHandle -1
#endif


using namespace FX;

/*******************************************************************************/

namespace FX {


// Create new map object
FXMemMap::FXMemMap():maphandle(BadHandle),mapbase(NULL),mapoffset(0L),mapposition(0L),maplength(0){
  }


// Open file and map it
void *FXMemMap::openMap(const FXString& filename,FXlong off,FXival len,FXuint m,FXuint p){
  if(open(filename,m,p)){
    void* result=map(off,len);
    if(result){
      return result;
      }
    close();
    }
  return NULL;
  }


// Attach to existing file handle and map it
void* FXMemMap::openMap(FXInputHandle h,FXlong off,FXival len,FXuint m){
  if(open(h,m)){
    void* result=map(off,len);
    if(result){
      return result;
      }
    close();
    }
  return NULL;
  }


// Map an already open file
void *FXMemMap::map(FXlong off,FXival len){
  if(isOpen()){

    // Get file size
    FXlong filesize=size();
    if(0<=filesize){

#ifdef WIN32

      // Map whole file
      if(len==-1) len=filesize-off;

      // Set access flags
      DWORD prot=0;
      if(access&ReadOnly){ prot=PAGE_READONLY; }
      if(access&WriteOnly){ prot=PAGE_READWRITE; }

      DWORD flag=0;
      if(access&ReadOnly){ flag=FILE_MAP_READ; }
      if(access&WriteOnly){ flag=FILE_MAP_WRITE; }

      // Now map it
      FXInputHandle hnd=::CreateFileMapping(handle(),NULL,prot,0,len,NULL);
      if(hnd!=NULL){
        FXuchar* ptr=(FXuchar*)::MapViewOfFile(hnd,flag,0,off,len);
        if(ptr!=NULL){
          maphandle=hnd;
          mapbase=ptr;
          maplength=len;
          mapoffset=off;
          mapposition=off;
          return mapbase;
          }
        ::CloseHandle(hnd);
        }
#else

      // Map whole file
      if(len==-1) len=filesize-off;

      // Trying to map region larger than the file
      if(filesize<off+len){
        if(access&WriteOnly){
          truncate(off+len);            // Extends the file if writing
          }
        else{
          len=filesize-off;             // Map smaller region when reading
          }
        }

      // Set access flags
      FXint prot=PROT_NONE;
      if(access&ReadOnly){ prot|=PROT_READ; }
      if(access&WriteOnly){ prot|=PROT_WRITE|PROT_READ; }
      if(access&Executable){ prot|=PROT_EXEC; }

      // Now map it
      FXuchar* ptr=(FXuchar*)::mmap(NULL,len,prot,MAP_SHARED,handle(),off);
      if(ptr!=MAP_FAILED){
        mapbase=ptr;
        maplength=len;
        mapoffset=off;
        mapposition=off;
        return mapbase;
        }
#endif
      }
    }
  return NULL;
  }


// Unmap the view of the file
void* FXMemMap::unmap(){
  if(mapbase){
#ifdef WIN32
    ::UnmapViewOfFile(mapbase);
    ::CloseHandle(maphandle);
#else
    ::munmap(mapbase,maplength);
#endif
    maphandle=BadHandle;
    mapbase=NULL;
    mapoffset=0L;
    mapposition=0L;
    maplength=0;
    }
  return NULL;
  }


// Get current file position
FXlong FXMemMap::position() const {
  return mapposition;
  }


// Change file position, returning new position from start
FXlong FXMemMap::position(FXlong off,FXuint from){
  if(mapbase){
    if(from==Current) off=off+mapposition;
    else if(from==End) off=off+mapoffset+maplength;       // FIXME is this what we want?
    mapposition=off;
    return mapposition;
    }
  return -1;
  }


// Read block of bytes, returning number of bytes read
FXival FXMemMap::readBlock(void* data,FXival count){
  if(mapbase && mapoffset<=mapposition && mapposition<=mapoffset+maplength){
    if(mapposition+count>mapoffset+maplength) count=mapoffset+maplength-mapposition;
    memmove(data,mapbase+mapposition-mapoffset,count);
    mapposition+=count;
    return count;
    }
  return -1;
  }


// Write block of bytes, returning number of bytes written
FXival FXMemMap::writeBlock(const void* data,FXival count){
  if(mapbase && mapoffset<=mapposition && mapposition<=mapoffset+maplength){
    if(mapposition+count>mapoffset+maplength) count=mapoffset+maplength-mapposition;
    memmove(mapbase+mapposition-mapoffset,data,count);
    mapposition+=count;
    return count;
    }
  return -1;
  }


// Synchronize disk
FXbool FXMemMap::flush(){
  if(mapbase){
#ifdef WIN32
    return ::FlushViewOfFile(mapbase,(size_t)maplength)!=0;
#else
    return ::msync((char*)mapbase,(size_t)maplength,MS_SYNC|MS_INVALIDATE)==0;
#endif
    }
  return false;
  }


// Close file, and also the map
FXbool FXMemMap::close(){
  unmap();
  return FXFile::close();
  }


// Delete the mapping
FXMemMap::~FXMemMap(){
  close();
  }


}

