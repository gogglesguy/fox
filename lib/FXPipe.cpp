/********************************************************************************
*                                                                               *
*                             P i p e   C l a s s                               *
*                                                                               *
*********************************************************************************
* Copyright (C) 2005,2012 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "FXArray.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXString.h"
#include "FXPath.h"
#include "FXIO.h"
#include "FXIODevice.h"
#include "FXPipe.h"



/*
  Notes:

  - Obviously this will get fleshed out some more...
*/


#if defined(WIN32)
#define BadHandle INVALID_HANDLE_VALUE
#else
#define BadHandle -1
#endif


using namespace FX;

/*******************************************************************************/

namespace FX {

/*
// create the security descriptor
SECURITY_ATTRIBUTES saAttr;
saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
saAttr.bInheritHandle = TRUE;
saAttr.lpSecurityDescriptor = NULL;
CreatePipe(  &hStdInRead,  &hStdInWrite, &saAttr, 0);
CreatePipe( &hStdOutRead, &hStdOutWrite, &saAttr, 0);
CreatePipe( &hStdErrRead, &hStdErrWrite, &saAttr, 0);

// Create pipes for stdin and stdout
CreatePipe(&hReadStdin, &hWriteStdin, NULL, 0);
CreatePipe(&hReadStdout, &hWriteStdout, NULL, 0);

// hook them up to the process we're about to create
startup_info.hStdOutput = hWriteStdout;
startup_info.hStdInput = hReadStdin;
*/

// Construct file and attach existing handle h
FXPipe::FXPipe(FXInputHandle h,FXuint m){
  open(h,m);
  }


// Open device with access mode and handle
FXbool FXPipe::open(FXInputHandle h,FXuint m){
  return FXIODevice::open(h,m);
  }


// Read block
FXival FXPipe::readBlock(void* data,FXival count){
  FXival nread=-1;
  if(isOpen()){
#if defined(WIN32)
    DWORD nr;
    if(::ReadFile(device,data,(DWORD)count,&nr,NULL)!=0){
      nread=(FXival)nr;
      }
#else
    do{
      nread=::read(device,data,count);
      }
    while(nread<0 && errno==EINTR);
#endif
    }
  return nread;
  }


// Write block
FXival FXPipe::writeBlock(const void* data,FXival count){
  FXival nwritten=-1;
  if(isOpen()){
#if defined(WIN32)
    DWORD nw;
    if(::WriteFile(device,data,(DWORD)count,&nw,NULL)!=0){
      nwritten=(FXival)nw;
      }
#else
    do{
      nwritten=::write(device,data,count);
      }
    while(nwritten<0 && errno==EINTR);
#endif
    }
  return nwritten;
  }


// Close pipe
FXbool FXPipe::close(){
  if(isOpen()){
    if(access&OwnHandle){
#if defined(WIN32)
      if(::CloseHandle(device)!=0){
        device=BadHandle;
        access=NoAccess;
        return true;
        }
#else
      if(::close(device)==0){
        device=BadHandle;
        access=NoAccess;
        return true;
        }
#endif
      }
    device=BadHandle;
    access=NoAccess;
    }
  return false;
  }


// Create new (empty) file
FXbool FXPipe::create(const FXString& file,FXuint perm){
  if(!file.empty()){
#if defined(WIN32)
/*
HANDLE WINAPI CreateNamedPipe(
  __in      LPCTSTR lpName,
  __in      DWORD dwOpenMode,
  __in      DWORD dwPipeMode,
  __in      DWORD nMaxInstances,
  __in      DWORD nOutBufferSize,
  __in      DWORD nInBufferSize,
  __in      DWORD nDefaultTimeOut,
  __in_opt  LPSECURITY_ATTRIBUTES lpSecurityAttributes
);
*/
#else
    return ::mkfifo(file.text(),perm)==0;
#endif
    }
  return false;
  }


// Destroy
FXPipe::~FXPipe(){
  close();
  }


}

