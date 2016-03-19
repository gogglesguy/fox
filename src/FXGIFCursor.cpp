/********************************************************************************
*                                                                               *
*                        G I F   C u r s o r   O b j e c t                      *
*                                                                               *
*********************************************************************************
* Copyright (C) 2000,2007 by Daniel Gehriger.   All Rights Reserved.            *
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
* $Id: FXGIFCursor.cpp,v 1.37 2007/07/09 16:26:54 fox Exp $                     *
********************************************************************************/
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "FXHash.h"
#include "FXThread.h"
#include "FXElement.h"
#include "FXStream.h"
#include "FXMemoryStream.h"
#include "FXString.h"
#include "FXSize.h"
#include "FXPoint.h"
#include "FXRectangle.h"
#include "FXSettings.h"
#include "FXRegistry.h"
#include "FXApp.h"
#include "FXGIFCursor.h"


/*
  Notes:
  - Tossed old code now that FXCursor has an RGBA representation.
  - Now uses actual alpha color from the GIF file.
  - Need function to force alpha channel based on transparent color.
  - Optionally let system guess a transparancy color based on the corners.
  - If that doesn't work, you can force a specific transparency color.
*/


using namespace FX;

/*******************************************************************************/

namespace FX {


// Suggested file extension
const FXchar FXGIFCursor::fileExt[]="gif";


// Object implementation
FXIMPLEMENT(FXGIFCursor,FXCursor,NULL,0)


// Constructor
FXGIFCursor::FXGIFCursor(FXApp* a,const void *pix,FXint hx,FXint hy):FXCursor(a,NULL,0,0,0,0){
  if(pix){
    FXMemoryStream ms;
    ms.open(FXStreamLoad,(FXuchar*)pix);
    fxloadGIF(ms,data,width,height);
    hotx=FXCLAMP(0,hx,width-1);
    hoty=FXCLAMP(0,hy,height-1);
    options|=CURSOR_OWNED;
    ms.close();
    }
  }


// Save object to stream
FXbool FXGIFCursor::savePixels(FXStream& store) const {
  if(fxsaveGIF(store,data,width,height)){
    return true;
    }
  return false;
  }


// Load object from stream
FXbool FXGIFCursor::loadPixels(FXStream& store){
  if(options&CURSOR_OWNED){freeElms(data);}
  if(fxloadGIF(store,data,width,height)){
    options|=CURSOR_OWNED;
    return true;
    }
  return false;
  }

}


