/********************************************************************************
*                                                                               *
*                     T h e   A d i e   T e x t   E d i t o r                   *
*                                                                               *
*********************************************************************************
* Copyright (C) 1998,2014 by Jeroen van der Zijp.   All Rights Reserved.        *
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
  FXMAPFUNC(SEL_COMMAND,Adie::ID_SYNTAXPATHS,Adie::onCmdSyntaxPaths),
  FXMAPFUNC(SEL_UPDATE,Adie::ID_SYNTAXPATHS,Adie::onUpdSyntaxPaths),
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
  searchicon=new FXGIFIcon(this,search_gif,0,IMAGE_ALPHAGUESS);
  searchnexticon=new FXGIFIcon(this,searchnext_gif,0,IMAGE_ALPHAGUESS);
  searchprevicon=new FXGIFIcon(this,searchprev_gif,0,IMAGE_ALPHAGUESS);
  bookseticon=new FXGIFIcon(this,bookset_gif);
  booknexticon=new FXGIFIcon(this,booknext_gif);
  bookprevicon=new FXGIFIcon(this,bookprev_gif);
  bookdelicon=new FXGIFIcon(this,bookdel_gif);
  shiftlefticon=new FXGIFIcon(this,shiftleft_gif);
  shiftrighticon=new FXGIFIcon(this,shiftright_gif);
  configicon=new FXGIFIcon(this,config_gif);
  browsericon=new FXGIFIcon(this,browser);
  nobrowsericon=new FXGIFIcon(this,nobrowser);
  uppercaseicon=new FXGIFIcon(this,uppercase);
  lowercaseicon=new FXGIFIcon(this,lowercase);
  backwardicon=new FXGIFIcon(this,backward_gif);
  forwardicon=new FXGIFIcon(this,forward_gif);

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
  associations=new FXFileAssociations(this);
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


// Close all windows
long Adie::onCmdCloseAll(FXObject*,FXSelector,void*){
  while(0<windowlist.no() && windowlist[0]->close(true)){}
  return 1;
  }


// Change syntax paths
long Adie::onCmdSyntaxPaths(FXObject* sender,FXSelector,void*){
  sender->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_GETSTRINGVALUE),(void*)&syntaxpaths);
  reg().writeStringEntry("SETTINGS","syntaxpaths",syntaxpaths.text());
  return 1;
  }


// Update syntax paths
long Adie::onUpdSyntaxPaths(FXObject* sender,FXSelector,void*){
  sender->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_SETSTRINGVALUE),(void*)&syntaxpaths);
  return 1;
  }

/*******************************************************************************/

// Print command line help
static void printusage(){
  printf("Usage: adie [options] files...\n");
  printf("  options:\n");
  printf("  -?, -h, --help                      Print help.\n");
  printf("  -V, --version                       Print version number.\n");
  printf("  -v, --view                          Start in view-only mode.\n");
  printf("  -e, --edit                          Start in edit-mode.\n");
  printf("  -l NUM, --line NUM                  Jump cursor position to line number.\n");
  printf("  -c NUM, --col NUM                   Jump cursor position to column.\n");
  printf("  -S SYNTAXFILE, --syntax SYNTAXFILE  Load given syntax file.\n");
  printf("  -L LANGUAGE, --lang LANGUAGE        Force language mode.\n");
  }


// Print verson info
static void printversion(){
  printf("A.d.i.e. - ADvanced Interactive Editor %d.%d.%d.\n",VERSION_MAJOR,VERSION_MINOR,VERSION_PATCH);
  printf("Copyright (C) 2000,2014 Jeroen van der Zijp.  All Rights Reserved.\n\n");
  printf("Please visit: http://www.fox-toolkit.org for further information.\n");
  printf("\n");
  printf("This program is free software: you can redistribute it and/or modify\n");
  printf("it under the terms of the GNU General Public License as published by\n");
  printf("the Free Software Foundation, either version 3 of the License, or\n");
  printf("(at your option) any later version.\n");
  printf("\n");
  printf("This program is distributed in the hope that it will be useful,\n");
  printf("but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
  printf("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
  printf("GNU General Public License for more details.\n");
  printf("\n");
  printf("You should have received a copy of the GNU General Public License\n");
  printf("along with this program.  If not, see <http://www.gnu.org/licenses/>.\n");
  }


// Start the application
FXint Adie::start(int argc,char** argv){
  FXString    file,lang,execpath,iconpath,syntaxfile;
  TextWindow *window=NULL;
  Syntax     *syntax=NULL;
  FXbool      edit=true;
  FXint       line=0;
  FXint       col=0;
  FXint       arg=1;

  // After init, the registry has been loaded
  init(argc,argv);

  // Make a tool tip
  new FXToolTip(this,0);

  // Create it
  create();

  // Exec path is default for syntax path
  execpath=FXSystem::getExecPath();

  // Get syntax path
  syntaxpaths=reg().readStringEntry("SETTINGS","syntaxpaths",execpath.text());

  // Hunt for the syntax file
  syntaxfile=FXPath::search(syntaxpaths,"Adie.stx");

  // Get icon search path
  iconpath=reg().readStringEntry("SETTINGS","iconpath",FXIconCache::defaultIconPath);

  // Change icon search path
  associations->setIconPath(iconpath);

  // Parse options first
  while(arg<argc && argv[arg][0]=='-'){
    if(compare(argv[arg],"-v")==0 || compare(argv[arg],"--view")==0){
      edit=false;
      }
    else if(compare(argv[arg],"-e")==0 || compare(argv[arg],"--edit")==0){
      edit=true;
      }
    else if(compare(argv[arg],"-?")==0 || compare(argv[arg],"-h")==0 || compare(argv[arg],"--help")==0){
      printusage();
      return 0;
      }
    else if(compare(argv[arg],"-V")==0 || compare(argv[arg],"--version")==0){
      printversion();
      return 0;
      }
    else if(compare(argv[arg],"-l")==0 || compare(argv[arg],"--line")==0){
      if(++arg>=argc){ fprintf(stderr,"Adie: missing line number.\n"); return 1; }
      sscanf(argv[arg],"%d",&line);
      }
    else if(compare(argv[arg],"-c")==0 || compare(argv[arg],"--col")==0){
      if(++arg>=argc){ fprintf(stderr,"Adie: missing column number.\n"); return 1; }
      sscanf(argv[arg],"%d",&col);
      }
    else if(compare(argv[arg],"-S")==0 || compare(argv[arg],"--syntax")==0){
      if(++arg>=argc){ fprintf(stderr,"Adie: missing syntax file.\n"); return 1; }
      syntaxfile=argv[arg];
      }
    else if(compare(argv[arg],"-S")==0 || compare(argv[arg],"--lang")==0){
      if(++arg>=argc){ fprintf(stderr,"Adie: missing language mode.\n"); return 1; }
      lang=argv[arg];
      }
    else{
      fprintf(stderr,"Adie: unknown command line argument.\n");
      return 1;
      }
    arg++;
    }

  // Load syntax file
  if(!syntaxfile.empty()){
    if(!SyntaxParser::parseFile(syntaxes,syntaxfile)){
      fprintf(stderr,"Adie: unable to parse syntax file: %s.\n",syntaxfile.text());
      }
    }

  // Get syntax
  syntax=getSyntaxByName(lang);

  // Parse filenames
  while(arg<argc){

    // Make new window
    window=new TextWindow(this);
    window->create();

    // Compute absolute path
    file=FXPath::absolute(argv[arg]);

    // Start in directory with empty untitled file
    if(FXStat::isDirectory(file)){
      file=FXPath::absolute(file,"untitled");
      window->setFilename(file);
      window->setBrowserCurrentFile(file);
      }

    // Start in directory with existing, accessible file
    else if(FXStat::isFile(file) && window->loadFile(file)){
      window->readBookmarks(file);
      window->readView(file);
      window->setEditable(edit);
      window->determineSyntax();
      if(line) window->visitLine(line,col);
      }

    // Start in directory with empty or inaccessible file
    else{
      window->setFilename(file);
      window->determineSyntax();
      window->setBrowserCurrentFile(file);
      window->determineSyntax();
      }

    // Override language mode?
    if(syntax){
      window->setSyntax(syntax);
      }
    arg++;
    }

  // Start in current directory with empty untitled file
  if(!window){

    // New window
    window=new TextWindow(this);
    window->create();

    // Compute absolute path
    file=FXPath::absolute("untitled");
    window->setFilename(file);
    window->setBrowserCurrentFile(file);

    // Override language mode?
    if(syntax){
      window->setSyntax(syntax);
      }
    }

  // Now run
  return run();
  }

/*******************************************************************************/

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
  delete searchicon;
  delete searchnexticon;
  delete searchprevicon;
  delete bookseticon;
  delete booknexticon;
  delete bookprevicon;
  delete bookdelicon;
  delete shiftlefticon;
  delete shiftrighticon;
  delete configicon;
  delete browsericon;
  delete nobrowsericon;
  delete uppercaseicon;
  delete lowercaseicon;
  delete backwardicon;
  delete forwardicon;
  }

