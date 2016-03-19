/********************************************************************************
*                                                                               *
*                     T h e   A d i e   T e x t   E d i t o r                   *
*                                                                               *
*********************************************************************************
* Copyright (C) 1998,2011 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include <new>
#include <signal.h>
#include "HelpWindow.h"
#include "Preferences.h"
#include "Commands.h"
#include "Syntax.h"
#include "SyntaxParser.h"
#include "TextWindow.h"
#include "Adie.h"
#include "icons.h"

/*
  Notes:
  - One single collection of icons.
  - Manage list of open windows.
*/


/*******************************************************************************/


// Map
FXDEFMAP(Adie) AdieMap[]={
  FXMAPFUNC(SEL_SIGNAL,Adie::ID_CLOSEALL,Adie::onCmdCloseAll),
  FXMAPFUNC(SEL_COMMAND,Adie::ID_CLOSEALL,Adie::onCmdCloseAll),
  };


// Object implementation
FXIMPLEMENT(Adie,FXApp,AdieMap,ARRAYNUMBER(AdieMap))


// Make some windows
Adie::Adie(const FXString& name):FXApp(name){

  // Make some icons; these are shared between all text windows
  bigicon=new FXGIFIcon(this,big_gif);
  smallicon=new FXGIFIcon(this,small_gif);
  newicon=new FXGIFIcon(this,new_gif,0,IMAGE_ALPHAGUESS);
  reloadicon=new FXGIFIcon(this,reload_gif);
  openicon=new FXGIFIcon(this,open_gif);
  saveicon=new FXGIFIcon(this,save_gif);
  saveasicon=new FXGIFIcon(this,saveas_gif,0,IMAGE_ALPHAGUESS);
  printicon=new FXGIFIcon(this,print_gif);
  cuticon=new FXGIFIcon(this,cut_gif);
  copyicon=new FXGIFIcon(this,copy_gif);
  pasteicon=new FXGIFIcon(this,paste_gif);
  deleteicon=new FXGIFIcon(this,delete_gif);
  undoicon=new FXGIFIcon(this,undo_gif);
  redoicon=new FXGIFIcon(this,redo_gif);
  fontsicon=new FXGIFIcon(this,fonts_gif);
  helpicon=new FXGIFIcon(this,help_gif);
  quiticon=new FXGIFIcon(this,quit_gif);
  shiftlefticon=new FXGIFIcon(this,shiftleft_gif);
  shiftrighticon=new FXGIFIcon(this,shiftright_gif);
  configicon=new FXGIFIcon(this,config_gif);
  browsericon=new FXGIFIcon(this,browser);
  nobrowsericon=new FXGIFIcon(this,nobrowser);
  uppercaseicon=new FXGIFIcon(this,uppercase);
  lowercaseicon=new FXGIFIcon(this,lowercase);

  searchicon=new FXGIFIcon(this,search_gif,0,IMAGE_ALPHAGUESS);
  searchnexticon=new FXGIFIcon(this,searchnext_gif,0,IMAGE_ALPHAGUESS);
  searchprevicon=new FXGIFIcon(this,searchprev_gif,0,IMAGE_ALPHAGUESS);
  bookseticon=new FXGIFIcon(this,bookset_gif);
  booknexticon=new FXGIFIcon(this,booknext_gif);
  bookprevicon=new FXGIFIcon(this,bookprev_gif);
  bookdelicon=new FXGIFIcon(this,bookdel_gif);

#ifndef DEBUG
  // If interrupt happens, quit gracefully; we may want to
  // save edit buffer contents w/o asking if display gets
  // disconnected or if hangup signal is received.
  addSignal(SIGINT,this,ID_CLOSEALL);
#ifndef WIN32
  addSignal(SIGQUIT,this,ID_CLOSEALL);
  addSignal(SIGHUP,this,ID_CLOSEALL);
  addSignal(SIGPIPE,this,ID_CLOSEALL);
#endif
#endif

  // File associations, shared between all windows
  associations=new FXFileDict(this);
  }


// Initialize application
void Adie::init(int& argc,char** argv,FXbool connect){
  FXString execpath,iconpath,syntaxfile;

  // After init, the registry has been loaded
  FXApp::init(argc,argv,connect);

  // Exec path is default for syntax path
  execpath=FXSystem::getExecPath();

  // Get icon search path
  iconpath=reg().readStringEntry("SETTINGS","iconpath",FXIconDict::defaultIconPath);

  // Change icon search path
  associations->setIconPath(iconpath);

  // Get syntax path
  syntaxpath=reg().readStringEntry("SETTINGS","syntaxpath",execpath.text());

  // Hunt for the syntax file
  syntaxfile=FXPath::search(syntaxpath,"Adie.stx");

  // Load syntax file
  if(!syntaxfile.empty()){
    SyntaxParser::parseFile(syntaxfile,syntaxes);
    }
  }


// Get syntax for language name
Syntax* Adie::getSyntaxByName(const FXString& lang){
  FXTRACE((10,"Adie::getSyntaxByName(%s)\n",lang.text()));
  if(!lang.empty()){
    for(FXint syn=0; syn<syntaxes.no(); syn++){
      if(syntaxes[syn]->getName()==lang){
        FXTRACE((10,"syntaxes[%d]: language: %s matched name: %s!\n",syn,syntaxes[syn]->getName().text(),lang.text()));
        return syntaxes[syn];
        }
      }
    }
  return NULL;
  }


// Get syntax by matching file patterns
Syntax* Adie::getSyntaxByPattern(const FXString& file){
  FXTRACE((10,"Adie::getSyntaxByPattern(%s)\n",file.text()));
  if(!file.empty()){
    for(FXint syn=0; syn<syntaxes.no(); syn++){
      if(syntaxes[syn]->matchFilename(file)){
        FXTRACE((10,"syntaxes[%d]: language: %s matched file: %s!\n",syn,syntaxes[syn]->getName().text(),file.text()));
        return syntaxes[syn];
        }
      }
    }
  return NULL;
  }


// Get syntax by matching file contents
Syntax* Adie::getSyntaxByContents(const FXString& contents){
  FXTRACE((10,"Adie::getSyntaxByContents(%s)\n",contents.text()));
  if(!contents.empty()){
    for(FXint syn=0; syn<syntaxes.no(); syn++){
      if(syntaxes[syn]->matchContents(contents)){
        FXTRACE((10,"syntaxes[%d]: language: %s matched contents: %s!\n",syn,syntaxes[syn]->getName().text(),contents.text()));
        return syntaxes[syn];
        }
      }
    }
  return NULL;
  }


// Exit application
void Adie::exit(FXint code){

  // Save syntax paths
  reg().writeStringEntry("SETTINGS","syntaxpath",syntaxpath.text());

  // Writes registry, and quits
  FXApp::exit(code);
  }


// Close all windows
long Adie::onCmdCloseAll(FXObject*,FXSelector,void*){
  while(0<windowlist.no() && windowlist[0]->close(true)){}
  return 1;
  }


// Clean up the mess
Adie::~Adie(){
  for(int i=0; i<syntaxes.no(); i++) delete syntaxes[i];
  FXASSERT(windowlist.no()==0);
  delete associations;
  delete bigicon;
  delete smallicon;
  delete newicon;
  delete reloadicon;
  delete openicon;
  delete saveicon;
  delete saveasicon;
  delete printicon;
  delete cuticon;
  delete copyicon;
  delete pasteicon;
  delete deleteicon;
  delete undoicon;
  delete redoicon;
  delete fontsicon;
  delete helpicon;
  delete quiticon;
  delete shiftlefticon;
  delete shiftrighticon;
  delete configicon;
  delete browsericon;
  delete nobrowsericon;
  delete uppercaseicon;
  delete lowercaseicon;
  delete searchicon;
  delete searchnexticon;
  delete searchprevicon;
  delete bookseticon;
  delete booknexticon;
  delete bookprevicon;
  delete bookdelicon;
  }

