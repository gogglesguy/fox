/********************************************************************************
*                                                                               *
*                        F i l e    L i s t   O b j e c t                       *
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
* $Id: FXFileList.cpp,v 1.285 2008/06/03 20:55:37 fox Exp $                     *
********************************************************************************/
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "fxkeys.h"
#include "fxascii.h"
#include "fxunicode.h"
#include "FXHash.h"
#include "FXThread.h"
#include "FXStream.h"
#include "FXObjectList.h"
#include "FXString.h"
#include "FXSize.h"
#include "FXPoint.h"
#include "FXRectangle.h"
#include "FXSystem.h"
#include "FXPath.h"
#include "FXStat.h"
#include "FXFile.h"
#include "FXURL.h"
#include "FXSettings.h"
#include "FXRegistry.h"
#include "FXApp.h"
#include "FXFont.h"
#include "FXIcon.h"
#include "FXGIFIcon.h"
#include "FXScrollBar.h"
#include "FXIconSource.h"
#include "FXIconDict.h"
#include "FXShell.h"
#include "FXPopup.h"
#include "FXMenuPane.h"
#include "FXMenuCaption.h"
#include "FXMenuCommand.h"
#include "FXMenuCascade.h"
#include "FXMenuRadio.h"
#include "FXMenuCheck.h"
#include "FXMenuSeparator.h"
#include "FXFileDict.h"
#include "FXHeader.h"
#include "FXIconList.h"
#include "FXFileList.h"
#include "FXCopyDialog.h"
#include "FXDir.h"
#include "FX88591Codec.h"
#include "icons.h"
#include "FXMessageBox.h"

/*
  Notes:
  - Share icons with other widgets; upgrade icons to some nicer ones.
  - Should some of these icons move to FXFileDict?
  - Clipboard of filenames.
  - Clipboard, DND, etc. support.
  - When being dragged over, if hovering over a directory item for some
    time we need to open it.
  - We should generate SEL_INSERTED, SEL_DELETED, SEL_REPLACED, SEL_CHANGED
    messages as the FXFileList updates itself from the file system.
  - The solution currently used to determine whether or not to blend the
    icon isn't so great; this class shouldn't have to know about FXPNGIcon.
  - Should blending also happen in FXIconDict? Or more general solution.
  - If you land in a large directory with images, things are a tad slow;
    need to speed this up some how.
*/


#define OPENDIRDELAY        700000000   // Delay before opening directory
#define REFRESHINTERVAL     1000000000  // Interval between refreshes
#define REFRESHFREQUENCY    30          // File systems not supporting mod-time, refresh every nth time

#ifndef TIMEFORMAT
#define TIMEFORMAT "%m/%d/%Y %H:%M:%S"
#endif

using namespace FX;

/*******************************************************************************/

namespace FX {


// Object implementation
FXIMPLEMENT(FXFileItem,FXIconItem,NULL,0)


// Map
FXDEFMAP(FXFileList) FXFileListMap[]={
  FXMAPFUNC(SEL_DND_ENTER,0,FXFileList::onDNDEnter),
  FXMAPFUNC(SEL_DND_LEAVE,0,FXFileList::onDNDLeave),
  FXMAPFUNC(SEL_DND_DROP,0,FXFileList::onDNDDrop),
  FXMAPFUNC(SEL_DND_MOTION,0,FXFileList::onDNDMotion),
  FXMAPFUNC(SEL_DND_REQUEST,0,FXFileList::onDNDRequest),
  FXMAPFUNC(SEL_BEGINDRAG,0,FXFileList::onBeginDrag),
  FXMAPFUNC(SEL_DRAGGED,0,FXFileList::onDragged),
  FXMAPFUNC(SEL_ENDDRAG,0,FXFileList::onEndDrag),
  FXMAPFUNC(SEL_CLIPBOARD_LOST,0,FXFileList::onClipboardLost),
  FXMAPFUNC(SEL_CLIPBOARD_REQUEST,0,FXFileList::onClipboardRequest),
  FXMAPFUNC(SEL_CHORE,FXFileList::ID_DROPACTION,FXFileList::onDropAction),
  FXMAPFUNC(SEL_CHORE,FXFileList::ID_PREVIEWCHORE,FXFileList::onPreviewChore),
  FXMAPFUNC(SEL_TIMEOUT,FXFileList::ID_OPENTIMER,FXFileList::onOpenTimer),
  FXMAPFUNC(SEL_TIMEOUT,FXFileList::ID_REFRESHTIMER,FXFileList::onRefreshTimer),
  FXMAPFUNC(SEL_UPDATE,FXFileList::ID_DIRECTORY_UP,FXFileList::onUpdDirectoryUp),
  FXMAPFUNC(SEL_UPDATE,FXFileList::ID_SORT_BY_NAME,FXFileList::onUpdSortByName),
  FXMAPFUNC(SEL_UPDATE,FXFileList::ID_SORT_BY_TYPE,FXFileList::onUpdSortByType),
  FXMAPFUNC(SEL_UPDATE,FXFileList::ID_SORT_BY_SIZE,FXFileList::onUpdSortBySize),
  FXMAPFUNC(SEL_UPDATE,FXFileList::ID_SORT_BY_TIME,FXFileList::onUpdSortByTime),
  FXMAPFUNC(SEL_UPDATE,FXFileList::ID_SORT_BY_USER,FXFileList::onUpdSortByUser),
  FXMAPFUNC(SEL_UPDATE,FXFileList::ID_SORT_BY_GROUP,FXFileList::onUpdSortByGroup),
  FXMAPFUNC(SEL_UPDATE,FXFileList::ID_SORT_REVERSE,FXFileList::onUpdSortReverse),
  FXMAPFUNC(SEL_UPDATE,FXFileList::ID_SORT_CASE,FXFileList::onUpdSortCase),
  FXMAPFUNC(SEL_UPDATE,FXFileList::ID_SET_PATTERN,FXFileList::onUpdSetPattern),
  FXMAPFUNC(SEL_UPDATE,FXFileList::ID_SET_DIRECTORY,FXFileList::onUpdSetDirectory),
  FXMAPFUNC(SEL_UPDATE,FXFileList::ID_SHOW_HIDDEN,FXFileList::onUpdShowHidden),
  FXMAPFUNC(SEL_UPDATE,FXFileList::ID_HIDE_HIDDEN,FXFileList::onUpdHideHidden),
  FXMAPFUNC(SEL_UPDATE,FXFileList::ID_TOGGLE_HIDDEN,FXFileList::onUpdToggleHidden),
  FXMAPFUNC(SEL_UPDATE,FXFileList::ID_TOGGLE_IMAGES,FXFileList::onUpdToggleImages),
  FXMAPFUNC(SEL_UPDATE,FXFileList::ID_HEADER,FXFileList::onUpdHeader),
  FXMAPFUNC(SEL_COMMAND,FXFileList::ID_HEADER,FXFileList::onCmdHeader),
  FXMAPFUNC(SEL_COMMAND,FXFileList::ID_DIRECTORY_UP,FXFileList::onCmdDirectoryUp),
  FXMAPFUNC(SEL_COMMAND,FXFileList::ID_SORT_BY_NAME,FXFileList::onCmdSortByName),
  FXMAPFUNC(SEL_COMMAND,FXFileList::ID_SORT_BY_TYPE,FXFileList::onCmdSortByType),
  FXMAPFUNC(SEL_COMMAND,FXFileList::ID_SORT_BY_SIZE,FXFileList::onCmdSortBySize),
  FXMAPFUNC(SEL_COMMAND,FXFileList::ID_SORT_BY_TIME,FXFileList::onCmdSortByTime),
  FXMAPFUNC(SEL_COMMAND,FXFileList::ID_SORT_BY_USER,FXFileList::onCmdSortByUser),
  FXMAPFUNC(SEL_COMMAND,FXFileList::ID_SORT_BY_GROUP,FXFileList::onCmdSortByGroup),
  FXMAPFUNC(SEL_COMMAND,FXFileList::ID_SORT_REVERSE,FXFileList::onCmdSortReverse),
  FXMAPFUNC(SEL_COMMAND,FXFileList::ID_SORT_CASE,FXFileList::onCmdSortCase),
  FXMAPFUNC(SEL_COMMAND,FXFileList::ID_SET_PATTERN,FXFileList::onCmdSetPattern),
  FXMAPFUNC(SEL_COMMAND,FXFileList::ID_SET_DIRECTORY,FXFileList::onCmdSetDirectory),
  FXMAPFUNC(SEL_COMMAND,FXFileList::ID_SETVALUE,FXFileList::onCmdSetValue),
  FXMAPFUNC(SEL_COMMAND,FXFileList::ID_SETSTRINGVALUE,FXFileList::onCmdSetStringValue),
  FXMAPFUNC(SEL_COMMAND,FXFileList::ID_GETSTRINGVALUE,FXFileList::onCmdGetStringValue),
  FXMAPFUNC(SEL_COMMAND,FXFileList::ID_SHOW_HIDDEN,FXFileList::onCmdShowHidden),
  FXMAPFUNC(SEL_COMMAND,FXFileList::ID_HIDE_HIDDEN,FXFileList::onCmdHideHidden),
  FXMAPFUNC(SEL_COMMAND,FXFileList::ID_TOGGLE_HIDDEN,FXFileList::onCmdToggleHidden),
  FXMAPFUNC(SEL_COMMAND,FXFileList::ID_TOGGLE_IMAGES,FXFileList::onCmdToggleImages),
  FXMAPFUNC(SEL_COMMAND,FXFileList::ID_REFRESH,FXFileList::onCmdRefresh),
  FXMAPFUNC(SEL_COMMAND,FXFileList::ID_CUT_SEL,FXFileList::onCmdCutSel),
  FXMAPFUNC(SEL_COMMAND,FXFileList::ID_COPY_SEL,FXFileList::onCmdCopySel),
  FXMAPFUNC(SEL_COMMAND,FXFileList::ID_DELETE_SEL,FXFileList::onCmdDeleteSel),
  FXMAPFUNC(SEL_COMMAND,FXFileList::ID_PASTE_SEL,FXFileList::onCmdPasteSel),
  FXMAPFUNC(SEL_COMMAND,FXFileList::ID_DROPCOPY,FXFileList::onCmdDropCopy),
  FXMAPFUNC(SEL_COMMAND,FXFileList::ID_DROPMOVE,FXFileList::onCmdDropMove),
  FXMAPFUNC(SEL_COMMAND,FXFileList::ID_DROPLINK,FXFileList::onCmdDropLink),
  };


// Object implementation
FXIMPLEMENT(FXFileList,FXIconList,FXFileListMap,ARRAYNUMBER(FXFileListMap))


// For serialization
FXFileList::FXFileList(){
  dropEnable();
  associations=NULL;
  big_folder=NULL;
  mini_folder=NULL;
  big_doc=NULL;
  mini_doc=NULL;
  big_app=NULL;
  mini_app=NULL;
  list=NULL;
#ifdef WIN32
  matchmode=FILEMATCH_FILE_NAME|FILEMATCH_NOESCAPE|FILEMATCH_CASEFOLD;
  sortfunc=ascendingCase;
#else
  matchmode=FILEMATCH_FILE_NAME|FILEMATCH_NOESCAPE;
  sortfunc=ascending;
#endif
  imagesize=32;
  dropaction=DRAG_MOVE;
  draggable=true;
  timestamp=0;
  counter=0;
  };


// File List
FXFileList::FXFileList(FXComposite *p,FXObject* tgt,FXSelector sel,FXuint opts,FXint x,FXint y,FXint w,FXint h):FXIconList(p,tgt,sel,opts,x,y,w,h),directory(PATHSEPSTRING),pattern("*"){
  dropEnable();
  appendHeader(tr("Name\tName"),NULL,200);
  appendHeader(tr("Type\tFile type"),NULL,100);
  appendHeader(tr("Size\tFile size"),NULL,60);
  appendHeader(tr("Modified Date\tDate when last modified"),NULL,150);
  appendHeader(tr("User\tUser name"),NULL,50);
  appendHeader(tr("Group\tGroup name"),NULL,50);
  appendHeader(tr("Attributes\tFile attributes"),NULL,100);
#ifndef WIN32
  appendHeader(tr("Link\tSymbolic link to"),NULL,200);
#endif
  associations=NULL;
  if(!(options&FILELIST_NO_OWN_ASSOC)) associations=new FXFileDict(getApp());
  big_folder=new FXGIFIcon(getApp(),bigfolder);
  mini_folder=new FXGIFIcon(getApp(),minifolder);
  big_doc=new FXGIFIcon(getApp(),bigdoc);
  mini_doc=new FXGIFIcon(getApp(),minidoc);
  big_app=new FXGIFIcon(getApp(),bigapp);
  mini_app=new FXGIFIcon(getApp(),miniapp);
  list=NULL;
#ifdef WIN32
  matchmode=FILEMATCH_FILE_NAME|FILEMATCH_NOESCAPE|FILEMATCH_CASEFOLD;
  sortfunc=ascendingCase;
#else
  matchmode=FILEMATCH_FILE_NAME|FILEMATCH_NOESCAPE;
  sortfunc=ascending;
#endif
  imagesize=32;
  dropaction=DRAG_MOVE;
  draggable=true;
  timestamp=0;
  counter=0;
  }


// Starts the timer
void FXFileList::create(){
  if(!id()) getApp()->addTimeout(this,ID_REFRESHTIMER,REFRESHINTERVAL);
  FXIconList::create();
  if(!deleteType){deleteType=getApp()->registerDragType(deleteTypeName);}
  if(!urilistType){urilistType=getApp()->registerDragType(urilistTypeName);}
  if(!utf8Type){utf8Type=getApp()->registerDragType(utf8TypeName);}
  if(!textType){textType=getApp()->registerDragType(textTypeName);}
  big_folder->create();
  mini_folder->create();
  big_doc->create();
  mini_doc->create();
  big_app->create();
  mini_app->create();
  scan(false);
  }


// Detach disconnects the icons
void FXFileList::detach(){
  if(id()) getApp()->removeTimeout(this,ID_REFRESHTIMER);
  if(id()) getApp()->removeTimeout(this,ID_OPENTIMER);
  FXIconList::detach();
  big_folder->detach();
  mini_folder->detach();
  big_doc->detach();
  mini_doc->detach();
  big_app->detach();
  mini_app->detach();
  deleteType=0;
  urilistType=0;
  utf8Type=0;
  textType=0;
  }


// Destroy zaps the icons
void FXFileList::destroy(){
  if(id()) getApp()->removeTimeout(this,ID_REFRESHTIMER);
  if(id()) getApp()->removeTimeout(this,ID_OPENTIMER);
  FXIconList::destroy();
  big_folder->destroy();
  mini_folder->destroy();
  big_doc->destroy();
  mini_doc->destroy();
  big_app->destroy();
  mini_app->destroy();
  }


// Create custom item
FXIconItem *FXFileList::createItem(const FXString& text,FXIcon *big,FXIcon* mini,void* ptr){
  return new FXFileItem(text,big,mini,ptr);
  }

/*******************************************************************************/


// Compare file names
FXint FXFileList::ascending(const FXIconItem* a,const FXIconItem* b){
  register FXint diff=static_cast<const FXFileItem*>(b)->isDirectory() - static_cast<const FXFileItem*>(a)->isDirectory();
  if(diff==0){
    diff=compareSection(a->label.text(),b->label.text(),0);
    }
  return diff;
  }


// Compare file names, case insensitive
FXint FXFileList::ascendingCase(const FXIconItem* a,const FXIconItem* b){
  register FXint diff=static_cast<const FXFileItem*>(b)->isDirectory() - static_cast<const FXFileItem*>(a)->isDirectory();
  if(diff==0){
    diff=compareSectionCase(a->label.text(),b->label.text(),0);
    }
  return diff;
  }


// Compare file types
FXint FXFileList::ascendingType(const FXIconItem* a,const FXIconItem* b){
  register FXint diff=static_cast<const FXFileItem*>(b)->isDirectory() - static_cast<const FXFileItem*>(a)->isDirectory();
  if(diff==0){
    diff=compareSection(a->label.text(),b->label.text(),1);
    if(diff==0){
      diff=compareSectionCase(a->label.text(),b->label.text(),0);
      }
    }
  return diff;
  }


// Compare file size
FXint FXFileList::ascendingSize(const FXIconItem* a,const FXIconItem* b){
  register FXint diff=static_cast<const FXFileItem*>(b)->isDirectory() - static_cast<const FXFileItem*>(a)->isDirectory();
  if(diff==0){
    diff=static_cast<const FXFileItem*>(a)->size - static_cast<const FXFileItem*>(b)->size;
    if(diff==0){
      diff=compareSectionCase(a->label.text(),b->label.text(),0);
      }
    }
  return diff;
  }


// Compare file time
FXint FXFileList::ascendingTime(const FXIconItem* a,const FXIconItem* b){
  register FXint diff=(FXint)((FXFileItem*)b)->isDirectory() - (FXint)((FXFileItem*)a)->isDirectory();
  if(diff==0){
    diff=static_cast<const FXFileItem*>(a)->date - static_cast<const FXFileItem*>(b)->date;
    if(diff==0){
      diff=compareSectionCase(a->label.text(),b->label.text(),0);
      }
    }
  return diff;
  }


// Compare file user
FXint FXFileList::ascendingUser(const FXIconItem* a,const FXIconItem* b){
  register FXint diff=static_cast<const FXFileItem*>(b)->isDirectory() - static_cast<const FXFileItem*>(a)->isDirectory();
  if(diff==0){
    diff=compareSection(a->label.text(),b->label.text(),4);
    if(diff==0){
      diff=compareSectionCase(a->label.text(),b->label.text(),0);
      }
    }
  return diff;
  }


// Compare file group
FXint FXFileList::ascendingGroup(const FXIconItem* a,const FXIconItem* b){
  register FXint diff=static_cast<const FXFileItem*>(b)->isDirectory() - static_cast<const FXFileItem*>(a)->isDirectory();
  if(diff==0){
    diff=compareSection(a->label.text(),b->label.text(),5);
    if(diff==0){
      diff=compareSectionCase(a->label.text(),b->label.text(),0);
      }
    }
  return diff;
  }


// Reversed compare file name, case insensitive
FXint FXFileList::descendingCase(const FXIconItem* a,const FXIconItem* b){
  return -FXFileList::ascendingCase(a,b);
  }


// Reversed compare file name
FXint FXFileList::descending(const FXIconItem* a,const FXIconItem* b){
  return -FXFileList::ascending(a,b);
  }


// Reversed compare file type
FXint FXFileList::descendingType(const FXIconItem* a,const FXIconItem* b){
  return -FXFileList::ascendingType(a,b);
  }


// Reversed compare file size
FXint FXFileList::descendingSize(const FXIconItem* a,const FXIconItem* b){
  return -FXFileList::ascendingSize(a,b);
  }


// Reversed compare file time
FXint FXFileList::descendingTime(const FXIconItem* a,const FXIconItem* b){
  return -FXFileList::ascendingTime(a,b);
  }


// Reversed compare file user
FXint FXFileList::descendingUser(const FXIconItem* a,const FXIconItem* b){
  return -FXFileList::ascendingUser(a,b);
  }


// Reversed compare file group
FXint FXFileList::descendingGroup(const FXIconItem* a,const FXIconItem* b){
  return -FXFileList::ascendingGroup(a,b);
  }


/*******************************************************************************/


// Return uri-list of selected files
FXString FXFileList::getSelectedFiles() const {
  FXString result;
  for(FXint i=0; i<getNumItems(); i++){
    if(isItemSelected(i) && !isItemNavigational(i)){
      result.append(FXURL::fileToURL(getItemPathname(i)));
      result.append("\r\n");
      }
    }
  return result;
  }


// Decode urilist to list of filenames
static FXint decodeURIList(FXString& result,const FXString& string){
  register FXint beg=0,end=0,n=0;
  result=FXString::null;
  while(end<string.length()){
    beg=end;
    while(end<string.length() && string[end]!='\n' && string[end]!='\r') end++;
    result.append(FXURL::decode(FXURL::fileFromURL(string.mid(beg,end-beg))));
    result.append("\n");
    while(string[end]=='\n' || string[end]=='\r') end++;
    n++;
    }
  return n;
  }


// Encode list of filenames to urilist
static FXint encodeURIList(FXString& result,const FXString& string){
  register FXint beg=0,end=0,n=0;
  result=FXString::null;
  while(end<string.length()){
    beg=end;
    while(end<string.length() && string[end]!='\n' && string[end]!='\r') end++;
    result.append(FXURL::encode(FXURL::fileToURL(string.mid(beg,end-beg))));
    result.append("\r\n");
    while(string[end]=='\n' || string[end]=='\r') end++;
    n++;
    }
  return n;
  }

#if 0
 - kde_clipboard=registerDragType("application/x-kde-cutselection");
 - gnome_clipboard=registerDragType("x-special/gnome-copied-files");
 - gnome_dragndrop=registerDragType("x-special/gnome-icon-list");
 - urilistType


My internal code to keep track of the selected files put on the clipboard:

   for(int i=0; i<filelist->getNumItems(); i++){
      if(filelist->isItemSelected(i) && filelist->getItemFilename(i)!=".." && filelist->getItemFilename(i)!="."){
        clipboard.append(FXURL::fileToURL(filelist->getItemPathname(i)));
        }
      }

The actual request is handled like this:

  if (event->target==fileapp->kde_clipboard) {
    /// 1 == cut, 0 == copy
    setDNDData(FROM_CLIPBOARD,event->target,clipboard_cut ? "1" : "0");
    }
  else if (event->target==urilistType){
    FXString list;
    list+=clipboard[0];
    for (int i=1;i<clipboard.no();i++){
      list+="\r\n";
      list+=clipboard[i];
      }
    setDNDData(FROM_CLIPBOARD,event->target,list);
    }
  else if (event->target==fileapp->gnome_clipboard){
    FXString list;

    if (clipboard_cut)
      list+="cut\n";
    else
      list+="copy\n";

    list+=clipboard[0];
    for (int i=1;i<clipboard.no();i++){
      list+="\n";
      list+=clipboard[i];
      }
    setDNDData(FROM_CLIPBOARD,event->target,list);
    }

#endif

// FIXME share code with file clipboard...

/*
// Deal with the drop that has just occurred
long FXFileList::onDropAction(FXObject*,FXSelector,void*){
  FXint beg,end;
  if(dropaction==DRAG_MOVE){
    FXCopyDialog dialog(this,tr("Move Files"),tr("Move these files:"),tr("To:"),big_folder);
    dialog.setSourceFiles(dropfiles);
    dialog.setDestinationFile(dropdirectory);
    if(dialog.execute()){
      beg=end=0;
      while(end<dropfiles.length()){
        beg=end;
        while(end<dropfiles.length() && dropfiles[end]!='\n' && dropfiles[end]!='\r') end++;
        FXTRACE((100,"Moving file: %s to %s\n",dropfiles.mid(beg,end-beg).text(),dropdirectory.text()));
//          if(!FXFile::moveFiles(filesrc,filedst)) getApp()->beep();
        while(dropfiles[end]=='\n' || dropfiles[end]=='\r') end++;
        }
      }
    }
  else if(dropaction==DRAG_COPY){
    FXCopyDialog dialog(this,tr("Copy Files"),tr("Copy these files:"),tr("To:"),big_folder);
    dialog.setSourceFiles(dropfiles);
    dialog.setDestinationFile(dropdirectory);
    if(dialog.execute()){
      }
    }
  else if(dropaction==DRAG_LINK){
    FXCopyDialog dialog(this,tr("Link Files"),tr("Link these files:"),tr("To:"),big_folder);
    dialog.setSourceFiles(dropfiles);
    dialog.setDestinationFile(dropdirectory);
    if(dialog.execute()){
      }
    }
#if 0
  FXString filesrc,filedst,url;
  FXint beg,end;

    // Loop over urls
    for(beg=0; beg<dropfiles.length(); beg=end+2){
      if((end=dropfiles.find_first_of("\r\n",beg))<0) end=dropfiles.length();

      // File url
      url=dropfiles.mid(beg,end-beg);

      // Source filename
      filesrc=FXURL::fileFromURL(url);

      // Destination filename
      filedst=dropdirectory+PATHSEPSTRING+FXPath::name(filesrc);

      // Move, Copy, or Link as appropriate
      switch(dropaction){
        case DRAG_MOVE:
          FXTRACE((100,"Moving file: %s to %s\n",filesrc.text(),filedst.text()));
          if(!FXFile::moveFiles(filesrc,filedst)) getApp()->beep();
          break;
        case DRAG_COPY:
          FXTRACE((100,"Copying file: %s to %s\n",filesrc.text(),filedst.text()));
          if(!FXFile::copyFiles(filesrc,filedst)) getApp()->beep();
          break;
        case DRAG_LINK:
          FXTRACE((100,"Linking file: %s to %s\n",filesrc.text(),filedst.text()));
          if(!FXFile::symlink(filesrc,filedst)) getApp()->beep();
          break;
        }
      }

    beg=end=0;
    while(end<dropfiles.length()){
      beg=end;
      while(end<dropfiles.length() && dropfiles[end]!='\n' && dropfiles[end]!='\r') end++;
       appendItem(dropfiles.mid(beg,end-beg),icon,ptr,notify);
      while(dropfiles[end]=='\n' || dropfiles[end]=='\r') end++;
      }

    }
  FXDeleteDialog dialog2(this,tr("Delete Files"),tr("Delete these files:"),big_folder);
  dialog2.setSourceFiles(dropfiles);
  dialog2.execute();
#endif

  // Forget drop data
  dropdirectory=FXString::null;
  dropfiles=FXString::null;
  dropaction=DRAG_REJECT;
  return 1;
  }
*/

/*******************************************************************************/

// Delete selection
long FXFileList::onCmdDeleteSel(FXObject*,FXSelector,void*){
  return 1;
  }


// Paste clipboard
long FXFileList::onCmdPasteSel(FXObject*,FXSelector,void*){
  return 1;
  }


// Cut
long FXFileList::onCmdCutSel(FXObject*,FXSelector,void*){
  if(acquireClipboard(&urilistType,1)){
    clipfiles=getSelectedFiles();
    }
  return 1;
  }


// Copy
long FXFileList::onCmdCopySel(FXObject*,FXSelector,void*){
  if(acquireClipboard(&urilistType,1)){
    clipfiles=getSelectedFiles();
    }
  return 1;
  }


// We lost the selection somehow
long FXFileList::onClipboardLost(FXObject* sender,FXSelector sel,void* ptr){
  FXIconList::onClipboardLost(sender,sel,ptr);
  clipfiles=FXString::null;
  return 1;
  }


// Somebody wants our selection
long FXFileList::onClipboardRequest(FXObject* sender,FXSelector sel,void* ptr){

  // Try base class first
  if(FXIconList::onClipboardRequest(sender,sel,ptr)) return 1;

  // Return list of filenames as a uri-list
  if(((FXEvent*)ptr)->target==urilistType){
    setDNDData(FROM_CLIPBOARD,((FXEvent*)ptr)->target,clipfiles);
    return 1;
    }

  return 0;
  }


/*******************************************************************************/

// Copy
long FXFileList::onCmdDropCopy(FXObject*,FXSelector,void*){
  FXString filedst,filesrc;
  FXint beg,end;
  for(beg=0; beg<dropfiles.length(); beg=end+2){
    if((end=dropfiles.find_first_of("\r\n",beg))<0) end=dropfiles.length();
    filesrc=FXURL::fileFromURL(dropfiles.mid(beg,end-beg));
    filedst=FXPath::absolute(dropdirectory,FXPath::name(filesrc));
    FXTRACE((100,"Copying file: %s to %s\n",filesrc.text(),filedst.text()));
    }
  return 1;
  }


// Copy
long FXFileList::onCmdDropMove(FXObject*,FXSelector,void*){
  FXTRACE((100,"Moving file: %s to %s\n",dropfiles.text(),dropdirectory.text()));
  return 1;
  }


// Copy
long FXFileList::onCmdDropLink(FXObject*,FXSelector,void*){
  FXTRACE((100,"Linking file: %s to %s\n",dropfiles.text(),dropdirectory.text()));
  return 1;
  }


// Deal with the drop that has just occurred
long FXFileList::onDropAction(FXObject*,FXSelector,void* ptr){
  FXMenuPane dropmenu(this);
  FXGIFIcon filemoveicon(getApp(),filemove);
  FXGIFIcon filecopyicon(getApp(),filecopy);
  FXGIFIcon filelinkicon(getApp(),filelink);
  FXGIFIcon filecancelicon(getApp(),filecancel);
  new FXMenuCommand(&dropmenu,tr("Move Here"),&filemoveicon,this,ID_DROPMOVE);
  new FXMenuCommand(&dropmenu,tr("Copy Here"),&filecopyicon,this,ID_DROPCOPY);
  new FXMenuCommand(&dropmenu,tr("Link Here"),&filelinkicon,this,ID_DROPLINK);
  new FXMenuSeparator(&dropmenu);
  new FXMenuCommand(&dropmenu,tr("Cancel"),&filecancelicon);
  dropmenu.create();
  dropmenu.popup(NULL,((FXEvent*)ptr)->root_x,((FXEvent*)ptr)->root_y);
  getApp()->runModalWhileShown(&dropmenu);
  dropdirectory=FXString::null;
  dropfiles=FXString::null;
  dropaction=DRAG_REJECT;
  return 1;
  }


// Change directory when hovering over a folder
long FXFileList::onOpenTimer(FXObject*,FXSelector,void*){
  FXint xx,yy,index; FXuint buttons;
  getCursorPosition(xx,yy,buttons);
  index=getItemAt(xx,yy);
  if(0<=index && isItemDirectory(index)){
    dropdirectory=getItemPathname(index);
    setDirectory(dropdirectory);
    getApp()->addTimeout(this,ID_OPENTIMER,OPENDIRDELAY);
    }
  return 1;
  }


// Handle drag-and-drop enter, remember current directory
long FXFileList::onDNDEnter(FXObject* sender,FXSelector sel,void* ptr){
  FXIconList::onDNDEnter(sender,sel,ptr);
  startdirectory=getDirectory();
  return 1;
  }


// Handle drag-and-drop leave, restore current directory prior to drag
long FXFileList::onDNDLeave(FXObject* sender,FXSelector sel,void* ptr){
  FXIconList::onDNDLeave(sender,sel,ptr);
  stopAutoScroll();
  getApp()->removeTimeout(this,ID_OPENTIMER);
  setDirectory(startdirectory);
  startdirectory.clear();
  dropdirectory.clear();
  dropaction=DRAG_REJECT;
  return 1;
  }


// Handle drag-and-drop motion
long FXFileList::onDNDMotion(FXObject* sender,FXSelector sel,void* ptr){
  FXEvent *event=(FXEvent*)ptr;
  FXint index=-1;

  // Cancel open up timer
  getApp()->removeTimeout(this,ID_OPENTIMER);

  // Start autoscrolling
  if(startAutoScroll(event,false)) return 1;

  // Give base class a shot
  if(FXIconList::onDNDMotion(sender,sel,ptr)) return 1;

  // Dropping list of filenames
  if(offeredDNDType(FROM_DRAGNDROP,urilistType)){

    // Drop in the background
    dropdirectory=getDirectory();

    // What is being done (move,copy,link)
    dropaction=inquireDNDAction();

    // We will open up a folder if we can hover over it for a while
    index=getItemAt(event->win_x,event->win_y);
    if(0<=index && isItemDirectory(index)){

      // Set open up timer
      getApp()->addTimeout(this,ID_OPENTIMER,OPENDIRDELAY);

      // Directory where to drop, or directory to open up
      dropdirectory=getItemPathname(index);
      }

    // See if dropdirectory is writable
    if(FXStat::isWritable(dropdirectory)){
      acceptDrop(DRAG_ACCEPT);
      }
    return 1;
    }
  return 0;
  }


// Handle drag-and-drop drop
long FXFileList::onDNDDrop(FXObject* sender,FXSelector sel,void* ptr){

  // Stop scrolling
  stopAutoScroll();

  // Cancel open up timer
  getApp()->removeTimeout(this,ID_OPENTIMER);

  // Restore start directory
  setDirectory(startdirectory);

  // Clear stuff
  startdirectory.clear();

  // Perhaps target wants to deal with it
  if(FXIconList::onDNDDrop(sender,sel,ptr)) return 1;

  // Get list of files as uri-list
  if(getDNDData(FROM_DRAGNDROP,urilistType,dropfiles)){
//    FXTRACE((1,"dropfiles=\n%s\n",dropfiles.text()));
//    FXTRACE((1,"droppedfile=%s\n",FXURL::fileFromURL(dropfiles).text()));
    dropFinished(DRAG_ACCEPT);
    switch(dropaction){
      case DRAG_COPY:
        break;
      case DRAG_MOVE:
        break;
      case DRAG_LINK:
        break;
      default:
        getApp()->addChore(this,ID_DROPACTION,ptr);
        break;
      }
    return 1;
    }
  return 0;
  }


// Somebody wants our dragged data
long FXFileList::onDNDRequest(FXObject* sender,FXSelector sel,void* ptr){

  // Perhaps the target wants to supply its own data
  if(FXIconList::onDNDRequest(sender,sel,ptr)) return 1;

  // Return list of filenames as a uri-list
  if(((FXEvent*)ptr)->target==urilistType){
    setDNDData(FROM_DRAGNDROP,((FXEvent*)ptr)->target,dragfiles);
    return 1;
    }

  // Delete selected files
  if(((FXEvent*)ptr)->target==deleteType){
    FXTRACE((100,"Delete files not yet implemented\n"));
    return 1;
    }

  return 0;
  }


// Start a drag operation
long FXFileList::onBeginDrag(FXObject* sender,FXSelector sel,void* ptr){
  if(!FXIconList::onBeginDrag(sender,sel,ptr)){
    beginDrag(&urilistType,1);
    dragfiles=getSelectedFiles();
    }
  return 1;
  }


// Dragged stuff around
long FXFileList::onDragged(FXObject* sender,FXSelector sel,void* ptr){
  FXEvent* event=(FXEvent*)ptr;
  if(!FXIconList::onDragged(sender,sel,ptr)){
    FXDragAction action=DRAG_ASK;
    if(event->state&CONTROLMASK) action=DRAG_COPY;
    if(event->state&SHIFTMASK) action=DRAG_MOVE;
    if(event->state&ALTMASK) action=DRAG_LINK;
    handleDrag(event->root_x,event->root_y,action);
    if(didAccept()){
      switch(action){
        case DRAG_COPY:
          setDragCursor(getApp()->getDefaultCursor(DEF_DNDCOPY_CURSOR));
          break;
        case DRAG_MOVE:
          setDragCursor(getApp()->getDefaultCursor(DEF_DNDMOVE_CURSOR));
          break;
        case DRAG_LINK:
          setDragCursor(getApp()->getDefaultCursor(DEF_DNDLINK_CURSOR));
          break;
        default:
          setDragCursor(getApp()->getDefaultCursor(DEF_DNDASK_CURSOR));
          break;
        }
      }
    else{
      setDragCursor(getApp()->getDefaultCursor(DEF_DNDSTOP_CURSOR));
      }
    }
  return 1;
  }


// End drag operation
long FXFileList::onEndDrag(FXObject* sender,FXSelector sel,void* ptr){
  if(!FXIconList::onEndDrag(sender,sel,ptr)){
    endDrag((didAccept()!=DRAG_REJECT));
    setDragCursor(getDefaultCursor());
    dragfiles=FXString::null;
    }
  return 1;
  }

/*******************************************************************************/

// Load preview icon given path
FXIcon* FXFileList::getItemPreviewIcon(FXint index) const {
  if(getAssociations() && getAssociations()->getIconDict() && getAssociations()->getIconDict()->getIconSource()){
    return getAssociations()->getIconDict()->getIconSource()->loadScaledIconFile(getItemPathname(index),imagesize);
    }
  return NULL;
  }


// Change directory when hovering over a folder
long FXFileList::onPreviewChore(FXObject*,FXSelector,void* ptr){
  register FXint index=(FXint)(FXival)ptr;
  register FXIcon *icon;
  if(index<getNumItems()){
    if((icon=getItemPreviewIcon(index))!=NULL){
      icon->create();
      setItemBigIcon(index,icon,true);
      }
   if(++index<getNumItems()){
     getApp()->addChore(this,ID_PREVIEWCHORE,(void*)(FXival)index);
     }
   }
  return 1;
  }

/*******************************************************************************/

// Set value from a message
long FXFileList::onCmdSetValue(FXObject*,FXSelector,void* ptr){
  setCurrentFile((const FXchar*)ptr);
  return 1;
  }


// Set current directory from dir part of filename
long FXFileList::onCmdSetStringValue(FXObject*,FXSelector,void* ptr){
  setCurrentFile(*((FXString*)ptr));
  return 1;
  }


// Get current file name (NULL if no current file)
long FXFileList::onCmdGetStringValue(FXObject*,FXSelector,void* ptr){
  *((FXString*)ptr)=getCurrentFile();
  return 1;
  }


// Toggle hidden files display
long FXFileList::onCmdToggleHidden(FXObject*,FXSelector,void*){
  showHiddenFiles(!showHiddenFiles());
  return 1;
  }


// Update toggle hidden files widget
long FXFileList::onUpdToggleHidden(FXObject* sender,FXSelector,void*){
  sender->handle(this,showHiddenFiles()?FXSEL(SEL_COMMAND,ID_CHECK):FXSEL(SEL_COMMAND,ID_UNCHECK),NULL);
  return 1;
  }


// Show hidden files
long FXFileList::onCmdShowHidden(FXObject*,FXSelector,void*){
  showHiddenFiles(true);
  return 1;
  }


// Update show hidden files widget
long FXFileList::onUpdShowHidden(FXObject* sender,FXSelector,void*){
  sender->handle(this,showHiddenFiles()?FXSEL(SEL_COMMAND,ID_CHECK):FXSEL(SEL_COMMAND,ID_UNCHECK),NULL);
  return 1;
  }


// Hide hidden files
long FXFileList::onCmdHideHidden(FXObject*,FXSelector,void*){
  showHiddenFiles(false);
  return 1;
  }


// Update hide hidden files widget
long FXFileList::onUpdHideHidden(FXObject* sender,FXSelector,void*){
  sender->handle(this,showHiddenFiles()?FXSEL(SEL_COMMAND,ID_UNCHECK):FXSEL(SEL_COMMAND,ID_CHECK),NULL);
  return 1;
  }


// Toggle image preview
long FXFileList::onCmdToggleImages(FXObject*,FXSelector,void*){
  showImages(!showImages());
  return 1;
  }


// Update image preview
long FXFileList::onUpdToggleImages(FXObject* sender,FXSelector,void*){
  sender->handle(this,showImages()?FXSEL(SEL_COMMAND,ID_CHECK):FXSEL(SEL_COMMAND,ID_UNCHECK),NULL);
  return 1;
  }


// Move up one level
long FXFileList::onCmdDirectoryUp(FXObject*,FXSelector,void*){
  setDirectory(FXPath::upLevel(directory));
  return 1;
  }


// Determine if we can still go up more
long FXFileList::onUpdDirectoryUp(FXObject* sender,FXSelector,void*){
  sender->handle(this,FXPath::isTopDirectory(directory)?FXSEL(SEL_COMMAND,ID_DISABLE):FXSEL(SEL_COMMAND,ID_ENABLE),NULL);
  return 1;
  }


// Change pattern
long FXFileList::onCmdSetPattern(FXObject*,FXSelector,void* ptr){
  setPattern((const char*)ptr);
  return 1;
  }


// Update pattern
long FXFileList::onUpdSetPattern(FXObject* sender,FXSelector,void*){
  sender->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_SETVALUE),(void*)pattern.text());
  return 1;
  }


// Change directory
long FXFileList::onCmdSetDirectory(FXObject*,FXSelector,void* ptr){
  setDirectory((const char*)ptr);
  return 1;
  }


// Update directory
long FXFileList::onUpdSetDirectory(FXObject* sender,FXSelector,void*){
  sender->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_SETVALUE),(void*)directory.text());
  return 1;
  }


// Sort by name
long FXFileList::onCmdSortByName(FXObject*,FXSelector,void*){
#ifdef WIN32
  if(sortfunc==ascending) sortfunc=descending;
  else if(sortfunc==ascendingCase) sortfunc=descendingCase;
  else if(sortfunc==descending) sortfunc=ascending;
  else sortfunc=ascendingCase;
#else
  if(sortfunc==ascending) sortfunc=descending;
  else if(sortfunc==ascendingCase) sortfunc=descendingCase;
  else if(sortfunc==descending) sortfunc=ascending;
  else sortfunc=ascending;
#endif
  scan(true);
  return 1;
  }


// Update sender
long FXFileList::onUpdSortByName(FXObject* sender,FXSelector,void*){
  sender->handle(this,(sortfunc==ascending || sortfunc==descending || sortfunc==ascendingCase || sortfunc==descendingCase) ? FXSEL(SEL_COMMAND,ID_CHECK) : FXSEL(SEL_COMMAND,ID_UNCHECK),NULL);
  return 1;
  }


// Sort by type
long FXFileList::onCmdSortByType(FXObject*,FXSelector,void*){
  sortfunc=(sortfunc==ascendingType) ? descendingType : ascendingType;
  scan(true);
  return 1;
  }


// Update sender
long FXFileList::onUpdSortByType(FXObject* sender,FXSelector,void*){
  sender->handle(this,(sortfunc==ascendingType || sortfunc==descendingType) ? FXSEL(SEL_COMMAND,ID_CHECK) : FXSEL(SEL_COMMAND,ID_UNCHECK),NULL);
  return 1;
  }


// Sort by size
long FXFileList::onCmdSortBySize(FXObject*,FXSelector,void*){
  sortfunc=(sortfunc==ascendingSize) ? descendingSize : ascendingSize;
  scan(true);
  return 1;
  }


// Update sender
long FXFileList::onUpdSortBySize(FXObject* sender,FXSelector,void*){
  sender->handle(this,(sortfunc==ascendingSize || sortfunc==descendingSize) ? FXSEL(SEL_COMMAND,ID_CHECK) : FXSEL(SEL_COMMAND,ID_UNCHECK),NULL);
  return 1;
  }


// Sort by time
long FXFileList::onCmdSortByTime(FXObject*,FXSelector,void*){
  sortfunc=(sortfunc==ascendingTime) ? descendingTime : ascendingTime;
  scan(true);
  return 1;
  }


// Update sender
long FXFileList::onUpdSortByTime(FXObject* sender,FXSelector,void*){
  sender->handle(this,(sortfunc==ascendingTime || sortfunc==descendingTime) ? FXSEL(SEL_COMMAND,ID_CHECK) : FXSEL(SEL_COMMAND,ID_UNCHECK),NULL);
  return 1;
  }


// Sort by user
long FXFileList::onCmdSortByUser(FXObject*,FXSelector,void*){
  sortfunc=(sortfunc==ascendingUser) ? descendingUser : ascendingUser;
  scan(true);
  return 1;
  }


// Update sender
long FXFileList::onUpdSortByUser(FXObject* sender,FXSelector,void*){
  sender->handle(this,(sortfunc==ascendingUser || sortfunc==descendingUser) ? FXSEL(SEL_COMMAND,ID_CHECK) : FXSEL(SEL_COMMAND,ID_UNCHECK),NULL);
  return 1;
  }


// Sort by group
long FXFileList::onCmdSortByGroup(FXObject*,FXSelector,void*){
  sortfunc=(sortfunc==ascendingGroup) ? descendingGroup : ascendingGroup;
  scan(true);
  return 1;
  }


// Update sender
long FXFileList::onUpdSortByGroup(FXObject* sender,FXSelector,void*){
  sender->handle(this,(sortfunc==ascendingGroup || sortfunc==descendingGroup) ? FXSEL(SEL_COMMAND,ID_CHECK) : FXSEL(SEL_COMMAND,ID_UNCHECK),NULL);
  return 1;
  }


// Reverse sort order
long FXFileList::onCmdSortReverse(FXObject*,FXSelector,void*){
  if(sortfunc==ascending) sortfunc=descending;
  else if(sortfunc==descending) sortfunc=ascending;
  else if(sortfunc==ascendingCase) sortfunc=descendingCase;
  else if(sortfunc==descendingCase) sortfunc=ascendingCase;
  else if(sortfunc==ascendingType) sortfunc=descendingType;
  else if(sortfunc==descendingType) sortfunc=ascendingType;
  else if(sortfunc==ascendingSize) sortfunc=descendingSize;
  else if(sortfunc==descendingSize) sortfunc=ascendingSize;
  else if(sortfunc==ascendingTime) sortfunc=descendingTime;
  else if(sortfunc==descendingTime) sortfunc=ascendingTime;
  else if(sortfunc==ascendingUser) sortfunc=descendingUser;
  else if(sortfunc==descendingUser) sortfunc=ascendingUser;
  else if(sortfunc==ascendingGroup) sortfunc=descendingGroup;
  else if(sortfunc==descendingGroup) sortfunc=ascendingGroup;
  scan(true);
  return 1;
  }


// Update sender
long FXFileList::onUpdSortReverse(FXObject* sender,FXSelector,void*){
  FXSelector selector=FXSEL(SEL_COMMAND,ID_UNCHECK);
  if(sortfunc==descending) selector=FXSEL(SEL_COMMAND,ID_CHECK);
  else if(sortfunc==descendingCase) selector=FXSEL(SEL_COMMAND,ID_CHECK);
  else if(sortfunc==descendingType) selector=FXSEL(SEL_COMMAND,ID_CHECK);
  else if(sortfunc==descendingSize) selector=FXSEL(SEL_COMMAND,ID_CHECK);
  else if(sortfunc==descendingTime) selector=FXSEL(SEL_COMMAND,ID_CHECK);
  else if(sortfunc==descendingUser) selector=FXSEL(SEL_COMMAND,ID_CHECK);
  else if(sortfunc==descendingGroup) selector=FXSEL(SEL_COMMAND,ID_CHECK);
  sender->handle(this,selector,NULL);
  return 1;
  }


// Toggle case sensitivity
long FXFileList::onCmdSortCase(FXObject*,FXSelector,void*){
  if(sortfunc==ascending) sortfunc=ascendingCase;
  else if(sortfunc==ascendingCase) sortfunc=ascending;
  else if(sortfunc==descending) sortfunc=descendingCase;
  else if(sortfunc==descendingCase) sortfunc=descending;
  scan(true);
  return 1;
  }


// Check if case sensitive
long FXFileList::onUpdSortCase(FXObject* sender,FXSelector,void*){
  sender->handle(this,(sortfunc==ascendingCase || sortfunc==descendingCase) ? FXSEL(SEL_COMMAND,ID_CHECK) : FXSEL(SEL_COMMAND,ID_UNCHECK),NULL);
  sender->handle(this,(sortfunc==ascendingCase || sortfunc==ascending || sortfunc==descendingCase || sortfunc==descending) ? FXSEL(SEL_COMMAND,ID_ENABLE) : FXSEL(SEL_COMMAND,ID_DISABLE),NULL);
  return 1;
  }


// Clicked header button
long FXFileList::onCmdHeader(FXObject*,FXSelector,void* ptr){
  if(((FXuint)(FXuval)ptr)<6) handle(this,FXSEL(SEL_COMMAND,(ID_SORT_BY_NAME+(FXuint)(FXuval)ptr)),NULL);
  return 1;
  }


// Clicked header button
long FXFileList::onUpdHeader(FXObject*,FXSelector,void*){
  header->setArrowDir(0,(sortfunc==ascending || sortfunc==ascendingCase)  ? FXHeaderItem::ARROW_DOWN : (sortfunc==descending || sortfunc==descendingCase) ? FXHeaderItem::ARROW_UP : FXHeaderItem::ARROW_NONE);   // Name
  header->setArrowDir(1,(sortfunc==ascendingType)  ? FXHeaderItem::ARROW_DOWN : (sortfunc==descendingType) ? FXHeaderItem::ARROW_UP : FXHeaderItem::ARROW_NONE);   // Type
  header->setArrowDir(2,(sortfunc==ascendingSize)  ? FXHeaderItem::ARROW_DOWN : (sortfunc==descendingSize) ? FXHeaderItem::ARROW_UP : FXHeaderItem::ARROW_NONE);   // Size
  header->setArrowDir(3,(sortfunc==ascendingTime)  ? FXHeaderItem::ARROW_DOWN : (sortfunc==descendingTime) ? FXHeaderItem::ARROW_UP : FXHeaderItem::ARROW_NONE);   // Date
  header->setArrowDir(4,(sortfunc==ascendingUser)  ? FXHeaderItem::ARROW_DOWN : (sortfunc==descendingUser) ? FXHeaderItem::ARROW_UP : FXHeaderItem::ARROW_NONE);   // User
  header->setArrowDir(5,(sortfunc==ascendingGroup) ? FXHeaderItem::ARROW_DOWN : (sortfunc==descendingGroup)? FXHeaderItem::ARROW_UP : FXHeaderItem::ARROW_NONE);   // Group
  return 1;
  }

/*******************************************************************************/

// Refresh; don't update if user is interacting with the list
long FXFileList::onRefreshTimer(FXObject*,FXSelector,void*){
  if(flags&FLAG_UPDATE){
    counter=(counter+1)%REFRESHFREQUENCY;
    scan(false);
    }
  getApp()->addTimeout(this,ID_REFRESHTIMER,REFRESHINTERVAL);
  return 0;
  }


// Force an immediate update of the list
long FXFileList::onCmdRefresh(FXObject*,FXSelector,void*){
  scan(true);
  return 1;
  }


// Scan items to see if listing is necessary
void FXFileList::scan(FXbool force){
  FXStat info;

  // Stat the current directory
  if(FXStat::statFile(directory,info)){

    // New date of directory
    FXTime newdate=info.modified();

    // Forced, date was changed, or failed to get proper date and counter expired
    if(force || (timestamp!=newdate) || (counter==0)){

      // And do the refresh
      listItems(force);
      sortItems();

      // Remember when we did this
      timestamp=newdate;
      }
    }

  // Move to higher directory
  else{
    setDirectory(FXPath::upLevel(directory));
    }
  }


// Set current filename
void FXFileList::setCurrentFile(const FXString& pathname,FXbool notify){
  FXTRACE((100,"%s::setCurrentFile(%s)\n",getClassName(),pathname.text()));
  if(!pathname.empty()){
    setDirectory(FXPath::directory(pathname));
    FXint index=findItem(FXPath::name(pathname));
    makeItemVisible(index);
    setAnchorItem(index);
    setCurrentItem(index,notify);
    if(0<=index){
      selectItem(index);
      }
    }
  }


// Get pathname to current file, if any
FXString FXFileList::getCurrentFile() const {
  if(current<0) return FXString::null;
  return getItemPathname(current);
  }


// Set directory being displayed
void FXFileList::setDirectory(const FXString& pathname){
  if(!pathname.empty()){
    FXTRACE((100,"%s::setDirectory(%s)\n",getClassName(),pathname.text()));
    FXString path=FXPath::absolute(directory,pathname);
    while(!FXPath::isTopDirectory(path) && !(FXPath::isShare(path) || FXStat::isDirectory(path))){
      path=FXPath::upLevel(path);
      }
    if(directory!=path){
      directory=path;
      clearItems();
      counter=0;
      list=NULL;
      scan(true);
      }
    }
  }


// Set the pattern to filter
void FXFileList::setPattern(const FXString& ptrn){
  if(ptrn.empty()) return;
  if(pattern!=ptrn){
    pattern=ptrn;
    scan(true);
    }
  }


// Change file match mode
void FXFileList::setMatchMode(FXuint mode){
  if(matchmode!=mode){
    matchmode=mode;
    scan(true);
    }
  }


// Return true if showing hidden files
FXbool FXFileList::showHiddenFiles() const {
  return (options&FILELIST_SHOWHIDDEN)!=0;
  }


// Change show hidden files mode
void FXFileList::showHiddenFiles(FXbool flag){
  FXuint opts=((-(FXint)flag^options)&FILELIST_SHOWHIDDEN)^options;
  if(opts!=options){
    options=opts;
    scan(true);
    }
  }


// Return true if displaying image
FXbool FXFileList::showImages() const {
  return (options&FILELIST_SHOWIMAGES)!=0;
  }


// Change show image display mode
void FXFileList::showImages(FXbool flag){
  FXuint opts=((-(FXint)flag^options)&FILELIST_SHOWIMAGES)^options;
  if(opts!=options){
    options=opts;
    scan(true);
    }
  }


// Change images preview size
void FXFileList::setImageSize(FXint size){
  if(size!=imagesize){
    imagesize=size;
    scan(true);
    }
  }


// Return true if showing directories only
FXbool FXFileList::showOnlyDirectories() const {
  return (options&FILELIST_SHOWDIRS)!=0;
  }


// Change show directories only mode
void FXFileList::showOnlyDirectories(FXbool flag){
  FXuint opts=(((0-flag)^options)&FILELIST_SHOWDIRS)^options;
  if(opts!=options){
    options=opts;
    scan(true);
    }
  }


// Return true if showing files only
FXbool FXFileList::showOnlyFiles() const {
  return (options&FILELIST_SHOWFILES)!=0;
  }


// Show files only
void FXFileList::showOnlyFiles(FXbool flag){
  FXuint opts=(((0-flag)^options)&FILELIST_SHOWFILES)^options;
  if(opts!=options){
    options=opts;
    scan(true);
    }
  }


// Return true if showing parent directories
FXbool FXFileList::showParents() const {
  return (options&FILELIST_NO_PARENT)==0;
  }


// Show parent directories
void FXFileList::showParents(FXbool flag) {
  FXuint opts=(((flag-1)^options)&FILELIST_NO_PARENT)^options;
  if(opts!=options){
    options=opts;
    scan(true);
    }
  }


// Compare till '\t' or '\0'
static FXbool fileequal(const FXString& a,const FXString& b){
  register const FXuchar *p1=(const FXuchar *)a.text();
  register const FXuchar *p2=(const FXuchar *)b.text();
  register FXint c1,c2;
  do{
    c1=*p1++;
    c2=*p2++;
    }
  while(c1!='\0' && c1!='\t' && c1==c2);
  return (c1=='\0' || c1=='\t') && (c2=='\0' || c2=='\t');
  }


// List directory
void FXFileList::listItems(FXbool force){
  FXFileItem *oldlist=list;     // Old insert-order list
  FXFileItem *newlist=NULL;     // New insert-order list
  FXFileItem **po=&oldlist;     // Head of old list
  FXFileItem **pn=&newlist;     // Head of new list
  FXFileItem *curitem=NULL;
  FXFileItem *item,*link,**pp;
  FXString pathname;
  FXString extension;
  FXString name;
  FXString grpid;
  FXString usrid;
  FXString atts;
  FXString mod;
  FXString linkname;
  FXbool islink;
  FXbool istop;
  FXStat info;
  FXDir  dir;

  // Remember current item
  if(0<=current){ curitem=(FXFileItem*)items[current]; }

  // Start inserting
  items.clear();

  // Are we at the top directory?
  istop=FXPath::isTopDirectory(directory);

  // Assume not a link
  islink=false;

  // Get directory stream pointer
  if(dir.open(directory)){

    // Loop over directory entries
    while(dir.next()){

      // Get file name
      name=dir.name();

      // Hidden files of the form ".xxx" are normally not shown, but ".." is so we can
      // navigate up as well as down.  However, at the root level we can't go up any
      // further so we show "." but not ".."; this allows us to explicitly select "/."
      // as a directory when we're in directory selection mode.
      if(name[0]=='.'){
        if(name[1]==0){
          if((options&FILELIST_NO_PARENT) || !istop) continue;
          }
        else if(name[1]=='.' && name[2]==0){
          if((options&FILELIST_NO_PARENT) || istop) continue;
          }
        else{
          if(!(options&FILELIST_SHOWHIDDEN)) continue;
          }
        }

      // Build full pathname
      pathname=directory;
      if(!ISPATHSEP(pathname[pathname.length()-1])) pathname+=PATHSEPSTRING;
      pathname+=name;

#ifdef WIN32

      // Get file/link info
      if(!FXStat::statFile(pathname,info)) continue;

      // Hidden file or directory normally not shown
      if(info.isHidden() && !(options&FILELIST_SHOWHIDDEN)) continue;

#else

      // Get file/link info
      if(!FXStat::statLink(pathname,info)) continue;

      // If its a link, get the info on file itself
      islink=info.isLink();
      if(islink && !FXStat::statFile(pathname,info)) continue;

#endif

      // If it is a file and we want only directories or doesn't match, skip it
      if(!info.isDirectory() && ((options&FILELIST_SHOWDIRS) || !FXPath::match(pattern,name,matchmode))) continue;

      // If it is a directory and we want only files, skip it
      if(info.isDirectory() && (options&FILELIST_SHOWFILES)) continue;


      // Find it, and take it out from the old list if found
      for(pp=po; (item=*pp)!=NULL; pp=&item->link){
        if(fileequal(item->label,name)){
          *pp=item->link;
          item->link=NULL;
          po=pp;
          goto fnd;
          }
        }

      // Make new item if we have to
      item=(FXFileItem*)createItem(FXString::null,NULL,NULL,NULL);

      // Add to insert-order list
fnd:  *pn=item;
      pn=&item->link;

      // Append
      if(item==curitem) current=items.no();
      items.append(item);

      // Update only if something changed
      if(force || item->label.empty() || item->date!=info.modified() || item->size!=info.size()){

        // Obtain user name
        usrid=FXSystem::userName(info.user());

        // Obtain group name
        grpid=FXSystem::groupName(info.group());

        // Permissions
        atts=FXSystem::modeString(info.mode());

        // Mod time
        mod=FXSystem::localTime(info.modified());

        // Link
        if(islink) linkname=FXFile::symlink(pathname); else linkname=FXString::null;

        // Update flags
        if(info.isExecutable()){item->state|=FXFileItem::EXECUTABLE;}else{item->state&=~FXFileItem::EXECUTABLE;}
        if(info.isDirectory()){item->state|=FXFileItem::FOLDER;item->state&=~FXFileItem::EXECUTABLE;}else{item->state&=~FXFileItem::FOLDER;}
        if(info.isCharacter()){item->state|=FXFileItem::CHARDEV;item->state&=~FXFileItem::EXECUTABLE;}else{item->state&=~FXFileItem::CHARDEV;}
        if(info.isBlock()){item->state|=FXFileItem::BLOCKDEV;item->state&=~FXFileItem::EXECUTABLE;}else{item->state&=~FXFileItem::BLOCKDEV;}
        if(info.isFifo()){item->state|=FXFileItem::FIFO;item->state&=~FXFileItem::EXECUTABLE;}else{item->state&=~FXFileItem::FIFO;}
        if(info.isSocket()){item->state|=FXFileItem::SOCK;item->state&=~FXFileItem::EXECUTABLE;}else{item->state&=~FXFileItem::SOCK;}
        if(islink){item->state|=FXFileItem::SYMLINK;}else{item->state&=~FXFileItem::SYMLINK;}

        // We can drag items only if allowed
        item->setDraggable(draggable);

        // File size
        item->size=info.size();

        // File access time
        item->date=info.modified();

        // No associations
        item->assoc=NULL;

        // Determine icons and type
        if(item->isDirectory()){
          extension="File Folder";
          item->setBigIcon(big_folder);
          item->setMiniIcon(mini_folder);
          if(associations) item->assoc=associations->findDirBinding(pathname.text());
          }
        else if(item->isExecutable()){
          extension="Application";
          item->setBigIcon(big_app);
          item->setMiniIcon(mini_app);
          if(associations) item->assoc=associations->findExecBinding(pathname.text());
          }
        else{
          extension=FXPath::extension(pathname).upper();
          if(!extension.empty()) extension+=" File";
          item->setBigIcon(big_doc);
          item->setMiniIcon(mini_doc);
          if(associations) item->assoc=associations->findFileBinding(pathname.text());
          }

        // If association is found, use it
        if(item->assoc){
          extension=item->assoc->extension;
          if(item->assoc->bigicon) item->setBigIcon(item->assoc->bigicon);
          if(item->assoc->miniicon) item->setMiniIcon(item->assoc->miniicon);
          }

        // Update item information
        item->label.format("%s\t%s\t%lld\t%s\t%s\t%s\t%s\t%s",name.text(),extension.text(),item->size,mod.text(),usrid.text(),grpid.text(),atts.text(),linkname.text());

        // Create item
        if(id()) item->create();
        }
      }
    dir.close();
    }


  // Wipe items remaining in list:- they have disappeared!!
  for(item=oldlist; item; item=link){
    link=item->link;
    delete item;
    }

  // Validate
  if(current>=items.no()) current=-1;
  if(anchor>=items.no()) anchor=-1;
  if(extent>=items.no()) extent=-1;

  // Remember new list
  list=newlist;

  if(options&FILELIST_SHOWIMAGES){
    getApp()->addChore(this,ID_PREVIEWCHORE,(void*)(FXival)0);
    }

  // Gotta recalc size of content
  recalc();
  }


/*******************************************************************************/


// Is directory
FXbool FXFileList::isItemDirectory(FXint index) const {
  if(index<0 || items.no()<=index){ fxerror("%s::isItemDirectory: index out of range.\n",getClassName()); }
  return ((FXFileItem*)items[index])->isDirectory();
  }


// Is share
FXbool FXFileList::isItemShare(FXint index) const {
  if(index<0 || items.no()<=index){ fxerror("%s::isItemShare: index out of range.\n",getClassName()); }
  return ((FXFileItem*)items[index])->isShare();
  }


// Is file
FXbool FXFileList::isItemFile(FXint index) const {
  if(index<0 || items.no()<=index){ fxerror("%s::isItemFile: index out of range.\n",getClassName()); }
  return ((FXFileItem*)items[index])->isFile();
  }


// Is executable
FXbool FXFileList::isItemExecutable(FXint index) const {
  if(index<0 || items.no()<=index){ fxerror("%s::isItemExecutable: index out of range.\n",getClassName()); }
  return ((FXFileItem*)items[index])->isExecutable();
  }


// Return true if item is navigational item like '.' or '..'
FXbool FXFileList::isItemNavigational(FXint index) const {
  if(index<0 || items.no()<=index){ fxerror("%s::isItemNavigational: index out of range.\n",getClassName()); }
  return ((FXFileItem*)items[index])->isNavigational();
  }


// Get file name from item
FXString FXFileList::getItemFilename(FXint index) const {
  if(index<0 || items.no()<=index){ fxerror("%s::getItemFilename: index out of range.\n",getClassName()); }
  return items[index]->label.section('\t',0);
  }


// Get full pathname to item
FXString FXFileList::getItemPathname(FXint index) const {
  if(index<0 || items.no()<=index){ fxerror("%s::getItemPathname: index out of range.\n",getClassName()); }
  return FXPath::absolute(directory,items[index]->label.section('\t',0));
  }


// Get associations (if any) from the file
FXFileAssoc* FXFileList::getItemAssoc(FXint index) const {
  if(index<0 || items.no()<=index){ fxerror("%s::getItemAssoc: index out of range.\n",getClassName()); }
  return ((FXFileItem*)items[index])->getAssoc();
  }


// Change file associations; delete the old one unless it was shared
void FXFileList::setAssociations(FXFileDict* assocs,FXbool owned){
  if(associations!=assocs){
    if(!(options&FILELIST_NO_OWN_ASSOC)) delete associations;
    associations=assocs;
    scan(true);
    }
  options^=((owned-1)^options)&FILELIST_NO_OWN_ASSOC;
  }


// Set draggable files
void FXFileList::setDraggableFiles(FXbool flg){
  if(draggable!=flg){
    draggable=flg;
    scan(true);
    }
  }


// Save data
void FXFileList::save(FXStream& store) const {
  FXIconList::save(store);
  store << associations;
  store << big_folder;
  store << mini_folder;
  store << big_doc;
  store << mini_doc;
  store << big_app;
  store << mini_app;
  store << directory;
  store << pattern;
  store << matchmode;
  store << imagesize;
  store << draggable;
  }


// Load data
void FXFileList::load(FXStream& store){
  FXIconList::load(store);
  store >> associations;
  store >> big_folder;
  store >> mini_folder;
  store >> big_doc;
  store >> mini_doc;
  store >> big_app;
  store >> mini_app;
  store >> directory;
  store >> pattern;
  store >> matchmode;
  store >> imagesize;
  store >> draggable;
  }


// Cleanup
FXFileList::~FXFileList(){
  getApp()->removeChore(this,ID_DROPACTION);
  getApp()->removeChore(this,ID_PREVIEWCHORE);
  getApp()->removeTimeout(this,ID_OPENTIMER);
  getApp()->removeTimeout(this,ID_REFRESHTIMER);
  if(!(options&FILELIST_NO_OWN_ASSOC)) delete associations;
  delete big_folder;
  delete mini_folder;
  delete big_doc;
  delete mini_doc;
  delete big_app;
  delete mini_app;
  associations=(FXFileDict*)-1L;
  big_folder=(FXIcon*)-1L;
  mini_folder=(FXIcon*)-1L;
  big_doc=(FXIcon*)-1L;
  mini_doc=(FXIcon*)-1L;
  big_app=(FXIcon*)-1L;
  mini_app=(FXIcon*)-1L;
  list=(FXFileItem*)-1L;
  }

}

