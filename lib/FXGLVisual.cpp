/********************************************************************************
*                                                                               *
*                            V i s u a l   C l a s s                            *
*                                                                               *
*********************************************************************************
* Copyright (C) 1999,2011 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "FXHash.h"
#include "FXThread.h"
#include "FXElement.h"
#include "FXStream.h"
#include "FXString.h"
#include "FXSize.h"
#include "FXPoint.h"
#include "FXRectangle.h"
#include "FXException.h"
#include "FXSettings.h"
#include "FXRegistry.h"
#include "FXAccelTable.h"
#include "FXFont.h"
#include "FXEvent.h"
#include "FXWindow.h"
#include "FXApp.h"
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


#ifdef HAVE_GL_H

#ifndef PFD_SUPPORT_COMPOSITION
#define PFD_SUPPORT_COMPOSITION         0x00008000
#endif


// Palette struct
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

#endif


// Initialize
void FXGLVisual::create(){
  if(!xid){
    if(getApp()->isInitialized()){
      FXTRACE((100,"%s::create %p\n",getClassName(),this));
#ifdef HAVE_GL_H
      PIXELFORMATDESCRIPTOR pfd;
      int glredsize,glgreensize,glbluesize,glalphasize,gldepthsize,glstencilsize,glsamples,glaccumredsize,glaccumgreensize,glaccumbluesize,glaccumalphasize,gldouble,glstereo,glsupport,gldrawwnd,gldrawbmp,glrender,glaccel,glneedpal,glswapcopy,glcomposition;
      int dred,dgreen,dblue,dalpha,ddepth,dstencil,dsamples,daccred,daccgreen,daccblue,daccalpha,match;
      int bestmatch=1000000000;
      int best=-1;
      int att;
      int npf;
      int chosen,v;
      HDC hdc;

      // Get some window handle
      hdc=GetDC(GetDesktopWindow());

      // Get number of supported pixel formats
      pfd.nSize=sizeof(PIXELFORMATDESCRIPTOR);
      pfd.nVersion=1;

      // Get number of supported pixel formats
      if(0<(npf=DescribePixelFormat(hdc,1,sizeof(PIXELFORMATDESCRIPTOR),&pfd))){

        FXTRACE((140,"Found %d OpenGL configs\n",npf));

        // Try to find the best
        for(v=1; v<=npf; v++){

          // Get info about this visual
          DescribePixelFormat(hdc,v,sizeof(PIXELFORMATDESCRIPTOR),&pfd);

          // Make sure this visual is valid
          chosen=ChoosePixelFormat(hdc,&pfd);
          if(chosen!=v) continue;

          // OpenGL support is required
          if(!(pfd.dwFlags&PFD_SUPPORT_OPENGL)) continue;

          // Draw to window is required
          if(!(pfd.dwFlags&PFD_DRAW_TO_WINDOW)) continue;

          // Get supported render type; we don't want index mode
          if(pfd.iPixelType==PFD_TYPE_COLORINDEX) continue;

          // Skip accelerated formats if VISUAL_NO_ACCEL specified
          if((flags&VISUAL_NO_ACCEL) && !(pfd.dwFlags&PFD_GENERIC_FORMAT)) continue;

          // Hardware accelerated format
          glaccel=(pfd.dwFlags&PFD_GENERIC_FORMAT)==0;

          // Double buffer capable
          gldouble=(pfd.dwFlags&PFD_DOUBLEBUFFER)!=0;

          // Stereo capable
          glstereo=(pfd.dwFlags&PFD_STEREO)!=0;

          // Swap buffers by copying
          glswapcopy=(pfd.dwFlags&PFD_SWAP_COPY)!=0;

          // Windows Vista and Windows 7 composition support
          glcomposition=(pfd.dwFlags&PFD_SUPPORT_COMPOSITION)!=0;

          // Get planes
          glredsize=pfd.cRedBits;
          glgreensize=pfd.cGreenBits;
          glbluesize=pfd.cBlueBits;
          glalphasize=pfd.cAlphaBits;
          gldepthsize=pfd.cDepthBits;
          glstencilsize=pfd.cStencilBits;
          glaccumredsize=pfd.cAccumRedBits;
          glaccumgreensize=pfd.cAccumGreenBits;
          glaccumbluesize=pfd.cAccumBlueBits;
          glaccumalphasize=pfd.cAccumAlphaBits;

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

          // Bitmap drawing would be nice
          if(!(pfd.dwFlags&PFD_DRAW_TO_BITMAP)){
            match+=1;
            }

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

          // Swap copies also important
          if(flags&VISUAL_SWAP_COPY){
            if(!glswapcopy) match+=10000000;
            }

          // Composition support would be nice to have
          if(!glcomposition) match+=1;

          // Trace
          FXTRACE((150,"Config: #%d: match=%d\n",v,match));
          FXTRACE((150,"  drawables  = %s%s\n",(pfd.dwFlags&PFD_DRAW_TO_WINDOW)?"PFD_DRAW_TO_WINDOW ":"",(pfd.dwFlags&PFD_DRAW_TO_BITMAP)?"PFD_DRAW_TO_BITMAP ":""));
          FXTRACE((150,"  render     = %s\n",(pfd.iPixelType==PFD_TYPE_COLORINDEX)?"PFD_TYPE_COLORINDEX":(pfd.iPixelType==PFD_TYPE_RGBA)?"PFD_TYPE_RGBA":""));
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
          FXTRACE((150,"  accel      = %d\n",glaccel));

          // May the best visual win
          if(match<=bestmatch){
            actualRedSize=glredsize;
            actualGreenSize=glgreensize;
            actualBlueSize=glbluesize;
            actualAlphaSize=glalphasize;
            actualDepthSize=gldepthsize;
            actualStencilSize=glstencilsize;
            actualMultiSamples=0;
            actualAccumRedSize=glaccumredsize;
            actualAccumGreenSize=glaccumgreensize;
            actualAccumBlueSize=glaccumbluesize;
            actualAccumAlphaSize=glaccumalphasize;
            doubleBuffer=gldouble;
            stereoBuffer=glstereo;
            accelerated=glaccel;
            copying=glswapcopy;
            bestmatch=match;
            best=v;
            }
          }
        }

      // Hopefully, we got one
      if(0<=best){
        FXTRACE((140,"Best Config: #%d: match=%d\n",best,bestmatch));

        // Fill in visual data
        depth=actualRedSize+actualGreenSize+actualBlueSize;
        numred=(1<<actualRedSize);
        numgreen=(1<<actualGreenSize);
        numblue=(1<<actualBlueSize);
        numcolors=numred*numgreen*numblue;

        // Make a palette for it if needed
        if(pfd.dwFlags&PFD_NEED_PALETTE){
          colormap=makeOpenGLPalette(&pfd);
          freemap=true;
          }

        // Remember best config
        xid=(void*)(FXival)best;
        }

      // Done with that window
      ::ReleaseDC(GetDesktopWindow(),hdc);

      // Make non-current
      wglMakeCurrent(NULL,NULL);
#endif
      }

    // Test if successful
    if(!xid){
      throw FXWindowException("unable to create GL context.");
      }
    }
  }


#else //////////////////////////////// X11 OLD //////////////////////////////////


#ifndef GLX_ARB_multisample
#define GLX_SAMPLE_BUFFERS_ARB             100000
#define GLX_SAMPLES_ARB                    100001
#endif
#ifndef GLX_EXT_visual_rating
#define GLX_VISUAL_CAVEAT_EXT              0x20
#define GLX_SLOW_VISUAL_EXT                0x8001
#define GLX_NON_CONFORMANT_VISUAL_EXT      0x800D
#endif

// Initialize
void FXGLVisual::create(){
  if(!xid){
    if(getApp()->isInitialized()){
      FXTRACE((100,"%s::create %p\n",getClassName(),this));
#ifdef HAVE_GL_H
      int majoropcode,errorbase,eventbase;

      // Assume the default
      visual=DefaultVisual((Display*)getApp()->getDisplay(),DefaultScreen((Display*)getApp()->getDisplay()));
      depth=DefaultDepth((Display*)getApp()->getDisplay(),DefaultScreen((Display*)getApp()->getDisplay()));

      // OpenGL is available if we're talking to an OpenGL-capable X-Server
      if(XQueryExtension((Display*)getApp()->getDisplay(),"GLX",&majoropcode,&errorbase,&eventbase)){
        if(glXQueryExtension((Display*)getApp()->getDisplay(),&errorbase,&eventbase)){
          int major,minor;

          // Try get OpenGL version info
          if(glXQueryVersion((Display*)getApp()->getDisplay(),&major,&minor)){
            XVisualInfo vitemplate,*vi; int nvi;

            // Report version found
            FXTRACE((100,"Found GLX version: %d.%d (Major: %d, Error: %d, Event: %d)\n",major,minor,majoropcode,errorbase,eventbase));

            // Scan for all visuals of given screen
            vitemplate.screen=DefaultScreen((Display*)getApp()->getDisplay());
            vi=XGetVisualInfo((Display*)getApp()->getDisplay(),VisualScreenMask,&vitemplate,&nvi);
            if(vi){
              int glredsize,glgreensize,glbluesize,glalphasize,gldepthsize,glstencilsize,glsamples,glaccumredsize,glaccumgreensize,glaccumbluesize,glaccumalphasize,gldouble,glstereo,glusable,glrender,gllevel,glaccel;
              int dred,dgreen,dblue,dalpha,ddepth,dstencil,dsamples,daccred,daccgreen,daccblue,daccalpha,match,visualid;
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

                // Get supported drawable targets
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

                // Get multisample support (if we don't succeed, set it to zero)
                if(glXGetConfig((Display*)getApp()->getDisplay(),&vi[v],GLX_SAMPLES_ARB,&glsamples)!=Success) glsamples=0;

                // Check if accelerated or not (assume yes)
                if(glXGetConfig((Display*)getApp()->getDisplay(),&vi[v],GLX_VISUAL_CAVEAT_EXT,&glaccel)==Success){
                  glaccel=(glaccel!=GLX_SLOW_VISUAL_EXT);        // Penalize if any caveats present
                  }
                else{
                  glaccel=1;
                  }

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
                else{
                  if(glsamples>0) match+=100;
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
                FXTRACE((150,"  samples    = %d\n",glsamples));
                FXTRACE((150,"  accel      = %d\n",glaccel));

                // May the best config win
                if(match<=bestmatch){

                  // All other things being equal, we prefer default visual!
                  if((match<bestmatch) || (visualid==defvisualid)){
                    actualRedSize=glredsize;
                    actualGreenSize=glgreensize;
                    actualBlueSize=glbluesize;
                    actualAlphaSize=glalphasize;
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

                // Remember best config
                xid=((Visual*)visual)->visualid;
                }

              // Free visual info
              XFree((char*)vi);
              }
            }
          }
        }
#endif
      }

    // Test if successful
    if(!xid){
      throw FXWindowException("no matching GL configuration.");
      }
    }
  }


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
