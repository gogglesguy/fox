/********************************************************************************
*                                                                               *
*                        P P M   I c o n   O b j e c t                          *
*                                                                               *
*********************************************************************************
* Copyright (C) 1997,2008 by Jeroen van der Zijp.   All Rights Reserved.        *
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
* $Id: FXPPMIcon.cpp,v 1.19 2008/07/02 19:36:27 fox Exp $                       *
********************************************************************************/
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "FXHash.h"
#include "FXThread.h"
#include "FXStream.h"
#include "FXMemoryStream.h"
#include "FXString.h"
#include "FXSize.h"
#include "FXPoint.h"
#include "FXRectangle.h"
#include "FXObject.h"
#include "FXSettings.h"
#include "FXRegistry.h"
#include "FXApp.h"
#include "FXId.h"
#include "FXDrawable.h"
#include "FXImage.h"
#include "FXIcon.h"
#include "FXPPMIcon.h"


/*
  Notes:
  - PPM does not support alpha in the file format.
*/

using namespace FX;

/*******************************************************************************/

namespace FX {


// Suggested file extension
const FXchar FXPPMIcon::fileExt[]="ppm";


// Suggested mime type
const FXchar FXPPMIcon::mimeType[]="image/x-portable-pixmap";


// Object implementation
FXIMPLEMENT(FXPPMIcon,FXIcon,NULL,0)


// Initialize nicely
FXPPMIcon::FXPPMIcon(FXApp* a,const void *pix,FXColor clr,FXuint opts,FXint w,FXint h):FXIcon(a,NULL,clr,opts,w,h){
  if(pix){
    FXMemoryStream ms(FXStreamLoad,(FXuchar*)pix);
    loadPixels(ms);
    }
  }


// Save object to stream
FXbool FXPPMIcon::savePixels(FXStream& store) const {
  if(fxsavePPM(store,data,width,height)){
    return true;
    }
  return false;
  }


// Load object from stream
FXbool FXPPMIcon::loadPixels(FXStream& store){
  FXColor *pixels; FXint w,h;
  if(fxloadPPM(store,pixels,w,h)){
    setData(pixels,IMAGE_OWNED,w,h);
    if(options&IMAGE_ALPHAGUESS) transp=guesstransp();
    return true;
    }
  return false;
  }


// Clean up
FXPPMIcon::~FXPPMIcon(){
  }

}
