/********************************************************************************
*                                                                               *
*                    O p e n G L   C a n v a s   O b j e c t                    *
*                                                                               *
*********************************************************************************
* Copyright (C) 1997,2007 by Jeroen van der Zijp.   All Rights Reserved.        *
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
* $Id: FXGLCanvas.cpp,v 1.81 2007/12/31 15:25:36 fox Exp $                      *
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
#include "FXSettings.h"
#include "FXRegistry.h"
#include "FXAccelTable.h"
#include "FXApp.h"
#include "FXException.h"
#include "FXVisual.h"
#include "FXGLVisual.h"
#include "FXCursor.h"
#include "FXGLCanvas.h"
#include "FXGLContext.h"


/*
  Notes:
  - Since this only adds SetPixelFormat, perhaps not a bad idea to contemplate
    moving this call to SetPixelFormat somewhere else [candidates are FXGLVisual,
    FXWindow, or FXGLContext].
*/


using namespace FX;

/*******************************************************************************/

namespace FX {


// Object implementation
FXIMPLEMENT(FXGLCanvas,FXCanvas,NULL,0)


// For serialization
FXGLCanvas::FXGLCanvas(){
  flags|=FLAG_ENABLED|FLAG_SHOWN;
  context=NULL;
  xxx=0;
  }


// Construct a GL canvas with its private context and private display lists
FXGLCanvas::FXGLCanvas(FXComposite* p,FXGLVisual *vis,FXObject* tgt,FXSelector sel,FXuint opts,FXint x,FXint y,FXint w,FXint h):FXCanvas(p,tgt,sel,opts|GLCANVAS_OWN_CONTEXT,x,y,w,h){
  flags|=FLAG_ENABLED|FLAG_SHOWN;
  context=new FXGLContext(getApp(),vis);
  visual=vis;
  xxx=0;
  }


// Construct a GL canvas with its private context but shared display lists
FXGLCanvas::FXGLCanvas(FXComposite* p,FXGLVisual *vis,FXGLCanvas* share,FXObject* tgt,FXSelector sel,FXuint opts,FXint x,FXint y,FXint w,FXint h):FXCanvas(p,tgt,sel,opts|GLCANVAS_OWN_CONTEXT,x,y,w,h){
  flags|=FLAG_ENABLED|FLAG_SHOWN;
  context=new FXGLContext(getApp(),vis,share->getContext());
  visual=vis;
  xxx=0;
  }


// Construct a GL canvas with a shared context
FXGLCanvas::FXGLCanvas(FXComposite* p,FXGLContext* ctx,FXObject* tgt,FXSelector sel,FXuint opts,FXint x,FXint y,FXint w,FXint h):FXCanvas(p,tgt,sel,opts,x,y,w,h){
  flags|=FLAG_ENABLED|FLAG_SHOWN;
  context=ctx;
  visual=ctx->getVisual();
  xxx=0;
  }


#ifdef WIN32
const void* FXGLCanvas::GetClass() const { return TEXT("FXGLCanvas"); }
#endif


// Change context
void FXGLCanvas::setContext(FXGLContext *ctx,FXbool owned){
  if(!ctx){ fxerror("%s::setContext: NULL context\n",getClassName()); }
  if(xid){ fxerror("%s::setContext: context should be set before calling create()\n",getClassName()); }
  if(context!=ctx){
    if(options&GLCANVAS_OWN_CONTEXT) delete context;
    context=ctx;
    visual=ctx->getVisual();
    }
  options^=((0-owned)^options)&GLCANVAS_OWN_CONTEXT;
  }


// Return true if it is sharing display lists
FXbool FXGLCanvas::isShared() const {
  return (context->getShared()!=NULL);
  }


// Create X window (GL CANVAS)
void FXGLCanvas::create(){
  FXWindow::create();
  if(xid){
    if(getApp()->isInitialized()){
      context->create();
#ifdef HAVE_GL_H
#if defined(WIN32)
      PIXELFORMATDESCRIPTOR pfd;
      HDC hdc=::GetDC((HWND)xid);       // FIXME should this be this->GetDC()
      pfd.nSize=sizeof(PIXELFORMATDESCRIPTOR);
      pfd.nVersion=1;
      DescribePixelFormat(hdc,(FXint)(FXival)visual->visual,sizeof(PIXELFORMATDESCRIPTOR),&pfd);     // FIXME needed?
      SetPixelFormat(hdc,(FXint)(FXival)visual->visual,&pfd);
      ::ReleaseDC((HWND)xid,hdc);       // FIXME should this be this->ReleaseDC()
#elif defined(GLX_VERSION_1_3)
      xxx=glXCreateWindow((Display*)getApp()->getDisplay(),(GLXFBConfig)visual->id(),xid,NULL);
#else
      /////
#endif
#endif
      }
    }
  }


// Detach the GL Canvas
void FXGLCanvas::detach(){
  context->detach();
  FXWindow::detach();
  }


// Destroy the GL Canvas
void FXGLCanvas::destroy(){
  if(xid){
    if(getApp()->isInitialized()){
#ifdef HAVE_GL_H
#if defined(WIN32)
      /////
#elif defined(GLX_VERSION_1_3)
      glXDestroyWindow((Display*)getApp()->getDisplay(),xxx);
#else
      /////
#endif
#endif
      }
    }
  FXCanvas::destroy();
  }


//  Make the rendering context of GL Canvas current
FXbool FXGLCanvas::makeCurrent(){
  return context->begin(this);
  }


//  Make the rendering context of GL Canvas current
FXbool FXGLCanvas::makeNonCurrent(){
  return context->end();
  }


//  Return true if this window's context is current
FXbool FXGLCanvas::isCurrent() const {
  return context->isCurrent();
  }


// Used by GL to swap the buffers in double buffer mode, or flush a single buffer
void FXGLCanvas::swapBuffers(){
  context->swapBuffers();
  }


// Save object to stream
void FXGLCanvas::save(FXStream& store) const {
  FXWindow::save(store);
  store << context;
  }


// Load object from stream
void FXGLCanvas::load(FXStream& store){
  FXWindow::load(store);
  store >> context;
  }


// Close and release any resources
FXGLCanvas::~FXGLCanvas(){
  if(options&GLCANVAS_OWN_CONTEXT) delete context;
  context=(FXGLContext*)-1L;
  destroy();
  }

}
