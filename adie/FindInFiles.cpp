/********************************************************************************
*                                                                               *
*                    F i n d   P a t t e r n   I n   F i l e s                  *
*                                                                               *
*********************************************************************************
* Copyright (C) 1998,2013 by Jeroen van der Zijp.   All Rights Reserved.        *
*********************************************************************************
* This program is free software: you can redistribute it and/or modify          *
* it under the terms of the GNU General Public License as published by          *
* the Free Software Foundation, either version 3 of the License, or             *
* (at your option) any later version.                                           *
*                                                                               *
* This program is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                 *
* GNU General Public License for more details.                                  *
*                                                                               *
* You should have received a copy of the GNU General Public License             *
* along with this program.  If not, see <http://www.gnu.org/licenses/>.         *
********************************************************************************/
#include "fx.h"
#include "fxkeys.h"
#include "icons.h"
#include "help.h"
#include "Preferences.h"
#include "Commands.h"
#include "Syntax.h"
#include "TextWindow.h"
#include "Adie.h"
#include "FindInFiles.h"

/*
  Notes:

  - Need exclusion clauses: don't scan files with certain extensions, don't
    scan files that exceed certain size, don't scan files whose first chunk
    of data matches certain pattern (e.g. "PNG" or "GIF89a", etc).
  - Probably need to make this standard re-usable widget.
*/

#define HORZ_PAD      12
#define VERT_PAD      2

/*******************************************************************************/

// Map
FXDEFMAP(FindInFiles) FindInFilesMap[]={
  FXMAPFUNC(SEL_COMMAND,FindInFiles::ID_SEARCH,FindInFiles::onCmdSearch),
  FXMAPFUNC(SEL_COMMAND,FindInFiles::ID_FOLDER,FindInFiles::onCmdFolder),
  FXMAPFUNC(SEL_KEYPRESS,FindInFiles::ID_SEARCH_TEXT,FindInFiles::onArrowKey),
  FXMAPFUNC(SEL_MOUSEWHEEL,FindInFiles::ID_SEARCH_TEXT,FindInFiles::onMouseWheel),
  FXMAPFUNCS(SEL_UPDATE,FindInFiles::ID_HIST_UP,FindInFiles::ID_HIST_DN,FindInFiles::onUpdHistory),
  FXMAPFUNCS(SEL_COMMAND,FindInFiles::ID_HIST_UP,FindInFiles::ID_HIST_DN,FindInFiles::onCmdHistory),
  };


// Object implementation
FXIMPLEMENT(FindInFiles,FXDialogBox,FindInFilesMap,ARRAYNUMBER(FindInFilesMap))


// Construct file in files dialog
FindInFiles::FindInFiles(Adie *a):FXDialogBox(a,"Find In Files",DECOR_TITLE|DECOR_BORDER|DECOR_RESIZE,0,0,600,400, 10,10,10,10, 10,10){

  // Buttons at bottom
  FXHorizontalFrame* buttons=new FXHorizontalFrame(this,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|PACK_UNIFORM_WIDTH|PACK_UNIFORM_HEIGHT,0,0,0,0,0,0,0,0);
  new FXButton(buttons,tr("&Search"),NULL,this,ID_SEARCH,BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_FILL_Y|LAYOUT_RIGHT,0,0,0,0,HORZ_PAD,HORZ_PAD,VERT_PAD,VERT_PAD);
  new FXButton(buttons,tr("&Cancel"),NULL,this,ID_CLOSE,BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_Y|LAYOUT_RIGHT,0,0,0,0,6,6,VERT_PAD,VERT_PAD);

  // Above button block
  new FXHorizontalSeparator(this,SEPARATOR_GROOVE|LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);

  // Options block
  FXHorizontalFrame* frame=new FXHorizontalFrame(this,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|PACK_UNIFORM_WIDTH|PACK_UNIFORM_HEIGHT,0,0,0,0,0,0,0,0);
  new FXRadioButton(frame,tr("Ex&act"),NULL,0,ICON_BEFORE_TEXT|LAYOUT_CENTER_X);
  new FXRadioButton(frame,tr("I&gnore Case"),NULL,0,ICON_BEFORE_TEXT|LAYOUT_CENTER_X);
  new FXRadioButton(frame,tr("E&xpression"),NULL,0,ICON_BEFORE_TEXT|LAYOUT_CENTER_X);
  new FXCheckButton(frame,tr("&Recursive"),NULL,0,ICON_BEFORE_TEXT|LAYOUT_CENTER_X);
  new FXCheckButton(frame,tr("&Hidden Files"),NULL,0,ICON_BEFORE_TEXT|LAYOUT_CENTER_X);

  // Entry block
  FXMatrix *matrix=new FXMatrix(this,3,MATRIX_BY_COLUMNS|LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X,0,0,0,0,0,0,0,0);

  // Text field with history
  new FXLabel(matrix,tr("S&earch for:"),NULL,JUSTIFY_RIGHT|LAYOUT_FILL_X|LAYOUT_CENTER_Y);
  FXHorizontalFrame* searchbox=new FXHorizontalFrame(matrix,FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_CENTER_Y|LAYOUT_FILL_COLUMN,0,0,0,0, 0,0,0,0, 0,0);
  findstring=new FXTextField(searchbox,26,this,ID_SEARCH_TEXT,TEXTFIELD_ENTER_ONLY|LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 3,3,3,3);
  FXVerticalFrame* searcharrows=new FXVerticalFrame(searchbox,LAYOUT_RIGHT|LAYOUT_FILL_Y|PACK_UNIFORM_HEIGHT,0,0,0,0, 0,0,0,0, 0,0);
  FXArrowButton* ar1=new FXArrowButton(searcharrows,this,ID_HIST_UP,FRAME_RAISED|FRAME_THICK|ARROW_UP|ARROW_REPEAT|LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH, 0,0,16,0, 1,1,1,1);
  FXArrowButton* ar2=new FXArrowButton(searcharrows,this,ID_HIST_DN,FRAME_RAISED|FRAME_THICK|ARROW_DOWN|ARROW_REPEAT|LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH, 0,0,16,0, 1,1,1,1);
  ar1->setArrowSize(5);
  ar2->setArrowSize(5);
  new FXFrame(matrix,0);

  // Folder to search
  new FXLabel(matrix,tr("In &Folder:"),NULL,JUSTIFY_RIGHT|LAYOUT_FILL_X|LAYOUT_CENTER_Y);
  filefolder=new FXTextField(matrix,40,this,ID_FOLDER_TEXT,JUSTIFY_LEFT|FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_CENTER_Y|LAYOUT_FILL_COLUMN|LAYOUT_FILL_X,0,0,0,0, 3,3,3,3);
  new FXButton(matrix,"...",NULL,this,ID_FOLDER,LAYOUT_CENTER_Y|FRAME_RAISED|FRAME_THICK|LAYOUT_FIX_WIDTH,0,0,20,0);

  // Filter for files
  new FXLabel(matrix,tr("F&ilter:"),NULL,JUSTIFY_RIGHT|LAYOUT_FILL_X|LAYOUT_CENTER_Y);
  filefilter=new FXComboBox(matrix,10,this,ID_FILTER_TEXT,COMBOBOX_STATIC|LAYOUT_FILL_X|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_COLUMN,0,0,0,0, 3,3,3,3);
  filefilter->setNumVisible(4);
  new FXFrame(matrix,0);

  // Matching files
  FXHorizontalFrame* resultbox=new FXHorizontalFrame(this,LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_SUNKEN|FRAME_THICK,0,0,0,0,0,0,0,0);
  locations=new FXIconList(resultbox,NULL,0,LAYOUT_FILL_X|LAYOUT_FILL_Y|ICONLIST_DETAILED|ICONLIST_SINGLESELECT);
  locations->appendHeader("File",NULL,100);
  locations->appendHeader("Line",NULL,50);
  locations->appendHeader("Text",NULL,200);

  // Set title
  setTitle(tr("Find In Files"));

  // Initial pattern
  setPatternList(tr("All Files (*)"));
  index=-1;
  }


// Set text or pattern to search for
void FindInFiles::setSearchText(const FXString& text){
  findstring->setText(text);
  }


// Return text or pattern the user has entered
FXString FindInFiles::getSearchText() const {
  return findstring->getText();
  }


// Change directory
void FindInFiles::setDirectory(const FXString& path){
  filefolder->setText(path);
  }


// Return directory
FXString FindInFiles::getDirectory() const {
  return filefolder->getText();
  }


// Set list of patterns
void FindInFiles::setPatternList(const FXString& patterns){
  filefilter->clearItems();
  filefilter->fillItems(patterns);
  if(!filefilter->getNumItems()) filefilter->appendItem(tr("All Files (*)"));
  filefilter->setNumVisible(FXMIN(filefilter->getNumItems(),12));
  setCurrentPattern(0);
  }


// Return list of patterns
FXString FindInFiles::getPatternList() const {
  FXString pat;
  for(FXint i=0; i<filefilter->getNumItems(); i++){
    if(!pat.empty()) pat+='\n';
    pat+=filefilter->getItemText(i);
    }
  return pat;
  }


// Set currently selected pattern
void FindInFiles::setCurrentPattern(FXint patno){
  if(patno<0 || patno>=filefilter->getNumItems()){ fxerror("%s::setCurrentPattern: index out of range.\n",getClassName()); }
  filefilter->setCurrentItem(patno);
  }


// Return current pattern number
FXint FindInFiles::getCurrentPattern() const {
  return filefilter->getCurrentItem();
  }


// Change pattern text for pattern number
void FindInFiles::setPatternText(FXint patno,const FXString& text){
  if(patno<0 || patno>=filefilter->getNumItems()){ fxerror("%s::setPatternText: index out of range.\n",getClassName()); }
  filefilter->setItemText(patno,text);
  }


// Get pattern text for given pattern number
FXString FindInFiles::getPatternText(FXint patno) const {
  if(patno<0 || patno>=filefilter->getNumItems()){ fxerror("%s::getPatternText: index out of range.\n",getClassName()); }
  return filefilter->getItemText(patno);
  }


// Return number of patterns
FXint FindInFiles::getNumPatterns() const {
  return filefilter->getNumItems();
  }


// Allow pattern entry
void FindInFiles::allowPatternEntry(FXbool flag){
  filefilter->setComboStyle(flag?COMBOBOX_NORMAL:COMBOBOX_STATIC);
  }


// Return true if pattern entry is allowed
FXbool FindInFiles::allowPatternEntry() const {
  return (filefilter->getComboStyle()!=COMBOBOX_STATIC);
  }


// Start search
long FindInFiles::onCmdSearch(FXObject*,FXSelector,void*){
  appendhist(getSearchText());
  return 1;
  }


// Set directory to search in
long FindInFiles::onCmdFolder(FXObject*,FXSelector,void*){
  FXString path=FXFileDialog::getOpenDirectory(this,"Search In Folder",getDirectory());
  if(!path.empty()){
    setDirectory(path);
    }
  return 1;
  }

/*
  if(FXSELID(sel)==ID_HIST_UP){
    if(current<20 && getApp()->reg().readStringEntry("FindInFiles",regkey[current],NULL)) current++;
    }
  else{
    if(current>0) current--;
    }
  if(current){
    setSearchText(getApp()->reg().readStringEntry("FindInFiles",regkey[current-1],FXString::null));
    }
  else{
    setSearchText(FXString::null);
    setReplaceText(FXString::null);
    setSearchMode(SEARCH_EXACT|SEARCH_FORWARD);
    }
*/

// Add string to history buffer
void FindInFiles::appendhist(const FXString& string){
  if(!string.empty() && string!=history[0]){
    for(FXint i=19; i>0; i--){
      history[i].adopt(history[i-1]);
      }
    history[0]=string;
    index=0;
    }
  }


// Scroll back in history
void FindInFiles::scrollback(){
  if(index<19 && !history[index+1].empty()){
    setSearchText(history[++index]);
    }
  else{
    getApp()->beep();
    }
  }


// Scroll forward in history
void FindInFiles::scrollforw(){
  if(0<=index){
    --index;
    if(0<=index){
      setSearchText(history[index]);
      }
    else{
      setSearchText(FXString::null);
      }
    }
  }

// FIXME this should be a widget (FXListSpinner) !!

// Registry keys for history
static const FXchar regkey[20][3]={
  "SA","SB","SC","SD","SE","SF","SG","SH","SI","SJ","SK","SL","SM","SN","SO","SP","SQ","SR","SS","ST"
  };


// Update arrows
long FindInFiles::onUpdHistory(FXObject* sender,FXSelector sel,void*){
  if(FXSELID(sel)==ID_HIST_UP){
    sender->handle(this,(index<19 && !history[index+1].empty())?FXSEL(SEL_COMMAND,ID_ENABLE):FXSEL(SEL_COMMAND,ID_DISABLE),NULL);
    }
  else{
    sender->handle(this,(-1<index)?FXSEL(SEL_COMMAND,ID_ENABLE):FXSEL(SEL_COMMAND,ID_DISABLE),NULL);
    }
  return 1;
  }


// Scroll back in search history
long FindInFiles::onCmdHistory(FXObject*,FXSelector sel,void*){
  if(FXSELID(sel)==ID_HIST_UP){
    scrollback();
    }
  else{
    scrollforw();
    }
  return 1;
  }


// Hitting arrows in text field scrolls through history
long FindInFiles::onArrowKey(FXObject*,FXSelector,void* ptr){
  switch(((FXEvent*)ptr)->code){
    case KEY_Up:
    case KEY_KP_Up:
      scrollback();
      return 1;
    case KEY_Down:
    case KEY_KP_Down:
      scrollforw();
      return 1;
    }
  return 0;
  }


// Wheeling in text field scrolls through history
long FindInFiles::onMouseWheel(FXObject*,FXSelector,void* ptr){
  if(((FXEvent*)ptr)->code>0){
    scrollback();
    }
  else if(((FXEvent*)ptr)->code<0){
    scrollforw();
    }
  return 1;
  }


// Clean up
FindInFiles::~FindInFiles(){
  locations=(FXIconList*)-1L;
  findstring=(FXTextField*)-1L;
  filefolder=(FXTextField*)-1L;
  filefilter=(FXComboBox*)-1L;
  }
