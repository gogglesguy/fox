/********************************************************************************
*                                                                               *
*                     G L  R e n d e r i n g   C o n t e x t                    *
*                                                                               *
*********************************************************************************
* Copyright (C) 2000,2007 by Jeroen van der Zijp.   All Rights Reserved.        *
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
* $Id: FXGLContext.cpp,v 1.101 2007/07/09 16:31:34 fox Exp $                    *
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
#include "FXDrawable.h"
#include "FXGLConfig.h"
#include "FXGLContext.h"


/*
  Notes:
  - Creates GL Context based on frame buffer properties described in the desired FXGLConfig.
  - When realized, match actual hardware against desired frame buffer properties and create a
    GL Context conformant with the best matching hardware configuration.  Note that we don't
    have a window yet, necessarily!
  - The matching hardware frame buffer configuration is described in the actual FXGLConfig.
  - For frame buffer configurations conformant with window or image rendering, you can then
    use the realized GL context to determine Visual (PixelFormat) for the drawable.
  - It is also possible to perform off-screen OpenGL rendering using PBuffer rendering; this
    is done completely in the OpenGL driver and window-system independent!
  - There will be three different ways to make a GL Canvas:

      1 Each GL Canvas has its own GL Context; The GL Context is owned by the GL Canvas and
        destroyed when the GL Canvas is.

      2 Each GL Canvas has its own GL Context, but it may share the display list and other
        GL objects with those of another GL Context.  Thus the other GL Context with which
        it shares must be passed in.

      3 The GL Canvas shares the GL Context with other GL Canvasses.  This is probably the
        most efficient way as all the GL state information is preserved between the GL Canvas
        windows.
*/

//#undef GLX_VERSION_1_3


using namespace FX;

/*******************************************************************************/

namespace FX {


// Make GL context
FXGLContext::FXGLContext():drawable(NULL),shared(NULL),format(NULL){
  FXTRACE((100,"FXGLContext::FXGLContext %p\n",this));
  }


// Make a GL context
FXGLContext::FXGLContext(FXApp *a,FXGLContext* shr):FXId(a),drawable(NULL),shared(shr),desired(FXGLConfig::DoubleBuffer|FXGLConfig::DrawWindow),format(NULL){
  FXTRACE((100,"FXGLContext::FXGLContext %p\n",this));
  }


// Make a GL context
FXGLContext::FXGLContext(FXApp *a,const FXGLConfig& cfg,FXGLContext* shr):FXId(a),drawable(NULL),shared(shr),desired(cfg),format(NULL){
  FXTRACE((100,"FXGLContext::FXGLContext %p\n",this));
  }


#if defined(WIN32) ///////////////// WIN32 //////////////////////////////////////


#ifndef WGL_ARB_pixel_format
#define WGL_NUMBER_PIXEL_FORMATS_ARB   0x2000
#define WGL_DRAW_TO_WINDOW_ARB         0x2001
#define WGL_DRAW_TO_BITMAP_ARB         0x2002
#define WGL_ACCELERATION_ARB           0x2003
#define WGL_NEED_PALETTE_ARB           0x2004
#define WGL_NEED_SYSTEM_PALETTE_ARB    0x2005
#define WGL_SWAP_LAYER_BUFFERS_ARB     0x2006
#define WGL_SWAP_METHOD_ARB            0x2007
#define WGL_NUMBER_OVERLAYS_ARB        0x2008
#define WGL_NUMBER_UNDERLAYS_ARB       0x2009
#define WGL_TRANSPARENT_ARB            0x200A
#define WGL_TRANSPARENT_RED_VALUE_ARB  0x2037
#define WGL_TRANSPARENT_GREEN_VALUE_ARB 0x2038
#define WGL_TRANSPARENT_BLUE_VALUE_ARB 0x2039
#define WGL_TRANSPARENT_ALPHA_VALUE_ARB 0x203A
#define WGL_TRANSPARENT_INDEX_VALUE_ARB 0x203B
#define WGL_SHARE_DEPTH_ARB            0x200C
#define WGL_SHARE_STENCIL_ARB          0x200D
#define WGL_SHARE_ACCUM_ARB            0x200E
#define WGL_SUPPORT_GDI_ARB            0x200F
#define WGL_SUPPORT_OPENGL_ARB         0x2010
#define WGL_DOUBLE_BUFFER_ARB          0x2011
#define WGL_STEREO_ARB                 0x2012
#define WGL_PIXEL_TYPE_ARB             0x2013
#define WGL_COLOR_BITS_ARB             0x2014
#define WGL_RED_BITS_ARB               0x2015
#define WGL_RED_SHIFT_ARB              0x2016
#define WGL_GREEN_BITS_ARB             0x2017
#define WGL_GREEN_SHIFT_ARB            0x2018
#define WGL_BLUE_BITS_ARB              0x2019
#define WGL_BLUE_SHIFT_ARB             0x201A
#define WGL_ALPHA_BITS_ARB             0x201B
#define WGL_ALPHA_SHIFT_ARB            0x201C
#define WGL_ACCUM_BITS_ARB             0x201D
#define WGL_ACCUM_RED_BITS_ARB         0x201E
#define WGL_ACCUM_GREEN_BITS_ARB       0x201F
#define WGL_ACCUM_BLUE_BITS_ARB        0x2020
#define WGL_ACCUM_ALPHA_BITS_ARB       0x2021
#define WGL_DEPTH_BITS_ARB             0x2022
#define WGL_STENCIL_BITS_ARB           0x2023
#define WGL_AUX_BUFFERS_ARB            0x2024
#define WGL_NO_ACCELERATION_ARB        0x2025
#define WGL_GENERIC_ACCELERATION_ARB   0x2026
#define WGL_FULL_ACCELERATION_ARB      0x2027
#define WGL_SWAP_EXCHANGE_ARB          0x2028
#define WGL_SWAP_COPY_ARB              0x2029
#define WGL_SWAP_UNDEFINED_ARB         0x202A
#define WGL_TYPE_RGBA_ARB              0x202B
#define WGL_TYPE_COLORINDEX_ARB        0x202C
#endif

#ifndef WGL_ARB_pbuffer
#define WGL_DRAW_TO_PBUFFER_ARB        0x202D
#define WGL_MAX_PBUFFER_PIXELS_ARB     0x202E
#define WGL_MAX_PBUFFER_WIDTH_ARB      0x202F
#define WGL_MAX_PBUFFER_HEIGHT_ARB     0x2030
#define WGL_PBUFFER_LARGEST_ARB        0x2033
#define WGL_PBUFFER_WIDTH_ARB          0x2034
#define WGL_PBUFFER_HEIGHT_ARB         0x2035
#define WGL_PBUFFER_LOST_ARB           0x2036
#endif

#ifndef WGL_ARB_multisample
#define WGL_SAMPLE_BUFFERS_ARB         0x2041
#define WGL_SAMPLES_ARB                0x2042
#endif

#ifndef WGL_ARB_pixel_format_float
#define WGL_TYPE_RGBA_FLOAT_ARB        0x21A0
#endif


typedef BOOL (WINAPI * PFNWGLGETPIXELFORMATATTRIBIVARBPROC) (HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, int *piValues);


// A pixel format with hardware acceleration
static const PIXELFORMATDESCRIPTOR pfdpick={
  sizeof(PIXELFORMATDESCRIPTOR),1,PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER,PFD_TYPE_RGBA,24,0,0,0,0,0,0,0,0,0,0,0,0,0,24,0,0,PFD_MAIN_PLANE,0,0,0,0
  };


// Create GL context
void FXGLContext::create(){
#ifdef HAVE_GL_H
  if(!xid){
    if(getApp()->isInitialized()){
      PIXELFORMATDESCRIPTOR pfd;
      int v;

      FXTRACE((100,"FXGLContext::create %p\n",this));

      // Shared context must exist
      if(shared && !shared->id()){ fxerror("%s::create: shared context not created yet.\n",getClassName()); }

      // Register dummy window class
      WNDCLASSA wglClass={0,DefWindowProc,0,0,(HINSTANCE)getApp()->getDisplay(),NULL,NULL,NULL,NULL,"wglFOXInit"};
      RegisterClassA(&wglClass);

      // Create dummy window
      HWND hwnd=CreateWindowA("wglFOXInit","",0,0,0,0,0,(HWND)NULL,(HMENU)NULL,(HINSTANCE)getApp()->getDisplay(),NULL);

      // Get device handle
      HDC hdc=GetDC(hwnd);

      // Choose pdf & set it
      v=ChoosePixelFormat(hdc,&pfdpick);
      if(SetPixelFormat(hdc,v,&pfd)){

        // Make a temporary context
        HGLRC context=wglCreateContext(hdc);
        if(context){

          // Make context current
          if(wglMakeCurrent(hdc,context)){
            int glredsize,glgreensize,glbluesize,glalphasize,gldepthsize,glstencilsize,glaccumredsize,glaccumgreensize,glaccumbluesize,glaccumalphasize,gldouble,glstereo,gldrawwnd,gldrawbmp,gldrawpbuf,glrenderpal,glrenderrgb,glrenderfloat,glsamples;
            int dred,dgreen,dblue,dalpha,ddepth,dstencil,dsamples,daccred,daccgreen,daccblue,daccalpha,dmatch;
            int bestmatch=1000000000;
            int best=-1;
            int att;
            int npf;

            // Get the proc address; need this to obtain advanced GL capabilities
            PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB=(PFNWGLGETPIXELFORMATATTRIBIVARBPROC)wglGetProcAddress("wglGetPixelFormatAttribivARB");
            if(wglGetPixelFormatAttribivARB){
              FXTRACE((100,"wglGetPixelFormatAttribivARB=%p\n",wglGetPixelFormatAttribivARB));

              // Get number of supported pixel formats
              att=WGL_NUMBER_PIXEL_FORMATS_ARB;
              if(wglGetPixelFormatAttribivARB(hdc,0,0,1,&att,&npf) && 0<npf){

                FXTRACE((140,"Found %d OpenGL configs\n",npf));

                // Try to find the best
                for(v=1; v<=npf; v++){
                  FXTRACE((140,"v: %d\n",v));

                  // Get supported drawable types
                  att=WGL_DRAW_TO_WINDOW_ARB;
                  if(!wglGetPixelFormatAttribivARB(hdc,v,0,1,&att,&gldrawwnd)) continue;
                 // if((desired.flags&FXGLConfig::DrawWindow) && !gldrawwnd) continue;
                  att=WGL_DRAW_TO_BITMAP_ARB;
                  if(!wglGetPixelFormatAttribivARB(hdc,v,0,1,&att,&gldrawbmp)) continue;
                  //if((desired.flags&FXGLConfig::DrawImage) && !gldrawbmp) continue;
                  att=WGL_DRAW_TO_PBUFFER_ARB;
                  if(!wglGetPixelFormatAttribivARB(hdc,v,0,1,&att,&gldrawpbuf)) continue;
                //  if((desired.flags&FXGLConfig::DrawBuffer) && !gldrawpbuf) continue;


                  // Get render type
                  att=WGL_PIXEL_TYPE_ARB;
                  if(!wglGetPixelFormatAttribivARB(hdc,v,0,1,&att,&glrenderpal)) continue;
/*
                  att=WGL_TYPE_RGBA_ARB;
                  if(!wglGetPixelFormatAttribivARB(hdc,v,0,1,&att,&glrenderrgb)) continue;
                  att=WGL_TYPE_RGBA_FLOAT_ARB;
                  if(!wglGetPixelFormatAttribivARB(hdc,v,0,1,&att,&glrenderfloat)) continue;
*/

                  // Get stereo and double buffer support
                  att=WGL_DOUBLE_BUFFER_ARB;
                  if(!wglGetPixelFormatAttribivARB(hdc,v,0,1,&att,&gldouble)) continue;
                  att=WGL_STEREO_ARB;
                  if(!wglGetPixelFormatAttribivARB(hdc,v,0,1,&att,&glstereo)) continue;

                  // Get plane depths
                  att=WGL_RED_BITS_ARB;
                  if(!wglGetPixelFormatAttribivARB(hdc,v,0,1,&att,&glredsize)) continue;
                  att=WGL_GREEN_BITS_ARB;
                  if(!wglGetPixelFormatAttribivARB(hdc,v,0,1,&att,&glgreensize)) continue;
                  att=WGL_BLUE_BITS_ARB;
                  if(!wglGetPixelFormatAttribivARB(hdc,v,0,1,&att,&glbluesize)) continue;
                  att=WGL_ALPHA_BITS_ARB;
                  if(!wglGetPixelFormatAttribivARB(hdc,v,0,1,&att,&glalphasize)) continue;
                  att=WGL_DEPTH_BITS_ARB;
                  if(!wglGetPixelFormatAttribivARB(hdc,v,0,1,&att,&gldepthsize)) continue;
                  att=WGL_STENCIL_BITS_ARB;
                  if(!wglGetPixelFormatAttribivARB(hdc,v,0,1,&att,&glstencilsize)) continue;
                  att=WGL_ACCUM_RED_BITS_ARB;
                  if(!wglGetPixelFormatAttribivARB(hdc,v,0,1,&att,&glaccumredsize)) continue;
                  att=WGL_ACCUM_GREEN_BITS_ARB;
                  if(!wglGetPixelFormatAttribivARB(hdc,v,0,1,&att,&glaccumgreensize)) continue;
                  att=WGL_ACCUM_BLUE_BITS_ARB;
                  if(!wglGetPixelFormatAttribivARB(hdc,v,0,1,&att,&glaccumbluesize)) continue;
                  att=WGL_ACCUM_ALPHA_BITS_ARB;
                  if(!wglGetPixelFormatAttribivARB(hdc,v,0,1,&att,&glaccumalphasize)) continue;

                  // Get multisample support (if we don't succeed, set it to zero)
                  att=WGL_SAMPLES_ARB;
                  if(!wglGetPixelFormatAttribivARB(hdc,v,0,1,&att,&glsamples)) glsamples=0;

                  // We prefer to get a few MORE bits in RGBA than we asked for
                  dred   = glredsize-desired.redSize;     if(dred<0)   dred   *= -100;
                  dgreen = glgreensize-desired.greenSize; if(dgreen<0) dgreen *= -100;
                  dblue  = glbluesize-desired.blueSize;   if(dblue<0)  dblue  *= -100;
                  dalpha = glalphasize-desired.alphaSize; if(dalpha<0) dalpha *= -100;

                  // Prefer better Z than asked, but colors more important
                  ddepth = gldepthsize-desired.depthSize; if(ddepth<0) ddepth *= -10;

                  // We care about colors and Z depth more than stencil depth
                  dstencil = glstencilsize-desired.stencilSize; if(dstencil<0) dstencil *= -1;

                  // Accumulation buffers
                  daccred=glaccumredsize-desired.accumRedSize;       if(daccred<0)   daccred   *= -1;
                  daccgreen=glaccumgreensize-desired.accumGreenSize; if(daccgreen<0) daccgreen *= -1;
                  daccblue=glaccumbluesize-desired.accumBlueSize;    if(daccblue<0)  daccblue  *= -1;
                  daccalpha=glaccumalphasize-desired.accumAlphaSize; if(daccalpha<0) daccalpha *= -1;

                  // Want the best colors, of course
                  dmatch=dred+dgreen+dblue+dalpha;

                  // Accumulation buffers
                  dmatch+=daccred+daccgreen+daccblue+daccalpha;

                  // Extra penalty for no alpha if we asked for alpha, but no
                  // penalty at all if there is alpha and we didn't ask for it.
                  if(desired.alphaSize>0){
                    if(glalphasize<1) dmatch+=100000;
                    }

                  // Wanted Z-buffer
                  if(desired.depthSize>0){
                    if(gldepthsize<1) dmatch+=10000000;
                    else dmatch+=ddepth;
                    }
                  else{
                    if(gldepthsize>0) dmatch+=10000000;
                    }

                  // Stencil buffers desired
                  if(desired.stencilSize>0){
                    if(glstencilsize<1) dmatch+=10000;
                    else dmatch+=dstencil;
                    }
                  else{
                    if(glstencilsize>0) dmatch+=1;
                    }

                  // Multisamples
                  if(desired.multiSamples>0){
                    dsamples=glsamples-desired.multiSamples;
                    dmatch+=FXABS(dsamples);
                    }

                  // Double buffering also quite strongly preferred
                  if(desired.flags&FXGLConfig::DoubleBuffer){
                    if(!gldouble) dmatch+=1000000;
                    }
                  else{
                    if(gldouble) dmatch+=1000000;
                    }

                  // Stereo not so important
                  if(desired.flags&FXGLConfig::StereoBuffer){
                    if(!glstereo) dmatch+=10000;
                    }
                  else{
                    if(glstereo) dmatch+=10000;
                    }

                  // Want float buffer
                  if(desired.flags&FXGLConfig::FloatBuffer){
                    if(!glrenderfloat) dmatch+=1000;
                    }
                  else{
                    if(!glrenderrgb) dmatch+=1000;
                    }

                  // Trace
                  FXTRACE((150,"PixelFormat: #%d: match=%d\n",v,dmatch));
                  FXTRACE((150,"  drawables  = %s%s%s\n",gldrawwnd?"WGL_DRAW_TO_WINDOW_ARB ":"",gldrawbmp?"WGL_DRAW_TO_BITMAP_ARB ":"",gldrawpbuf?"WGL_DRAW_TO_PBUFFER_ARB":""));
                //  FXTRACE((150,"  render     = %s%s%s\n",glrenderrgb?"WGL_TYPE_RGBA_ARB ":"",glrenderpal?"WGL_TYPE_COLORINDEX_ARB ":"",glrenderfloat?"WGL_TYPE_RGBA_FLOAT_ARB":""));
                  FXTRACE((150,"  render     = %x\n",glrenderpal));
                  FXTRACE((150,"  red size   = %d\n",glredsize));
                  FXTRACE((150,"  green size = %d\n",glgreensize));
                  FXTRACE((150,"  blue size  = %d\n",glbluesize));
                  FXTRACE((150,"  alpha size = %d\n",glalphasize));
                  FXTRACE((150,"  depth size = %d\n",gldepthsize));
                  FXTRACE((150,"  double buf = %d\n",gldouble));
                  FXTRACE((150,"  stencil    = %d\n",glstencilsize));
                  FXTRACE((150,"  acc red    = %d\n",glaccumredsize));
                  FXTRACE((150,"  acc green  = %d\n",glaccumgreensize));
                  FXTRACE((150,"  acc blue   = %d\n",glaccumbluesize));
                  FXTRACE((150,"  acc alpha  = %d\n",glaccumalphasize));
                  FXTRACE((150,"  stereo     = %d\n",glstereo));
                  FXTRACE((150,"  samples    = %d\n",glsamples));

                  // May the best config win
                  if(dmatch<=bestmatch){
                    actual.redSize=glredsize;
                    actual.greenSize=glgreensize;
                    actual.blueSize=glbluesize;
                    actual.alphaSize=glalphasize;
                    actual.depthSize=gldepthsize;
                    actual.stencilSize=glstencilsize;
                    actual.multiSamples=glsamples;
                    actual.accumRedSize=glaccumredsize;
                    actual.accumGreenSize=glaccumgreensize;
                    actual.accumBlueSize=glaccumbluesize;
                    actual.accumAlphaSize=glaccumalphasize;
                    actual.flags=0;
                    if(gldrawwnd) actual.flags|=FXGLConfig::DrawWindow;
                    if(gldrawbmp) actual.flags|=FXGLConfig::DrawImage;
                    if(gldrawpbuf) actual.flags|=FXGLConfig::DrawBuffer;
                    if(glrenderfloat) actual.flags|=FXGLConfig::FloatBuffer;
                    if(gldouble) actual.flags|=FXGLConfig::DoubleBuffer;
                    if(glstereo) actual.flags|=FXGLConfig::StereoBuffer;
                    bestmatch=dmatch;
                    format=(void*)(FXival)v;            // Remember pixel format
                    best=v;
                    }
                  }
                }
              }

            // Unable to get proc address; try old-fashioned way
            else{
              // FIXME
              }

            // Hopefully, we got one
            if(0<=best){
              FXTRACE((140,"Best PixelFormat: #%d: match=%d\n",best,bestmatch));
             // xid=(FXID)glXCreateNewContext((Display*)getApp()->getDisplay(),fb[best],GLX_RGBA_TYPE,shared?(GLXContext)shared->id():(GLXContext)NULL,(desired.flags&FXGLConfig::Indirect)==0);
             // if(xid && !glXIsDirect((Display*)getApp()->getDisplay(),(GLXContext)xid)){
             //   actual.flags|=FXGLConfig::Indirect;
             //   }
              }

            // Make non-current
            wglMakeCurrent(NULL,NULL);
            }

          // Delete context
          wglDeleteContext(context);
          }
        }

      // Release hdc
      ReleaseDC(hwnd,hdc);

      // Destroy dummy window
      DestroyWindow(hwnd);

      // Check if successful
      if(!xid){
       // throw FXWindowException("unable to create GL context.");
        }
      }
    }
#endif
  }


#elif defined(GLX_VERSION_1_3) //////// X11 NEW /////////////////////////////////


/*
struct Overlay {
  FXuint visualid;      // Visual ID
  FXuint transparency;	// 0: none; 1: pixel; 2: mask (?)
  FXuint value;		// the transparent pixel
  FXuint layer;		// -1: underlay; 0: normal; 1: popup; 2: overlay
  };


// Get overlay visuals
static int getOverlays(Display *display,Overlay*& overlays){
  unsigned long nitems,bytes_after;
  int result,actual_format;
  Atom actual_type;
  Overlay *data=NULL;
  Atom XA_SERVER_OVERLAY_VISUALS=XInternAtom(display,"SERVER_OVERLAY_VISUALS",False);
  result=XGetWindowProperty(display,RootWindow(display,DefaultScreen(display)),XA_SERVER_OVERLAY_VISUALS,0,(65536/sizeof(long)),False,XA_SERVER_OVERLAY_VISUALS,&actual_type,&actual_format,&nitems,&bytes_after,(unsigned char **)&data);
  if(result==Success && actual_type==XA_SERVER_OVERLAY_VISUALS && actual_format==32 && 1<=nitems){
    overlays=data;
    return nitems/(sizeof(Overlay)/sizeof(FXuint));
    }
  if(data) XFree(data);
  overlays=NULL;
  return 0;
  }

Overlay *overlays=NULL;
int noverlays=getOverlays(DISPLAY(getApp()),overlays);
FXTRACE((100,"noverlays=%d\n",noverlays));
if(noverlays){
  for(i=0; i<noverlays; i++){
    FXTRACE((100,"visual       = %d\n",overlays[i].visualid));
    FXTRACE((100,"transparency = %d\n",overlays[i].transparency));
    FXTRACE((100,"value        = %08x\n",overlays[i].value));
    FXTRACE((100,"layer        = %d\n",overlays[i].layer));
    }
  XFree(overlays);
  }
XVisualInfo *vis=glXGetVisualFromFBConfig((Display*)getApp()->getDisplay(),GLXFBConfig config);
extern Visual *_XVIDtoVisual(Display* dpy,VisualID id);


*/

#ifndef GLX_ARB_fbconfig_float
#define GLX_RGBA_FLOAT_TYPE_ARB            0x20B9
#define GLX_RGBA_FLOAT_BIT_ARB             0x00000004
#endif
#ifndef GLX_ARB_multisample
#define GLX_SAMPLE_BUFFERS_ARB             100000
#define GLX_SAMPLES_ARB                    100001
#endif


// Create GL context
void FXGLContext::create(){
#ifdef HAVE_GL_H
  if(!xid){
    if(getApp()->isInitialized()){
      FXTRACE((100,"FXGLContext::create %p\n",this));

      // Shared context must exist
      if(shared && !shared->id()){ fxerror("%s::create: shared context not created yet.\n",getClassName()); }

      // Check for GL extension
      if(glXQueryExtension((Display*)getApp()->getDisplay(),NULL,NULL)){
        GLXFBConfig *fb;
        int nfb;

        // Get frame buffer configurations
        fb=glXGetFBConfigs((Display*)getApp()->getDisplay(),DefaultScreen((Display*)getApp()->getDisplay()),&nfb);
        if(fb){
          int glredsize,glgreensize,glbluesize,glalphasize,gldepthsize,glstencilsize,glaccumredsize,glaccumgreensize,glaccumbluesize,glaccumalphasize,gldouble,glstereo,glvisual,gldrawables,glrender,gllevel,glsamples;
          int dred,dgreen,dblue,dalpha,ddepth,dstencil,dsamples,daccred,daccgreen,daccblue,daccalpha,dmatch,fbid;
          int defvisual=XVisualIDFromVisual(DefaultVisual((Display*)getApp()->getDisplay(),DefaultScreen((Display*)getApp()->getDisplay())));
          int bestmatch=1000000000;
          int best=-1;
          int v;

          FXTRACE((140,"Found %d OpenGL configs\n",nfb));
          FXTRACE((140,"Default visual=0x%02x\n",defvisual));

          FXTRACE((140,"glXGetClientString(GLX_VENDOR)=%s\n",glXGetClientString((Display*)getApp()->getDisplay(),GLX_VENDOR)));
          FXTRACE((140,"glXGetClientString(GLX_VERSION)=%s\n",glXGetClientString((Display*)getApp()->getDisplay(),GLX_VERSION)));
          FXTRACE((140,"glXGetClientString(GLX_EXTENSIONS)=%s\n",glXGetClientString((Display*)getApp()->getDisplay(),GLX_EXTENSIONS)));

          // Find the best one
          for(v=0; v<nfb; v++){

            // Get supported drawable types
            if(glXGetFBConfigAttrib((Display*)getApp()->getDisplay(),fb[v],GLX_DRAWABLE_TYPE,&gldrawables)!=Success) continue;
            if((desired.flags&FXGLConfig::DrawWindow) && !(gldrawables&GLX_WINDOW_BIT)) continue;
            if((desired.flags&FXGLConfig::DrawImage) && !(gldrawables&GLX_PIXMAP_BIT)) continue;
            if((desired.flags&FXGLConfig::DrawBuffer) && !(gldrawables&GLX_PBUFFER_BIT)) continue;

            // Get supported render type; we don't want index mode
            if(glXGetFBConfigAttrib((Display*)getApp()->getDisplay(),fb[v],GLX_RENDER_TYPE,&glrender)!=Success) continue;
            if(!(glrender&GLX_RGBA_BIT|GLX_RGBA_FLOAT_BIT_ARB)) continue;

            // Get overlay level; we don't want overlays
            if(glXGetFBConfigAttrib((Display*)getApp()->getDisplay(),fb[v],GLX_LEVEL,&gllevel)!=Success) continue;
            if(gllevel!=0) continue;

            // Visual ID if it matters
            if(glXGetFBConfigAttrib((Display*)getApp()->getDisplay(),fb[v],GLX_VISUAL_ID,&glvisual)!=Success) continue;

            // Get stereo and double buffer support
            if(glXGetFBConfigAttrib((Display*)getApp()->getDisplay(),fb[v],GLX_DOUBLEBUFFER,&gldouble)!=Success) continue;
            if(glXGetFBConfigAttrib((Display*)getApp()->getDisplay(),fb[v],GLX_STEREO,&glstereo)!=Success) continue;

            // Get plane depths
            if(glXGetFBConfigAttrib((Display*)getApp()->getDisplay(),fb[v],GLX_RED_SIZE,&glredsize)!=Success) continue;
            if(glXGetFBConfigAttrib((Display*)getApp()->getDisplay(),fb[v],GLX_GREEN_SIZE,&glgreensize)!=Success) continue;
            if(glXGetFBConfigAttrib((Display*)getApp()->getDisplay(),fb[v],GLX_BLUE_SIZE,&glbluesize)!=Success) continue;
            if(glXGetFBConfigAttrib((Display*)getApp()->getDisplay(),fb[v],GLX_ALPHA_SIZE,&glalphasize)!=Success) continue;
            if(glXGetFBConfigAttrib((Display*)getApp()->getDisplay(),fb[v],GLX_DEPTH_SIZE,&gldepthsize)!=Success) continue;
            if(glXGetFBConfigAttrib((Display*)getApp()->getDisplay(),fb[v],GLX_STENCIL_SIZE,&glstencilsize)!=Success) continue;
            if(glXGetFBConfigAttrib((Display*)getApp()->getDisplay(),fb[v],GLX_ACCUM_RED_SIZE,&glaccumredsize)!=Success) continue;
            if(glXGetFBConfigAttrib((Display*)getApp()->getDisplay(),fb[v],GLX_ACCUM_GREEN_SIZE,&glaccumgreensize)!=Success) continue;
            if(glXGetFBConfigAttrib((Display*)getApp()->getDisplay(),fb[v],GLX_ACCUM_BLUE_SIZE,&glaccumbluesize)!=Success) continue;
            if(glXGetFBConfigAttrib((Display*)getApp()->getDisplay(),fb[v],GLX_ACCUM_ALPHA_SIZE,&glaccumalphasize)!=Success) continue;

            // Get multisample support (if we don't succeed, set it to zero)
            if(glXGetFBConfigAttrib((Display*)getApp()->getDisplay(),fb[v],GLX_SAMPLES_ARB,&glsamples)!=Success) glsamples=0;

            // Frame buffer config id
            if(glXGetFBConfigAttrib((Display*)getApp()->getDisplay(),fb[v],GLX_FBCONFIG_ID,&fbid)!=Success) continue;

            // We prefer to get a few MORE bits in RGBA than we asked for
            dred   = glredsize-desired.redSize;     if(dred<0)   dred   *= -100;
            dgreen = glgreensize-desired.greenSize; if(dgreen<0) dgreen *= -100;
            dblue  = glbluesize-desired.blueSize;   if(dblue<0)  dblue  *= -100;
            dalpha = glalphasize-desired.alphaSize; if(dalpha<0) dalpha *= -100;

            // Prefer better Z than asked, but colors more important
            ddepth = gldepthsize-desired.depthSize; if(ddepth<0) ddepth *= -10;

            // We care about colors and Z depth more than stencil depth
            dstencil = glstencilsize-desired.stencilSize; if(dstencil<0) dstencil *= -1;

            // Accumulation buffers
            daccred=glaccumredsize-desired.accumRedSize;       if(daccred<0)   daccred   *= -1;
            daccgreen=glaccumgreensize-desired.accumGreenSize; if(daccgreen<0) daccgreen *= -1;
            daccblue=glaccumbluesize-desired.accumBlueSize;    if(daccblue<0)  daccblue  *= -1;
            daccalpha=glaccumalphasize-desired.accumAlphaSize; if(daccalpha<0) daccalpha *= -1;

            // Want the best colors, of course
            dmatch=dred+dgreen+dblue+dalpha;

            // Accumulation buffers
            dmatch+=daccred+daccgreen+daccblue+daccalpha;

            // Extra penalty for no alpha if we asked for alpha, but no
            // penalty at all if there is alpha and we didn't ask for it.
            if(desired.alphaSize>0){
              if(glalphasize<1) dmatch+=100000;
              }

            // Wanted Z-buffer
            if(desired.depthSize>0){
              if(gldepthsize<1) dmatch+=10000000;
              else dmatch+=ddepth;
              }
            else{
              if(gldepthsize>0) dmatch+=10000000;
              }

            // Stencil buffers desired
            if(desired.stencilSize>0){
              if(glstencilsize<1) dmatch+=10000;
              else dmatch+=dstencil;
              }
            else{
              if(glstencilsize>0) dmatch+=1;
              }

            // Multisamples
            if(desired.multiSamples>0){
              dsamples=glsamples-desired.multiSamples;
              dmatch+=FXABS(dsamples);
              }

            // Double buffering also quite strongly preferred
            if(desired.flags&FXGLConfig::DoubleBuffer){
              if(!gldouble) dmatch+=1000000;
              }
            else{
              if(gldouble) dmatch+=1000000;
              }

            // Stereo not so important
            if(desired.flags&FXGLConfig::StereoBuffer){
              if(!glstereo) dmatch+=10000;
              }
            else{
              if(glstereo) dmatch+=10000;
              }

            // Want float buffer
            if(desired.flags&FXGLConfig::FloatBuffer){
              if(!(glrender&GLX_RGBA_FLOAT_BIT_ARB)) dmatch+=1000;
              }
            else{
              if(!(glrender&GLX_RGBA_BIT)) dmatch+=1000;
              }

            // Trace
            FXTRACE((150,"FBConfig: #%d: fbid=%02x fbconfig=%p visual=0x%02x match=%d\n",v,fbid,fb[v],glvisual,dmatch));
            FXTRACE((150,"  drawables  = %s%s%s\n",(gldrawables&GLX_WINDOW_BIT)?"GLX_WINDOW_BIT ":"",(gldrawables&GLX_PIXMAP_BIT)?"GLX_PIXMAP_BIT ":"",(gldrawables&GLX_PBUFFER_BIT)?"GLX_PBUFFER_BIT":""));
            FXTRACE((150,"  render     = %s%s%s\n",(glrender&GLX_RGBA_BIT)?"GLX_RGBA_BIT ":"",(glrender&GLX_COLOR_INDEX_BIT)?"GLX_COLOR_INDEX_BIT ":"",(glrender&GLX_RGBA_FLOAT_BIT_ARB)?"GLX_RGBA_FLOAT_BIT_ARB":""));
            FXTRACE((150,"  red size   = %d\n",glredsize));
            FXTRACE((150,"  green size = %d\n",glgreensize));
            FXTRACE((150,"  blue size  = %d\n",glbluesize));
            FXTRACE((150,"  alpha size = %d\n",glalphasize));
            FXTRACE((150,"  depth size = %d\n",gldepthsize));
            FXTRACE((150,"  double buf = %d\n",gldouble));
            FXTRACE((150,"  stencil    = %d\n",glstencilsize));
            FXTRACE((150,"  acc red    = %d\n",glaccumredsize));
            FXTRACE((150,"  acc green  = %d\n",glaccumgreensize));
            FXTRACE((150,"  acc blue   = %d\n",glaccumbluesize));
            FXTRACE((150,"  acc alpha  = %d\n",glaccumalphasize));
            FXTRACE((150,"  stereo     = %d\n",glstereo));
            FXTRACE((150,"  samples    = %d\n",glsamples));

            // May the best config win
            if(dmatch<=bestmatch){

              // All other things being equal, we prefer default visual!
              if((dmatch<bestmatch) || (glvisual==defvisual)){
                actual.redSize=glredsize;
                actual.greenSize=glgreensize;
                actual.blueSize=glbluesize;
                actual.alphaSize=glalphasize;
                actual.depthSize=gldepthsize;
                actual.stencilSize=glstencilsize;
                actual.multiSamples=glsamples;
                actual.accumRedSize=glaccumredsize;
                actual.accumGreenSize=glaccumgreensize;
                actual.accumBlueSize=glaccumbluesize;
                actual.accumAlphaSize=glaccumalphasize;
                actual.flags=0;
                if(gldrawables&GLX_WINDOW_BIT) actual.flags|=FXGLConfig::DrawWindow;
                if(gldrawables&GLX_PIXMAP_BIT) actual.flags|=FXGLConfig::DrawImage;
                if(gldrawables&GLX_PBUFFER_BIT) actual.flags|=FXGLConfig::DrawBuffer;
                if(glrender&GLX_RGBA_FLOAT_BIT_ARB) actual.flags|=FXGLConfig::FloatBuffer;
                if(gldouble) actual.flags|=FXGLConfig::DoubleBuffer;
                if(glstereo) actual.flags|=FXGLConfig::StereoBuffer;
                bestmatch=dmatch;
                format=fb[v];                           // Remember fb config
                best=v;
                }
              }
            }

          // Hopefully, we got one
          if(0<=best){
            FXTRACE((140,"Best FBConfig: #%d: fbconfig=%p match=%d\n",best,format,bestmatch));
            xid=(FXID)glXCreateNewContext((Display*)getApp()->getDisplay(),fb[best],GLX_RGBA_TYPE,shared?(GLXContext)shared->id():(GLXContext)NULL,(desired.flags&FXGLConfig::Indirect)==0);
            if(xid && !glXIsDirect((Display*)getApp()->getDisplay(),(GLXContext)xid)){
              actual.flags|=FXGLConfig::Indirect;
              }
            }

          // Free list
          XFree(fb);
          }
        }

      // Check if successful
      if(!xid){
        throw FXWindowException("unable to create GL context.");
        }
      }
    }
#endif
  }



#else //////////////////////////////// X11 OLD //////////////////////////////////



// Create GL context
void FXGLContext::create(){
#ifdef HAVE_GL_H
  if(!xid){
    if(getApp()->isInitialized()){
      FXTRACE((100,"FXGLContext::create %p\n",this));

      // Shared context must exist
      if(shared && !shared->id()){ fxerror("%s::create: shared context not created yet.\n",getClassName()); }

      // Check for GL extension
      if(glXQueryExtension((Display*)getApp()->getDisplay(),NULL,NULL)){
        XVisualInfo vitemplate;
        XVisualInfo *vi;
        int nvi;

        // Get visuals
        vitemplate.screen=DefaultScreen((Display*)getApp()->getDisplay());
        vi=XGetVisualInfo((Display*)getApp()->getDisplay(),VisualScreenMask,&vitemplate,&nvi);
        if(vi){
          int glredsize,glgreensize,glbluesize,glalphasize,gldepthsize,glstencilsize,glaccumredsize,glaccumgreensize,glaccumbluesize,glaccumalphasize,gldouble,glstereo,glvisual,gldrawables,glrender,gllevel;
          int dred,dgreen,dblue,dalpha,ddepth,dstencil,daccred,daccgreen,daccblue,daccalpha,dmatch;
          int defvisual=XVisualIDFromVisual(DefaultVisual((Display*)getApp()->getDisplay(),DefaultScreen((Display*)getApp()->getDisplay())));
          int bestmatch=1000000000;
          int best=-1;
          int v;

          FXTRACE((150,"Found %d OpenGL visuals\n",nvi));
          FXTRACE((150,"Default visual=0x%02x\n",defvisual));

          // Find the best one
          for(v=0; v<nvi; v++){

            // GL support is requested
            if(glXGetConfig((Display*)getApp()->getDisplay(),&vi[v],GLX_USE_GL,&gldrawables)!=Success) continue;
            if(!gldrawables) continue;

            // Get supported render type
            if(glXGetConfig((Display*)getApp()->getDisplay(),&vi[v],GLX_RGBA,&glrender)!=Success) continue;
            if(!glrender) continue;

            // Get overlay level
            if(glXGetConfig((Display*)getApp()->getDisplay(),&vi[v],GLX_LEVEL,&gllevel)!=Success) continue;
            if(gllevel) continue;

            // Get stereo and double buffer support
            if(glXGetConfig((Display*)getApp()->getDisplay(),&vi[v],GLX_DOUBLEBUFFER,&gldouble)!=Success) continue;
            if(glXGetConfig((Display*)getApp()->getDisplay(),&vi[v],GLX_STEREO,&glstereo)!=Success) continue;

            // Visual ID if it matters
            glvisual=vi[v].visualid;

            // Get plane depths
            if(glXGetConfig((Display*)getApp()->getDisplay(),&vi[v],GLX_RED_SIZE,&glredsize)!=Success) continue;
            if(glXGetConfig((Display*)getApp()->getDisplay(),&vi[v],GLX_GREEN_SIZE,&glgreensize)!=Success) continue;
            if(glXGetConfig((Display*)getApp()->getDisplay(),&vi[v],GLX_BLUE_SIZE,&glbluesize)!=Success) continue;
            if(glXGetConfig((Display*)getApp()->getDisplay(),&vi[v],GLX_ALPHA_SIZE,&glalphasize)!=Success) continue;
            if(glXGetConfig((Display*)getApp()->getDisplay(),&vi[v],GLX_DEPTH_SIZE,&gldepthsize)!=Success) continue;
            if(glXGetConfig((Display*)getApp()->getDisplay(),&vi[v],GLX_STENCIL_SIZE,&glstencilsize)!=Success) continue;
            if(glXGetConfig((Display*)getApp()->getDisplay(),&vi[v],GLX_ACCUM_RED_SIZE,&glaccumredsize)!=Success) continue;
            if(glXGetConfig((Display*)getApp()->getDisplay(),&vi[v],GLX_ACCUM_GREEN_SIZE,&glaccumgreensize)!=Success) continue;
            if(glXGetConfig((Display*)getApp()->getDisplay(),&vi[v],GLX_ACCUM_BLUE_SIZE,&glaccumbluesize)!=Success) continue;
            if(glXGetConfig((Display*)getApp()->getDisplay(),&vi[v],GLX_ACCUM_ALPHA_SIZE,&glaccumalphasize)!=Success) continue;

            // We prefer to get a few MORE bits in RGBA than we asked for
            dred   = glredsize-desired.redSize;     if(dred<0)   dred   *= -100;
            dgreen = glgreensize-desired.greenSize; if(dgreen<0) dgreen *= -100;
            dblue  = glbluesize-desired.blueSize;   if(dblue<0)  dblue  *= -100;
            dalpha = glalphasize-desired.alphaSize; if(dalpha<0) dalpha *= -100;

            // Prefer better Z than asked, but colors more important
            ddepth = gldepthsize-desired.depthSize; if(ddepth<0) ddepth *= -10;

            // We care about colors and Z depth more than stencil depth
            dstencil = glstencilsize-desired.stencilSize; if(dstencil<0) dstencil *= -1;

            // Accumulation buffers
            daccred=glaccumredsize-desired.accumRedSize;       if(daccred<0)   daccred   *= -1;
            daccgreen=glaccumgreensize-desired.accumGreenSize; if(daccgreen<0) daccgreen *= -1;
            daccblue=glaccumbluesize-desired.accumBlueSize;    if(daccblue<0)  daccblue  *= -1;
            daccalpha=glaccumalphasize-desired.accumAlphaSize; if(daccalpha<0) daccalpha *= -1;

            // Want the best colors, of course
            dmatch=dred+dgreen+dblue+dalpha;

            // Accumulation buffers
            dmatch+=daccred+daccgreen+daccblue+daccalpha;

            // Extra penalty for no alpha if we asked for alpha, but no
            // penalty at all if there is alpha and we didn't ask for it.
            if(desired.alphaSize>0){
              if(glalphasize<1) dmatch+=100000;
              }

            // Wanted Z-buffer
            if(desired.depthSize>0){
              if(gldepthsize<1) dmatch+=10000000;
              else dmatch+=ddepth;
              }
            else{
              if(gldepthsize>0) dmatch+=10000000;
              }

            // Stencil buffers desired
            if(desired.stencilSize>0){
              if(glstencilsize<1) dmatch+=10000;
              else dmatch+=dstencil;
              }
            else{
              if(glstencilsize>0) dmatch+=1;
              }

            // Double buffering also quite strongly preferred
            if(desired.flags&FXGLConfig::DoubleBuffer){
              if(!gldouble) dmatch+=1000000;
              }
            else{
              if(gldouble) dmatch+=1000000;
              }

            // Stereo not so important
            if(desired.flags&FXGLConfig::StereoBuffer){
              if(!glstereo) dmatch+=10000;
              }
            else{
              if(glstereo) dmatch+=10000;
              }

            // Trace
            FXTRACE((150,"Visual: #%d: visual=0x%02x match=%d\n",v,glvisual,dmatch));
            FXTRACE((150,"  red size   = %d\n",glredsize));
            FXTRACE((150,"  green size = %d\n",glgreensize));
            FXTRACE((150,"  blue size  = %d\n",glbluesize));
            FXTRACE((150,"  alpha size = %d\n",glalphasize));
            FXTRACE((150,"  depth size = %d\n",gldepthsize));
            FXTRACE((150,"  double buf = %d\n",gldouble));
            FXTRACE((150,"  stencil    = %d\n",glstencilsize));
            FXTRACE((150,"  acc red    = %d\n",glaccumredsize));
            FXTRACE((150,"  acc green  = %d\n",glaccumgreensize));
            FXTRACE((150,"  acc blue   = %d\n",glaccumbluesize));
            FXTRACE((150,"  acc alpha  = %d\n",glaccumalphasize));
            FXTRACE((150,"  stereo     = %d\n",glstereo));

            // May the best config win
            if(dmatch<=bestmatch){

              // All other things being equal, we prefer default visual!
              if((dmatch<bestmatch) || (glvisual==defvisual)){
                actual.redSize=glredsize;
                actual.greenSize=glgreensize;
                actual.blueSize=glbluesize;
                actual.alphaSize=glalphasize;
                actual.depthSize=gldepthsize;
                actual.stencilSize=glstencilsize;
                actual.multiSamples=0;
                actual.accumRedSize=glaccumredsize;
                actual.accumGreenSize=glaccumgreensize;
                actual.accumBlueSize=glaccumbluesize;
                actual.accumAlphaSize=glaccumalphasize;
                actual.flags|=(FXGLConfig::DrawWindow|FXGLConfig::DrawImage);
                if(gldouble) actual.flags|=FXGLConfig::DoubleBuffer;
                if(glstereo) actual.flags|=FXGLConfig::StereoBuffer;
                bestmatch=dmatch;
                format=(void*)(FXival)vi[v].visualid;   // Remember visual id
                best=v;
                }
              }
            }

          // Hopefully, we got one
          if(0<=best){
            FXTRACE((140,"Best Visual: #%d: visualid=%p match=%d\n",best,format,bestmatch));
            xid=(FXID)glXCreateContext((Display*)getApp()->getDisplay(),&vi[best],shared?(GLXContext)shared->id():(GLXContext)NULL,(desired.flags&FXGLConfig::Indirect)==0);
            if(xid && !glXIsDirect((Display*)getApp()->getDisplay(),(GLXContext)xid)){
              actual.flags|=FXGLConfig::Indirect;
              }
            }

          // Free stuff
          XFree(vi);
          }
        }

      // Check if successful
      if(!xid){
        throw FXWindowException("unable to create GL context.");
        }
      }
    }
#endif
  }


#endif //////////////////////////////////////////////////////////////////////////


// Detach the GL context
void FXGLContext::detach(){
#ifdef HAVE_GL_H
  if(xid){
    FXTRACE((100,"FXGLContext::detach %p\n",this));
    drawable=NULL;
    xid=0;
    }
#endif
  }


// Destroy the GL context
void FXGLContext::destroy(){
#ifdef HAVE_GL_H
  if(xid){
    if(getApp()->isInitialized()){
      FXTRACE((100,"FXGLContext::destroy %p\n",this));
#ifdef WIN32
      wglDeleteContext((HGLRC)xid);
#else
      glXDestroyContext((Display*)getApp()->getDisplay(),(GLXContext)xid);
#endif
      }
    drawable=NULL;
    xid=0;
    }
#endif
  }


//  Make the rendering context of drawable current
FXbool FXGLContext::begin(FXDrawable *draw){
#ifdef HAVE_GL_H
  if(xid && !drawable){
#ifdef WIN32
    HDC hdc=(HDC)draw->GetDC();
    if(draw->getVisual()->colormap){
      SelectPalette(hdc,(HPALETTE)draw->getVisual()->colormap,false);
      RealizePalette(hdc);
      }
    if(wglMakeCurrent(hdc,(HGLRC)xid)){
      drawable=draw;
      return true;
      }
#else
    if(glXMakeCurrent((Display*)getApp()->getDisplay(),draw->id(),(GLXContext)xid)){
      drawable=draw;
      return true;
      }
#endif
    }
#endif
  return false;
  }


// Make the rendering context of drawable non-current
FXbool FXGLContext::end(){
#ifdef HAVE_GL_H
  if(xid && drawable){
#ifdef WIN32
    HDC hdc=wglGetCurrentDC();
    if(wglMakeCurrent(NULL,NULL)!=0){
      drawable->ReleaseDC(hdc);
      drawable=NULL;
      return true;
      }
#else
    if(glXMakeCurrent((Display*)getApp()->getDisplay(),None,(GLXContext)NULL)){
      drawable=NULL;
      return true;
      }
#endif
    }
#endif
  return false;
  }


// Used by GL to swap the buffers in double buffer mode, or flush a single buffer
void FXGLContext::swapBuffers(){
#ifdef HAVE_GL_H
  if(xid){
#ifdef WIN32
    if(wglSwapLayerBuffers(wglGetCurrentDC(),WGL_SWAP_MAIN_PLANE)==false){
      SwapBuffers(wglGetCurrentDC());
      }
#else
    glXSwapBuffers((Display*)getApp()->getDisplay(),glXGetCurrentDrawable());
#endif
    }
#endif
  }


// Close and release any resources
FXGLContext::~FXGLContext(){
  FXTRACE((100,"FXGLContext::~FXGLContext %p\n",this));
  destroy();
  drawable=(FXDrawable*)-1L;
  shared=(FXGLContext*)-1L;
  }

}
