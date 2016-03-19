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
#ifndef FINDINFILES_H
#define FINDINFILES_H


class Adie;


/// Find patterns in Files
class FindInFiles : public FXDialogBox {
  FXDECLARE(FindInFiles)
protected:
  FXIconList  *locations;
  FXTextField *findstring;
  FXTextField *filefolder;
  FXComboBox  *filefilter;
  FXString     history[20];
  FXuint       searchflags;
  FXint        index;
protected:
  static const FXchar sectionName[];
private:
  FindInFiles(){}
  FindInFiles(const FindInFiles&);
  void appendhist(const FXString& string);
  void scrollback();
  void scrollforw();
public:
  long onCmdSearch(FXObject*,FXSelector,void*);
  long onUpdHistory(FXObject*,FXSelector,void*);
  long onCmdHistory(FXObject*,FXSelector,void*);
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
    ID_LAST
    };
public:

  /// Create find-in-files widget
  FindInFiles(Adie *a);

  /// Create server-side resources
  virtual void create();

  /// Destroy server-side resources
  virtual void destroy();

  /// Change directory
  void setDirectory(const FXString& path);

  /// Set text or pattern to search for
  void setSearchText(const FXString& text);

  /// Return text or pattern the user has entered
  FXString getSearchText() const;

  /// Return directory
  FXString getDirectory() const;

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
