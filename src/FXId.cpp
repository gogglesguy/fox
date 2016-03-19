/********************************************************************************
*                                                                               *
*                                  X - O b j e c t                              *
*                                                                               *
*********************************************************************************
* Copyright (C) 1997,2009 by Jeroen van der Zijp.   All Rights Reserved.        *
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
* $Id: FXId.cpp,v 1.30 2009/01/06 13:24:35 fox Exp $                            *
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
#include "FXRegistry.h"
#include "FXApp.h"
#include "FXId.h"
#include "FXException.h"

/*
  Notes:
  - Base class for all creatable resources connected via the display.
*/

using namespace FX;

enum { MAGIC = 0x464f5831 };


/*******************************************************************************/

namespace FX {

// Object implementation
FXIMPLEMENT_ABSTRACT(FXId,FXObject,NULL,0)


// Create resource
void FXId::create(){ }


// Detach resource
void FXId::detach(){ }


// Destroy resource
void FXId::destroy(){ }


// Save data
void FXId::save(FXStream& store) const {
  FXuint version=MAGIC;
  FXObject::save(store);
  store << version;
  store << app;
  }


// Load data
void FXId::load(FXStream& store){
  FXuint version;
  FXObject::load(store);
  store >> version;
  if(version!=MAGIC){
    store.setError(FXStreamFormat);
    throw FXResourceException("expected to match MAGIC tag");
    }
  store >> app;
  }


// Destructor
FXId::~FXId(){
  app=(FXApp*)-1L;
  data=(void*)-1L;
  }

}
