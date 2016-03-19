/********************************************************************************
*                                                                               *
*         I n t e r - T h r e a d    M e s s a g i n g    C h a n n e l         *
*                                                                               *
*********************************************************************************
* Copyright (C) 2006,2007 by Jeroen van der Zijp.   All Rights Reserved.        *
*********************************************************************************
* This library is free software; you can redistribute it and/or                 *
* modify it under the terms of the GNU Lesser General Public                    *
* License as published by the Free Software Foundation; either                  *
* version 2.1 of the License, or (at your option) any later version.            *
*                                                                               *
* This library is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             *
* Lesser General Public License for more details.                               *
*                                                                               *
* You should have received a copy of the GNU Lesser General Public              *
* License along with this library; if not, write to the Free Software           *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.    *
*********************************************************************************
* $Id: FXMessageChannel.cpp,v 1.6 2007/04/25 16:13:21 fox Exp $                 *
********************************************************************************/
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "FXException.h"
#include "FXHash.h"
#include "FXThread.h"
#include "FXStream.h"
#include "FXString.h"
#include "FXSize.h"
#include "FXPoint.h"
#include "FXRectangle.h"
#include "FXRegistry.h"
#include "FXApp.h"
#include "FXMessageChannel.h"


/*
  Notes:
  - Inter-thread messaging is handy to have.
  - Redo this in terms of FXPipe when that becomes possible.
*/


// Maximum message size
#define MAXMESSAGE 8192

// Bad handle value
#ifdef WIN32
#define BadHandle  INVALID_HANDLE_VALUE
#else
#define BadHandle  -1
#endif


using namespace FX;


/*******************************************************************************/

namespace FX {


// Structure of message
struct FXMessage {
  FXObject  *target;            // Message target
  FXSelector message;           // Message type,id
#if !(defined(__LP64__) || defined(_LP64) || (_MIPS_SZLONG == 64) || (__WORDSIZE == 64) || defined(_WIN64))
  FXint      pad;               // Padding for 32-bit
#endif
  FXint      size;              // Message size
  };


// Structure of message+payload
struct FXDataMessage {
  FXObject  *target;            // Message target
  FXSelector message;           // Message type,id
#if !(defined(__LP64__) || defined(_LP64) || (_MIPS_SZLONG == 64) || (__WORDSIZE == 64) || defined(_WIN64))
  FXint      pad;               // Padding for 32-bit
#endif
  FXint      size;              // Message size
  FXlong     data[MAXMESSAGE/sizeof(FXlong)];
  };


// Map
FXDEFMAP(FXMessageChannel) FXMessageChannelMap[]={
  FXMAPFUNC(SEL_IO_READ,FXMessageChannel::ID_IO_READ,FXMessageChannel::onMessage)
  };


// Object implementation
FXIMPLEMENT(FXMessageChannel,FXObject,FXMessageChannelMap,ARRAYNUMBER(FXMessageChannelMap));


// Initialize to empty
FXMessageChannel::FXMessageChannel():app((FXApp*)-1L){
  fd[0]=fd[1]=BadHandle;
  }


// Add handler to application
FXMessageChannel::FXMessageChannel(FXApp* a):app(a){
#ifdef WIN32
  SECURITY_ATTRIBUTES sa;
  sa.nLength=sizeof(sa);
  sa.lpSecurityDescriptor=NULL;         // Default ACL
  sa.bInheritHandle=false;              // Don't inherit handles
  if(::CreatePipe(&fd[0],&fd[1],&sa,0)==0){ throw FXResourceException("unable to create pipe."); }
  app->addInput(this,ID_IO_READ,fd[0],INPUT_READ,NULL);
#else
  if(::pipe(fd)!=0){ throw FXResourceException("unable to create pipe."); }
  ::fcntl(fd[0],F_SETFD,FD_CLOEXEC);
  ::fcntl(fd[1],F_SETFD,FD_CLOEXEC);
  app->addInput(this,ID_IO_READ,fd[0],INPUT_READ,NULL);
#endif
  }

//  event=CreateEvent(NULL,TRUE,FALSE,NULL);
//  if(event==NULL){ throw FXResourceException("unable to create event."); }
//  app->addInput(event,INPUT_READ,this,ID_IO_READ);

//  ResetEvent(event);

// Fire signal message to target
long FXMessageChannel::onMessage(FXObject*,FXSelector,void*){
  FXDataMessage m;
#ifdef WIN32
  DWORD nread=-1;
  if(::ReadFile(fd[0],&m,sizeof(FXMessage),&nread,NULL) && nread==sizeof(FXMessage)){
    if(0<m.size && (::ReadFile(fd[0],&m.data,m.size,&nread,NULL) && nread==m.size)){
      return m.target && m.target->tryHandle(this,m.message,m.data);
      }
    return m.target && m.target->tryHandle(this,m.message,NULL);
    }
#else
  if(::read(fd[0],&m,sizeof(FXMessage))==sizeof(FXMessage)){
    if(0<m.size && (::read(fd[0],m.data,m.size)==m.size)){
      return m.target && m.target->tryHandle(this,m.message,m.data);
      }
    return m.target && m.target->tryHandle(this,m.message,NULL);
    }
#endif
  return 0;
  }

//  SetEvent(event);

// Send a message to a target
FXbool FXMessageChannel::message(FXObject* tgt,FXSelector msg,const void* data,FXint size){
  FXMutexLock locker(mx);
  FXMessage m;
  m.target=tgt;
  m.message=msg;
#if !(defined(__LP64__) || defined(_LP64) || (_MIPS_SZLONG == 64) || (__WORDSIZE == 64) || defined(_WIN64))
  m.pad=0;
#endif
  m.size=size;
#ifdef WIN32
  DWORD nwritten=-1;
  if(::WriteFile(fd[1],&m,sizeof(FXMessage),&nwritten,NULL) && nwritten==sizeof(FXMessage)){
    if(m.size<=0 || (::WriteFile(fd[1],data,m.size,&nwritten,NULL) && nwritten==m.size)){
      return true;
      }
    }
#else
  if(::write(fd[1],&m,sizeof(FXMessage))==sizeof(FXMessage)){
    if(m.size<=0 || (::write(fd[1],data,m.size)==m.size)){
      return true;
      }
    }
#endif
  return false;
  }


// Remove handler from application
FXMessageChannel::~FXMessageChannel(){
#ifdef WIN32
  app->removeInput(fd[0],INPUT_READ);
  ::CloseHandle(fd[0]);
  ::CloseHandle(fd[0]);
#else
  app->removeInput(fd[0],INPUT_READ);
  ::close(fd[0]);
  ::close(fd[1]);
#endif
  app=(FXApp*)-1L;
  }

}
