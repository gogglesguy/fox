/********************************************************************************
*                                                                               *
*         I n t e r - T h r e a d    M e s s a g i n g    S e r v i c e         *
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
* $Id: FXDirWatch.h,v 1.6 2007/02/07 20:21:53 fox Exp $                         *
********************************************************************************/
#ifndef FXDIRWATCH_H
#define FXDIRWATCH_H

#ifndef FXOBJECT_H
#include "FXObject.h"
#endif


namespace FX {

class FXApp;


/**
*/
class FXDirWatch : public FXObject {
  FXDECLARE(FXDirWatch)
private:
  FXApp *app;
private:
  FXInputHandle hnd;
protected:
  FXDirWatch();
private:
  FXDirWatch(const FXDirWatch&);
  FXDirWatch& operator=(const FXDirWatch&);
public:
  enum{
    ID_IO_READ=1,
    ID_LAST
    };
public:
  long onMessage(FXObject*,FXSelector,void*);
public:

  /// Initialize directory watcher
  FXDirWatch(FXApp* a);

  /// Get application pointer
  FXApp* getApp() const { return app; }

  FXbool watch(const FXString& dir);

  /// Clean up directory watcher
  virtual ~FXDirWatch();
  };

}

#endif


