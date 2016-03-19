/********************************************************************************
*                                                                               *
*                            X B M   I m a g e   O b j e c t                    *
*                                                                               *
*********************************************************************************
* Copyright (C) 2003,2012 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#ifndef FXXBMIMAGE_H
#define FXXBMIMAGE_H

#ifndef FXIMAGE_H
#include "FXImage.h"
#endif

namespace FX {


/// X Bitmap image
class FXAPI FXXBMImage : public FXImage {
  FXDECLARE(FXXBMImage)
protected:
  FXXBMImage(){}
private:
  FXXBMImage(const FXXBMImage&);
  FXXBMImage &operator=(const FXXBMImage&);
public:
  static const FXchar fileExt[];
  static const FXchar mimeType[];
public:

  /// Construct image from memory stream formatted in X Bitmap format
  FXXBMImage(FXApp* a,const FXuchar *pixels=NULL,const FXuchar *mask=NULL,FXuint opts=0,FXint w=1,FXint h=1);

  /// Save pixels into stream in X Bitmap format
  virtual FXbool savePixels(FXStream& store) const;

  /// Load pixels from stream in X Bitmap format
  virtual FXbool loadPixels(FXStream& store);

  /// Destroy icon
  virtual ~FXXBMImage();
  };


#ifndef FXLOADXBM
#define FXLOADXBM

/**
* Check if stream contains a XBM, return true if so.
*/
extern FXAPI FXbool fxcheckXBM(FXStream& store);

/**
* Load an XBM (X Bitmap) from pixel array and mask array.
* Upon successful return, the pixel array and size are returned.
* If an error occurred, the pixel array is set to NULL.
*/
extern FXAPI FXbool fxloadXBM(FXColor*& data,const FXuchar *pix,const FXuchar *msk,FXint width,FXint height);


/**
* Load an XBM (X Bitmap) file from a stream.
* Upon successful return, the pixel array and size, and hot-spot are returned.
* If an error occurred, the pixel array is set to NULL.
*/
extern FXAPI FXbool fxloadXBM(FXStream& store,FXColor*& data,FXint& width,FXint& height,FXint& hotx,FXint& hoty);


/**
* Save an XBM (X Bitmap) file to a stream; if the parameters hotx and hoty are set
* to -1, no hotspot location is saved.
*/
extern FXAPI FXbool fxsaveXBM(FXStream& store,const FXColor *data,FXint width,FXint height,FXint hotx=-1,FXint hoty=-1);

/**
* Save a PostScript file to a stream; format the picture to the maximal
* size that fits within the given margins of the indicated paper size.
*/
extern FXAPI FXbool fxsavePS(FXStream& store,const FXColor *data,FXint width,FXint height,FXint paperw=612,FXint paperh=792,FXint margin=35,FXbool color=true);

#endif

}

#endif
