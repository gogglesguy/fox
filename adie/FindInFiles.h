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
#ifndef FINDINFILES_H
#define FINDINFILES_H


class Adie;
class FindInFiles;


/// Directory search visitor
class SearchVisitor : public FXGlobVisitor {
private:
  FXRex    rex;         // Regex parser
private:
  FXint loadFileBody(const FXString& file,FXString& body) const;
  FXint searchFileBody(const FXString& body) const;
public:
  SearchVisitor(const FXString& pattern=FXString::null,FXint mode=FXRex::Normal);
  virtual FXuint visit(const FXString& path);
  };


/// Find patterns in Files
class FindInFiles : public FXDialogBox {
  FXDECLARE(FindInFiles)
protected:
  FXIconList  *locations;               // Search hits
  FXTextField *findstring;              // String to search for
  FXTextField *filefolder;              // Folder to search
  FXComboBox  *filefilter;              // File filters
  FXString     filePattern;             // Search files matching pattern
  FXString     patternHistory[20];      // Search string history
  FXuint       optionsHistory[20];      // Search option history
  FXuint       searchmode;              // Search options
  FXint        index;                   // History index
protected:
  static const FXchar sectionName[];
private:
  FindInFiles(){}
  FindInFiles(const FindInFiles&);
  void appendHistory(const FXString& patt,FXuint opts);
  void readRegistry();
  void writeRegistry();
public:
  long onCmdSearch(FXObject*,FXSelector,void*);
  long onCmdFilter(FXObject*,FXSelector,void*);
  long onUpdHistoryUp(FXObject*,FXSelector,void*);
  long onUpdHistoryDn(FXObject*,FXSelector,void*);
  long onCmdHistoryUp(FXObject*,FXSelector,void*);
  long onCmdHistoryDn(FXObject*,FXSelector,void*);
  long onCmdFolder(FXObject*,FXSelector,void*);
  long onArrowKey(FXObject*,FXSelector,void*);
  long onMouseWheel(FXObject*,FXSelector,void*);
  long onUpdFlags(FXObject*,FXSelector,void*);
  long onCmdFlags(FXObject*,FXSelector,void*);
  long onCmdFileDblClicked(FXObject*,FXSelector,void*);
public:
  enum {
    SearchExact    = 0,         /// Search exact matches
    SearchCaseFold = 1,         /// Search with case folding
    SearchRegex    = 2,         /// Search regular expression
    SearchRecurse  = 4,         /// Search files recursively
    SeachHidden    = 8          /// Search hidden files also
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
    ID_EXACT,
    ID_ICASE,
    ID_REGEX,
    ID_RECURSIVE,
    ID_HIDDEN,
    ID_FILELIST,
    ID_PAUSE,
    ID_LAST
    };
public:

  /// Create find-in-files widget
  FindInFiles(Adie *a);

  /// Create server-side resources
  virtual void create();

  /// Close the window, return true if actually closed
  virtual FXbool close(FXbool notify=false);

  /// Change directory
  void setDirectory(const FXString& path);

  /// Return directory
  FXString getDirectory() const;

  /// Set text or pattern to search for
  void setSearchText(const FXString& text);

  /// Return text or pattern the user has entered
  FXString getSearchText() const;

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

  virtual ~FindInFiles();
  };

#endif
