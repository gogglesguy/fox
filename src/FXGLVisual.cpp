/********************************************************************************
*                                                                               *
*                            V i s u a l   C l a s s                            *
*                                                                               *
*********************************************************************************
* Copyright (C) 1999,2007 by Jeroen van der Zijp.   All Rights Reserved.        *
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
* $Id: FXGLVisual.cpp,v 1.117 2007/12/31 18:23:00 fox Exp $                     *
********************************************************************************/
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "FXHash.h"
#include "FXThread.h"
#include "FXElement.h"
#include "FXStream.h"
#include "FXString.h"
#include "FXSize.h"
#include "FXPoint.h"
#include "FXRectangle.h"
#include "FXSettings.h"
#include "FXRegistry.h"
#include "FXAccelTable.h"
#include "FXApp.h"
#include "FXFont.h"
#include "FXException.h"
#include "FXGLVisual.h"


/*
  Notes:

  - FXGLVisual builds a visual/pixelformat suitable for GL drawing.

  - Selection of visual/pixelformat is based on a best match to a
    given set of hints, according to some heuristics:

    1) Prefer color depth close to asked ones; really like to
       get a bit MORE color, rather than LESS, however.

    2) If we wanted Z-buffer, it is STRONGLY preferred; If there's
       a choice, we prefer more color depth over more Z-depth; if
       we already have more colors than requested, we prefer to meet
       requested Z depth.

    3) If we wanted double buffer, we strongly prefer it over color and
       Z depth, but HAVING a Z-buffer is still more important.

    4) If we wanted alpha buffer, it is preferred, but Z-buffering
       and double buffering are considered more important.
       If there's a choice, we prefer to receive a few MORE bits of
       alpha buffer than we asked for, rather than LESS.

    5) If we wanted stereo, we prefer it, but almost everything except
       the color-, alpha-, and Z-depths are more important.

  - Some further tuning may be desired, but I think this should satisfy
    most cases....

  - Note that as long as OpenGL is in any way supported, you should ALWAYS
    be able to get at least some visual/pixelformat you can draw on.

  - As far as hardware acceleration goes, H/W acceleration should be
    enabled, possibly at the expense of color-, alpha-, and Z-depth;
    but NEVER at the expense of presence or absence of a requested feature.
    We only drop FEATURES which are requested if there is neither hardware
    nor software support.

    For example, we may trade in some Z-depth, but not the entire Z-buffer,
    to get a hardware accelerated visual/pixelformat.
*/


using namespace FX;

/*******************************************************************************/

namespace FX {

// Object implementation
FXIMPLEMENT(FXGLVisual,FXVisual,NULL,0)


// Deserialization
FXGLVisual::FXGLVisual(){
  redSize=0;
  greenSize=0;
  blueSize=0;
  alphaSize=0;
  depthSize=0;
  stencilSize=0;
  multiSamples=0;
  accumRedSize=0;
  accumGreenSize=0;
  accumBlueSize=0;
  accumAlphaSize=0;
  actualRedSize=0;
  actualGreenSize=0;
  actualBlueSize=0;
  actualAlphaSize=0;
  actualDepthSize=0;
  actualStencilSize=0;
  actualMultiSamples=0;
  actualAccumRedSize=0;
  actualAccumGreenSize=0;
  actualAccumBlueSize=0;
  actualAccumAlphaSize=0;
  doubleBuffer=false;
  stereoBuffer=false;
  accelerated=false;
  copying=false;
  }


// Construct
FXGLVisual::FXGLVisual(FXApp* a,FXuint flgs):FXVisual(a,flgs,0){
  FXTRACE((100,"FXGLVisual::FXGLVisual %p\n",this));
  redSize=8;
  greenSize=8;
  blueSize=8;
  alphaSize=0;
  depthSize=24;
  stencilSize=0;
  multiSamples=0;
  accumRedSize=0;
  accumGreenSize=0;
  accumBlueSize=0;
  accumAlphaSize=0;
  actualRedSize=0;
  actualGreenSize=0;
  actualBlueSize=0;
  actualAlphaSize=0;
  actualDepthSize=0;
  actualStencilSize=0;
  actualMultiSamples=0;
  actualAccumRedSize=0;
  actualAccumGreenSize=0;
  actualAccumBlueSize=0;
  actualAccumAlphaSize=0;
  doubleBuffer=false;
  stereoBuffer=false;
  accelerated=false;
  copying=false;
  }


// Test if OpenGL is possible
FXbool FXGLVisual::hasOpenGL(FXApp* application){
  if(application->isInitialized()){
#ifdef HAVE_GL_H
#ifdef WIN32            // WIN32
    return true;
#else                   // UNIX
    return glXQueryExtension((Display*)application->getDisplay(),NULL,NULL);
#endif
#endif
    }
  return false;
  }


/*******************************************************************************/


#if defined(WIN32) ///////////////// WIN32 //////////////////////////////////////


struct LOGPALETTE256 {
  WORD         palVersion;
  WORD         palNumEntries;
  PALETTEENTRY palPalEntry[257];
  };


// System colors to match against
static FXuchar defSysClr[20][3] = {
    {  0,  0,  0},
    {128,  0,  0},
    {  0,128,  0},
    {128,128,  0},
    {  0,  0,128},
    {128,  0,128},
    {  0,128,128},
    {192,192,192},

    {192,220,192},
    {166,202,240},
    {255,251,240},
    {160,160,164},

    {128,128,128},
    {255,  0,  0},
    {  0,255,  0},
    {255,255,  0},
    {  0,  0,255},
    {255,  0,255},
    {  0,255,255},
    {255,255,255}
    };

static int defaultOverride[13] = {
  0, 3, 24, 27, 64, 67, 88, 173, 181, 236, 247, 164, 91
  };


// Make palette
static HPALETTE makeOpenGLPalette(PIXELFORMATDESCRIPTOR* info){
  LOGPALETTE256 palette;
  int num,i,j,rr,gg,bb;
  int rmax,gmax,bmax;
  HPALETTE hPalette;

  // Size of palette array
  num=1<<((PIXELFORMATDESCRIPTOR*)info)->cColorBits;

  FXASSERT(num<=256);

  // Maximum values each color
  rmax=(1 << info->cRedBits)-1;
  gmax=(1 << info->cGreenBits)-1;
  bmax=(1 << info->cBlueBits)-1;

  // Build palette
  for(rr=0; rr<=rmax; rr++){
    for(gg=0; gg<=gmax; gg++){
      for(bb=0; bb<=bmax; bb++){
        i = (rr << info->cRedShift) | (gg << info->cGreenShift) | (bb << info->cBlueShift);
        palette.palPalEntry[i].peRed = (255*rr)/rmax;
        palette.palPalEntry[i].peGreen = (255*gg)/gmax;
        palette.palPalEntry[i].peBlue = (255*bb)/bmax;
        palette.palPalEntry[i].peFlags = PC_NOCOLLAPSE;
        }
      }
    }

  // For 8-bit palette
  if((info->cColorBits==8) && (info->cRedBits==3) && (info->cRedShift==0) && (info->cGreenBits==3) && (info->cGreenShift==3) && (info->cBlueBits==2) && (info->cBlueShift==6)){
    for(j=1; j<=12; j++){
      palette.palPalEntry[defaultOverride[j]].peRed=defSysClr[j][0];
      palette.palPalEntry[defaultOverride[j]].peGreen=defSysClr[j][1];
      palette.palPalEntry[defaultOverride[j]].peBlue=defSysClr[j][1];
      palette.palPalEntry[defaultOverride[j]].peFlags=0;
      }
    }

  // Fill in the rest
  palette.palVersion=0x300;
  palette.palNumEntries=num;

  // Make palette
  hPalette=CreatePalette((const LOGPALETTE*)&palette);

  return hPalette;
  }

/*
typedef struct tagPIXELFORMATDESCRIPTOR {
  WORD  nSize;
  WORD  nVersion;
  DWORD dwFlags;
  BYTE  iPixelType;
  BYTE  cColorBits;
  BYTE  cRedBits;
  BYTE  cRedShift;
  BYTE  cGreenBits;
  BYTE  cGreenShift;
  BYTE  cBlueBits;
  BYTE  cBlueShift;
  BYTE  cAlphaBits;
  BYTE  cAlphaShift;
  BYTE  cAccumBits;
  BYTE  cAccumRedBits;
  BYTE  cAccumGreenBits;
  BYTE  cAccumBlueBits;
  BYTE  cAccumAlphaBits;
  BYTE  cDepthBits;
  BYTE  cStencilBits;
  BYTE  cAuxBuffers;
  BYTE  iLayerType;
  BYTE  bReserved;
  DWORD dwLayerMask;
  DWORD dwVisibleMask;
  DWORD dwDamageMask;
  } PIXELFORMATDESCRIPTOR;
*/

#if 0
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

use TEXT("GLTEMP") window class!

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
            int dred,dgreen,dblue,dalpha,ddepth,dstencil,dsamples,daccred,daccgreen,daccblue,daccalpha,match;
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
                  match=dred+dgreen+dblue+dalpha;

                  // Accumulation buffers
                  match+=daccred+daccgreen+daccblue+daccalpha;

                  // Extra penalty for no alpha if we asked for alpha, but no
                  // penalty at all if there is alpha and we didn't ask for it.
                  if(desired.alphaSize>0){
                    if(glalphasize<1) match+=100000;
                    }

                  // Wanted Z-buffer
                  if(desired.depthSize>0){
                    if(gldepthsize<1) match+=10000000;
                    else match+=ddepth;
                    }
                  else{
                    if(gldepthsize>0) match+=10000000;
                    }

                  // Stencil buffers desired
                  if(desired.stencilSize>0){
                    if(glstencilsize<1) match+=10000;
                    else match+=dstencil;
                    }
                  else{
                    if(glstencilsize>0) match+=1;
                    }

                  // Multisamples
                  if(desired.multiSamples>0){
                    dsamples=glsamples-desired.multiSamples;
                    match+=FXABS(dsamples);
                    }

                  // Double buffering also quite strongly preferred
                  if(desired.flags&FXGLConfig::DoubleBuffer){
                    if(!gldouble) match+=1000000;
                    }
                  else{
                    if(gldouble) match+=1000000;
                    }

                  // Stereo not so important
                  if(desired.flags&FXGLConfig::StereoBuffer){
                    if(!glstereo) match+=10000;
                    }
                  else{
                    if(glstereo) match+=10000;
                    }

                  // Want float buffer
                  if(desired.flags&FXGLConfig::FloatBuffer){
                    if(!glrenderfloat) match+=1000;
                    }
                  else{
                    if(!glrenderrgb) match+=1000;
                    }

                  // Trace
                  FXTRACE((150,"PixelFormat: #%d: match=%d\n",v,match));
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
                  if(match<=bestmatch){
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
                    bestmatch=match;
                    format=v;                   // Remember pixel format
                    visual=v;                   // Remember pixel format
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
        throw FXWindowException("unable to create GL context.");
        }
      }
    }
#endif
  }
#endif


// Initialize
void FXGLVisual::create(){
#ifdef HAVE_GL_H
  if(!xid){
    if(getApp()->isInitialized()){
      FXTRACE((100,"%s::create %p\n",getClassName(),this));
      PIXELFORMATDESCRIPTOR pfd;
      HDC hdc;
      int gdblbuf,glstereo,gldepth,glaccel;
      int glred,glgreen,glblue,glalpha,glstencil;
      int glaccred,glaccgreen,glaccblue,glaccalpha;
      int glindex,glswapcopy;
      int bestvis,dmatch,bestmatch;
      int dred,dgreen,dblue,ddepth,dalpha,dstencil;
      int daccred,daccgreen,daccblue,daccalpha;
      int chosen,nvi,i;


      // Get some window handle
      hdc=GetDC(GetDesktopWindow());

      // Get number of supported pixel formats
      pfd.nSize=sizeof(PIXELFORMATDESCRIPTOR);
      pfd.nVersion=1;
      nvi=DescribePixelFormat(hdc,1,sizeof(PIXELFORMATDESCRIPTOR),&pfd);
      if(nvi==0){ fxerror("%s::create: no OpenGL visual available.\n",getClassName()); }

      // Try to find the best
      bestvis=-1;
      bestmatch=1000000000;

      for(i=1; i<=nvi; i++) {

        // Get info about this visual
        DescribePixelFormat(hdc,i,sizeof(PIXELFORMATDESCRIPTOR),&pfd);

        // Make sure this visual is valid
        chosen=ChoosePixelFormat(hdc,&pfd);
        if(chosen!=i) continue;

        // Draw to window is required
        if(!(pfd.dwFlags&PFD_DRAW_TO_WINDOW)) continue;

        // OpenGL support is required
        if(!(pfd.dwFlags&PFD_SUPPORT_OPENGL)) continue;

        // RGBA support is required
        if(pfd.iPixelType!=PFD_TYPE_RGBA) continue;

        // Skip accelerated formats if VISUAL_NOACCEL specified
        if((flags&VISUAL_NO_ACCEL) && !(pfd.dwFlags&PFD_GENERIC_FORMAT)) continue;

        // Is this a hardware-accelerated visual?
        glaccel=(pfd.dwFlags&PFD_GENERIC_FORMAT)==0;

        // Must have single/double buffer as requested
        gdblbuf=(pfd.dwFlags&PFD_DOUBLEBUFFER)!=0;

        // Stereo support as requested
        glstereo=(pfd.dwFlags&PFD_STEREO)!=0;

        // Must have RGB as requested
        glindex=(pfd.iPixelType&PFD_TYPE_COLORINDEX)!=0;

        // Copy swap buffer
        glswapcopy=(pfd.dwFlags&PFD_SWAP_COPY)!=0;

        // Get planes
        glred=pfd.cRedBits;
        glgreen=pfd.cGreenBits;
        glblue=pfd.cBlueBits;
        glalpha=pfd.cAlphaBits;
        gldepth=pfd.cDepthBits;
        glstencil=pfd.cStencilBits;
        glaccred=pfd.cAccumRedBits;
        glaccgreen=pfd.cAccumGreenBits;
        glaccblue=pfd.cAccumBlueBits;
        glaccalpha=pfd.cAccumAlphaBits;

        // We prefer to get a few MORE bits in RGBA than we asked for
        dred   = glred-redSize;     if(dred<0)   dred   *= -100;
        dgreen = glgreen-greenSize; if(dgreen<0) dgreen *= -100;
        dblue  = glblue-blueSize;   if(dblue<0)  dblue  *= -100;
        dalpha = glalpha-alphaSize; if(dalpha<0) dalpha *= -100;

        // Prefer better Z than asked, but colors more important
        ddepth = gldepth-depthSize; if(ddepth<0) ddepth *= -10;

        // We care about colors and Z depth more than stencil depth
        dstencil = glstencil-stencilSize; if(dstencil<0) dstencil *= -1;

        // Accumulation buffers
        daccred=glaccred-accumRedSize;       if(daccred<0)   daccred   *= -1;
        daccgreen=glaccgreen-accumGreenSize; if(daccgreen<0) daccgreen *= -1;
        daccblue=glaccblue-accumBlueSize;    if(daccblue<0)  daccblue  *= -1;
        daccalpha=glaccalpha-accumAlphaSize; if(daccalpha<0) daccalpha *= -1;

        // Want the best colors, of course
        dmatch=dred+dgreen+dblue+dalpha;

        // Accumulation buffers
        dmatch+=daccred+daccgreen+daccblue+daccalpha;

        // Hardware accelerated a plus
        if(!glaccel){
          dmatch+=10000;
          }

        // Extra penalty for no alpha if we asked for alpha, but no
        // penalty at all if there is alpha and we didn't ask for it.
        if(alphaSize>0){
          if(glalpha<1) dmatch+=100000;
          }

        // Wanted Z-buffer
        if(depthSize>0){
          if(gldepth<1) dmatch+=100000000;
          else dmatch+=ddepth;
          }
        else{
          if(gldepth>0) dmatch+=100000000;
          }

        // Double buffering also quite strongly preferred
        if(flags&VISUAL_DOUBLE_BUFFER){
          if(!gdblbuf) dmatch+=10000000;
          }
        else{
          if(gdblbuf) dmatch+=10000000;
          }

        // Color index mode only if requested
        if(flags&VISUAL_INDEX){
          if(!glindex) dmatch+=10000000;
          }
        else{
          if(glindex) dmatch+=10000000;
          }

        // Stencil buffers desired
        if(stencilSize>0){
          if(glstencil<1) dmatch+=10000;
          else dmatch+=dstencil;
          }
        else{
          if(glstencil>0) dmatch+=1;
          }

        // Stereo not so important
        if(flags&VISUAL_STEREO){
          if(!glstereo) dmatch+=100000;
          }
        else{
          if(glstereo) dmatch+=100000;
          }

        // Swap copies also important
        if(flags&VISUAL_SWAP_COPY){
          if(!glswapcopy) dmatch+=10000000;
          }

        // Trace
        FXTRACE((150,"Pixel Format (%d) match value = %d\n",i,dmatch));
        FXTRACE((150,"  red size    = %d\n",glred));
        FXTRACE((150,"  green size  = %d\n",glgreen));
        FXTRACE((150,"  blue size   = %d\n",glblue));
        FXTRACE((150,"  alpha size  = %d\n",glalpha));
        FXTRACE((150,"  depth size  = %d\n",gldepth));
        FXTRACE((150,"  double buf  = %d\n",gdblbuf));
        FXTRACE((150,"  stencil     = %d\n",glstencil));
        FXTRACE((150,"  acc red     = %d\n",glaccred));
        FXTRACE((150,"  acc green   = %d\n",glaccgreen));
        FXTRACE((150,"  acc blue    = %d\n",glaccblue));
        FXTRACE((150,"  acc alpha   = %d\n",glaccalpha));
        FXTRACE((150,"  stereo      = %d\n",glstereo));
        FXTRACE((150,"  accelerated = %d\n",glaccel));
        FXTRACE((150,"  swap copy   = %d\n",glswapcopy));

        // May the best visual win
        if(dmatch<=bestmatch){
          actualRedSize=glred;
          actualGreenSize=glgreen;
          actualBlueSize=glblue;
          actualAlphaSize=glalpha;
          actualDepthSize=gldepth;
          actualStencilSize=glstencil;
          actualAccumRedSize=glaccred;
          actualAccumGreenSize=glaccgreen;
          actualAccumBlueSize=glaccblue;
          actualAccumAlphaSize=glaccalpha;
          doubleBuffer=gdblbuf;
          stereoBuffer=glstereo;
          accelerated=glaccel;
          copying=glswapcopy;
          bestmatch=dmatch;
          bestvis=i;
          }
        }

      // Still no luck
      if(bestvis<0){ fxerror("%s::create: no OpenGL visual available.\n",getClassName()); }

      // Report best visual
      FXTRACE((150,"Best Pixel Format (%d) match value = %d\n",bestvis,bestmatch));

      // Get the true h/w capabilities
      DescribePixelFormat(hdc,bestvis,sizeof(PIXELFORMATDESCRIPTOR),&pfd);
      visual=(void*)(FXival)bestvis;
      depth=pfd.cRedBits+pfd.cGreenBits+pfd.cBlueBits;
      numred=(1<<pfd.cRedBits);
      numgreen=(1<<pfd.cGreenBits);
      numblue=(1<<pfd.cBlueBits);
      numcolors=numred*numgreen*numblue;

      // Make a palette for it if needed
      if(pfd.dwFlags&PFD_NEED_PALETTE){
        colormap=makeOpenGLPalette(&pfd);
        freemap=true;
        }

      // Done with that window
      ReleaseDC(GetDesktopWindow(),hdc);

      xid=(FXID)bestvis;

      // Report what we got if tracing enabled
      FXTRACE((150,"  redSize:        %d\n",getActualRedSize()));
      FXTRACE((150,"  greenSize:      %d\n",getActualGreenSize()));
      FXTRACE((150,"  blueSize:       %d\n",getActualBlueSize()));
      FXTRACE((150,"  alphaSize:      %d\n",getActualAlphaSize()));
      FXTRACE((150,"  depthSize:      %d\n",getActualDepthSize()));
      FXTRACE((150,"  stencilSize:    %d\n",getActualStencilSize()));
      FXTRACE((150,"  accumRedSize:   %d\n",getActualAccumRedSize()));
      FXTRACE((150,"  accumGreenSize: %d\n",getActualAccumGreenSize()));
      FXTRACE((150,"  accumBlueSize:  %d\n",getActualAccumBlueSize()));
      FXTRACE((150,"  accumAlphaSize: %d\n",getActualAccumAlphaSize()));
      FXTRACE((150,"  doubleBuffer:   %d\n",isDoubleBuffer()));
      FXTRACE((150,"  stereo:         %d\n",isStereo()));
      FXTRACE((150,"  accelerated:    %d\n",isAccelerated()));
      }
    }
#endif
  }


#elif defined(GLX_VERSION_1_3) //////// X11 NEW /////////////////////////////////


#ifndef GLX_ARB_fbconfig_float
#define GLX_RGBA_FLOAT_TYPE_ARB            0x20B9
#define GLX_RGBA_FLOAT_BIT_ARB             0x00000004
#endif
#ifndef GLX_SAMPLE_BUFFERS
#define GLX_SAMPLE_BUFFERS                 100000
#endif
#ifndef GLX_SAMPLES
#define GLX_SAMPLES                        100001
#endif


// Initialize
void FXGLVisual::create(){
#ifdef HAVE_GL_H
  if(!xid){
    if(getApp()->isInitialized()){
      FXTRACE((100,"%s::create %p\n",getClassName(),this));

      // Assume the default
      visual=DefaultVisual((Display*)getApp()->getDisplay(),DefaultScreen((Display*)getApp()->getDisplay()));
      depth=DefaultDepth((Display*)getApp()->getDisplay(),DefaultScreen((Display*)getApp()->getDisplay()));

      // OpenGL is available if we're talking to an OpenGL-capable X-Server
      if(glXQueryExtension((Display*)getApp()->getDisplay(),NULL,NULL)){
        GLXFBConfig *fb;
        int nfb;

        // Get frame buffer configurations
        fb=glXGetFBConfigs((Display*)getApp()->getDisplay(),DefaultScreen((Display*)getApp()->getDisplay()),&nfb);
        if(fb){
          int glredsize,glgreensize,glbluesize,glalphasize,gldepthsize,glstencilsize,glsamples,glaccumredsize,glaccumgreensize,glaccumbluesize,glaccumalphasize,gldouble,glstereo,gldrawables,glrender,gllevel,glaccel;
          int dred,dgreen,dblue,dalpha,ddepth,dstencil,dsamples,daccred,daccgreen,daccblue,daccalpha,match,visualid,configid;
          int defvisualid=XVisualIDFromVisual(DefaultVisual((Display*)getApp()->getDisplay(),DefaultScreen((Display*)getApp()->getDisplay())));
          int bestmatch=1000000000;
          int bestvisualid=0;
          int best=-1;
          int v;

          FXTRACE((150,"Found %d configs\n",nfb));
          FXTRACE((150,"Default visualid=0x%02x\n",defvisualid));

          // Find the best one
          for(v=0; v<nfb; v++){

            // Get supported drawable types
            if(glXGetFBConfigAttrib((Display*)getApp()->getDisplay(),fb[v],GLX_DRAWABLE_TYPE,&gldrawables)!=Success) continue;
            if((flags&VISUAL_WINDOW) && !(gldrawables&GLX_WINDOW_BIT)) continue;
            if((flags&VISUAL_IMAGE) && !(gldrawables&GLX_PIXMAP_BIT)) continue;
            if((flags&VISUAL_BUFFER) && !(gldrawables&GLX_PBUFFER_BIT)) continue;

            // Get supported render type; we don't want index mode
            if(glXGetFBConfigAttrib((Display*)getApp()->getDisplay(),fb[v],GLX_RENDER_TYPE,&glrender)!=Success) continue;
            if(!(glrender&GLX_RGBA_BIT|GLX_RGBA_FLOAT_BIT_ARB)) continue;

            // Get overlay level; we don't want overlays
            if(glXGetFBConfigAttrib((Display*)getApp()->getDisplay(),fb[v],GLX_LEVEL,&gllevel)!=Success) continue;
            if(gllevel!=0) continue;

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
            if(glXGetFBConfigAttrib((Display*)getApp()->getDisplay(),fb[v],GLX_SAMPLES,&glsamples)!=Success) glsamples=0;

            // Frame buffer config id
            if(glXGetFBConfigAttrib((Display*)getApp()->getDisplay(),fb[v],GLX_FBCONFIG_ID,&configid)!=Success) continue;

            // Visual ID if it matters
            if(glXGetFBConfigAttrib((Display*)getApp()->getDisplay(),fb[v],GLX_VISUAL_ID,&visualid)!=Success) continue;

            // Check if accelerated or not
            if(glXGetFBConfigAttrib((Display*)getApp()->getDisplay(),fb[v],GLX_CONFIG_CAVEAT,&glaccel)!=Success) continue;
            glaccel=(glaccel!=GLX_SLOW_CONFIG);

            // We prefer to get a few MORE bits in RGBA than we asked for
            dred   = glredsize-redSize;     if(dred<0)   dred   *= -100;
            dgreen = glgreensize-greenSize; if(dgreen<0) dgreen *= -100;
            dblue  = glbluesize-blueSize;   if(dblue<0)  dblue  *= -100;
            dalpha = glalphasize-alphaSize; if(dalpha<0) dalpha *= -100;

            // Prefer better Z than asked, but colors more important
            ddepth = gldepthsize-depthSize; if(ddepth<0) ddepth *= -10;

            // We care about colors and Z depth more than stencil depth
            dstencil = glstencilsize-stencilSize; if(dstencil<0) dstencil *= -1;

            // Accumulation buffers
            daccred=glaccumredsize-accumRedSize;       if(daccred<0)   daccred   *= -1;
            daccgreen=glaccumgreensize-accumGreenSize; if(daccgreen<0) daccgreen *= -1;
            daccblue=glaccumbluesize-accumBlueSize;    if(daccblue<0)  daccblue  *= -1;
            daccalpha=glaccumalphasize-accumAlphaSize; if(daccalpha<0) daccalpha *= -1;

            // Want the best colors, of course
            match=dred+dgreen+dblue+dalpha;

            // Accumulation buffers
            match+=daccred+daccgreen+daccblue+daccalpha;

            // Hardware accelerated is normally a plus
            if(!glaccel && !(flags&VISUAL_NO_ACCEL)){
              match+=10000;
              }

            // Extra penalty for no alpha if we asked for alpha, but no
            // penalty at all if there is alpha and we didn't ask for it.
            if(alphaSize>0){
              if(glalphasize<1) match+=100000;
              }

            // Wanted Z-buffer
            if(depthSize>0){
              if(gldepthsize<1) match+=10000000;
              else match+=ddepth;
              }
            else{
              if(gldepthsize>0) match+=10000000;
              }

            // Stencil buffers desired
            if(stencilSize>0){
              if(glstencilsize<1) match+=10000;
              else match+=dstencil;
              }
            else{
              if(glstencilsize>0) match+=1;
              }

            // Multisamples
            if(multiSamples>0){
              dsamples=glsamples-multiSamples;
              if(dsamples<0) dsamples*=-10;
              match+=dsamples;
              }

            // Double buffering also quite strongly preferred
            if(flags&VISUAL_DOUBLE_BUFFER){
              if(!gldouble) match+=1000000;
              }
            else{
              if(gldouble) match+=1000000;
              }

            // Stereo not so important
            if(flags&VISUAL_STEREO){
              if(!glstereo) match+=10000;
              }
            else{
              if(glstereo) match+=10000;
              }

            // Want float buffer
            if(flags&VISUAL_FLOAT){
              if(!(glrender&GLX_RGBA_FLOAT_BIT_ARB)) match+=1000;
              }
            else{
              if(!(glrender&GLX_RGBA_BIT)) match+=1000;
              }

            // Trace
            FXTRACE((150,"Config: #%d: match=%d\n",v,match));
            FXTRACE((150,"  drawables  = %s%s%s\n",(gldrawables&GLX_WINDOW_BIT)?"GLX_WINDOW_BIT ":"",(gldrawables&GLX_PIXMAP_BIT)?"GLX_PIXMAP_BIT ":"",(gldrawables&GLX_PBUFFER_BIT)?"GLX_PBUFFER_BIT":""));
            FXTRACE((150,"  render     = %s%s%s\n",(glrender&GLX_RGBA_BIT)?"GLX_RGBA_BIT ":"",(glrender&GLX_COLOR_INDEX_BIT)?"GLX_COLOR_INDEX_BIT ":"",(glrender&GLX_RGBA_FLOAT_BIT_ARB)?"GLX_RGBA_FLOAT_BIT_ARB":""));
            FXTRACE((150,"  format     = %zd\n",fb[v]));
            FXTRACE((150,"  configid   = 0x%02x\n",configid));
            FXTRACE((150,"  visualid   = 0x%02x\n",visualid));
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
            FXTRACE((150,"  accel      = %d\n",glaccel));

            // May the best config win
            if(match<=bestmatch){

              // All other things being equal, we prefer default visual!
              if((match<bestmatch) || (visualid==defvisualid)){
                actualRedSize=glredsize;
                accumGreenSize=glgreensize;
                accumBlueSize=glbluesize;
                accumAlphaSize=glalphasize;
                actualDepthSize=gldepthsize;
                actualStencilSize=glstencilsize;
                actualMultiSamples=glsamples;
                actualAccumRedSize=glaccumredsize;
                actualAccumGreenSize=glaccumgreensize;
                actualAccumBlueSize=glaccumbluesize;
                actualAccumAlphaSize=glaccumalphasize;
                doubleBuffer=gldouble;
                stereoBuffer=glstereo;
                accelerated=glaccel;
                copying=false;
                bestmatch=match;
                bestvisualid=visualid;
                best=v;
                }
              }
            }

          // We should have one now
          if(0<=best){
            FXTRACE((140,"Best Config: #%d: match=%d\n",best,bestmatch));

            // For window/pixmap drawable, get visual and depth
            if(bestvisualid){
              XVisualInfo vitemplate,*vi;
              int nvi;

              // Get visual, depth
              vitemplate.screen=DefaultScreen((Display*)getApp()->getDisplay());
              vitemplate.visualid=bestvisualid;
              vi=XGetVisualInfo((Display*)getApp()->getDisplay(),VisualScreenMask|VisualIDMask,&vitemplate,&nvi);
              if(vi){
                visual=vi[0].visual;
                depth=vi[0].depth;

                // Initialize colormap
                setupcolormap();

                // Make GC's for this visual
                gc=setupgc(false);
                scrollgc=setupgc(true);

                XFree((char*)vi);
                }
              }

            // Remember fb config
            xid=(FXID)fb[best];
            }

          // Free configs
          XFree(fb);
          }
        }
      }
    if(!xid){ throw FXWindowException("no matching GL configuration."); }
    }
#endif
  }


#else //////////////////////////////// X11 OLD //////////////////////////////////


// Initialize
void FXGLVisual::create(){
#ifdef HAVE_GL_H
  if(!xid){
    if(getApp()->isInitialized()){
      FXTRACE((100,"%s::create %p\n",getClassName(),this));

      // Assume the default
      visual=DefaultVisual((Display*)getApp()->getDisplay(),DefaultScreen((Display*)getApp()->getDisplay()));
      depth=DefaultDepth((Display*)getApp()->getDisplay(),DefaultScreen((Display*)getApp()->getDisplay()));

      // OpenGL is available if we're talking to an OpenGL-capable X-Server
      if(glXQueryExtension((Display*)getApp()->getDisplay(),NULL,NULL)){
        XVisualInfo vitemplate,*vi;
        int nvi;

        // Scan for all visuals of given screen
        vitemplate.screen=DefaultScreen((Display*)getApp()->getDisplay());
        vi=XGetVisualInfo((Display*)getApp()->getDisplay(),VisualScreenMask,&vitemplate,&nvi);
        if(vi){
          int glredsize,glgreensize,glbluesize,glalphasize,gldepthsize,glstencilsize,glaccumredsize,glaccumgreensize,glaccumbluesize,glaccumalphasize,gldouble,glstereo,glusable,glrender,gllevel;
          int dred,dgreen,dblue,dalpha,ddepth,dstencil,daccred,daccgreen,daccblue,daccalpha,match,visualid;
          int defvisualid=XVisualIDFromVisual(DefaultVisual((Display*)getApp()->getDisplay(),DefaultScreen((Display*)getApp()->getDisplay())));
          int bestmatch=1000000000;
          int best=-1;
          int v;

          FXTRACE((150,"Found %d configs\n",nvi));
          FXTRACE((150,"Default visualid=0x%02x\n",defvisualid));

          // Find the best one
          for(v=0; v<nvi; v++){

            // GL support is requested
            if(glXGetConfig((Display*)getApp()->getDisplay(),&vi[v],GLX_USE_GL,&glusable)!=Success) continue;
            if(!glusable) continue;

            // Get supported render type
            if(glXGetConfig((Display*)getApp()->getDisplay(),&vi[v],GLX_RGBA,&glrender)!=Success) continue;
            if(!glrender) continue;

            // Get overlay level
            if(glXGetConfig((Display*)getApp()->getDisplay(),&vi[v],GLX_LEVEL,&gllevel)!=Success) continue;
            if(gllevel) continue;

            // Get stereo and double buffer support
            if(glXGetConfig((Display*)getApp()->getDisplay(),&vi[v],GLX_DOUBLEBUFFER,&gldouble)!=Success) continue;
            if(glXGetConfig((Display*)getApp()->getDisplay(),&vi[v],GLX_STEREO,&glstereo)!=Success) continue;

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

            // Visual ID if it matters
            visualid=vi[v].visualid;

            // We prefer to get a few MORE bits in RGBA than we asked for
            dred   = glredsize-redSize;     if(dred<0)   dred   *= -100;
            dgreen = glgreensize-greenSize; if(dgreen<0) dgreen *= -100;
            dblue  = glbluesize-blueSize;   if(dblue<0)  dblue  *= -100;
            dalpha = glalphasize-alphaSize; if(dalpha<0) dalpha *= -100;

            // Prefer better Z than asked, but colors more important
            ddepth = gldepthsize-depthSize; if(ddepth<0) ddepth *= -10;

            // We care about colors and Z depth more than stencil depth
            dstencil = glstencilsize-stencilSize; if(dstencil<0) dstencil *= -1;

            // Accumulation buffers
            daccred=glaccumredsize-accumRedSize;       if(daccred<0)   daccred   *= -1;
            daccgreen=glaccumgreensize-accumGreenSize; if(daccgreen<0) daccgreen *= -1;
            daccblue=glaccumbluesize-accumBlueSize;    if(daccblue<0)  daccblue  *= -1;
            daccalpha=glaccumalphasize-accumAlphaSize; if(daccalpha<0) daccalpha *= -1;

            // Want the best colors, of course
            match=dred+dgreen+dblue+dalpha;

            // Accumulation buffers
            match+=daccred+daccgreen+daccblue+daccalpha;

            // Extra penalty for no alpha if we asked for alpha, but no
            // penalty at all if there is alpha and we didn't ask for it.
            if(alphaSize>0){
              if(glalphasize<1) match+=100000;
              }

            // Wanted Z-buffer
            if(depthSize>0){
              if(gldepthsize<1) match+=10000000;
              else match+=ddepth;
              }
            else{
              if(gldepthsize>0) match+=10000000;
              }

            // Stencil buffers desired
            if(stencilSize>0){
              if(glstencilsize<1) match+=10000;
              else match+=dstencil;
              }
            else{
              if(glstencilsize>0) match+=1;
              }

            // Double buffering also quite strongly preferred
            if(flags&VISUAL_DOUBLE_BUFFER){
              if(!gldouble) match+=1000000;
              }
            else{
              if(gldouble) match+=1000000;
              }

            // Stereo not so important
            if(flags&VISUAL_STEREO){
              if(!glstereo) match+=10000;
              }
            else{
              if(glstereo) match+=10000;
              }

            // Trace
            FXTRACE((150,"Config: #%d: match=%d\n",v,match));
            FXTRACE((150,"  visualid   = 0x%02x\n",visualid));
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
            if(match<=bestmatch){

              // All other things being equal, we prefer default visual!
              if((match<bestmatch) || (visualid==defvisualid)){
                actualRedSize=glredsize;
                accumGreenSize=glgreensize;
                accumBlueSize=glbluesize;
                accumAlphaSize=glalphasize;
                actualDepthSize=gldepthsize;
                actualStencilSize=glstencilsize;
                actualMultiSamples=0;
                actualAccumRedSize=glaccumredsize;
                actualAccumGreenSize=glaccumgreensize;
                actualAccumBlueSize=glaccumbluesize;
                actualAccumAlphaSize=glaccumalphasize;
                doubleBuffer=gldouble;
                stereoBuffer=glstereo;
                accelerated=true;
                copying=false;
                bestmatch=match;
                best=v;
                }
              }
            }

          // We should have one now
          if(0<=best){
            FXTRACE((140,"Best Config: #%d: match=%d\n",best,bestmatch));

            // Remember visual, depth, visualid
            visual=vi[best].visual;
            depth=vi[best].depth;

            // Initialize colormap
            setupcolormap();

            // Make GC's for this visual
            scrollgc=setupgc(true);
            gc=setupgc(false);

            // Visual is realized
            xid=((Visual*)visual)->visualid;
            }

          // Free visual info
          XFree((char*)vi);
          }
        }
      }
    if(!xid){ throw FXWindowException("no matching GL configuration."); }
    }
#endif
  }


#if 0
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

glw=glXCreateWindow((Display*)getApp()->getDisplay(),GLXFBConfig config,xid,NULL);
glXDestroyWindow((Display*)getApp()->getDisplay(),glw);
#endif

#endif //////////////////////////////////////////////////////////////////////////


// Detach visual
void FXGLVisual::detach(){
#ifdef HAVE_GL_H
  if(xid){
    FXTRACE((100,"%s::detach %p\n",getClassName(),this));
    colormap=0;
    freemap=false;
    xid=0;
    }
#endif
  }


// Destroy visual
void FXGLVisual::destroy(){
#ifdef HAVE_GL_H
  if(xid){
    if(getApp()->isInitialized()){
      FXTRACE((100,"%s::destroy %p\n",getClassName(),this));
#ifdef WIN32
      if(colormap){ DeleteObject(colormap); }
#else
      if(freemap){ XFreeColormap((Display*)getApp()->getDisplay(),colormap); }
      if(scrollgc){ XFreeGC((Display*)getApp()->getDisplay(),(GC)scrollgc); }
      if(gc){ XFreeGC((Display*)getApp()->getDisplay(),(GC)gc); }
#endif
      }
#ifndef WIN32
    scrollgc=0;
    gc=0;
#endif
    colormap=0;
    freemap=false;
    xid=0;
    }
#endif
  }


// Save to stream
void FXGLVisual::save(FXStream& store) const {
  FXVisual::save(store);
  store << redSize;
  store << greenSize;
  store << blueSize;
  store << alphaSize;
  store << depthSize;
  store << stencilSize;
  store << multiSamples;
  store << accumRedSize;
  store << accumGreenSize;
  store << accumBlueSize;
  store << accumAlphaSize;
  }


// Load from stream
void FXGLVisual::load(FXStream& store){
  FXVisual::load(store);
  store >> redSize;
  store >> greenSize;
  store >> blueSize;
  store >> alphaSize;
  store >> depthSize;
  store >> stencilSize;
  store >> multiSamples;
  store >> accumRedSize;
  store >> accumGreenSize;
  store >> accumBlueSize;
  store >> accumAlphaSize;
  }


// Destroy
FXGLVisual::~FXGLVisual(){
  FXTRACE((100,"FXGLVisual::~FXGLVisual %p\n",this));
  destroy();
  }


}
