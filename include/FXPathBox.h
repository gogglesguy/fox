/********************************************************************************
*                                                                               *
*                       P a t h   B o x   W i d g e t                           *
*                                                                               *
*********************************************************************************
* Copyright (C) 2025 by Jeroen van der Zijp.   All Rights Reserved.             *
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
********************************************************************************/
#ifndef FXPATHBOX_H
#define FXPATHBOX_H

#ifndef FXFRAME_H
#include "FXFrame.h"
#endif

namespace FX {


class FXIcon;
class FXMenuButton;
class FXFileAssociations;


/// PathBox options
enum {
  PATHBOX_NO_OWN_ASSOC = 0x00020000      /// Do not create associations for files
  };


/**
* Path selection widget.
* Quickly jump around directories, given an initial
* starting directory.
*/
class FXAPI FXPathBox : public FXFrame {
  FXDECLARE(FXPathBox)
protected:
  FXString            path;             // Path text
  FXFont             *font;             // Label font
  FXIcon             *icon;             // Icon at left
  FXFileAssociations *associations;     // Association table
  FXIcon             *foldericon;       // Folder icons
  FXIcon             *cdromicon;        // CDROM icon
  FXIcon             *harddiskicon;     // Hard disk icon
  FXIcon             *netdriveicon;     // Networked drive icon
  FXIcon             *floppyicon;       // Floppy icon
  FXIcon             *nethoodicon;      // Network neighborhood icon
  FXIcon             *zipdiskicon;      // Zip drive icon
  FXint               hiliteBeg;        // Highlight when hovering
  FXint               hiliteEnd;        // Highlight when hovering
  FXint               selectBeg;        // Selection begin
  FXint               selectEnd;        // Selection end
  FXColor             textColor;        // Text color
  FXColor             selbackColor;     // Selected background color
  FXColor             seltextColor;     // Selected text color
  FXint               columns;          // Number of columns visible
  FXString            help;             // Help message
  FXString            tip;              // Tooltip
  FXuchar             mode;             // Mode widget is in
protected:
  FXPathBox();
  void drawArrow(FXDCWindow& dc,FXint ix,FXint iy,FXint s) const;
  FXint getPathComponent(FXint mx,FXint my,FXint& f,FXint& t) const;
  static FXbool selectPathComponent(const FXString& base,const FXString& path,FXint& f,FXint& t);
protected:
  enum {
    MOUSE_NONE,         // No mouse operation
    MOUSE_DRAG,         // Dragging text
    MOUSE_TRYDRAG       // Tentative drag
    };
private:
  FXPathBox(const FXPathBox&);
  FXPathBox &operator=(const FXPathBox&);
public:
  long onPaint(FXObject*,FXSelector,void*);
  long onEnter(FXObject*,FXSelector,void*);
  long onLeave(FXObject*,FXSelector,void*);
  long onLeftBtnPress(FXObject*,FXSelector,void*);
  long onLeftBtnRelease(FXObject*,FXSelector,void*);
  long onMiddleBtnPress(FXObject*,FXSelector,void*);
  long onMiddleBtnRelease(FXObject*,FXSelector,void*);
  long onRightBtnPress(FXObject*,FXSelector,void*);
  long onRightBtnRelease(FXObject*,FXSelector,void*);
  long onMotion(FXObject*,FXSelector,void*);
  long onCmdSetValue(FXObject*,FXSelector,void*);
  long onCmdSetStringValue(FXObject*,FXSelector,void*);
  long onCmdGetStringValue(FXObject*,FXSelector,void*);
  long onCmdSetHelp(FXObject*,FXSelector,void*);
  long onCmdGetHelp(FXObject*,FXSelector,void*);
  long onCmdSetTip(FXObject*,FXSelector,void*);
  long onCmdGetTip(FXObject*,FXSelector,void*);
  long onQueryHelp(FXObject*,FXSelector,void*);
  long onQueryTip(FXObject*,FXSelector,void*);
  long onTipTimer(FXObject*,FXSelector,void*);
public:

  /// Construct a Directory Box
  FXPathBox(FXComposite *p,FXObject* tgt=nullptr,FXSelector sel=0,FXuint opts=FRAME_SUNKEN|FRAME_THICK,FXint x=0,FXint y=0,FXint w=0,FXint h=0,FXint pl=DEFAULT_PAD,FXint pr=DEFAULT_PAD,FXint pt=DEFAULT_PAD,FXint pb=DEFAULT_PAD);

  /// Create server-side resources
  virtual void create();

  /// Detach server-side resources
  virtual void detach();

  /// Destroy server-side resources
  virtual void destroy();

  /// Enable the window
  virtual void enable();

  /// Disable the window
  virtual void disable();

  /// Return default width
  virtual FXint getDefaultWidth();

  /// Return default height
  virtual FXint getDefaultHeight();

  /// Set current directory
  void setDirectory(const FXString& newpath,FXbool notify=false);

  /// Return current directory
  const FXString& getDirectory() const { return path; }

  /// Set the text font
  void setFont(FXFont *fnt);

  /// Get the text font
  FXFont* getFont() const { return font; }

  /// Change file associations; delete the old one unless it was shared
  void setAssociations(FXFileAssociations* assoc,FXbool owned=false,FXbool notify=false);

  /// Return file associations
  FXFileAssociations* getAssociations() const { return associations; }

  /// Change number of visible columns
  void setNumColumns(FXint cols);

  /// Return number of visible columns
  FXint getNumColumns() const { return columns; }

  /// Set the current text color
  void setTextColor(FXColor clr);

  /// Get the current text color
  FXColor getTextColor() const { return textColor; }

  /// Change selected background color
  void setSelBackColor(FXColor clr);

  /// Return selected background color
  FXColor getSelBackColor() const { return selbackColor; }

  /// Change selected text color
  void setSelTextColor(FXColor clr);

  /// Return selected text color
  FXColor getSelTextColor() const { return seltextColor; }

  /// Set the status line help text for this label
  void setHelpText(const FXString& text){ help=text; }

  /// Get the status line help text for this label
  const FXString& getHelpText() const { return help; }

  /// Set the tool tip message for this label
  void setTipText(const FXString& text){ tip=text; }

  /// Get the tool tip message for this label
  const FXString& getTipText() const { return tip; }

  /// Save to stream
  virtual void save(FXStream& store) const;

  /// Load from stream
  virtual void load(FXStream& store);

  /// Destructor
  virtual ~FXPathBox();
  };

}

#endif
