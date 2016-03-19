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
#ifndef ADIE_H
#define ADIE_H


// Version
#define VERSION_MAJOR 3
#define VERSION_MINOR 3
#define VERSION_PATCH 0


class HelpWindow;
class Preferences;
class TextWindow;
struct ParseInfo;


// Main Application class
class Adie : public FXApp {
  FXDECLARE(Adie)
  friend class TextWindow;
protected:
  TextWindowList  windowlist;                   // Window list
  FXFileDict     *associations;                 // File association table
  FXSyntaxList    syntaxes;                     // List of syntax patterns
  FXString        syntaxpath;                   // Where to look for syntax file
  FXIcon         *bigicon;                      // Big application icon
  FXIcon         *smallicon;                    // Small application icon
  FXIcon         *newicon;
  FXIcon         *reloadicon;
  FXIcon         *openicon;
  FXIcon         *saveicon;
  FXIcon         *saveasicon;
  FXIcon         *printicon;
  FXIcon         *cuticon;
  FXIcon         *copyicon;
  FXIcon         *pasteicon;
  FXIcon         *deleteicon;
  FXIcon         *undoicon;
  FXIcon         *redoicon;
  FXIcon         *fontsicon;
  FXIcon         *helpicon;
  FXIcon         *quiticon;
  FXIcon         *searchicon;
  FXIcon         *searchnexticon;
  FXIcon         *searchprevicon;
  FXIcon         *bookseticon;
  FXIcon         *booknexticon;
  FXIcon         *bookprevicon;
  FXIcon         *bookdelicon;
  FXIcon         *shiftlefticon;
  FXIcon         *shiftrighticon;
  FXIcon         *configicon;
  FXIcon         *browsericon;
  FXIcon         *nobrowsericon;
  FXIcon         *uppercaseicon;
  FXIcon         *lowercaseicon;
private:
  Adie(){}
  Adie(const Adie&);
  Adie& operator=(const Adie&);
  FXbool loadSyntaxFile(const FXString& file);
public:
  enum{
    ID_CLOSEALL=FXApp::ID_LAST,
    ID_LAST
    };
public:
  long onCmdCloseAll(FXObject*,FXSelector,void*);
public:

  // Construct application object
  Adie(const FXString& name);

  // Initialize application
  virtual void init(int& argc,char** argv,FXbool connect=true);

  // Set syntax paths
  void setSyntaxPaths(const FXString& paths){ syntaxpath=paths; }

  // Get syntax paths
  const FXString& getSyntaxPaths() const { return syntaxpath; }

  // Get syntax for language name
  FXSyntax* getSyntaxForLanguage(const FXString& name) const;

  // Get syntax from file name
  FXSyntax* getSyntaxForFile(const FXString& file) const;

  // Get syntax based on contents
  FXSyntax* getSyntaxForContents(const FXString& contents) const;

  // Exit application
  virtual void exit(FXint code=0);

  // Delete application object
  virtual ~Adie();
  };

#endif

