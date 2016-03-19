/********************************************************************************
*                                                                               *
*           G L -  F r a m e   B u f f e r   C o n f i g u r a t i o n          *
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
* $Id: FXGLConfig.cpp,v 1.6 2007/02/07 20:22:08 fox Exp $                       *
********************************************************************************/
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXGLConfig.h"


/*
  Notes:
  - FXGLConfig describes the buffer configuration for OpenGL rendering.
  - It is matched againts what the hardware supports; generally, the
    matching process tries to give you the closest hardware configuration
    to the desired parameters.
  - Note, multiSamples==0 means no multi-sampling, but if you want multi-
    sampling then the value should be at least 2!
*/


using namespace FX;

/*******************************************************************************/

namespace FX {


// Save object to a stream
FXStream& operator<<(FXStream& store,const FXGLConfig& cfg){
  store << cfg.redSize;
  store << cfg.greenSize;
  store << cfg.blueSize;
  store << cfg.alphaSize;
  store << cfg.depthSize;
  store << cfg.stencilSize;
  store << cfg.multiSamples;
  store << cfg.accumRedSize;
  store << cfg.accumGreenSize;
  store << cfg.accumBlueSize;
  store << cfg.accumAlphaSize;
  store << cfg.flags;
  return store;
  }


// Load object from a stream
FXStream& operator>>(FXStream& store,FXGLConfig& cfg){
  store >> cfg.redSize;
  store >> cfg.greenSize;
  store >> cfg.blueSize;
  store >> cfg.alphaSize;
  store >> cfg.depthSize;
  store >> cfg.stencilSize;
  store >> cfg.multiSamples;
  store >> cfg.accumRedSize;
  store >> cfg.accumGreenSize;
  store >> cfg.accumBlueSize;
  store >> cfg.accumAlphaSize;
  store >> cfg.flags;
  return store;
  }


}
