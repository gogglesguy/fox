/********************************************************************************
*                                                                               *
*           G L -  F r a m e   B u f f e r   C o n f i g u r a t i o n          *
*                                                                               *
*********************************************************************************
* Copyright (C) 2006,2007 by Jeroen van der Zijp.   All Rights Reserved.        *
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
* $Id: FXGLConfig.h,v 1.11 2007/07/09 16:02:44 fox Exp $                        *
********************************************************************************/
#ifndef FXGLCONFIG_H
#define FXGLCONFIG_H


#ifndef FXOBJECT_H
#include "FXObject.h"
#endif

namespace FX {


/**
* OpenGL uses a complex arrangement of buffers. The configuration of
* these buffers is described in FXGLConfig. To create a GL context, the
* desired buffer configuration must be supplied. This is then matched against
* the hardware-supported configurations. The actual configuration used
* can be obtained after realizing the GL context, and is also described
* using FXGLConfig.
*/
class FXAPI FXGLConfig {
public:
  FXuchar redSize;              /// Red bits
  FXuchar greenSize;            /// Green depth
  FXuchar blueSize;             /// Blue bits
  FXuchar alphaSize;            /// Alpha bits
  FXuchar depthSize;            /// Depth bits
  FXuchar stencilSize;          /// Stencil bits
  FXuchar multiSamples;         /// Samples per pixel
  FXuchar accumRedSize;         /// Red accu buffer bits
  FXuchar accumGreenSize;       /// Green accu buffer bits
  FXuchar accumBlueSize;        /// Blue accu buffer bits
  FXuchar accumAlphaSize;       /// Alpha accu buffer bits
  FXuchar flags;                /// Configuration flags
public:
  enum {
    SingleBuffer = 0x00,        /// Single buffer mode
    DoubleBuffer = 0x01,        /// Double buffer mode
    StereoBuffer = 0x02,        /// Stereo buffer mode
    FloatBuffer  = 0x04,        /// Floating point buffer
    DrawWindow   = 0x08,        /// Able to draw on a window
    DrawImage    = 0x10,        /// Able to draw on an image
    DrawBuffer   = 0x20,        /// Able to draw to PBuffer
    Indirect     = 0x40         /// Indirect rendering mode
    };
public:

  /// Construct GL configuration, all zeroed out
  FXGLConfig():redSize(0),greenSize(0),blueSize(0),alphaSize(0),depthSize(0),stencilSize(0),multiSamples(0),accumRedSize(0),accumGreenSize(0),accumBlueSize(0),accumAlphaSize(0),flags(0){ }

  /// Construct GL configuration, with default depths and given flags
  FXGLConfig(FXuchar flgs):redSize(8),greenSize(8),blueSize(8),alphaSize(0),depthSize(24),stencilSize(0),multiSamples(0),accumRedSize(0),accumGreenSize(0),accumBlueSize(0),accumAlphaSize(0),flags(flgs){ }

  /// Construct GL configuration with given parameters for r,g,b,a channels, depth-, and stencil-buffer, for double-buffered rendering to a window
  FXGLConfig(FXuchar rbits,FXuchar gbits,FXuchar bbits,FXuchar abits=0,FXuchar zbits=24,FXuchar sbits=0,FXuchar flgs=DoubleBuffer|DrawWindow):redSize(rbits),greenSize(gbits),blueSize(bbits),alphaSize(abits),depthSize(zbits),stencilSize(sbits),multiSamples(0),accumRedSize(0),accumGreenSize(0),accumBlueSize(0),accumAlphaSize(0),flags(flgs){ }

  /// Has double buffering
  FXbool doubleBuffer() const { return (flags&DoubleBuffer)!=0; }

  /// Has stereo buffering
  FXbool stereoBuffer() const { return (flags&StereoBuffer)!=0; }

  /// Is direct
  FXbool direct() const { return (flags&Indirect)==0; }

  /// Save config to a stream
  friend FXAPI FXStream& operator<<(FXStream& store,const FXGLConfig& cfg);

  /// Load config from a stream
  friend FXAPI FXStream& operator>>(FXStream& store,FXGLConfig& cfg);
  };


extern FXAPI FXStream& operator<<(FXStream& store,const FXGLConfig& cfg);
extern FXAPI FXStream& operator>>(FXStream& store,FXGLConfig& cfg);

}

#endif
