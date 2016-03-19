/********************************************************************************
*                                                                               *
*                       R o o t   W i n d o w   O b j e c t                     *
*                                                                               *
*********************************************************************************
* Copyright (C) 1997,2009 by Jeroen van der Zijp.   All Rights Reserved.        *
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
* $Id: FXRootWindow.cpp,v 1.45 2009/01/06 13:24:39 fox Exp $                    *
********************************************************************************/
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "FXHash.h"
#include "FXThread.h"
#include "FXStream.h"
#include "FXString.h"
#include "FXSize.h"
#include "FXPoint.h"
#include "FXRectangle.h"
#include "FXRegistry.h"
#include "FXApp.h"
#include "FXVisual.h"
#include "FXRootWindow.h"

/*
  Notes:

  - Size of FXRootWindow is now size of the entire virtual display, which
    is a tiled virtual area of primary and secondary display adapters.
*/


#define DISPLAY(app) ((Display*)((app)->display))

using namespace FX;

/*******************************************************************************/

namespace FX {

// Object implementation
FXIMPLEMENT(FXRootWindow,FXComposite,NULL,0)


// Construct root window
FXRootWindow::FXRootWindow(FXApp* a,FXVisual *vis):FXComposite(a,vis){
  }


#ifdef WIN32

// Returns device context
FXID FXRootWindow::GetDC() const {
  LockWindowUpdate(GetDesktopWindow());
  return GetDCEx(GetDesktopWindow(),NULL,DCX_CACHE|DCX_LOCKWINDOWUPDATE);
  }


// Release DC
int FXRootWindow::ReleaseDC(FXID hdc) const {
  int status=::ReleaseDC(GetDesktopWindow(),(HDC)hdc);
  LockWindowUpdate(NULL);
  return status;
  }

#endif


// When created, create subwindows ONLY
void FXRootWindow::create(){
  register FXWindow *child;
  if(!xid){
    if(getApp()->isInitialized()){
      FXTRACE((100,"%s::create %p\n",getClassName(),this));

      // Got to have a visual
      if(!visual){ fxerror("%s::create: trying to create window without a visual.\n",getClassName()); }

      // Initialize visual
      visual->create();

#ifdef WIN32

      // Get HWND of desktop window
      xid=GetDesktopWindow();

      // Obtain size
      HDC hdc=::GetDC((HWND)xid);
      width=GetDeviceCaps(hdc,HORZRES);
      height=GetDeviceCaps(hdc,VERTRES);
      ::ReleaseDC((HWND)xid,hdc);

      // Store for xid to C++ object mapping
      getApp()->hash.insert((void*)xid,this);

#else

      xid=RootWindow(DISPLAY(getApp()),DefaultScreen(DISPLAY(getApp())));
      width=DisplayWidth(DISPLAY(getApp()),DefaultScreen(DISPLAY(getApp())));
      height=DisplayHeight(DISPLAY(getApp()),DefaultScreen(DISPLAY(getApp())));

      // Store for xid to C++ object mapping
      getApp()->hash.insert((void*)xid,this);

#endif
      flags&=~FLAG_OWNED;

      // Normally create children
      for(child=getFirst(); child; child=child->getNext()) child->create();
      }
    }
  }

/*
//
//      max_monitor_cb
//
//  EnumDisplayMonitors callback routine
//
static BOOL max_monitor_cb(HMONITOR hmon,HDC hdc,LPRECT rect,LPARAM args){
  int w,h;
  MONITORINFO moninfo;
  moninfo.cbSize=sizeof(moninfo);
  GetMonitorInfo(hmon,&moninfo);
  w=moninfo.rcWork.right-moninfo.rcWork.left;
  h=moninfo.rcWork.bottom-moninfo.rcWork.top;
  if(((FXint*)args)[0]<w) ((FXint*)args)[0]=w;
  if(((FXint*)args)[1]<h) ((FXint*)args)[1]=h;
  return true;
  }


//
//      GetMaxScreenSize
//
//  Returns the biggest width and biggest height of all the
//  monitors on the system. Does not necessarily have to
//  refer to the same monitor device.
//
FXbool GetMaxScreenSize(FXint &width, FXint &height){
  FXint args[2]={0,0};
  if(EnumDisplayMonitors(NULL,NULL,(MONITORENUMPROC)max_monitor_cb,(LPARAM)args)){
    width=args[0];
    height=args[1];
    return true;
    }
  return false;
  }


One could also use:

GetSystemMetrics(SM_CXVIRTUALSCREEN)
GetSystemMetrics(SM_CYVIRTUALSCREEN)

//return GetSystemMetrics(SM_CXSCREEN);
//return GetSystemMetrics(SM_CYSCREEN);
*/


// Get default width
FXint FXRootWindow::getDefaultWidth(){
#ifdef WIN32
  HDC hdc=::GetDC(GetDesktopWindow());
  FXint w=GetDeviceCaps(hdc,HORZRES);
  ::ReleaseDC(GetDesktopWindow(),hdc);
  return w;
#else
  return DisplayWidth(DISPLAY(getApp()),DefaultScreen(DISPLAY(getApp())));
#endif
  }


// Get default height
FXint FXRootWindow::getDefaultHeight(){
#ifdef WIN32
  HDC hdc=::GetDC(GetDesktopWindow());
  FXint h=GetDeviceCaps(hdc,VERTRES);
  ::ReleaseDC(GetDesktopWindow(),hdc);
  return h;
#else
  return DisplayHeight(DISPLAY(getApp()),DefaultScreen(DISPLAY(getApp())));
#endif
  }


// Moving root has no effect
void FXRootWindow::move(FXint,FXint){ }


// Move and resize root has no effect
void FXRootWindow::position(FXint,FXint,FXint,FXint){ }


// Resize root window has no effect
void FXRootWindow::resize(FXint,FXint){ }


// Layout of root window
void FXRootWindow::layout(){ }


// Mark as dirty
void FXRootWindow::recalc(){ }


// Root can not be focused on
void FXRootWindow::setFocus(){ }


// Root can not be unfocused
void FXRootWindow::killFocus(){ }


// Does not destroy root window
FXRootWindow::~FXRootWindow(){
  xid=0;
  }

}
