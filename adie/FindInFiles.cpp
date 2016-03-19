/********************************************************************************
*                                                                               *
*                    F i n d   P a t t e r n   I n   F i l e s                  *
*                                                                               *
*********************************************************************************
* Copyright (C) 1998,2015 by Jeroen van der Zijp.   All Rights Reserved.        *
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
  - Keep registry entry for size & position of dialog.
*/

#define HORZ_PAD      12
#define VERT_PAD      2

/*******************************************************************************/

// Map
FXDEFMAP(FindInFiles) FindInFilesMap[]={
  FXMAPFUNC(SEL_COMMAND,FindInFiles::ID_SEARCH,FindInFiles::onCmdSearch),
  FXMAPFUNC(SEL_COMMAND,FindInFiles::ID_FOLDER,FindInFiles::onCmdFolder),
  FXMAPFUNC(SEL_COMMAND,FindInFiles::ID_SEARCH_TEXT,FindInFiles::onCmdSearch),
  FXMAPFUNC(SEL_KEYPRESS,FindInFiles::ID_SEARCH_TEXT,FindInFiles::onArrowKey),
  FXMAPFUNC(SEL_MOUSEWHEEL,FindInFiles::ID_SEARCH_TEXT,FindInFiles::onMouseWheel),
  FXMAPFUNC(SEL_UPDATE,FindInFiles::ID_HIST_UP,FindInFiles::onUpdHistoryUp),
  FXMAPFUNC(SEL_UPDATE,FindInFiles::ID_HIST_DN,FindInFiles::onUpdHistoryDn),
  FXMAPFUNC(SEL_COMMAND,FindInFiles::ID_HIST_UP,FindInFiles::onCmdHistoryUp),
  FXMAPFUNC(SEL_COMMAND,FindInFiles::ID_HIST_DN,FindInFiles::onCmdHistoryDn),
  FXMAPFUNC(SEL_DOUBLECLICKED,FindInFiles::ID_FILELIST,FindInFiles::onCmdFileDblClicked),
  FXMAPFUNC(SEL_COMMAND,FindInFiles::ID_FILTER_TEXT,FindInFiles::onCmdFilter),
  FXMAPFUNCS(SEL_UPDATE,FindInFiles::ID_EXACT,FindInFiles::ID_HIDDEN,FindInFiles::onUpdFlags),
  FXMAPFUNCS(SEL_COMMAND,FindInFiles::ID_EXACT,FindInFiles::ID_HIDDEN,FindInFiles::onCmdFlags),
  };


// Object implementation
FXIMPLEMENT(FindInFiles,FXDialogBox,FindInFilesMap,ARRAYNUMBER(FindInFilesMap))


// Search and replace dialog registry section name
const FXchar FindInFiles::sectionName[]="Find In Files";


// Registry keys for search pattern
static const FXchar skey[20][3]={
  "SA","SB","SC","SD","SE","SF","SG","SH","SI","SJ","SK","SL","SM","SN","SO","SP","SQ","SR","SS","ST"
  };


// Registry keys for search options
static const FXchar mkey[20][3]={
  "MA","MB","MC","MD","ME","MF","MG","MH","MI","MJ","MK","ML","MM","MN","MO","MP","MQ","MR","MS","MT"
  };


// Construct file in files dialog
FindInFiles::FindInFiles(Adie *a):FXDialogBox(a,"Find In Files",DECOR_TITLE|DECOR_BORDER|DECOR_RESIZE,0,0,600,400, 10,10,10,10, 10,10){

  // Buttons at bottom
  FXHorizontalFrame* buttons=new FXHorizontalFrame(this,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|PACK_UNIFORM_WIDTH|PACK_UNIFORM_HEIGHT,0,0,0,0,0,0,0,0);
  new FXButton(buttons,tr("&Search"),NULL,this,ID_SEARCH,BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_FILL_Y|LAYOUT_RIGHT,0,0,0,0,HORZ_PAD,HORZ_PAD,VERT_PAD,VERT_PAD);
  new FXButton(buttons,tr("&Close"),NULL,this,ID_CLOSE,BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_Y|LAYOUT_RIGHT,0,0,0,0,6,6,VERT_PAD,VERT_PAD);
  new FXToggleButton(buttons,tr("Pause"),tr("Resume"),NULL,NULL,this,ID_PAUSE,FRAME_RAISED|FRAME_THICK|LAYOUT_FILL_Y|LAYOUT_LEFT,0,0,0,0,HORZ_PAD,HORZ_PAD,VERT_PAD,VERT_PAD);

  // Above button block
  new FXHorizontalSeparator(this,SEPARATOR_GROOVE|LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);

  // Options block
  FXHorizontalFrame* frame=new FXHorizontalFrame(this,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|PACK_UNIFORM_WIDTH|PACK_UNIFORM_HEIGHT,0,0,0,0,0,0,0,0);
  new FXRadioButton(frame,tr("Ex&act"),this,ID_EXACT,ICON_BEFORE_TEXT|LAYOUT_CENTER_X);
  new FXRadioButton(frame,tr("I&gnore Case"),this,ID_ICASE,ICON_BEFORE_TEXT|LAYOUT_CENTER_X);
  new FXRadioButton(frame,tr("E&xpression"),this,ID_REGEX,ICON_BEFORE_TEXT|LAYOUT_CENTER_X);
  new FXCheckButton(frame,tr("&Recursive"),this,ID_RECURSIVE,ICON_BEFORE_TEXT|LAYOUT_CENTER_X);
  new FXCheckButton(frame,tr("&Hidden Files"),this,ID_HIDDEN,ICON_BEFORE_TEXT|LAYOUT_CENTER_X);

  // Entry block
  FXMatrix *matrix=new FXMatrix(this,3,MATRIX_BY_COLUMNS|LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X,0,0,0,0,0,0,0,0);

  // Text field with history
  new FXLabel(matrix,tr("S&earch for:"),NULL,JUSTIFY_RIGHT|LAYOUT_FILL_X|LAYOUT_CENTER_Y);
  FXHorizontalFrame* searchbox=new FXHorizontalFrame(matrix,FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_CENTER_Y|LAYOUT_FILL_COLUMN,0,0,0,0, 0,0,0,0, 0,0);
  findstring=new FXTextField(searchbox,26,this,ID_SEARCH_TEXT,TEXTFIELD_ENTER_ONLY|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  FXVerticalFrame* searcharrows=new FXVerticalFrame(searchbox,LAYOUT_RIGHT|LAYOUT_FILL_Y|PACK_UNIFORM_HEIGHT,0,0,0,0, 0,0,0,0, 0,0);
  FXArrowButton* ar1=new FXArrowButton(searcharrows,this,ID_HIST_UP,FRAME_RAISED|FRAME_THICK|ARROW_UP|ARROW_REPEAT|LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH, 0,0,16,0, 1,1,1,1);
  FXArrowButton* ar2=new FXArrowButton(searcharrows,this,ID_HIST_DN,FRAME_RAISED|FRAME_THICK|ARROW_DOWN|ARROW_REPEAT|LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH, 0,0,16,0, 1,1,1,1);
  ar1->setArrowSize(5);
  ar2->setArrowSize(5);
  new FXFrame(matrix,0);

  // Folder to search
  new FXLabel(matrix,tr("In &Folder:"),NULL,JUSTIFY_RIGHT|LAYOUT_FILL_X|LAYOUT_CENTER_Y);
  filefolder=new FXTextField(matrix,40,this,ID_FOLDER_TEXT,JUSTIFY_LEFT|FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_CENTER_Y|LAYOUT_FILL_COLUMN|LAYOUT_FILL_X);
  new FXButton(matrix,"...",NULL,this,ID_FOLDER,LAYOUT_CENTER_Y|FRAME_RAISED|FRAME_THICK|LAYOUT_FIX_WIDTH,0,0,20,0);

  // Filter for files
  new FXLabel(matrix,tr("F&ilter:"),NULL,JUSTIFY_RIGHT|LAYOUT_FILL_X|LAYOUT_CENTER_Y);
  filefilter=new FXComboBox(matrix,10,this,ID_FILTER_TEXT,COMBOBOX_STATIC|LAYOUT_FILL_X|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_COLUMN);
  filefilter->setNumVisible(4);
  new FXFrame(matrix,0);

  // Matching files
  FXHorizontalFrame* resultbox=new FXHorizontalFrame(this,LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_SUNKEN|FRAME_THICK,0,0,0,0,0,0,0,0);
  locations=new FXIconList(resultbox,this,ID_FILELIST,LAYOUT_FILL_X|LAYOUT_FILL_Y|ICONLIST_DETAILED|ICONLIST_SINGLESELECT);
  locations->appendHeader(tr("File"),NULL,300);
  locations->appendHeader(tr("Line"),NULL,50);
  locations->appendHeader(tr("Text"),NULL,200);

  // Set title
  setTitle(tr("Find In Files"));

  // Initial pattern
  setPatternList(tr("All Files (*)"));

  // Start with this
  filePattern="*";

  // Search flags
  searchmode=SearchExact;
  index=-1;
  }


// Create server-side resources
void FindInFiles::create(){
  FXTRACE((1,"FindInFiles::create\n"));
  readRegistry();
  FXDialogBox::create();
  }


// Close window
FXbool FindInFiles::close(FXbool notify){
  FXTRACE((1,"FindInFiles::close\n"));
  writeRegistry();
  return FXDialogBox::close(notify);
  }


// Load registy
void FindInFiles::readRegistry(){
  setWidth(getApp()->reg().readIntEntry(sectionName,"width",600));
  setHeight(getApp()->reg().readIntEntry(sectionName,"height",400));
  for(FXint i=0; i<20; ++i){
    if(getApp()->reg().readStringEntry(sectionName,skey[i],NULL)==NULL) break;
    patternHistory[i]=getApp()->reg().readStringEntry(sectionName,skey[i],FXString::null);
    optionsHistory[i]=getApp()->reg().readUIntEntry(sectionName,mkey[i],SearchExact);
    }
  }


// Save registry
void FindInFiles::writeRegistry(){
  getApp()->reg().writeIntEntry("SETTINGS","width",getWidth());
  getApp()->reg().writeIntEntry("SETTINGS","height",getHeight());
  for(FXint i=0; i<20; ++i){
    if(patternHistory[i].empty()) break;
    getApp()->reg().writeStringEntry(sectionName,skey[i],patternHistory[i].text());
    getApp()->reg().writeUIntEntry(sectionName,mkey[i],optionsHistory[i]);
    }
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


// Change file pattern
void FindInFiles::setPattern(const FXString& ptrn){
  filefilter->setText(ptrn);
  filePattern=ptrn;
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
  filePattern=FXFileSelector::patternFromText(filefilter->getItemText(patno));
  }


// Return current pattern number
FXint FindInFiles::getCurrentPattern() const {
  return filefilter->getCurrentItem();
  }


// Change pattern text for pattern number
void FindInFiles::setPatternText(FXint patno,const FXString& text){
  if(patno<0 || patno>=filefilter->getNumItems()){ fxerror("%s::setPatternText: index out of range.\n",getClassName()); }
  filefilter->setItemText(patno,text);
  if(patno==filefilter->getCurrentItem()){
    filePattern=FXFileSelector::patternFromText(text);
    }
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


// Add string to history buffer
void FindInFiles::appendHistory(const FXString& patt,FXuint opts){
  if(!patt.empty()){
    if(patt!=patternHistory[0]){
      for(FXint i=19; i>0; i--){
        swap(patternHistory[i],patternHistory[i-1]);
        swap(optionsHistory[i],optionsHistory[i-1]);
        }
      }
    patternHistory[0]=patt;
    optionsHistory[0]=opts;
    index=0;
    }
  }


// Update arrows
long FindInFiles::onUpdHistoryUp(FXObject* sender,FXSelector sel,void*){
  sender->handle(this,(index<19 && !patternHistory[index+1].empty())?FXSEL(SEL_COMMAND,ID_ENABLE):FXSEL(SEL_COMMAND,ID_DISABLE),NULL);
  return 1;
  }


// Update arrows
long FindInFiles::onUpdHistoryDn(FXObject* sender,FXSelector sel,void*){
  sender->handle(this,(-1<index)?FXSEL(SEL_COMMAND,ID_ENABLE):FXSEL(SEL_COMMAND,ID_DISABLE),NULL);
  return 1;
  }


// Scroll back in search history
long FindInFiles::onCmdHistoryUp(FXObject*,FXSelector,void*){
  if(index<19 && !patternHistory[index+1].empty()){
    ++index;
    setSearchText(patternHistory[index]);
    setSearchMode(optionsHistory[index]);
    }
  else{
    getApp()->beep();
    }
  return 1;
  }


// Scroll back in search history
long FindInFiles::onCmdHistoryDn(FXObject*,FXSelector,void*){
  if(0<=index){
    --index;
    if(0<=index){
      setSearchText(patternHistory[index]);
      setSearchMode(optionsHistory[index]);
      }
    else{
      setSearchText(FXString::null);
      setSearchMode(SearchExact);
      }
    }
  return 1;
  }


// Hitting arrows in text field scrolls through history
long FindInFiles::onArrowKey(FXObject*,FXSelector,void* ptr){
  switch(((FXEvent*)ptr)->code){
    case KEY_Up:
    case KEY_KP_Up:
      return onCmdHistoryUp(this,FXSEL(SEL_COMMAND,ID_HIST_UP),NULL);
    case KEY_Down:
    case KEY_KP_Down:
      return onCmdHistoryDn(this,FXSEL(SEL_COMMAND,ID_HIST_DN),NULL);
    }
  return 0;
  }


// Wheeling in text field scrolls through history
long FindInFiles::onMouseWheel(FXObject*,FXSelector,void* ptr){
  if(((FXEvent*)ptr)->code>0){
    return onCmdHistoryUp(this,FXSEL(SEL_COMMAND,ID_HIST_UP),NULL);
    }
  if(((FXEvent*)ptr)->code<0){
    return onCmdHistoryDn(this,FXSEL(SEL_COMMAND,ID_HIST_DN),NULL);
    }
  return 1;
  }


// Update flags
long FindInFiles::onUpdFlags(FXObject* sender,FXSelector sel,void*){
  FXuint value=0;
  switch(FXSELID(sel)){
    case ID_EXACT: value=!(searchmode&(SearchCaseFold|SearchRegex)); break;
    case ID_ICASE: value=(searchmode&SearchCaseFold); break;
    case ID_REGEX: value=(searchmode&SearchRegex); break;
    case ID_RECURSIVE: value=(searchmode&SearchRecurse); break;
    case ID_HIDDEN: value=(searchmode&SeachHidden); break;
    }
  sender->handle(this,value?FXSEL(SEL_COMMAND,FXWindow::ID_CHECK):FXSEL(SEL_COMMAND,FXWindow::ID_UNCHECK),NULL);
  return 1;
  }



// Handle flags
long FindInFiles::onCmdFlags(FXObject*,FXSelector sel,void*){
  switch(FXSELID(sel)){
    case ID_EXACT: searchmode=(searchmode&~(SearchCaseFold|SearchRegex)); break;
    case ID_ICASE: searchmode=(searchmode&~SearchRegex)|SearchCaseFold; break;
    case ID_REGEX: searchmode=(searchmode&~SearchCaseFold)|SearchRegex; break;
    case ID_RECURSIVE: searchmode^=SearchRecurse; break;
    case ID_HIDDEN: searchmode^=SeachHidden; break;
    }
  return 1;
  }

/*******************************************************************************/

// Set directory to search in
long FindInFiles::onCmdFolder(FXObject*,FXSelector,void*){
  FXString path=FXFileDialog::getOpenDirectory(this,tr("Search In Folder"),getDirectory());
  if(!path.empty()){
    setDirectory(path);
    }
  return 1;
  }


// File list double clicked
long FindInFiles::onCmdFileDblClicked(FXObject*,FXSelector,void* ptr){
  FXint which=(FXint)(FXival)ptr;
  return 1;
  }



// Change the pattern; change the filename to the suggested extension
long FindInFiles::onCmdFilter(FXObject*,FXSelector,void* ptr){
  filePattern=FXFileSelector::patternFromText((FXchar*)ptr);
  return 1;
  }


/*******************************************************************************/


// Initialize search parameters
SearchVisitor::SearchVisitor(const FXString& pattern,FXint mode):rex(pattern,mode){
  }


// Load file contents
FXint SearchVisitor::loadFileBody(const FXString& file,FXString& body) const {
  FXFile textfile(file,FXFile::Reading);
  if(textfile.isOpen()){
    FXlong size=textfile.size();
    if(0<size && size<2147483647){
      body.length(size);
      return textfile.readBlock(body.text(),body.length());
      }
    }
  return 0;
  }


// Search file contents for pattern
FXint SearchVisitor::searchFileBody(const FXString& body) const {
  FXint beg[10],end[10],pos=0,line=1;
  while(pos<body.length()){
    if(rex.amatch(body,pos,FXRex::Normal,beg,end,10)){
      fxmessage("hit: at %d\n",line);
      }
    line+=(body[pos++]=='\n');
    }
  return 0;
  }


// Visit file
FXuint SearchVisitor::visit(const FXString& path){
  if(FXGlobVisitor::visit(path)){
    FXString body;
    fxmessage("visit(%s)\n",path.text());
    if(loadFileBody(path,body)){
      fxmessage("loaded %s: %d bytes\n",path.text(),body.length());
      searchFileBody(body);
      return 1;
      }
    }
  return 0;
  }


// Start search
long FindInFiles::onCmdSearch(FXObject*,FXSelector,void*){
  FXuint opts=FXDir::AllDirs|FXRex::Capture;
  FXint rexmode=FXRex::Capture;
  if(getSearchMode()&SearchCaseFold) rexmode|=FXRex::IgnoreCase;           // Case insensitivity
  if(!(getSearchMode()&SearchRegex)) rexmode|=FXRex::Verbatim;             // Verbatim match
  if(getSearchMode()&SeachHidden) opts|=FXDir::HiddenFiles|FXDir::HiddenDirs;
//  SearchVisitor visitor(getSearchText(),rexmode);
  appendHistory(getSearchText(),getSearchMode());
//  visitor.traverse(getDirectory(),getPattern(),opts);
  
  FXGlobCountVisitor viz;
  viz.traverse(getDirectory(),getPattern(),opts);
  fxmessage("info: %s:\nfolders: %ld\nfiles:   %ld\nbytes:   %ld\ndepth:   %ld\n\n",getDirectory().text(),viz.getTotalFolders(),viz.getTotalFiles(),viz.getTotalBytes(),viz.getMaximumDepth());

  return 1;
  }

/*******************************************************************************/

// Clean up
FindInFiles::~FindInFiles(){
  locations=(FXIconList*)-1L;
  findstring=(FXTextField*)-1L;
  filefolder=(FXTextField*)-1L;
  filefilter=(FXComboBox*)-1L;
  }
