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
#ifndef REPLACEINFILES_H
#define REPLACEINFILES_H

class Adie;
class ReplaceInFiles;


/// Directory search visitor
class ReplaceVisitor : public FXGlobVisitor {
private:
  FXRex           rex;          // Regex parser
  FXString        searchfor;    // Search string
  FXString        replaceby;    // Replace string
  FXString        text;         // Text buffer
  FXlong          limit;        // File size limit
private:
  FXlong loadFile(const FXString& path);
  FXlong saveFile(const FXString& path) const;
  FXint searchFile(const FXString& path);
private:
  ReplaceVisitor(const ReplaceVisitor&);
  ReplaceVisitor& operator=(const ReplaceVisitor&);
public:

  // Construct search file for pattern visitor
  ReplaceVisitor():limit(100000000L){ }

  // Traverse files under path and search for pattern in each
  FXuint traverse(const FXString& path,const FXString& search,const FXString& replace,const FXString& wild="*",FXint mode=FXRex::Normal,FXuint opts=FXDir::MatchAll,FXint depth=1000);

  // Set file size limit
  void setLimit(FXlong size);

  // Get file size limit
  FXlong getLimit() const { return limit; }

  // Visit given file and scan it
  virtual FXuint visit(const FXString& path) override;
  };


/// Find patterns in Files
class ReplaceInFiles : public FXDialogBox {
  FXDECLARE(ReplaceInFiles)
protected:
  ReplaceVisitor  visitor;              // Search visitor
  FXTextField    *searchstring;         // String to search for
  FXTextField    *replacestring;        // String to search for
  FXTextField    *filefolder;           // Folder to search
  FXComboBox     *filefilter;           // File filters
  FXToggleButton *pausebutton;          // Pause button
  FXString        filePattern;          // Search files matching pattern
  FXString        searchHistory[20];    // Search string history
  FXString        replaceHistory[20];   // Replace string history
  FXString        folderHistory[20];    // Search folder history
  FXuint          patternHistory[20];   // Search wildcard history
  FXuint          optionsHistory[20];   // Search option history
  FXuint          searchmode;           // Search options
  FXString        savedsearchtext;      // Saved search text
  FXString        savedsearchfolder;    // Saved search folder
  FXuint          savedsearchmode;      // Saved search mode
  FXuint          savedcurrentpattern;  // Saved search pattern
  FXint           index;                // History index
protected:
  ReplaceInFiles();
private:
  ReplaceInFiles(const ReplaceInFiles&);
  ReplaceInFiles& operator=(const ReplaceInFiles&);
private:
  void readRegistry();
  void writeRegistry();
  void appendHistory(const FXString& text,const FXString& dir,FXuint patt,FXuint opts);
public:
  long onUpdStop(FXObject*,FXSelector,void*);
  long onCmdStop(FXObject*,FXSelector,void*);
  long onUpdPause(FXObject*,FXSelector,void*);
  long onCmdPause(FXObject*,FXSelector,void*);
  long onUpdSearch(FXObject*,FXSelector,void*);
  long onCmdSearch(FXObject*,FXSelector,void*);
  long onCmdFilter(FXObject*,FXSelector,void*);
  long onUpdFlags(FXObject*,FXSelector,void*);
  long onCmdFlags(FXObject*,FXSelector,void*);
  long onCmdFolder(FXObject*,FXSelector,void*);
  long onUpdHistoryUp(FXObject*,FXSelector,void*);
  long onUpdHistoryDn(FXObject*,FXSelector,void*);
  long onCmdHistoryUp(FXObject*,FXSelector,void*);
  long onCmdHistoryDn(FXObject*,FXSelector,void*);
  long onArrowKey(FXObject*,FXSelector,void*);
  long onMouseWheel(FXObject*,FXSelector,void*);
public:
  enum {
    SearchExact    = 0,         /// Search exact matches
    SearchCaseFold = 1,         /// Search with case folding
    SearchRegex    = 2,         /// Search regular expression
    SearchWords    = 4,         /// Search whole words
    SearchRecurse  = 8,         /// Search files recursively
    SeachHidden    = 16         /// Search hidden files also
    };
public:
  enum{
    ID_SEARCH=FXDialogBox::ID_LAST,
    ID_SEARCH_TEXT,
    ID_FOLDER_TEXT,
    ID_FILTER_TEXT,
    ID_FOLDER,
    ID_HIST_UP,
    ID_HIST_DN,
    ID_ICASE,
    ID_REGEX,
    ID_WORDS,
    ID_RECURSIVE,
    ID_HIDDEN,
    ID_FILELIST,
    ID_PAUSE,
    ID_STOP,
    ID_LAST
    };
public:

  /// Create find-in-files widget
  ReplaceInFiles(Adie *a);

  /// Return Adie application
  Adie* getApp() const { return (Adie*)FXDialogBox::getApp(); }

  /// Create server-side resources
  virtual void create() override;

  /// Close the window, return true if actually closed
  virtual FXbool close(FXbool notify=false) override;

  /// Change directory
  void setDirectory(const FXString& path);

  /// Return directory
  FXString getDirectory() const;

  /// Set text or pattern to search for
  void setSearchText(const FXString& text);

  /// Return text or pattern the user has entered
  FXString getSearchText() const;

  /// Set replace text
  void setReplaceText(const FXString& text);

  /// Return replace text the user has entered
  FXString getReplaceText() const;

  /// Set search match mode
  void setSearchMode(FXuint mode){ searchmode=mode; }

  /// Return search mode the user has selected
  FXuint getSearchMode() const { return searchmode; }

  /// Change file pattern
  void setPattern(const FXString& ptrn);

  /// Return file pattern
  const FXString& getPattern() const { return filePattern; }

  /// Set list of patterns
  void setPatternList(const FXString& patterns);

  /// Return list of patterns
  FXString getPatternList() const;

  /// Set currently selected pattern
  void setCurrentPattern(FXint patno);

  /// Return current pattern number
  FXint getCurrentPattern() const;

  /// Change pattern text for pattern number
  void setPatternText(FXint patno,const FXString& text);

  /// Get pattern text for given pattern number
  FXString getPatternText(FXint patno) const;

  /// Return number of patterns
  FXint getNumPatterns() const;

  /// Allow pattern entry
  void allowPatternEntry(FXbool flag);

  /// Return true if pattern entry is allowed
  FXbool allowPatternEntry() const;

  /// Destroy it
  virtual ~ReplaceInFiles();
  };

#endif
