/********************************************************************************
*                                                                               *
*                          D i c t i o n a r y    C l a s s                     *
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
* $Id: FXStringDict.cpp,v 1.22 2008/01/04 15:42:34 fox Exp $                    *
********************************************************************************/
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXStringDict.h"


/*
  Notes:
  - String dict may be useful in many applications.
  - Maybe dict[].data could be cast to FXString?
*/


using namespace FX;

/*******************************************************************************/

namespace FX {

// Object implementation
FXIMPLEMENT(FXStringDict,FXDict,NULL,0)


// Construct string dict
FXStringDict::FXStringDict(){
  }


// Copy constructor
FXStringDict::FXStringDict(const FXStringDict& orig):FXDict(orig){
  register FXint i;
  for(i=0; i<orig.total; i++){
    if(0<=dict[i].hash){
      dict[i].data=createData(orig.dict[i].data);
      }
    }
  }


// Assignment operator
FXStringDict& FXStringDict::operator=(const FXStringDict& orig){
  register FXint i;
  if(&orig!=this){
    FXDict::operator=(orig);
    for(i=0; i<orig.total; i++){
      if(0<=orig.dict[i].hash){
        dict[i].data=createData(orig.dict[i].data);
        }
      }
    }
  return *this;
  }


// Create string
void *FXStringDict::createData(void* ptr){
  return strdup((const char*)ptr);
  }


// Delete string
void FXStringDict::deleteData(void* ptr){
  free(ptr);
  }


// Destructor
FXStringDict::~FXStringDict(){
  clear();
  }

}
