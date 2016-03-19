/********************************************************************************
*                                                                               *
*                           C o l o r   D i a l o g                             *
*                                                                               *
*********************************************************************************
* Copyright (C) 1998,2011 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "fxkeys.h"
#include "FXHash.h"
#include "FXMutex.h"
#include "FXStream.h"
#include "FXString.h"
#include "FXSize.h"
#include "FXPoint.h"
#include "FXRectangle.h"
#include "FXSettings.h"
#include "FXRegistry.h"
#include "FXEvent.h"
#include "FXWindow.h"
#include "FXApp.h"
#include "FXFrame.h"
#include "FXLabel.h"
#include "FXButton.h"
#include "FXComposite.h"
#include "FXPacker.h"
#include "FXShell.h"
#include "FXTopWindow.h"
#include "FXDialogBox.h"
#include "FXColorSelector.h"
#include "FXColorDialog.h"


/*
  Notes:
  - Need shared instance of this dialog to pop up when double-clicking
    on a color well.
*/

using namespace FX;

/*******************************************************************************/

namespace FX {

// Map
FXDEFMAP(FXColorDialog) FXColorDialogMap[]={
  FXMAPFUNC(SEL_CHANGED,FXColorDialog::ID_COLORSELECTOR,FXColorDialog::onChgColor),
  FXMAPFUNC(SEL_COMMAND,FXColorDialog::ID_COLORSELECTOR,FXColorDialog::onCmdColor),
  FXMAPFUNC(SEL_COMMAND,FXColorDialog::ID_SETINTVALUE,FXColorDialog::onCmdSetIntValue),
  FXMAPFUNC(SEL_COMMAND,FXColorDialog::ID_GETINTVALUE,FXColorDialog::onCmdGetIntValue),
  };


// Object implementation
FXIMPLEMENT(FXColorDialog,FXDialogBox,FXColorDialogMap,ARRAYNUMBER(FXColorDialogMap))


// Construct color dialog box
FXColorDialog::FXColorDialog(FXWindow* own,const FXString& name,FXuint opts,FXint x,FXint y,FXint w,FXint h):FXDialogBox(own,name,opts|DECOR_TITLE|DECOR_BORDER|DECOR_RESIZE|DECOR_CLOSE,x,y,w,h,0,0,0,0,4,4){
  colorbox=new FXColorSelector(this,this,ID_COLORSELECTOR,LAYOUT_FILL_X|LAYOUT_FILL_Y);
  colorbox->acceptButton()->setTarget(this);
  colorbox->acceptButton()->setSelector(FXDialogBox::ID_ACCEPT);
  colorbox->cancelButton()->setTarget(this);
  colorbox->cancelButton()->setSelector(FXDialogBox::ID_CANCEL);
  }


// Construct free-floating color dialog box
FXColorDialog::FXColorDialog(FXApp* a,const FXString& name,FXuint opts,FXint x,FXint y,FXint w,FXint h):FXDialogBox(a,name,opts|DECOR_TITLE|DECOR_BORDER|DECOR_RESIZE|DECOR_CLOSE,x,y,w,h,0,0,0,0,4,4){
  colorbox=new FXColorSelector(this,this,ID_COLORSELECTOR,LAYOUT_FILL_X|LAYOUT_FILL_Y);
  colorbox->acceptButton()->setTarget(this);
  colorbox->acceptButton()->setSelector(FXDialogBox::ID_ACCEPT);
  colorbox->cancelButton()->setTarget(this);
  colorbox->cancelButton()->setSelector(FXDialogBox::ID_CANCEL);
  }


// Create server-side resources
void FXColorDialog::create(){
  setActivePanel(getApp()->reg().readIntEntry("Color Dialog","activecolorpane",COLORTAB_COLOR_RING));
  setWellColor( 0,getApp()->reg().readColorEntry("Color Dialog","WA",FXRGBA(255,255,255,255)));
  setWellColor( 1,getApp()->reg().readColorEntry("Color Dialog","WB",FXRGBA(204,204,204,255)));
  setWellColor( 2,getApp()->reg().readColorEntry("Color Dialog","WC",FXRGBA(153,153,153,255)));
  setWellColor( 3,getApp()->reg().readColorEntry("Color Dialog","WD",FXRGBA(102,102,102,255)));
  setWellColor( 4,getApp()->reg().readColorEntry("Color Dialog","WE",FXRGBA( 51, 51, 51,255)));
  setWellColor( 5,getApp()->reg().readColorEntry("Color Dialog","WF",FXRGBA(  0,  0,  0,255)));
  setWellColor( 6,getApp()->reg().readColorEntry("Color Dialog","WG",FXRGBA(255,  0,  0,255)));
  setWellColor( 7,getApp()->reg().readColorEntry("Color Dialog","WH",FXRGBA(  0,255,  0,255)));
  setWellColor( 8,getApp()->reg().readColorEntry("Color Dialog","WI",FXRGBA(  0,  0,255,255)));
  setWellColor( 9,getApp()->reg().readColorEntry("Color Dialog","WJ",FXRGBA(  0,255,255,255)));
  setWellColor(10,getApp()->reg().readColorEntry("Color Dialog","WK",FXRGBA(255,255,  0,255)));
  setWellColor(11,getApp()->reg().readColorEntry("Color Dialog","WL",FXRGBA(255,  0,255,255)));
  setWellColor(12,getApp()->reg().readColorEntry("Color Dialog","WM",FXRGBA(255,165,  0,255)));
  setWellColor(13,getApp()->reg().readColorEntry("Color Dialog","WN",FXRGBA(153,  0,  0,255)));
  setWellColor(14,getApp()->reg().readColorEntry("Color Dialog","WO",FXRGBA(  0,153,  0,255)));
  setWellColor(15,getApp()->reg().readColorEntry("Color Dialog","WP",FXRGBA(  0,  0,153,255)));
  setWellColor(16,getApp()->reg().readColorEntry("Color Dialog","WQ",FXRGBA(  0,153,153,255)));
  setWellColor(17,getApp()->reg().readColorEntry("Color Dialog","WR",FXRGBA(153,153,  0,255)));
  setWellColor(18,getApp()->reg().readColorEntry("Color Dialog","WS",FXRGBA(153,  0,153,255)));
  setWellColor(19,getApp()->reg().readColorEntry("Color Dialog","WT",FXRGBA(255,175,175,255)));
  setWellColor(20,getApp()->reg().readColorEntry("Color Dialog","WU",FXRGBA(175,255,175,255)));
  setWellColor(21,getApp()->reg().readColorEntry("Color Dialog","WV",FXRGBA(175,175,255,255)));
  setWellColor(22,getApp()->reg().readColorEntry("Color Dialog","WW",FXRGBA(175,255,255,255)));
  setWellColor(23,getApp()->reg().readColorEntry("Color Dialog","WX",FXRGBA(255,255,175,255)));
  FXDialogBox::create();
  }


// Destroy server-side resources
void FXColorDialog::destroy(){
  getApp()->reg().writeIntEntry("Color Dialog","activecolorpane",getActivePanel());
  getApp()->reg().writeColorEntry("Color Dialog","WA",getWellColor( 0));
  getApp()->reg().writeColorEntry("Color Dialog","WB",getWellColor( 1));
  getApp()->reg().writeColorEntry("Color Dialog","WC",getWellColor( 2));
  getApp()->reg().writeColorEntry("Color Dialog","WD",getWellColor( 3));
  getApp()->reg().writeColorEntry("Color Dialog","WE",getWellColor( 4));
  getApp()->reg().writeColorEntry("Color Dialog","WF",getWellColor( 5));
  getApp()->reg().writeColorEntry("Color Dialog","WG",getWellColor( 6));
  getApp()->reg().writeColorEntry("Color Dialog","WH",getWellColor( 7));
  getApp()->reg().writeColorEntry("Color Dialog","WI",getWellColor( 8));
  getApp()->reg().writeColorEntry("Color Dialog","WJ",getWellColor( 9));
  getApp()->reg().writeColorEntry("Color Dialog","WK",getWellColor(10));
  getApp()->reg().writeColorEntry("Color Dialog","WL",getWellColor(11));
  getApp()->reg().writeColorEntry("Color Dialog","WM",getWellColor(12));
  getApp()->reg().writeColorEntry("Color Dialog","WN",getWellColor(13));
  getApp()->reg().writeColorEntry("Color Dialog","WO",getWellColor(14));
  getApp()->reg().writeColorEntry("Color Dialog","WP",getWellColor(15));
  getApp()->reg().writeColorEntry("Color Dialog","WQ",getWellColor(16));
  getApp()->reg().writeColorEntry("Color Dialog","WR",getWellColor(17));
  getApp()->reg().writeColorEntry("Color Dialog","WS",getWellColor(18));
  getApp()->reg().writeColorEntry("Color Dialog","WT",getWellColor(19));
  getApp()->reg().writeColorEntry("Color Dialog","WU",getWellColor(20));
  getApp()->reg().writeColorEntry("Color Dialog","WV",getWellColor(21));
  getApp()->reg().writeColorEntry("Color Dialog","WW",getWellColor(22));
  getApp()->reg().writeColorEntry("Color Dialog","WX",getWellColor(23));
  FXDialogBox::destroy();
  }

/*

// Well names
const FXchar *const FXColorSelector::wellname[24]={
  "wella","wellb","wellc","welld",
  "welle","wellf","wellg","wellh",
  "welli","wellj","wellk","welll",
  "wellm","welln","wello","wellp",
  "wellq","wellr","wells","wellt",
  "wellu","wellv","wellw","wellx"
  };


  // Get custom well colors from defaults database
  colorwells[0]->setRGBA(getApp()->reg().readColorEntry("SETTINGS",wellname[0],FXRGBA(255,255,255,255)));
  colorwells[1]->setRGBA(getApp()->reg().readColorEntry("SETTINGS",wellname[1],FXRGBA(  0,  0,  0,255)));
  colorwells[2]->setRGBA(getApp()->reg().readColorEntry("SETTINGS",wellname[2],FXRGBA(255,  0,  0,255)));
  colorwells[3]->setRGBA(getApp()->reg().readColorEntry("SETTINGS",wellname[3],FXRGBA(  0,255,  0,255)));
  colorwells[4]->setRGBA(getApp()->reg().readColorEntry("SETTINGS",wellname[4],FXRGBA(  0,  0,255,255)));
  colorwells[5]->setRGBA(getApp()->reg().readColorEntry("SETTINGS",wellname[5],FXRGBA(  0,  0,255,255)));
  colorwells[6]->setRGBA(getApp()->reg().readColorEntry("SETTINGS",wellname[6],FXRGBA(255,255,  0,255)));
  colorwells[7]->setRGBA(getApp()->reg().readColorEntry("SETTINGS",wellname[7],FXRGBA(  0,255,255,255)));
  colorwells[8]->setRGBA(getApp()->reg().readColorEntry("SETTINGS",wellname[8],FXRGBA(255,  0,255,255)));
  colorwells[9]->setRGBA(getApp()->reg().readColorEntry("SETTINGS",wellname[9],FXRGBA(128,  0,  0,255)));
  colorwells[10]->setRGBA(getApp()->reg().readColorEntry("SETTINGS",wellname[10],FXRGBA(  0,128,  0,255)));
  colorwells[11]->setRGBA(getApp()->reg().readColorEntry("SETTINGS",wellname[11],FXRGBA(  0,  0,128,255)));
  colorwells[12]->setRGBA(getApp()->reg().readColorEntry("SETTINGS",wellname[12],FXRGBA(128,128,  0,255)));
  colorwells[13]->setRGBA(getApp()->reg().readColorEntry("SETTINGS",wellname[13],FXRGBA(128,  0,128,255)));
  colorwells[14]->setRGBA(getApp()->reg().readColorEntry("SETTINGS",wellname[14],FXRGBA(  0,128,128,255)));
  colorwells[15]->setRGBA(getApp()->reg().readColorEntry("SETTINGS",wellname[15],FXRGBA(  0,128,128,255)));
  colorwells[16]->setRGBA(getApp()->reg().readColorEntry("SETTINGS",wellname[16],FXRGBA(255,  0,255,255)));
  colorwells[17]->setRGBA(getApp()->reg().readColorEntry("SETTINGS",wellname[17],FXRGBA(128,  0,  0,255)));
  colorwells[18]->setRGBA(getApp()->reg().readColorEntry("SETTINGS",wellname[18],FXRGBA(  0,128,  0,255)));
  colorwells[19]->setRGBA(getApp()->reg().readColorEntry("SETTINGS",wellname[19],FXRGBA(  0,  0,128,255)));
  colorwells[20]->setRGBA(getApp()->reg().readColorEntry("SETTINGS",wellname[20],FXRGBA(128,128,  0,255)));
  colorwells[21]->setRGBA(getApp()->reg().readColorEntry("SETTINGS",wellname[21],FXRGBA(128,  0,128,255)));
  colorwells[22]->setRGBA(getApp()->reg().readColorEntry("SETTINGS",wellname[22],FXRGBA(  0,128,128,255)));
  colorwells[23]->setRGBA(getApp()->reg().readColorEntry("SETTINGS",wellname[23],FXRGBA(  0,128,128,255)));

  // Switch to correct pane

*/

// Change RGBA color
void FXColorDialog::setRGBA(FXColor clr){
  colorbox->setRGBA(clr);
  }


// Retrieve RGBA color
FXColor FXColorDialog::getRGBA() const {
  return colorbox->getRGBA();
  }


// Forward ColorSelector color change to target [a color well]
long FXColorDialog::onChgColor(FXObject*,FXSelector,void* ptr){
  return target && target->tryHandle(this,FXSEL(SEL_CHANGED,message),ptr);
  }


// Forward ColorSelector color command to target [a color well]
long FXColorDialog::onCmdColor(FXObject*,FXSelector,void* ptr){
  return target && target->tryHandle(this,FXSEL(SEL_COMMAND,message),ptr);
  }


// Update color dialog from a message
long FXColorDialog::onCmdSetIntValue(FXObject*,FXSelector,void* ptr){
  setRGBA(*((FXColor*)ptr));
  return 1;
  }


// Obtain value from color dialog
long FXColorDialog::onCmdGetIntValue(FXObject*,FXSelector,void* ptr){
  *((FXColor*)ptr)=getRGBA();
  return 1;
  }


// Change active panel
void FXColorDialog::setActivePanel(FXint pnl){
  colorbox->setActivePanel(pnl);
  }


// Return active panel
FXint FXColorDialog::getActivePanel() const {
  return colorbox->getActivePanel();
  }


// Change well color
void FXColorDialog::setWellColor(FXint w,FXColor clr){
  colorbox->setWellColor(w,clr);
  }


// Return well color
FXColor FXColorDialog::getWellColor(FXint w) const {
  return colorbox->getWellColor(w);
  }


// Change opaque only mode
void FXColorDialog::setOpaqueOnly(FXbool forceopaque){
  colorbox->setOpaqueOnly(forceopaque);
  }


// Return true if only opaque colors allowed
FXbool FXColorDialog::isOpaqueOnly() const {
  return colorbox->isOpaqueOnly();
  }


// Save data
void FXColorDialog::save(FXStream& store) const {
  FXDialogBox::save(store);
  store << colorbox;
  }


// Load data
void FXColorDialog::load(FXStream& store){
  FXDialogBox::load(store);
  store >> colorbox;
  }


// Cleanup
FXColorDialog::~FXColorDialog(){
  destroy();
  }

}
