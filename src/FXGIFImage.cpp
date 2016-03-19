/********************************************************************************
*                                                                               *
*                            G I F   I m a g e   O b j e c t                    *
*                                                                               *
*********************************************************************************
* Copyright (C) 1998,2008 by Jeroen van der Zijp.   All Rights Reserved.        *
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
* $Id: FXGIFImage.cpp,v 1.41 2008/07/02 19:36:27 fox Exp $                      *
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
#include "FXSettings.h"
#include "FXRegistry.h"
#include "FXApp.h"
#include "FXGIFImage.h"



/*
  Notes:
  - Only free image if owned!
*/

using namespace FX;


/*******************************************************************************/

namespace FX {


// Suggested file extension
const FXchar FXGIFImage::fileExt[]="gif";


// Suggested mime type
const FXchar FXGIFImage::mimeType[]="image/gif";


// Object implementation
FXIMPLEMENT(FXGIFImage,FXImage,NULL,0)


// Initialize
FXGIFImage::FXGIFImage(FXApp* a,const void *pix,FXuint opts,FXint w,FXint h):FXImage(a,NULL,opts,w,h){
  if(pix){
    FXMemoryStream ms(FXStreamLoad,(FXuchar*)pix);
    loadPixels(ms);
    }
  }


// Save object to stream
FXbool FXGIFImage::savePixels(FXStream& store) const {
  if(fxsaveGIF(store,data,width,height,true)){
    return true;
    }
  return false;
  }


// Load object from stream
FXbool FXGIFImage::loadPixels(FXStream& store){
  FXColor *pixels; FXint w,h;
  if(fxloadGIF(store,pixels,w,h,false)){
    setData(pixels,IMAGE_OWNED,w,h);
    return true;
    }
  return false;
  }


// Clean up
FXGIFImage::~FXGIFImage(){
  }

}
