/********************************************************************************
*                                                                               *
*                   FOX Desktop Setup - FOX Desktop Enviroment                  *
*                                                                               *
*********************************************************************************
* Copyright (C) 2004,2010 Sander Jansen.  All Rights Reserved.                  *
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
#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H


// Color theme info
struct ColorTheme {
  const FXchar* name;
  FXColor       base;
  FXColor       border;
  FXColor       back;
  FXColor       fore;
  FXColor       selback;
  FXColor       selfore;
  FXColor       tipback;
  FXColor       tipfore;
  FXColor       menuback;
  FXColor       menufore;
  };


// File binding
struct FXFileBinding {
  FXString key;
  FXString description;
  FXString command;
  FXString bigname;
  FXString bignameopen;
  FXString name;
  FXString nameopen;
  FXString mime;
  };


// Control panel widget
class FXDesktopSetup : public FXMainWindow {
  FXDECLARE(FXDesktopSetup)
private:
  FXSettings         desktopsettings;   // Desktop Settings Registry
  FXFont            *titlefont;
  FXIcon            *desktopicon;
  FXIcon            *icon_colors;
  FXIcon            *icon_settings;
  FXIcon            *icon_filebinding;
private:
  FXListBox         *list;
  FXList            *filebindinglist;
  FXComboBox        *mimetypelist;
  FXButton          *button_bigname;
  FXButton          *button_bignameopen;
  FXButton          *button_name;
  FXButton          *button_nameopen;
private:
  FXToolTip         *tooltip;
  FXTabBook         *tabbook;
  FXTabItem         *tabitem;
  FXVerticalFrame   *tabframe;
  FXVerticalFrame   *mainframe;
  FXVerticalFrame   *menuframe;
  FXHorizontalFrame *labeltextframe1;
  FXHorizontalFrame *labeltextframe2;
  FXHorizontalFrame *textframe1;
  FXHorizontalFrame *textframe2;
  FXHorizontalFrame *tabsubframe;
  FXGroupBox        *grpbox1;
  FXGroupBox        *grpbox2;
  FXLabel           *label1;
  FXLabel           *label2;
  FXLabel           *label3;
  FXLabel           *label4;
  FXLabel           *label5;
  FXLabel           *menulabels[6];
  FXTextField       *textfield1;
  FXButton          *button1;
  FXButton          *fontbutton;
  FXSeparator       *sep1;
  FXSeparator       *sep2;
  FXSeparator       *sep3;
private:
  ColorTheme         theme_current;     // Current Settings
  ColorTheme         theme_user;        // Theme User may have set, which is different from the other themes
  FXColor            hilite;
  FXColor            shadow;
private:
  FXFont            *font;
  FXbool             hascurrent;
  FXString           applicationname;   // If editing specific application settings
  FXString           vendorname;        // If editing specific vendor settings
  FXString           iconpath;          // Path where icons are found
  FXTime             typingSpeed;
  FXTime             clickSpeed;
  FXTime             scrollSpeed;
  FXTime             scrollDelay;
  FXTime             blinkSpeed;
  FXTime             animSpeed;
  FXTime             menuPause;
  FXTime             tooltipPause;
  FXTime             tooltipTime;
  FXuint             maxcolors;
  FXint              dragDelta;
  FXint              wheelLines;
  FXfloat            gamma;
  FXFileBinding      filebinding;       // Current file binding
private:
  FXDataTarget       target_base;
  FXDataTarget       target_back;
  FXDataTarget       target_border;
  FXDataTarget       target_fore;
  FXDataTarget       target_hilite;
  FXDataTarget       target_shadow;
  FXDataTarget       target_selfore;
  FXDataTarget       target_selback;
  FXDataTarget       target_tipfore;
  FXDataTarget       target_tipback;
  FXDataTarget       target_menufore;
  FXDataTarget       target_menuback;
  FXDataTarget       target_typingspeed;
  FXDataTarget       target_clickspeed;
  FXDataTarget       target_scrollspeed;
  FXDataTarget       target_scrolldelay;
  FXDataTarget       target_blinkspeed;
  FXDataTarget       target_animspeed;
  FXDataTarget       target_menupause;
  FXDataTarget       target_tooltippause;
  FXDataTarget       target_tooltiptime;
  FXDataTarget       target_dragdelta;
  FXDataTarget       target_wheellines;
  FXDataTarget       target_maxcolors;
  FXDataTarget       target_gamma;
  FXDataTarget       target_filebinding_description;
  FXDataTarget       target_filebinding_command;
  FXDataTarget       target_iconpath;
private:
  void setup();
  void setupFont();
  void setupColors();
  FXbool writeDesktop();
  void initColors();
  void saveFileBinding();
  FXString getOutputFile();
private:
  FXDesktopSetup(){}
  FXDesktopSetup(const FXDesktopSetup&);
  FXDesktopSetup& operator=(const FXDesktopSetup&);
public:
  enum {
    ID_COLORS=FXMainWindow::ID_LAST,
    ID_COLOR_THEME,
    ID_CHOOSE_FONT,
    ID_SELECT_COMMAND,
    ID_CREATE_FILEBINDING,
    ID_REMOVE_FILEBINDING,
    ID_RENAME_FILEBINDING,
    ID_SELECT_FILEBINDING,
    ID_SELECT_ICON_NAME,
    ID_SELECT_ICON_BIGNAME,
    ID_SELECT_ICON_NAMEOPEN,
    ID_SELECT_ICON_BIGNAMEOPEN,
    ID_SELECT_MIMETYPE
    };
public:
  long onColorChanged(FXObject*,FXSelector,void*);
  long onColorTheme(FXObject*,FXSelector,void*);
  long onChooseFont(FXObject*,FXSelector,void*);
  long onCmdFileBinding(FXObject*,FXSelector,void*);
  long onCmdMimeType(FXObject*,FXSelector,void*);
  long onCmdCreateFileBinding(FXObject*,FXSelector,void*);
  long onCmdRemoveFileBinding(FXObject*,FXSelector,void*);
  long onCmdRenameFileBinding(FXObject*,FXSelector,void*);
  long onCmdSelectCommand(FXObject*,FXSelector,void*);
  long onCmdSelectIcon(FXObject*,FXSelector,void*);
public:

  // Constructor
  FXDesktopSetup(FXApp *app);

  // Create widgets
  virtual void create();

  // Close the application, return TRUE if actually closed
  virtual FXbool close(FXbool notify=false);

  // Change application name
  void setApplicationName(const FXString& name){ applicationname=name; }
  
  // Return application name
  const FXString& getApplicationName() const { return applicationname; }

  // Change vendor name
  void setVendorName(const FXString& name){ vendorname=name; }
  
  // Return vendor name
  const FXString& getVendorName() const { return vendorname; }

  // Destructor
  virtual ~FXDesktopSetup();
  };


#endif
