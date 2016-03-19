/********************************************************************************
*                                                                               *
*      F i l e   C o p y / M o v e / L i n k / D e l e t e   D i a l o g s      *
*                                                                               *
*********************************************************************************
* Copyright (C) 2005,2007 by Jeroen van der Zijp.   All Rights Reserved.        *
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
* $Id: FXCopyDialog.h,v 1.8 2007/07/09 16:02:42 fox Exp $                       *
********************************************************************************/
#ifndef FXCOPYDIALOG_H
#define FXCOPYDIALOG_H


#ifndef FXDIALOGBOX_H
#include "FXDialogBox.h"
#endif

namespace FX {


class FXList;
class FXIcon;
class FXLabel;
class FXMatrix;
class FXTextField;


/// Delete dialog
class FXDeleteDialog : public FXDialogBox {
  FXDECLARE(FXDeleteDialog)
protected:
  FXMatrix *matrix;     // Container
  FXList   *srclist;    // Source file list
  FXLabel  *srclabel;   // Source label
protected:
  FXDeleteDialog(){}
private:
  FXDeleteDialog(const FXDeleteDialog&);
  FXDeleteDialog &operator=(const FXDeleteDialog&);
public:

  /// Construct copy/move/link/delete/rename dialog
  FXDeleteDialog(FXWindow *owner,const FXString& caption,const FXString& srctxt,FXIcon* icon=NULL,FXuint opts=0,FXint x=0,FXint y=0,FXint w=0,FXint h=0);

  /// Set source files from newline-separated list
  void setSourceFiles(const FXString& files);

  /// Change source label
  void setSourceLabel(const FXString& str);

  /// Get source label
  FXString getSourceLabel() const;
  };



/// Copy/Move/Link dialog
class FXCopyDialog : public FXDeleteDialog {
  FXDECLARE(FXCopyDialog)
protected:
  FXList   *dstlist;    // Destination file or folder
  FXLabel  *dstlabel;   // Destination label
protected:
  FXCopyDialog(){}
private:
  FXCopyDialog(const FXCopyDialog&);
  FXCopyDialog &operator=(const FXCopyDialog&);
public:

  /// Construct copy/move/link dialog
  FXCopyDialog(FXWindow *owner,const FXString& caption,const FXString& srctxt,const FXString& dsttxt,FXIcon* icon=NULL,FXuint opts=0,FXint x=0,FXint y=0,FXint w=0,FXint h=0);

  /// Set destination file
  void setDestinationFile(const FXString& file);

  /// Change destination label
  void setDestinationLabel(const FXString& str);

  /// Get destination label
  FXString getDestinationLabel() const;
  };

}

#endif
