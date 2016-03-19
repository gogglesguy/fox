/********************************************************************************
*                                                                               *
*                            H e l p   W i n d o w                              *
*                                                                               *
*********************************************************************************
* Copyright (C) 1998,2008 by Jeroen van der Zijp.   All Rights Reserved.        *
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
*********************************************************************************
* $Id: HelpWindow.h,v 1.9 2008/01/04 15:18:13 fox Exp $                         *
********************************************************************************/
#ifndef HELPWINDOW_H
#define HELPWINDOW_H


//class FXText;


/// Online help dialog box
class HelpWindow : public FXDialogBox {
  FXDECLARE(HelpWindow)
protected:
  FXText *helptext;         // Help display
private:
  HelpWindow(){}
  HelpWindow(const HelpWindow&);
public:
  HelpWindow(FXWindow *owner,const FXString& title);
  void setHelp(const FXString& help);
  FXString getHelp() const;
  virtual ~HelpWindow();
  };

#endif
