/********************************************************************************
*                                                                               *
*      F i l e   C o p y / M o v e / L i n k / D e l e t e   D i a l o g s      *
*                                                                               *
*********************************************************************************
* Copyright (C) 2005,2008 by Jeroen van der Zijp.   All Rights Reserved.        *
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
* $Id: FXCopyDialog.cpp,v 1.13 2008/01/04 15:42:06 fox Exp $                    *
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
#include "FXIcon.h"
#include "FXGIFIcon.h"
#include "FXSeparator.h"
#include "FXLabel.h"
#include "FXButton.h"
#include "FXScrollArea.h"
#include "FXObjectList.h"
#include "FXList.h"
#include "FXTextField.h"
#include "FXHorizontalFrame.h"
#include "FXVerticalFrame.h"
#include "FXMatrix.h"
#include "FXCopyDialog.h"


/*
  Notes:
  - Fill with a list of files, then ask the user to proceed or cancel
    the operation.
*/

// Padding for buttons
#define HORZ_PAD 20
#define VERT_PAD 2

using namespace FX;

/*******************************************************************************/

namespace FX {

FXIMPLEMENT(FXDeleteDialog,FXDialogBox,NULL,0)


// Construct delete dialog
FXDeleteDialog::FXDeleteDialog(FXWindow *own,const FXString& caption,const FXString& srctxt,FXIcon* icn,FXuint opts,FXint x,FXint y,FXint w,FXint h):FXDialogBox(own,caption,opts|DECOR_TITLE|DECOR_BORDER|DECOR_RESIZE,x,y,w,h,10,10,0,0, 10,0){

  // Close buttons below
  FXHorizontalFrame* buttons=new FXHorizontalFrame(this,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|PACK_UNIFORM_WIDTH,0,0,0,0, 0,0,10,10);
  new FXButton(buttons,tr("&OK"),NULL,this,FXDialogBox::ID_ACCEPT,BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_RIGHT|LAYOUT_CENTER_Y,0,0,0,0,HORZ_PAD,HORZ_PAD,VERT_PAD,VERT_PAD);
  new FXButton(buttons,tr("&Cancel"),NULL,this,FXDialogBox::ID_CANCEL,BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_RIGHT|LAYOUT_CENTER_Y,0,0,0,0,HORZ_PAD,HORZ_PAD,VERT_PAD,VERT_PAD);

  // Separator
  new FXHorizontalSeparator(this,SEPARATOR_GROOVE|LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);

  FXHorizontalFrame* toppart=new FXHorizontalFrame(this,LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0, 10,10);
  new FXLabel(toppart,FXString::null,icn,ICON_BEFORE_TEXT|JUSTIFY_CENTER_X|JUSTIFY_CENTER_Y|LAYOUT_FILL_Y|LAYOUT_FILL_X,0,0,0,0, 0,0,0,0);

  // Files involved
  matrix=new FXMatrix(toppart,2,MATRIX_BY_COLUMNS|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,10,10);

  // From part
  srclabel=new FXLabel(matrix,srctxt,NULL,LAYOUT_FILL_X|LAYOUT_CENTER_Y|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  FXHorizontalFrame* srcframe=new FXHorizontalFrame(matrix,FRAME_SUNKEN|FRAME_THICK|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_FILL_ROW|LAYOUT_FILL_COLUMN,0,0,0,0, 0,0,0,0, 10,10);
  srclist=new FXList(srcframe,NULL,0,LIST_MULTIPLESELECT|HSCROLLING_OFF|LAYOUT_FILL_Y|LAYOUT_FILL_X);
  srclist->setNumVisible(1);
  }


// Set source files from newline-separated list
void FXDeleteDialog::setSourceFiles(const FXString& files){
  srclist->fillItems(files);
  srclist->setNumVisible(FXMIN(10,srclist->getNumItems()));
  }


// Change source label
void FXDeleteDialog::setSourceLabel(const FXString& str){
  srclabel->setText(str);
  }


// Get source label
FXString FXDeleteDialog::getSourceLabel() const {
  return srclabel->getText();
  }


/*******************************************************************************/


FXIMPLEMENT(FXCopyDialog,FXDeleteDialog,NULL,0)


// Construct copy/move/link/rename dialog
FXCopyDialog::FXCopyDialog(FXWindow *own,const FXString& caption,const FXString& srctxt,const FXString& dsttxt,FXIcon* icn,FXuint opts,FXint x,FXint y,FXint w,FXint h):FXDeleteDialog(own,caption,srctxt,icn,opts,x,y,w,h){
  dstlabel=new FXLabel(matrix,dsttxt,NULL,LAYOUT_FILL_X|LAYOUT_TOP|JUSTIFY_RIGHT);
  FXHorizontalFrame* dstframe=new FXHorizontalFrame(matrix,FRAME_SUNKEN|FRAME_THICK|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_FILL_COLUMN,0,0,0,0, 0,0,0,0, 10,10);
  dstlist=new FXList(dstframe,NULL,0,LIST_MULTIPLESELECT|HSCROLLING_OFF|LAYOUT_FILL_Y|LAYOUT_FILL_X);
  dstlist->setNumVisible(1);
  }


// Set destination file
void FXCopyDialog::setDestinationFile(const FXString& file){
  dstlist->fillItems(file);
  }


// Change to label
void FXCopyDialog::setDestinationLabel(const FXString& str){
  dstlabel->setText(str);
  }


// Get to label
FXString FXCopyDialog::getDestinationLabel() const {
  return dstlabel->getText();
  }


}
