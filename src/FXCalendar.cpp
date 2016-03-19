/********************************************************************************
*                                                                               *
*                   B a s e   C a l e n d a r   W i d g e t                     *
*                                                                               *
*********************************************************************************
* Copyright (C) 2006,2008 by Sander Jansen.   All Rights Reserved.              *
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
* $Id: FXCalendar.cpp,v 1.17 2008/01/04 15:42:05 fox Exp $                      *
********************************************************************************/
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "fxascii.h"
#include "fxkeys.h"
#include "FXHash.h"
#include "FXThread.h"
#include "FXStream.h"
#include "FXString.h"
#include "FXSize.h"
#include "FXPoint.h"
#include "FXObjectList.h"
#include "FXRectangle.h"
#include "FXRegistry.h"
#include "FXApp.h"
#include "FXId.h"
#include "FXDrawable.h"
#include "FXDC.h"
#include "FXWindow.h"
#include "FXFrame.h"
#include "FXLabel.h"
#include "FXComposite.h"
#include "FXPacker.h"
#include "FXButton.h"
#include "FXArrowButton.h"
#include "FXMenuButton.h"
#include "FXToolTip.h"
#include "FXOptionMenu.h"
#include "FXHorizontalFrame.h"
#include "FXDCWindow.h"
#include "FXFont.h"
#include "FXDate.h"
#include "FXPopup.h"
#include "FXCalendarView.h"
#include "FXCalendar.h"

/*
  Notes:
  - Most of calendar is implemented in FXCalendarView
*/

#define CALENDAR_MASK (CALENDAR_SINGLESELECT|CALENDAR_BROWSESELECT|CALENDAR_WEEKNUMBERS|CALENDAR_STATIC|CALENDAR_HIDEOTHER)


using namespace FX;

/*******************************************************************************/

namespace FX {


// Map
FXDEFMAP(FXCalendar) FXCalendarMap[]={
  FXMAPFUNC(SEL_REPLACED,FXCalendar::ID_CALENDAR,FXCalendar::onCmdDate),
  FXMAPFUNCS(SEL_COMMAND,FXCalendar::ID_MONTH_START,FXCalendar::ID_MONTH_END,FXCalendar::onCmdMonth),
  FXMAPFUNC(SEL_COMMAND,FXCalendar::ID_NEXTYEAR,FXCalendar::onCmdNextYear),
  FXMAPFUNC(SEL_COMMAND,FXCalendar::ID_PREVYEAR,FXCalendar::onCmdPrevYear),
  FXMAPFUNC(SEL_COMMAND,FXCalendar::ID_NEXTMONTH,FXCalendar::onCmdNextMonth),
  FXMAPFUNC(SEL_COMMAND,FXCalendar::ID_PREVMONTH,FXCalendar::onCmdPrevMonth),
  FXMAPFUNC(SEL_COMMAND,FXCalendar::ID_MONTH,FXCalendar::onCmdSelectMonth),
  FXMAPFUNC(SEL_COMMAND,FXWindow::ID_SETVALUE,FXCalendar::onFwdToView),
  FXMAPFUNC(SEL_COMMAND,FXWindow::ID_SETINTVALUE,FXCalendar::onFwdToView),
  FXMAPFUNC(SEL_COMMAND,FXWindow::ID_GETINTVALUE,FXCalendar::onFwdToView),
  FXMAPFUNC(SEL_COMMAND,FXCalendar::ID_CALENDAR,FXCalendar::onFwdToTarget),
  FXMAPFUNC(SEL_CHANGED,FXCalendar::ID_CALENDAR,FXCalendar::onFwdToTarget),
  FXMAPFUNC(SEL_UPDATE,FXCalendar::ID_CALENDAR,FXCalendar::onFwdToTarget),
  };


// Implementation
FXIMPLEMENT(FXCalendar,FXPacker,FXCalendarMap,ARRAYNUMBER(FXCalendarMap));


FXCalendar::FXCalendar(){
  flags|=FLAG_ENABLED;
  }


// Construct and initialize calendar widget
FXCalendar::FXCalendar(FXComposite *p,FXObject* tgt,FXSelector sel,FXuint opts,FXint x,FXint y,FXint w,FXint h):FXPacker(p,opts,x,y,w,h,0,0,0,0,0,0){
  flags|=FLAG_ENABLED;
  target=tgt;
  message=sel;

  // Popup for months
  months=new FXPopup(this);
  for(FXint i=1; i<=12; i++){
    new FXOption(months,tr(FXDate::monthName(i)),NULL,this,ID_MONTH_START+(i-1),OPTIONMENU_NOGLYPH|LAYOUT_LEFT|JUSTIFY_CENTER_X|ICON_AFTER_TEXT);
    }

  // Caption above
  frame=new FXHorizontalFrame(this,LAYOUT_SIDE_TOP|LAYOUT_FILL_X,0,0,0,0,0,0,0,0,0,0);

  // Month selector
  arrows[0]=new FXArrowButton(frame,this,ID_PREVMONTH,ARROW_LEFT|ARROW_REPEAT|ARROW_TOOLBAR|FRAME_RAISED|LAYOUT_CENTER_Y);
  month=new FXOptionMenu(frame,months,OPTIONMENU_NOGLYPH|OPTIONMENU_TOOLBAR|FRAME_RAISED|JUSTIFY_CENTER_X|JUSTIFY_CENTER_Y|LAYOUT_CENTER_Y,0,0,0,0);
  month->setTarget(this);
  month->setSelector(ID_MONTH);

  // Year selector
  arrows[1]=new FXArrowButton(frame,this,ID_NEXTMONTH,ARROW_RIGHT|ARROW_REPEAT|ARROW_TOOLBAR|FRAME_RAISED|LAYOUT_CENTER_Y);
  arrows[2]=new FXArrowButton(frame,this,ID_NEXTYEAR,ARROW_RIGHT|ARROW_REPEAT|ARROW_TOOLBAR|FRAME_RAISED|LAYOUT_CENTER_Y|LAYOUT_RIGHT);
  year=new FXLabel(frame,"0000",NULL,LAYOUT_RIGHT|LAYOUT_CENTER_Y);
  arrows[3]=new FXArrowButton(frame,this,ID_PREVYEAR,ARROW_LEFT|ARROW_REPEAT|ARROW_TOOLBAR|FRAME_RAISED|LAYOUT_CENTER_Y|LAYOUT_RIGHT);

  // Main widget
  view=new FXCalendarView(this,this,ID_CALENDAR,(options&CALENDAR_MASK)|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_SIDE_BOTTOM);

  // Fix the options
  if(options&FRAME_SUNKEN && !(options&FRAME_RAISED)){
    if(options&FRAME_THICK)
      frame->setFrameStyle(FRAME_RAISED|FRAME_THICK);
    else
      frame->setFrameStyle(FRAME_RAISED);
    }
  else{
    frame->setFrameStyle(FRAME_NONE);
    }
  }


// Create X window
void FXCalendar::create(){
  FXDate date(view->getCurrentDate());
  FXPacker::create();
  year->setText(FXString::value(date.year()));
  month->setCurrentNo(date.month()-1);
  }


// Enable the window
void FXCalendar::enable(){
  FXPacker::enable();
  view->enable();
  month->enable();
  year->enable();
  arrows[0]->enable();
  arrows[1]->enable();
  arrows[2]->enable();
  arrows[3]->enable();
  }


// Disable the window
void FXCalendar::disable(){
  FXPacker::disable();
  view->disable();
  month->disable();
  year->disable();
  arrows[0]->disable();
  arrows[1]->disable();
  arrows[2]->disable();
  arrows[3]->disable();
  }


// Set date
void FXCalendar::setCurrentDate(FXDate date,FXbool notify){
  view->setCurrentDate(date,notify);
  }


// Get the current date
FXDate FXCalendar::getCurrentDate() const {
  return view->getCurrentDate();
  }


// Set the current month
void FXCalendar::setCurrentMonth(FXint mo,FXbool notify){
  view->setCurrentMonth(mo,notify);
  }


// Return the current month shown
FXint FXCalendar::getCurrentMonth() const {
  return view->getCurrentMonth();
  }


// Set the Calendar Style
void FXCalendar::setCalendarStyle(FXuint style){
  view->setCalendarStyle(style);
  }


// Get the Calendar Style
FXuint FXCalendar::getCalendarStyle() const {
  return view->getCalendarStyle();
  }


// Set the first day of the week [0 -> 6]
void FXCalendar::setFirstDay(FXint d){
  view->setFirstDay(d);
  view->update();
  }


// Get the first day of the week [0 -> 6]
FXint FXCalendar::getFirstDay() const {
  return view->getFirstDay();
  }


// Change the Frame Style
void FXCalendar::setFrameStyle(FXuint s){
  FXPacker::setFrameStyle(s);
  if(options&FRAME_SUNKEN && !(options&FRAME_RAISED)){
    if(options&FRAME_THICK)
      frame->setFrameStyle(FRAME_RAISED|FRAME_THICK);
    else
      frame->setFrameStyle(FRAME_RAISED);
    }
  else{
    frame->setFrameStyle(FRAME_NONE);
    }
  }


// Set the back color
void FXCalendar::setBackColor(FXColor c){
  FXPacker::setBackColor(c);
  view->setBackColor(c);
  }


// Get the back color
FXColor FXCalendar::getBackColor() const {
  return view->getBackColor();
  }


// Set the display color of titles
void FXCalendar::setTitleColor(FXColor c){
  view->setTitleColor(c);
  }


// Get the display color of titles
FXColor FXCalendar::getTitleColor() const {
  return view->getTitleColor();
  }


// Set the display color of titles
void FXCalendar::setTitleBackColor(FXColor c){
  view->setTitleBackColor(c);
  }


// Get the display color of titles
FXColor FXCalendar::getTitleBackColor() const {
  return view->getTitleBackColor();
  }


// Set the display color of non-weekend days
void FXCalendar::setDayColor(FXColor c){
  view->setDayColor(c);
  }


// Get the display color of non-weekend days
FXColor FXCalendar::getDayColor() const {
  return view->getDayColor();
  }


// Set the display color of non-weekend days not in the current month
void FXCalendar::setOtherDayColor(FXColor c){
  view->setOtherDayColor(c);
  }


// Get the display color of non-weekend days not in the current month
FXColor FXCalendar::getOtherDayColor() const {
  return view->getOtherDayColor();
  }


// Set the display color of today
void FXCalendar::setTodayColor(FXColor c){
  view->setTodayColor(c);
  }


// Get the display color of today
FXColor FXCalendar::getTodayColor() const {
  return view->getTodayColor();
  }


// Set the display color of days in the weekend
void FXCalendar::setWeekendColor(FXColor c){
  view->setWeekendColor(c);
  }


// Get the display color of days in the weekend
FXColor FXCalendar::getWeekendColor() const {
  return view->getWeekendColor();
  }


// Set the display color of days in the weekend not in the current month
void FXCalendar::setOtherWeekendColor(FXColor c){
  view->setOtherWeekendColor(c);
  }


// Get the display color of days in the weekend not in the current month
FXColor FXCalendar::getOtherWeekendColor() const {
  return view->getOtherWeekendColor();
  }


// Switch date
long FXCalendar::onCmdDate(FXObject*,FXSelector,void* ptr){
  FXDate date((FXuint)(FXival)(ptr));
  year->setText(FXString::value(date.year()));
  month->setCurrentNo(view->getCurrentMonth()-1);
  if(target) target->tryHandle(this,FXSEL(SEL_CHANGED,message),ptr);
  return 1;
  }


// Switch month
long FXCalendar::onCmdMonth(FXObject*,FXSelector sel,void*){
  view->setCurrentMonth((FXSELID(sel)-ID_MONTH_START)+1,true);
  return 1;
  }


// Select month
long FXCalendar::onCmdSelectMonth(FXObject*,FXSelector,void*ptr){
  view->setCurrentMonth(1+(FXint)(FXival)(ptr),true);
  return 1;
  }


// Go to next year
long FXCalendar::onCmdNextYear(FXObject*,FXSelector,void*){
  FXDate date=view->getCurrentDate();
  date.addYears(1);
  view->setCurrentDate(date,true);
  return 1;
  }


// Go to previous year
long FXCalendar::onCmdPrevYear(FXObject*,FXSelector,void*){
  FXDate date=view->getCurrentDate();
  date.addYears(-1);
  view->setCurrentDate(date,true);
  return 1;
  }


// Go to next month
long FXCalendar::onCmdNextMonth(FXObject*,FXSelector,void*){
  FXDate date=view->getCurrentDate();
  date.addMonths(1);
  view->setCurrentDate(date,true);
  return 1;
  }


// Go to previous month
long FXCalendar::onCmdPrevMonth(FXObject*,FXSelector,void*){
  FXDate date=view->getCurrentDate();
  date.addMonths(-1);
  view->setCurrentDate(date,true);
  return 1;
  }


// Forward to calendar view
long FXCalendar::onFwdToView(FXObject*sender,FXSelector sel,void*ptr){
  return view->handle(sender,sel,ptr);
  }


// Forward from calendar view
long FXCalendar::onFwdToTarget(FXObject*,FXSelector sel,void* ptr){
  return target && target->tryHandle(this,FXSEL(FXSELTYPE(sel),message),ptr);
  }


// Destroy
FXCalendar::~FXCalendar(){
  delete months;
  view=(FXCalendarView*)-1L;
  year=(FXLabel*)-1L;
  months=(FXPopup*)-1L;
  month=(FXOptionMenu*)-1L;
  frame=(FXHorizontalFrame*)-1L;
  arrows[0]=(FXArrowButton*)-1L;
  arrows[1]=(FXArrowButton*)-1L;
  arrows[2]=(FXArrowButton*)-1L;
  arrows[3]=(FXArrowButton*)-1L;
  }


}
