/********************************************************************************
*                                                                               *
*               R e p l a c e   I n   M u l t i p l e   F i l e s               *
*                                                                               *
*********************************************************************************
* Copyright (C) 2026 by Jeroen van der Zijp.   All Rights Reserved.             *
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
#include "Modeline.h"
#include "TextWindow.h"
#include "Adie.h"
#include "ReplaceInFiles.h"

/*
  Notes:

  - Need exclusion clauses: don't scan files with certain extensions, don't
    scan files that exceed certain size, don't scan files whose first chunk
    of data matches certain pattern (e.g. "PNG" or "GIF89a", etc).
  - Probably need to make this standard re-usable widget.
  - Keep registry entry for size & position of dialog.
  - Should remember current file pattern for file filter, instead of having
    to set it each time; filter to search should be different from current
    filter of TextWindow.
  - Remembering current pattern and search text would be nice, for repeated
    invocations.  Maybe some more tweaks.
*/
#define TOPIC_REPL    113

#define HORZ_PAD      12
#define VERT_PAD      2

/*******************************************************************************/

// Traverse files under path and search for pattern in each
FXuint ReplaceVisitor::traverse(const FXString& path,const FXString& search,const FXString& replace,const FXString& wild,FXint mode,FXuint opts,FXint depth){
  FXTRACE(TOPIC_REPL,"SearchVisitor::traverse(path=%s,search=%s,replace=%s,wild=%s,mode=%b,opts=%b,depth=%d)\n",path.text(),search.text(),replace.text(),wild.text(),mode,opts,depth);

  // Update
  searchfor=search;
  replaceby=replace;

  // Compile the pattern
  if(rex.parse(searchfor,mode)==FXRex::ErrOK){

    // Traverse directory and visit the files
    return FXGlobVisitor::traverse(path,wild,opts,depth);
    }
  return 0;
  }


// Set file size limit
void ReplaceVisitor::setLimit(FXlong size){
  limit=FXCLAMP(1,size,2147483647);
  }


// Visit file
FXuint ReplaceVisitor::visit(const FXString& path){
  if(FXGlobVisitor::visit(path)){
    if(searchFile(path)) return 1;
    }
  return 0;
  }


// FIXME want same options for load/save as in TextWindow

// Load file contents
FXlong ReplaceVisitor::loadFile(const FXString& path){
  FXFile file(path,FXFile::Reading);
  FXTRACE(TOPIC_REPL,"loadFile(%s)\n",path.text());
  if(file.isOpen()){
    FXlong size=file.size();
    if(0<size && size<=limit){
      if(text.length(size)){
        return file.readBlock(text.text(),text.length());
        }
      }
    }
  return 0L;
  }


// Save file contents
FXlong ReplaceVisitor::saveFile(const FXString& path) const {
  FXFile file(path,FXFile::Writing);
  FXTRACE(TOPIC_REPL,"saveFile(%s)\n",path.text());
  if(file.isOpen()){
    return file.writeBlock(text.text(),text.length());
    }
  return 0L;
  }


// Search file contents for pattern
FXint ReplaceVisitor::searchFile(const FXString& path){
  FXString dirpath=FXPath::directory(path);
  FXTRACE(TOPIC_REPL,"searchFile(%s)\n",path.text());

  // Load the entire file in text
  if(!loadFile(path)){
    FXbool changed=false;
    FXString string;
    FXint beg[10];
    FXint end[10];
    FXint pos=0;
    FXint hit=0;

    // Search from pos, return hit
    while((hit=rex.search(text,pos,text.length(),FXRex::Normal,beg,end,10))!=-1){

      // Build replacement string; some fragments of text may be used
      string=FXRex::substitute(text,beg,end,replaceby,10);

      // Replace beg[0]...end[0] in text by string
      text.replace(beg[0],end[0]-beg[0],string);

      // Continue search from end of replacement
      pos=beg[0]+string.length();

      changed=true;
      }

    // Save text back to file
    if(changed){

      // Can't save changes....
      if(!saveFile(path)) return 2;
      }

    // Continue
    return 1;
    }

  // Skip
  return 0;
  }


/*******************************************************************************/

// Map
FXDEFMAP(ReplaceInFiles) ReplaceInFilesMap[]={
  FXMAPFUNC(SEL_UPDATE,ReplaceInFiles::ID_STOP,ReplaceInFiles::onUpdStop),
  FXMAPFUNC(SEL_COMMAND,ReplaceInFiles::ID_STOP,ReplaceInFiles::onCmdStop),
  FXMAPFUNC(SEL_UPDATE,ReplaceInFiles::ID_PAUSE,ReplaceInFiles::onUpdPause),
  FXMAPFUNC(SEL_COMMAND,ReplaceInFiles::ID_PAUSE,ReplaceInFiles::onCmdPause),
  FXMAPFUNC(SEL_UPDATE,ReplaceInFiles::ID_SEARCH,ReplaceInFiles::onUpdSearch),
  FXMAPFUNC(SEL_COMMAND,ReplaceInFiles::ID_SEARCH,ReplaceInFiles::onCmdSearch),
  FXMAPFUNC(SEL_COMMAND,ReplaceInFiles::ID_FOLDER,ReplaceInFiles::onCmdFolder),
  FXMAPFUNC(SEL_UPDATE,ReplaceInFiles::ID_HIST_UP,ReplaceInFiles::onUpdHistoryUp),
  FXMAPFUNC(SEL_UPDATE,ReplaceInFiles::ID_HIST_DN,ReplaceInFiles::onUpdHistoryDn),
  FXMAPFUNC(SEL_COMMAND,ReplaceInFiles::ID_HIST_UP,ReplaceInFiles::onCmdHistoryUp),
  FXMAPFUNC(SEL_COMMAND,ReplaceInFiles::ID_HIST_DN,ReplaceInFiles::onCmdHistoryDn),
  FXMAPFUNC(SEL_COMMAND,ReplaceInFiles::ID_FILTER_TEXT,ReplaceInFiles::onCmdFilter),
  FXMAPFUNC(SEL_COMMAND,ReplaceInFiles::ID_SEARCH_TEXT,ReplaceInFiles::onCmdSearch),
  FXMAPFUNC(SEL_KEYPRESS,ReplaceInFiles::ID_SEARCH_TEXT,ReplaceInFiles::onArrowKey),
  FXMAPFUNC(SEL_MOUSEWHEEL,ReplaceInFiles::ID_SEARCH_TEXT,ReplaceInFiles::onMouseWheel),
  FXMAPFUNCS(SEL_UPDATE,ReplaceInFiles::ID_ICASE,ReplaceInFiles::ID_HIDDEN,ReplaceInFiles::onUpdFlags),
  FXMAPFUNCS(SEL_COMMAND,ReplaceInFiles::ID_ICASE,ReplaceInFiles::ID_HIDDEN,ReplaceInFiles::onCmdFlags),
  };


// Object implementation
FXIMPLEMENT(ReplaceInFiles,FXDialogBox,ReplaceInFilesMap,ARRAYNUMBER(ReplaceInFilesMap))


// Search and replace dialog registry section name
static const FXchar sectionName[]="Replace In Files";


// Registry keys for search strings
static const FXchar skey[20][3]={
  "SA","SB","SC","SD","SE","SF","SG","SH","SI","SJ","SK","SL","SM","SN","SO","SP","SQ","SR","SS","ST"
  };


// Registry keys for replace strings
static const FXchar rkey[20][3]={
  "RA","RB","RC","RD","RE","RF","RG","RH","RI","RJ","RK","RL","RM","RN","RO","RP","RQ","RR","RS","RT"
  };


// Registry keys for folder strings
static const FXchar fkey[20][3]={
  "FA","FB","FC","FD","FE","FF","FG","FH","FI","FJ","FK","FL","FM","FN","FO","FP","FQ","FR","FS","FT"
  };


// Registry keys for file pattern options
static const FXchar pkey[20][3]={
  "PA","PB","PC","PD","PE","PF","PG","PH","PI","PJ","PK","PL","PM","PN","PO","PP","PQ","PR","PS","PT"
  };


// Registry keys for search mode options
static const FXchar mkey[20][3]={
  "MA","MB","MC","MD","ME","MF","MG","MH","MI","MJ","MK","ML","MM","MN","MO","MP","MQ","MR","MS","MT"
  };


/*******************************************************************************/

// For deserialization
ReplaceInFiles::ReplaceInFiles(){
  searchstring=nullptr;
  replacestring=nullptr;
  filefolder=nullptr;
  filefilter=nullptr;
  pausebutton=nullptr;
  clearElms(optionsHistory,20);
  clearElms(patternHistory,20);
  searchmode=SearchExact|SearchRecurse;
  savedsearchmode=0;
  savedcurrentpattern=0;
  index=-1;
  }


// Construct file in files dialog
ReplaceInFiles::ReplaceInFiles(Adie *a):FXDialogBox(a,"Find In Files",DECOR_TITLE|DECOR_BORDER|DECOR_RESIZE|DECOR_CLOSE,0,0,600,400, 10,10,10,10, 10,10){

  // Buttons at bottom
  FXHorizontalFrame* bottomline=new FXHorizontalFrame(this,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X,0,0,0,0,0,0,0,0);
  FXHorizontalFrame* buttons=new FXHorizontalFrame(bottomline,LAYOUT_RIGHT|PACK_UNIFORM_WIDTH|PACK_UNIFORM_HEIGHT,0,0,0,0,0,0,0,0);
  new FXButton(buttons,tr("&Close"),nullptr,this,ID_CLOSE,BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_Y|LAYOUT_RIGHT,0,0,0,0,6,6,VERT_PAD,VERT_PAD);
  new FXButton(buttons,tr("&Delete"),nullptr,this,ID_DELETE,BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_FILL_Y|LAYOUT_RIGHT,0,0,0,0,HORZ_PAD,HORZ_PAD,VERT_PAD,VERT_PAD);
  new FXButton(buttons,tr("Stop"),nullptr,this,ID_STOP,BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_FILL_Y|LAYOUT_RIGHT,0,0,0,0,HORZ_PAD,HORZ_PAD,VERT_PAD,VERT_PAD);
  pausebutton=new FXToggleButton(buttons,tr("Pause"),tr("Resume"),nullptr,nullptr,this,ID_PAUSE,FRAME_RAISED|FRAME_THICK|LAYOUT_FILL_Y|LAYOUT_RIGHT,0,0,0,0,HORZ_PAD,HORZ_PAD,VERT_PAD,VERT_PAD);
  new FXButton(buttons,tr("&Search"),nullptr,this,ID_SEARCH,BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_FILL_Y|LAYOUT_RIGHT,0,0,0,0,HORZ_PAD,HORZ_PAD,VERT_PAD,VERT_PAD);

  // Above button block
  new FXHorizontalSeparator(this,SEPARATOR_GROOVE|LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);

  // Options block
  FXHorizontalFrame* frame=new FXHorizontalFrame(this,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|PACK_UNIFORM_WIDTH|PACK_UNIFORM_HEIGHT,0,0,0,0,0,0,0,0);
  new FXCheckButton(frame,tr("E&xpression\tRegular Expression"),this,ID_REGEX,ICON_BEFORE_TEXT|LAYOUT_CENTER_X);
  new FXCheckButton(frame,tr("W&ords\tWhole Words"),this,ID_WORDS,ICON_BEFORE_TEXT|LAYOUT_CENTER_X);
  new FXCheckButton(frame,tr("&Ignore Case\tCase insensitive"),this,ID_ICASE,ICON_BEFORE_TEXT|LAYOUT_CENTER_X);
  new FXCheckButton(frame,tr("&Recursive\tSearch subdirectories"),this,ID_RECURSIVE,ICON_BEFORE_TEXT|LAYOUT_CENTER_X);
  new FXCheckButton(frame,tr("&Hidden Files\tSearch hidden files"),this,ID_HIDDEN,ICON_BEFORE_TEXT|LAYOUT_CENTER_X);

  // Entry block
  FXMatrix *matrix=new FXMatrix(this,3,MATRIX_BY_COLUMNS|LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,0,0,0,0);

  // Text field with history
  new FXLabel(matrix,tr("Search &for:"),nullptr,JUSTIFY_RIGHT|LAYOUT_FILL_X|LAYOUT_CENTER_Y|LAYOUT_FILL_ROW);
  FXHorizontalFrame* searchbox=new FXHorizontalFrame(matrix,FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_CENTER_Y|LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW,0,0,0,0, 0,0,0,0, 0,0);
  searchstring=new FXTextField(searchbox,26,this,ID_SEARCH_TEXT,TEXTFIELD_ENTER_ONLY|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  searchstring->setTipText(tr("Search Pattern"));
  FXVerticalFrame* searcharrows=new FXVerticalFrame(searchbox,LAYOUT_RIGHT|LAYOUT_FILL_Y|PACK_UNIFORM_HEIGHT,0,0,0,0, 0,0,0,0, 0,0);
  FXArrowButton* ar1=new FXArrowButton(searcharrows,this,ID_HIST_UP,FRAME_RAISED|FRAME_THICK|ARROW_UP|ARROW_REPEAT|LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH, 0,0,16,0, 1,1,1,1);
  FXArrowButton* ar2=new FXArrowButton(searcharrows,this,ID_HIST_DN,FRAME_RAISED|FRAME_THICK|ARROW_DOWN|ARROW_REPEAT|LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH, 0,0,16,0, 1,1,1,1);
  ar1->setArrowSize(5);
  ar2->setArrowSize(5);
  new FXFrame(matrix,LAYOUT_FILL_ROW);

  // Text field with history
  new FXLabel(matrix,tr("Replace &by:"),nullptr,JUSTIFY_RIGHT|LAYOUT_FILL_X|LAYOUT_CENTER_Y|LAYOUT_FILL_ROW);
  FXHorizontalFrame* replacebox=new FXHorizontalFrame(matrix,FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_CENTER_Y|LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW,0,0,0,0, 0,0,0,0, 0,0);
  replacestring=new FXTextField(replacebox,26,nullptr,0,TEXTFIELD_ENTER_ONLY|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  replacestring->setTipText(tr("Replace Pattern"));
  FXVerticalFrame* replacearrows=new FXVerticalFrame(replacebox,LAYOUT_RIGHT|LAYOUT_FILL_Y|PACK_UNIFORM_HEIGHT,0,0,0,0, 0,0,0,0, 0,0);
  FXArrowButton* ar3=new FXArrowButton(replacearrows,nullptr,0,FRAME_RAISED|FRAME_THICK|ARROW_UP|ARROW_REPEAT|LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH, 0,0,16,0, 1,1,1,1);
  FXArrowButton* ar4=new FXArrowButton(replacearrows,nullptr,0,FRAME_RAISED|FRAME_THICK|ARROW_DOWN|ARROW_REPEAT|LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH, 0,0,16,0, 1,1,1,1);
  ar3->setArrowSize(5);
  ar4->setArrowSize(5);
  new FXFrame(matrix,LAYOUT_FILL_ROW);

  // Folder to search
  new FXLabel(matrix,tr("In Fo&lder:"),nullptr,JUSTIFY_RIGHT|LAYOUT_FILL_X|LAYOUT_CENTER_Y|LAYOUT_FILL_ROW);
  filefolder=new FXTextField(matrix,40,this,ID_FOLDER_TEXT,JUSTIFY_LEFT|FRAME_SUNKEN|FRAME_THICK|LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW);
  filefolder->setTipText(tr("Folder To Search"));
  new FXButton(matrix,"...\tSelect Folder\tSelect folder to search.",nullptr,this,ID_FOLDER,LAYOUT_CENTER_Y|FRAME_RAISED|FRAME_THICK|LAYOUT_FIX_WIDTH|LAYOUT_FILL_ROW,0,0,20,0);

  // Filter for files
  new FXLabel(matrix,tr("Filt&er:"),nullptr,JUSTIFY_RIGHT|LAYOUT_FILL_X|LAYOUT_CENTER_Y|LAYOUT_FILL_ROW);
  filefilter=new FXComboBox(matrix,10,this,ID_FILTER_TEXT,COMBOBOX_STATIC|LAYOUT_FILL_X|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW);
  filefilter->setTipText(tr("Files matching wildcard"));
  filefilter->setNumVisible(4);
  new FXFrame(matrix,LAYOUT_FILL_ROW);

  // Clean array
  clearElms(optionsHistory,20);
  clearElms(patternHistory,20);

  // Set title
  setTitle(tr("Replace In Files"));

  // Set icons
  setIcon(getApp()->bigicon);
  setMiniIcon(getApp()->smallicon);

  // Initial pattern
  setPatternList(tr("All Files (*)"));

  // Start with this
  filePattern="*";

  // Search flags
  searchmode=SearchExact|SearchRecurse;
  savedsearchmode=0;
  savedcurrentpattern=0;
  index=-1;
  }


// Load registy
void ReplaceInFiles::readRegistry(){
  FXTRACE(TOPIC_REPL,"ReplaceInFiles::readRegistry()\n");
  setWidth(getApp()->reg().readIntEntry(sectionName,"width",600));
  setHeight(getApp()->reg().readIntEntry(sectionName,"height",400));
  //setCurrentPattern(getApp()->reg().readIntEntry(sectionName,"searchpattern",0));
  for(FXint i=0; i<ARRAYNUMBER(searchHistory); ++i){
    searchHistory[i]=getApp()->reg().readStringEntry(sectionName,skey[i],FXString::null);
    if(searchHistory[i].empty()) break;
    replaceHistory[i]=getApp()->reg().readStringEntry(sectionName,rkey[i],FXString::null);
    folderHistory[i]=getApp()->reg().readStringEntry(sectionName,fkey[i],FXString::null);
    patternHistory[i]=getApp()->reg().readUIntEntry(sectionName,pkey[i],0);
    optionsHistory[i]=getApp()->reg().readUIntEntry(sectionName,mkey[i],SearchExact);
    }
  }


// Save registry
void ReplaceInFiles::writeRegistry(){
  FXTRACE(TOPIC_REPL,"ReplaceInFiles::writeRegistry()\n");
  getApp()->reg().writeIntEntry(sectionName,"width",getWidth());
  getApp()->reg().writeIntEntry(sectionName,"height",getHeight());
  getApp()->reg().writeIntEntry(sectionName,"searchpattern",getCurrentPattern());
  for(FXint i=0; i<ARRAYNUMBER(searchHistory); ++i){
    if(!searchHistory[i].empty()){
      getApp()->reg().writeStringEntry(sectionName,skey[i],searchHistory[i].text());
      getApp()->reg().writeStringEntry(sectionName,rkey[i],replaceHistory[i].text());
      getApp()->reg().writeStringEntry(sectionName,fkey[i],folderHistory[i].text());
      getApp()->reg().writeUIntEntry(sectionName,pkey[i],patternHistory[i]);
      getApp()->reg().writeUIntEntry(sectionName,mkey[i],optionsHistory[i]);
      }
    else{
      getApp()->reg().deleteEntry(sectionName,skey[i]);
      getApp()->reg().deleteEntry(sectionName,rkey[i]);
      getApp()->reg().deleteEntry(sectionName,fkey[i]);
      getApp()->reg().deleteEntry(sectionName,pkey[i]);
      getApp()->reg().deleteEntry(sectionName,mkey[i]);
      }
    }
  }


// Add string to history buffer
void ReplaceInFiles::appendHistory(const FXString& text,const FXString& dir,FXuint patt,FXuint opts){
  if(!text.empty()){
    if(text!=searchHistory[0]){
      for(FXint i=19; i>0; i--){
        swap(searchHistory[i],searchHistory[i-1]);
        swap(folderHistory[i],folderHistory[i-1]);
        swap(patternHistory[i],patternHistory[i-1]);
        swap(optionsHistory[i],optionsHistory[i-1]);
        }
      }
    searchHistory[0]=text;  // FIXME
    folderHistory[0]=dir;
    patternHistory[0]=patt;
    optionsHistory[0]=opts;
    index=0;
    }
  }


// Create server-side resources
void ReplaceInFiles::create(){
  readRegistry();
  FXDialogBox::create();
  searchstring->setFocus();
  }


// Close window
FXbool ReplaceInFiles::close(FXbool notify){
  writeRegistry();
  return FXDialogBox::close(notify);
  }


// Change directory
void ReplaceInFiles::setDirectory(const FXString& path){
  filefolder->setText(path);
  }


// Return directory
FXString ReplaceInFiles::getDirectory() const {
  return filefolder->getText();
  }


// Set text or pattern to search for
void ReplaceInFiles::setSearchText(const FXString& text){
  replacestring->setText(text);
  }


// Return text or pattern the user has entered
FXString ReplaceInFiles::getSearchText() const {
  return replacestring->getText();
  }


// Set text or pattern to search with
void ReplaceInFiles::setReplaceText(const FXString& text){
  replacestring->setText(text);
  }


// Return text or pattern the user has entered
FXString ReplaceInFiles::getReplaceText() const {
  return replacestring->getText();
  }



// Change file pattern
void ReplaceInFiles::setPattern(const FXString& ptrn){
  filefilter->setText(ptrn);
  filePattern=ptrn;
  }


// Set list of patterns
void ReplaceInFiles::setPatternList(const FXString& patterns){
  filefilter->clearItems();
  filefilter->fillItems(patterns);
  if(!filefilter->getNumItems()) filefilter->appendItem(tr("All Files (*)"));
  filefilter->setNumVisible(Math::imin(filefilter->getNumItems(),12));
  setCurrentPattern(0);
  }


// Return list of patterns
FXString ReplaceInFiles::getPatternList() const {
  FXString pat;
  for(FXint i=0; i<filefilter->getNumItems(); i++){
    if(!pat.empty()) pat+='\n';
    pat+=filefilter->getItemText(i);
    }
  return pat;
  }


// Set currently selected pattern
void ReplaceInFiles::setCurrentPattern(FXint patno){
  patno=FXCLAMP(0,patno,filefilter->getNumItems()-1);
  filefilter->setCurrentItem(patno);
  filePattern=FXFileSelector::patternFromText(filefilter->getItemText(patno));
  }


// Return current pattern number
FXint ReplaceInFiles::getCurrentPattern() const {
  return filefilter->getCurrentItem();
  }


// Change pattern text for pattern number
void ReplaceInFiles::setPatternText(FXint patno,const FXString& text){
  if(patno<0 || patno>=filefilter->getNumItems()){ fxerror("%s::setPatternText: index out of range.\n",getClassName()); }
  filefilter->setItemText(patno,text);
  if(patno==filefilter->getCurrentItem()){
    filePattern=FXFileSelector::patternFromText(text);
    }
  }


// Get pattern text for given pattern number
FXString ReplaceInFiles::getPatternText(FXint patno) const {
  if(patno<0 || patno>=filefilter->getNumItems()){ fxerror("%s::getPatternText: index out of range.\n",getClassName()); }
  return filefilter->getItemText(patno);
  }


// Return number of patterns
FXint ReplaceInFiles::getNumPatterns() const {
  return filefilter->getNumItems();
  }


// Allow pattern entry
void ReplaceInFiles::allowPatternEntry(FXbool flag){
  filefilter->setComboStyle(flag?COMBOBOX_NORMAL:COMBOBOX_STATIC);
  }


// Return true if pattern entry is allowed
FXbool ReplaceInFiles::allowPatternEntry() const {
  return (filefilter->getComboStyle()!=COMBOBOX_STATIC);
  }


/*******************************************************************************/

// Update stop button
long ReplaceInFiles::onUpdStop(FXObject* sender,FXSelector,void*){
 // sender->handle(this,proceed?FXSEL(SEL_COMMAND,ID_DISABLE):FXSEL(SEL_COMMAND,ID_ENABLE),nullptr);
  return 1;
  }


// Stop scanning disk
long ReplaceInFiles::onCmdStop(FXObject*,FXSelector,void*){
 // proceed=2;
  return 1;
  }


// Update pause/resume button
long ReplaceInFiles::onUpdPause(FXObject* sender,FXSelector,void*){
//  sender->handle(this,(proceed==0)?FXSEL(SEL_COMMAND,ID_CHECK):FXSEL(SEL_COMMAND,ID_UNCHECK),nullptr);
//  sender->handle(this,visitor.visiting()?FXSEL(SEL_COMMAND,ID_ENABLE):FXSEL(SEL_COMMAND,ID_DISABLE),nullptr);
  return 1;
  }


// Pause/resume
long ReplaceInFiles::onCmdPause(FXObject*,FXSelector,void*){
// proceed=(proceed==1)?0:1;
  return 1;
  }


// Grey out buttons if no search text
long ReplaceInFiles::onUpdSearch(FXObject* sender,FXSelector,void*){
  FXbool enabled=!visitor.visiting() && !searchstring->getText().empty();
  sender->handle(this,enabled?FXSEL(SEL_COMMAND,ID_ENABLE):FXSEL(SEL_COMMAND,ID_DISABLE),nullptr);
  return 1;
  }


// Start search
long ReplaceInFiles::onCmdSearch(FXObject*,FXSelector,void*){
  FXuint opts=FXDir::AllDirs;
  FXint rexmode=FXRex::Capture;
  FXint limit=1000;
  if(getSearchMode()&SearchCaseFold) rexmode|=FXRex::IgnoreCase;                // Case insensitivity
  if(getSearchMode()&SearchWords) rexmode|=FXRex::Words;                        // Whole Words
  if(!(getSearchMode()&SearchRegex)) rexmode|=FXRex::Verbatim;                  // Verbatim match
  if(getSearchMode()&SeachHidden) opts|=FXDir::HiddenFiles|FXDir::HiddenDirs;   // Visit hidden files and directories
  if(!(getSearchMode()&SearchRecurse)) limit=2;                                 // Don't recurse
  appendHistory(getSearchText(),getDirectory(),getCurrentPattern(),getSearchMode());
  visitor.traverse(getDirectory(),getSearchText(),getReplaceText(),getPattern(),rexmode,opts,limit);
  getApp()->refresh();
  return 1;
  }

/*******************************************************************************/

// Change the pattern; change the filename to the suggested extension
long ReplaceInFiles::onCmdFilter(FXObject*,FXSelector,void* ptr){
  filePattern=FXFileSelector::patternFromText((FXchar*)ptr);
  return 1;
  }


// Update flags
long ReplaceInFiles::onUpdFlags(FXObject* sender,FXSelector sel,void*){
  FXuint value=0;
  switch(FXSELID(sel)){
    case ID_ICASE: value=(searchmode&SearchCaseFold); break;
    case ID_REGEX: value=(searchmode&SearchRegex); break;
    case ID_WORDS: value=(searchmode&SearchWords); break;
    case ID_RECURSIVE: value=(searchmode&SearchRecurse); break;
    case ID_HIDDEN: value=(searchmode&SeachHidden); break;
    }
  sender->handle(this,value?FXSEL(SEL_COMMAND,FXWindow::ID_CHECK):FXSEL(SEL_COMMAND,FXWindow::ID_UNCHECK),nullptr);
  return 1;
  }


// Handle flags
long ReplaceInFiles::onCmdFlags(FXObject*,FXSelector sel,void*){
  switch(FXSELID(sel)){
    case ID_ICASE: searchmode^=SearchCaseFold; break;
    case ID_REGEX: searchmode^=SearchRegex; break;
    case ID_WORDS: searchmode^=SearchWords; break;
    case ID_RECURSIVE: searchmode^=SearchRecurse; break;
    case ID_HIDDEN: searchmode^=SeachHidden; break;
    }
  return 1;
  }


// Set directory to search in
long ReplaceInFiles::onCmdFolder(FXObject*,FXSelector,void*){
  FXString path=FXFileDialog::getOpenDirectory(this,tr("Search In Folder"),getDirectory());
  if(!path.empty()){
    setDirectory(path);
    }
  return 1;
  }


// Update arrows
long ReplaceInFiles::onUpdHistoryUp(FXObject* sender,FXSelector,void*){
  sender->handle(this,(index<19 && !searchHistory[index+1].empty())?FXSEL(SEL_COMMAND,ID_ENABLE):FXSEL(SEL_COMMAND,ID_DISABLE),nullptr);
  return 1;
  }


// Update arrows
long ReplaceInFiles::onUpdHistoryDn(FXObject* sender,FXSelector,void*){
  sender->handle(this,(-1<index)?FXSEL(SEL_COMMAND,ID_ENABLE):FXSEL(SEL_COMMAND,ID_DISABLE),nullptr);
  return 1;
  }


// Scroll back in search history
long ReplaceInFiles::onCmdHistoryUp(FXObject*,FXSelector,void*){
  if(index<19 && !searchHistory[index+1].empty()){
    ++index;
    if(index==0){
      savedsearchtext=getSearchText();
      savedsearchfolder=getDirectory();
      savedsearchmode=getSearchMode();
      savedcurrentpattern=getCurrentPattern();
      }
    setSearchText(searchHistory[index]);
    setDirectory(folderHistory[index]);
    setCurrentPattern(patternHistory[index]);
    setSearchMode(optionsHistory[index]);
    }
  else{
    getApp()->beep();
    }
  return 1;
  }


// Scroll back in search history
long ReplaceInFiles::onCmdHistoryDn(FXObject*,FXSelector,void*){
  if(0<=index){
    --index;
    if(0<=index){
      setSearchText(searchHistory[index]);
      setDirectory(folderHistory[index]);
      setCurrentPattern(patternHistory[index]);
      setSearchMode(optionsHistory[index]);
      }
    else{
      setSearchText(savedsearchtext);
      setDirectory(savedsearchfolder);
      setSearchMode(savedsearchmode);
      setCurrentPattern(savedcurrentpattern);
      }
    }
  return 1;
  }


// Hitting arrows in text field scrolls through history
long ReplaceInFiles::onArrowKey(FXObject*,FXSelector,void* ptr){
  switch(((FXEvent*)ptr)->code){
    case KEY_Up:
    case KEY_KP_Up:
      return onCmdHistoryUp(this,FXSEL(SEL_COMMAND,ID_HIST_UP),nullptr);
    case KEY_Down:
    case KEY_KP_Down:
      return onCmdHistoryDn(this,FXSEL(SEL_COMMAND,ID_HIST_DN),nullptr);
    }
  return 0;
  }


// Wheeling in text field scrolls through history
long ReplaceInFiles::onMouseWheel(FXObject*,FXSelector,void* ptr){
  if(((FXEvent*)ptr)->code>0){
    return onCmdHistoryUp(this,FXSEL(SEL_COMMAND,ID_HIST_UP),nullptr);
    }
  if(((FXEvent*)ptr)->code<0){
    return onCmdHistoryDn(this,FXSEL(SEL_COMMAND,ID_HIST_DN),nullptr);
    }
  return 1;
  }

/*******************************************************************************/

// Clean up
ReplaceInFiles::~ReplaceInFiles(){
  searchstring=(FXTextField*)-1L;
  replacestring=(FXTextField*)-1L;
  filefolder=(FXTextField*)-1L;
  filefilter=(FXComboBox*)-1L;
  pausebutton=(FXToggleButton*)-1L;
  }

