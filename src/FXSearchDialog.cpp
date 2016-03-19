/********************************************************************************
*                                                                               *
*                       T e x t   S e a r c h   D i a l o g                     *
*                                                                               *
*********************************************************************************
* Copyright (C) 2000,2008 by Jeroen van der Zijp.   All Rights Reserved.        *
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
* $Id: FXSearchDialog.cpp,v 1.38 2008/01/04 15:42:30 fox Exp $                  *
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
#include "FXAccelTable.h"
#include "FXApp.h"
#include "FXFont.h"
#include "FXFrame.h"
#include "FXLabel.h"
#include "FXButton.h"
#include "FXPacker.h"
#include "FXHorizontalFrame.h"
#include "FXTextField.h"
#include "FXReplaceDialog.h"
#include "FXSearchDialog.h"



/*
  Notes:

  - Search dialog is essentially a FXReplaceDialog with some of the buttons
    hidden.
*/

using namespace FX;

/*******************************************************************************/

namespace FX {


// Object implementation
FXIMPLEMENT(FXSearchDialog,FXReplaceDialog,NULL,0)



// File Open Dialog
FXSearchDialog::FXSearchDialog(FXWindow* own,const FXString& caption,FXIcon* icn,FXuint opts,FXint x,FXint y,FXint w,FXint h):FXReplaceDialog(own,caption,icn,opts,x,y,w,h){
  accept->setText(tr("&Search"));
  every->hide();
  replacelabel->hide();
  replacebox->hide();
  }


// Cleanup
FXSearchDialog::~FXSearchDialog(){
  }

}

