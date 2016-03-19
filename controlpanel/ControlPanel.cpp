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
#include <xincs.h>
#include <fx.h>
#include "ControlPanel.h"
#include "icons.h"

#define DEFAULT_SPACING 6



const ColorTheme ColorThemes[]={
//|--------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------|
//|        Name        |        Base      |       Border     |       Back       |      Fore        |      Selback     |      Selfore     |      Tipback     |     Tipfore      |      Menuback    |      Menufore    |
//|--------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------|
  {"FOX"               ,FXRGB(212,208,200),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB( 10, 36,106),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB( 10, 36,106),FXRGB(255,255,255)},
  {"Gnome"             ,FXRGB(214,215,214),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(  0,  0,128),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(  0,  0,128),FXRGB(255,255,255)},
  {"Atlas Green"       ,FXRGB(175,180,159),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(111,122, 99),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(111,122, 99),FXRGB(255,255,255)},
  {"BeOS"              ,FXRGB(217,217,217),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(168,168,168),FXRGB(  0,  0,  0),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(168,168,168),FXRGB(  0,  0,  0)},
  {"Blue Slate"        ,FXRGB(239,239,239),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(103,141,178),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(103,141,178),FXRGB(255,255,255)},
//|--------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------|
//|        Name        |        Base      |       Border     |       Back       |      Fore        |      Selback     |      Selfore     |      Tipback     |     Tipfore      |      Menuback    |      Menufore    |
//|--------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------|
  {"CDE"	       ,FXRGB(156,153,156),FXRGB(  0,  0,  0),FXRGB(131,129,131),FXRGB(255,255,255),FXRGB( 49, 97,131),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB( 49, 97,131),FXRGB(255,255,255)},
  {"Digital CDE"       ,FXRGB( 74,121,131),FXRGB(  0,  0,  0),FXRGB( 55, 77, 78),FXRGB(255,255,255),FXRGB( 82,102,115),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB( 82,102,115),FXRGB(255,255,255)},
  {"Dark Blue"         ,FXRGB( 66,103,148),FXRGB(  0,  0,  0),FXRGB(  0, 42, 78),FXRGB(255,255,255),FXRGB( 92,179,255),FXRGB(  0,  0,  0),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB( 92,179,255),FXRGB(  0,  0,  0)},
  {"Desert FOX"        ,FXRGB(214,205,187),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(128,  0,  0),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(128,  0,  0),FXRGB(255,255,255)},
  {"EveX"              ,FXRGB(230,222,220),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB( 10, 95,137),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB( 10, 95,137),FXRGB(255,255,255)},
//|--------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------|
//|        Name        |        Base      |       Border     |       Back       |      Fore        |      Selback     |      Selfore     |      Tipback     |     Tipfore      |      Menuback    |      Menufore    |
//|--------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------|
  {"Galaxy"            ,FXRGB(239,239,239),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(103,141,178),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(103,141,178),FXRGB(255,255,255)},
  {"iMac"              ,FXRGB(205,206,205),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(  0,  0,128),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(  0,  0,128),FXRGB(255,255,255)},
  {"KDE 1"             ,FXRGB(192,192,192),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(  0,  0,128),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(  0,  0,128),FXRGB(255,255,255)},
  {"KDE 2"             ,FXRGB(220,220,220),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB( 10, 95,137),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB( 10, 95,137),FXRGB(255,255,255)},
  {"KDE 3"             ,FXRGB(238,238,230),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(255,221,118),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(255,221,118),FXRGB(255,255,255)},
//|--------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------|
//|        Name        |        Base      |       Border     |       Back       |      Fore        |      Selback     |      Selfore     |      Tipback     |     Tipfore      |      Menuback    |      Menufore    |
//|--------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------|
  {"Keramik"           ,FXRGB(234,233,232),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(169,209,255),FXRGB(  0,  0,  0),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(169,209,255),FXRGB(  0,  0,  0)},
  {"Keramik Emerald"   ,FXRGB(238,238,230),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(134,204,134),FXRGB(  0,  0,  0),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(134,204,134),FXRGB(  0,  0,  0)},
  {"Keramik White"     ,FXRGB(239,239,239),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(103,141,178),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(103,141,178),FXRGB(255,255,255)},
  {"Mandrake"          ,FXRGB(230,231,230),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB( 33, 68,156),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB( 33, 68,156),FXRGB(255,255,255)},
  {"Media Peach"       ,FXRGB(239,239,239),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(103,141,178),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(103,141,178),FXRGB(255,255,255)},
//|--------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------|
//|        Name        |        Base      |       Border     |       Back       |      Fore        |      Selback     |      Selfore     |      Tipback     |     Tipfore      |      Menuback    |      Menufore    |
//|--------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------|
  {"Next"              ,FXRGB(168,168,168),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(  0,  0,  0),FXRGB(255,255,255)},
  {"Pale Gray"         ,FXRGB(214,214,214),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(  0,  0,  0),FXRGB(255,255,255)},
  {"Plastik"           ,FXRGB(239,239,239),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(103,141,178),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(103,141,178),FXRGB(255,255,255)},
  {"Pumpkin"           ,FXRGB(238,216,174),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(205,133, 63),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(205,133, 63),FXRGB(255,255,255)},
  {"Redmond 95"        ,FXRGB(195,195,195),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(  0,  0,128),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(  0,  0,128),FXRGB(255,255,255)},
//|--------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------|
//|        Name        |        Base      |       Border     |       Back       |      Fore        |      Selback     |      Selfore     |      Tipback     |     Tipfore      |      Menuback    |      Menufore    |
//|--------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------|
  {"Redmond 2000"      ,FXRGB(212,208,200),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(  0, 36,104),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(  0, 36,104),FXRGB(255,255,255)},
  {"Redmond XP"        ,FXRGB(238,238,230),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB( 74,121,205),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB( 74,121,205),FXRGB(255,255,255)},
  {"Solaris"           ,FXRGB(174,178,195),FXRGB(  0,  0,  0),FXRGB(147,151,165),FXRGB(  0,  0,  0),FXRGB(113,139,165),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(113,139,165),FXRGB(255,255,255)},
  {"Storm"             ,FXRGB(192,192,192),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(139,  0,139),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(139,  0,139),FXRGB(255,255,255)},
  {"Sea Sky"           ,FXRGB(165,178,198),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB( 49,101,156),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB( 49,101,156),FXRGB(255,255,255)},
  };


const FXint numThemes=ARRAYNUMBER(ColorThemes);

/*******************************************************************************/


// Messages
FXDEFMAP(FXDesktopSetup) FXDesktopSetupMap[]={
  FXMAPFUNC(SEL_COMMAND,FXDesktopSetup::ID_COLORS,FXDesktopSetup::onColorChanged),
  FXMAPFUNC(SEL_CHANGED,FXDesktopSetup::ID_COLORS,FXDesktopSetup::onColorChanged),
  FXMAPFUNC(SEL_COMMAND,FXDesktopSetup::ID_COLOR_THEME,FXDesktopSetup::onColorTheme),
  FXMAPFUNC(SEL_COMMAND,FXDesktopSetup::ID_CHOOSE_FONT,FXDesktopSetup::onChooseFont),
  FXMAPFUNC(SEL_CHANGED,FXDesktopSetup::ID_SELECT_FILEBINDING,FXDesktopSetup::onCmdFileBinding),
  FXMAPFUNC(SEL_COMMAND,FXDesktopSetup::ID_SELECT_COMMAND,FXDesktopSetup::onCmdSelectCommand),
  FXMAPFUNC(SEL_COMMAND,FXDesktopSetup::ID_SELECT_MIMETYPE,FXDesktopSetup::onCmdMimeType),
  FXMAPFUNC(SEL_COMMAND,FXDesktopSetup::ID_CREATE_FILEBINDING,FXDesktopSetup::onCmdCreateFileBinding),
  FXMAPFUNC(SEL_COMMAND,FXDesktopSetup::ID_REMOVE_FILEBINDING,FXDesktopSetup::onCmdRemoveFileBinding),
  FXMAPFUNC(SEL_COMMAND,FXDesktopSetup::ID_RENAME_FILEBINDING,FXDesktopSetup::onCmdRenameFileBinding),
  FXMAPFUNCS(SEL_COMMAND,FXDesktopSetup::ID_SELECT_ICON_BIG,FXDesktopSetup::ID_SELECT_ICON_MINIOPEN,FXDesktopSetup::onCmdSelectIcon),
  };


// Object implementation
FXIMPLEMENT(FXDesktopSetup,FXMainWindow,FXDesktopSetupMap,ARRAYNUMBER(FXDesktopSetupMap))

/*******************************************************************************/

// Construct window
FXDesktopSetup::FXDesktopSetup(FXApp *ap):FXMainWindow(ap,tr("FOX Desktop Setup"),NULL,NULL,DECOR_ALL,0,0,0,0){

  // Grab stock font's description
  FXFontDesc fontdescription=getApp()->getNormalFont()->getFontDesc();

  // Make some icons
  desktopicon=new FXGIFIcon(getApp(),controlpanel_gif);
  icon_colors=new FXGIFIcon(getApp(),colors_gif);
  icon_settings=new FXGIFIcon(getApp(),settings_gif);
  icon_filebinding=new FXGIFIcon(getApp(),filebinding_gif);

  // New font for sampler
  font=new FXFont(getApp(),fontdescription);

  // New font for caption; same typeface, only bigger
  fontdescription.size=(fontdescription.size*15)/10;
  titlefont=new FXFont(getApp(),fontdescription);

  // Main frame
  FXVerticalFrame *main=new FXVerticalFrame(this,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,0,0,0,0,0,0);

  // Create nice header
  FXLabel *caption=new FXLabel(main,tr("FOX Desktop Setup"),desktopicon,LAYOUT_FILL_X|JUSTIFY_LEFT|TEXT_AFTER_ICON,0,0,0,0,5,5,5,5);
  caption->setBackColor(FXRGB(255,255,255));
  caption->setTextColor(FXRGB(  0,  0,  0));
  caption->setFont(titlefont);

  // Separator between header and the rest
  new FXSeparator(main,SEPARATOR_GROOVE|LAYOUT_FILL_X);

  // Frame for contents below header
  FXHorizontalFrame *hmainframe=new FXHorizontalFrame(main,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,0,0,0,0,0,0);
  FXVerticalFrame *buttonframe=new FXVerticalFrame(hmainframe,LAYOUT_FILL_Y|LAYOUT_LEFT|PACK_UNIFORM_WIDTH|PACK_UNIFORM_HEIGHT,0,0,0,0, DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING);

  // Separator between button frame and subpanels
  new FXSeparator(hmainframe,SEPARATOR_GROOVE|LAYOUT_FILL_Y);

  // Switched frame
  FXSwitcher *switcher=new FXSwitcher(hmainframe,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,0,0,0,0);

  FXVerticalFrame* vframe1=new FXVerticalFrame(buttonframe,FRAME_SUNKEN,0,0,0,0,0,0,0,0);
  new FXButton(vframe1,tr("Themes"),icon_colors,switcher,FXSwitcher::ID_OPEN_FIRST,FRAME_RAISED|ICON_ABOVE_TEXT|LAYOUT_FILL);
  FXVerticalFrame* vframe2=new FXVerticalFrame(buttonframe,FRAME_SUNKEN,0,0,0,0,0,0,0,0);
  new FXButton(vframe2,tr("Bindings"),icon_filebinding,switcher,FXSwitcher::ID_OPEN_SECOND,FRAME_RAISED|ICON_ABOVE_TEXT|LAYOUT_FILL);
  FXVerticalFrame* vframe3=new FXVerticalFrame(buttonframe,FRAME_SUNKEN,0,0,0,0,0,0,0,0);
  new FXButton(vframe3,tr("General"),icon_settings,switcher,FXSwitcher::ID_OPEN_THIRD,FRAME_RAISED|ICON_ABOVE_TEXT|LAYOUT_FILL);

  /// Color Settings Panel ///
  FXVerticalFrame* vframe4=new FXVerticalFrame(switcher,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,0,0,0,0,0,0);

  FXHorizontalFrame* hframe1=new FXHorizontalFrame(vframe4,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,0,0,0,0,0,0);
  new FXSeparator(vframe4,SEPARATOR_GROOVE|LAYOUT_FILL_X);


  FXVerticalFrame* frame1=new FXVerticalFrame(hframe1,LAYOUT_FILL_Y,0,0,0,0,0,0,0,0,0,0);
  new FXSeparator(hframe1,SEPARATOR_GROOVE|LAYOUT_FILL_Y);

  FXVerticalFrame *themeframe=new FXVerticalFrame(frame1,LAYOUT_FILL_X,0,0,0,0,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING);
  new FXLabel(themeframe,tr("Theme: "),NULL,LAYOUT_CENTER_Y);
  list=new FXListBox(themeframe,this,ID_COLOR_THEME,LAYOUT_FILL_X|FRAME_SUNKEN|FRAME_THICK);
  list->setNumVisible(9);

  new FXSeparator(frame1,SEPARATOR_GROOVE|LAYOUT_FILL_X);

  // Matrix for color wells
  FXMatrix* matrix1=new FXMatrix(frame1,2,LAYOUT_FILL_Y|MATRIX_BY_COLUMNS,0,0,0,0,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING,1,1);

  new FXColorWell(matrix1,FXRGB(0,0,255),&target_base,FXDataTarget::ID_VALUE);
  new FXLabel(matrix1,tr("Base Color"),NULL,LAYOUT_CENTER_Y);

  new FXColorWell(matrix1,FXRGB(0,0,255),&target_border,FXDataTarget::ID_VALUE);
  new FXLabel(matrix1,tr("Border Color"),NULL,LAYOUT_CENTER_Y);

  new FXColorWell(matrix1,FXRGB(0,0,255),&target_fore,FXDataTarget::ID_VALUE);
  new FXLabel(matrix1,tr("Text Color"),NULL,LAYOUT_CENTER_Y);

  new FXColorWell(matrix1,FXRGB(0,0,255),&target_back,FXDataTarget::ID_VALUE);
  new FXLabel(matrix1,tr("Background Color"),NULL,LAYOUT_CENTER_Y);

  new FXColorWell(matrix1,FXRGB(0,0,255),&target_selfore,FXDataTarget::ID_VALUE);
  new FXLabel(matrix1,tr("Selected Text Color"),NULL,LAYOUT_CENTER_Y);

  new FXColorWell(matrix1,FXRGB(0,0,255),&target_selback,FXDataTarget::ID_VALUE);
  new FXLabel(matrix1,tr("Selected Background Color"),NULL,LAYOUT_CENTER_Y);

  new FXColorWell(matrix1,FXRGB(0,0,255),&target_menufore,FXDataTarget::ID_VALUE);
  new FXLabel(matrix1,tr("Selected Menu Text Color"),NULL,LAYOUT_CENTER_Y);

  new FXColorWell(matrix1,FXRGB(0,0,255),&target_menuback,FXDataTarget::ID_VALUE);
  new FXLabel(matrix1,tr("Selected Menu Background Color"),NULL,LAYOUT_CENTER_Y);

  new FXColorWell(matrix1,FXRGB(0,0,255),&target_tipfore,FXDataTarget::ID_VALUE);
  new FXLabel(matrix1,tr("Tip Text Color"),NULL,LAYOUT_CENTER_Y);

  new FXColorWell(matrix1,FXRGB(0,0,255),&target_tipback,FXDataTarget::ID_VALUE);
  new FXLabel(matrix1,tr("Tip Background Color"),NULL,LAYOUT_CENTER_Y);

  // Sample gui fragment showing colors
  FXVerticalFrame* frame2=new FXVerticalFrame(hframe1,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING,0,0);

  tabbook=new FXTabBook(frame2,NULL,0,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,0,0,0,0);
  tabitem=new FXTabItem(tabbook,tr(" Item 1 "));
  tabframe=new FXVerticalFrame(tabbook,LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_THICK|FRAME_RAISED);

  labeltextframe1=new FXHorizontalFrame(tabframe,LAYOUT_FILL_X);
  label1=new FXLabel(labeltextframe1,tr("Label with Text"),NULL);
  textfield1=new FXTextField(labeltextframe1,30,NULL,0,LAYOUT_FILL_X|FRAME_THICK|FRAME_SUNKEN);
  textfield1->setText(tr("Select this text, to see the selected colors"));

  labeltextframe2=new FXHorizontalFrame(tabframe,LAYOUT_FILL_X);
  textframe1=new FXHorizontalFrame(labeltextframe2,LAYOUT_FILL_X|FRAME_THICK|FRAME_SUNKEN,0,0,0,0,2,2,2,2,0,0);
  label3=new FXLabel(textframe1,tr("Selected Text (with focus)"),NULL,LAYOUT_FILL_X,0,0,0,0,1,1,1,1);
  textframe2=new FXHorizontalFrame(labeltextframe2,LAYOUT_FILL_X|FRAME_THICK|FRAME_SUNKEN,0,0,0,0,2,2,2,2,0,0);
  label4=new FXLabel(textframe2,tr("Selected Text (no focus)"),NULL,LAYOUT_FILL_X,0,0,0,0,1,1,1,1);

  sep1=new FXSeparator(tabframe,LAYOUT_FILL_X|SEPARATOR_LINE);

  tabsubframe=new FXHorizontalFrame(tabframe,LAYOUT_FILL_X|LAYOUT_FILL_Y);

  grpbox1=new FXGroupBox(tabsubframe,tr("MenuPane"),FRAME_GROOVE|LAYOUT_FILL_Y|LAYOUT_FILL_X);

  menuframe=new FXVerticalFrame(grpbox1,FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X|LAYOUT_CENTER_Y,0,0,0,0,0,0,0,0,0,0);
  menulabels[0]=new FXLabel(menuframe,tr("&Open"),NULL,LABEL_NORMAL,0,0,0,0,16,4);
  menulabels[1]=new FXLabel(menuframe,tr("S&ave"),NULL,LABEL_NORMAL,0,0,0,0,16,4);
  sep2=new FXSeparator(menuframe,LAYOUT_FILL_X|SEPARATOR_GROOVE);
  menulabels[2]=new FXLabel(menuframe,tr("I&mport"),NULL,LABEL_NORMAL,0,0,0,0,16,4);
  menulabels[4]=new FXLabel(menuframe,tr("Selected Menu Entry"),NULL,LABEL_NORMAL,0,0,0,0,16,4);
  menulabels[3]=new FXLabel(menuframe,tr("Print"),NULL,LABEL_NORMAL,0,0,0,0,16,4);
  sep3=new FXSeparator(menuframe,LAYOUT_FILL_X|SEPARATOR_GROOVE);
  menulabels[5]=new FXLabel(menuframe,tr("&Quit"),NULL,LABEL_NORMAL,0,0,0,0,16,4);

  grpbox2=new FXGroupBox(tabsubframe,tr("Tooltips"),FRAME_GROOVE|LAYOUT_FILL_Y|LAYOUT_FILL_X);

  label2=new FXLabel(grpbox2,tr("Sample Tooltip"),NULL,FRAME_LINE|LAYOUT_CENTER_X);
  label5=new FXLabel(grpbox2,tr("Multiline Sample\n Tooltip"),NULL,FRAME_LINE|LAYOUT_CENTER_X);

  FXHorizontalFrame* hframe2=new FXHorizontalFrame(vframe4,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING);

  new FXLabel(hframe2,tr("Normal Font: "),NULL,LAYOUT_CENTER_Y);
  fontbutton=new FXButton(hframe2," ",NULL,this,ID_CHOOSE_FONT,LAYOUT_CENTER_Y|FRAME_RAISED|JUSTIFY_CENTER_X|JUSTIFY_CENTER_Y|LAYOUT_FILL_X);

  /// File Binding Panel ///
  FXVerticalFrame* vframe5=new FXVerticalFrame(switcher,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,0,0,0,0,0,0);

  // Icon search path
  FXHorizontalFrame* hframe3=new FXHorizontalFrame(vframe5,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING);
  new FXLabel(hframe3,tr("Icon Search Path:"),NULL,LAYOUT_CENTER_Y);
  new FXTextField(hframe3,2,&target_iconpath,FXDataTarget::ID_VALUE,LAYOUT_SIDE_LEFT|LAYOUT_FILL_X|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK);

  new FXSeparator(vframe5,SEPARATOR_GROOVE|LAYOUT_FILL_X);

  FXHorizontalFrame* hframe4=new FXHorizontalFrame(vframe5,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,0,0,0,0,0,0);

  // File extensions
  FXVerticalFrame* vframe6=new FXVerticalFrame(hframe4,LAYOUT_FILL_Y,0,0,0,0,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING);

  // Caption above file extensions list
  new FXLabel(vframe6,tr("File Extensions: "));
  FXVerticalFrame* frame3=new FXVerticalFrame(vframe6,FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_Y|LAYOUT_FILL_X,0,0,0,0,0,0,0,0);

  // List of file bindings
  filebindinglist=new FXList(frame3,this,ID_SELECT_FILEBINDING,LAYOUT_FILL_Y|LAYOUT_FILL_X|LIST_BROWSESELECT);
  filebindinglist->setSortFunc(FXList::ascending);

  // Buttons
  FXHorizontalFrame *listbuttonframe=new FXHorizontalFrame(vframe6,PACK_UNIFORM_WIDTH|LAYOUT_FILL_X,0,0,0,0,0,0,0,0);
  new FXButton(listbuttonframe,tr("&New\tAdd New Binding"),NULL,this,ID_CREATE_FILEBINDING);
  new FXButton(listbuttonframe,tr("&Delete\tDelete Selected Binding"),NULL,this,ID_REMOVE_FILEBINDING);
  new FXButton(listbuttonframe,tr("&Edit\tEdit Binding Name"),NULL,this,ID_RENAME_FILEBINDING);

  new FXSeparator(hframe4,SEPARATOR_GROOVE|LAYOUT_FILL_Y);

  // Associated stuff
  FXVerticalFrame* vframe7=new FXVerticalFrame(hframe4,LAYOUT_FILL_Y|LAYOUT_FILL_X,0,0,0,0,0,0,0,0,0,0);

  // Description
  FXPacker *packer=new FXPacker(vframe7,LAYOUT_FILL_X,0,0,0,0,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING);
  new FXLabel(packer,tr("Description:"));
  new FXTextField(packer,2,&target_filebinding_description,FXDataTarget::ID_VALUE,LAYOUT_SIDE_LEFT|LAYOUT_FILL_X|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK);

  new FXSeparator(vframe7,SEPARATOR_GROOVE|LAYOUT_FILL_X);

  // Command to execute
  packer=new FXPacker(vframe7,LAYOUT_FILL_X,0,0,0,0,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING);
  new FXLabel(packer,"Command:");
  FXCheckButton *checkbutton1=new FXCheckButton(packer,tr("Run in terminal"),NULL,0,ICON_BEFORE_TEXT|LAYOUT_LEFT|LAYOUT_SIDE_BOTTOM);
  FXCheckButton *checkbutton2=new FXCheckButton(packer,tr("Change directory"),NULL,0,ICON_BEFORE_TEXT|LAYOUT_LEFT|LAYOUT_SIDE_BOTTOM);
  checkbutton1->disable();
  checkbutton2->disable();
  new FXButton(packer,"...",NULL,this,ID_SELECT_COMMAND,LAYOUT_SIDE_RIGHT|LAYOUT_CENTER_Y|FRAME_RAISED|FRAME_THICK);
  new FXTextField(packer,2,&target_filebinding_command,FXDataTarget::ID_VALUE,LAYOUT_SIDE_LEFT|LAYOUT_FILL_X|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK);

  new FXSeparator(vframe7,SEPARATOR_GROOVE|LAYOUT_FILL_X);

  // Mime types
  packer=new FXPacker(vframe7,LAYOUT_FILL_X,0,0,0,0,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING);
  new FXLabel(packer,tr("Mime Type:"));
  mimetypelist=new FXComboBox(packer,1,this,ID_SELECT_MIMETYPE,LAYOUT_FILL_X|FRAME_SUNKEN|FRAME_THICK|COMBOBOX_NO_REPLACE);
  mimetypelist->setSortFunc(FXList::ascending);
  mimetypelist->setNumVisible(9);

  new FXSeparator(vframe7,SEPARATOR_GROOVE|LAYOUT_FILL_X);

  // Icons to show
  FXMatrix *iconsmatrix=new FXMatrix(vframe7,4,MATRIX_BY_COLUMNS|LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING);
  new FXLabel(iconsmatrix,tr("Small"),NULL,LAYOUT_CENTER_X|LAYOUT_FILL_COLUMN);
  new FXLabel(iconsmatrix,tr("Big"),NULL,LAYOUT_CENTER_X|LAYOUT_FILL_COLUMN);
  new FXLabel(iconsmatrix,tr("Small Open"),NULL,LAYOUT_CENTER_X|LAYOUT_FILL_COLUMN);
  new FXLabel(iconsmatrix,tr("Big Open"),NULL,LAYOUT_CENTER_X|LAYOUT_FILL_COLUMN);
  iconbutton[MINI_ICON]=new FXButton(iconsmatrix,tr("\tChange icon"),NULL,this,ID_SELECT_ICON_MINI,FRAME_RAISED|LAYOUT_CENTER_X|LAYOUT_CENTER_Y|JUSTIFY_CENTER_X|JUSTIFY_CENTER_Y|LAYOUT_FILL_COLUMN|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT,0,0,68,68, 1,1,1,1);
  iconbutton[BIG_ICON]=new FXButton(iconsmatrix,tr("\tChange icon"),NULL,this,ID_SELECT_ICON_BIG,FRAME_RAISED|LAYOUT_CENTER_X|LAYOUT_CENTER_Y|JUSTIFY_CENTER_X|JUSTIFY_CENTER_Y|LAYOUT_FILL_COLUMN|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT,0,0,68,68, 1,1,1,1);
  iconbutton[MINI_ICON_OPEN]=new FXButton(iconsmatrix,tr("\tChange icon"),NULL,this,ID_SELECT_ICON_MINIOPEN,FRAME_RAISED|LAYOUT_CENTER_X|LAYOUT_CENTER_Y|JUSTIFY_CENTER_X|JUSTIFY_CENTER_Y|LAYOUT_FILL_COLUMN|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT,0,0,68,68, 1,1,1,1);
  iconbutton[BIG_ICON_OPEN]=new FXButton(iconsmatrix,tr("\tChange icon"),NULL,this,ID_SELECT_ICON_BIGOPEN,FRAME_RAISED|LAYOUT_CENTER_X|LAYOUT_CENTER_Y|JUSTIFY_CENTER_X|JUSTIFY_CENTER_Y|LAYOUT_FILL_COLUMN|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT,0,0,68,68, 1,1,1,1);

  /// Miscellaneous Parameters Panel ///
  FXHorizontalFrame* hframe5=new FXHorizontalFrame(switcher,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,0,0,0,0,0,0);
  FXMatrix* matrix2=new FXMatrix(hframe5,3,LAYOUT_FILL_Y|MATRIX_BY_COLUMNS,0,0,0,0,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING);

  // Type speed
  new FXLabel(matrix2,tr("Typing Speed\t\tTyping Speed"),NULL,LAYOUT_RIGHT|LAYOUT_CENTER_Y);
  FXSpinner* spinner1=new FXSpinner(matrix2,4,&target_typingspeed,FXDataTarget::ID_VALUE,FRAME_SUNKEN|FRAME_THICK);
  spinner1->setRange(1,9999);
  spinner1->setIncrement(500);
  new FXLabel(matrix2,"ms",NULL,LAYOUT_CENTER_Y);

  // Double click speed
  new FXLabel(matrix2,tr("Double Click Speed\t\tDouble Click Speed"),NULL,LAYOUT_RIGHT|LAYOUT_CENTER_Y);
  FXSpinner* spinner2=new FXSpinner(matrix2,4,&target_clickspeed,FXDataTarget::ID_VALUE,FRAME_SUNKEN|FRAME_THICK);
  spinner2->setRange(1,9999);
  spinner2->setIncrement(100);
  new FXLabel(matrix2,"ms",NULL,LAYOUT_CENTER_Y);

  // Scroll speed
  new FXLabel(matrix2,tr("Scroll Speed\t\tScroll Speed"),NULL,LAYOUT_RIGHT|LAYOUT_CENTER_Y);
  FXSpinner* spinner3=new FXSpinner(matrix2,4,&target_scrollspeed,FXDataTarget::ID_VALUE,FRAME_SUNKEN|FRAME_THICK);
  spinner3->setRange(1,9999);
  spinner3->setIncrement(10);
  new FXLabel(matrix2,"ms",NULL,LAYOUT_CENTER_Y);

  // Scroll delay
  new FXLabel(matrix2,tr("Scroll Delay\t\tScroll Delay"),NULL,LAYOUT_RIGHT|LAYOUT_CENTER_Y);
  FXSpinner* spinner4=new FXSpinner(matrix2,4,&target_scrolldelay,FXDataTarget::ID_VALUE,FRAME_SUNKEN|FRAME_THICK);
  spinner4->setRange(1,9999);
  spinner4->setIncrement(100);
  new FXLabel(matrix2,"ms",NULL,LAYOUT_CENTER_Y);

  // Cursor blink speed
  new FXLabel(matrix2,tr("Cursor Blink Speed\t\tCursor Blink Speed"),NULL,LAYOUT_RIGHT|LAYOUT_CENTER_Y);
  FXSpinner* spinner5=new FXSpinner(matrix2,4,&target_blinkspeed,FXDataTarget::ID_VALUE,FRAME_SUNKEN|FRAME_THICK);
  spinner5->setRange(1,9999);
  spinner5->setIncrement(100);
  new FXLabel(matrix2,"ms",NULL,LAYOUT_CENTER_Y);

  // Animation speed
  new FXLabel(matrix2,tr("Animation Speed\t\tAnimation Speed"),NULL,LAYOUT_RIGHT|LAYOUT_CENTER_Y);
  FXSpinner* spinner6=new FXSpinner(matrix2,4,&target_animspeed,FXDataTarget::ID_VALUE,FRAME_SUNKEN|FRAME_THICK);
  spinner6->setRange(1,9999);
  spinner6->setIncrement(1);
  new FXLabel(matrix2,"ms",NULL,LAYOUT_CENTER_Y);

  // Popup menu delay
  new FXLabel(matrix2,tr("Cascade Menu Popup Delay\t\tAmount of delay before cascading menu is shown"),NULL,LAYOUT_RIGHT|LAYOUT_CENTER_Y);
  FXSpinner* spinner7=new FXSpinner(matrix2,4,&target_menupause,FXDataTarget::ID_VALUE,FRAME_SUNKEN|FRAME_THICK);
  spinner7->setRange(1,9999);
  spinner7->setIncrement(100);
  new FXLabel(matrix2,"ms",NULL,LAYOUT_CENTER_Y);

  // Tooltip popup delay
  new FXLabel(matrix2,tr("Tooltip Popup Delay\t\tAmount of Delay before tooltip is shown "),NULL,LAYOUT_RIGHT|LAYOUT_CENTER_Y);
  FXSpinner* spinner8=new FXSpinner(matrix2,4,&target_tooltippause,FXDataTarget::ID_VALUE,FRAME_SUNKEN|FRAME_THICK);
  spinner8->setRange(1,9999);
  spinner8->setIncrement(100);
  new FXLabel(matrix2,"ms",NULL,LAYOUT_CENTER_Y);

  // Tooltip display time
  new FXLabel(matrix2,tr("Tooltip Time\t\tTime that tooltips are shown"),NULL,LAYOUT_RIGHT|LAYOUT_CENTER_Y);
  FXSpinner* spinner9=new FXSpinner(matrix2,4,&target_tooltiptime,FXDataTarget::ID_VALUE,FRAME_SUNKEN|FRAME_THICK);
  spinner9->setRange(1,9999);
  spinner9->setIncrement(500);
  new FXLabel(matrix2,"ms",NULL,LAYOUT_CENTER_Y);

  // Drag delta jitter
  new FXLabel(matrix2,tr("Drag Delta\t\tMinimum distance considered a mouse move"),NULL,LAYOUT_RIGHT|LAYOUT_CENTER_Y);
  FXSpinner* spinner10=new FXSpinner(matrix2,4,&target_dragdelta,FXDataTarget::ID_VALUE,FRAME_SUNKEN|FRAME_THICK);
  spinner10->setRange(1,9999);
  spinner10->setIncrement(1);
  new FXFrame(matrix2,FRAME_NONE);

  // Mouse wheel lines
  new FXLabel(matrix2,tr("Wheel Lines"),NULL,LAYOUT_RIGHT|LAYOUT_CENTER_Y);
  FXSpinner* spinner11=new FXSpinner(matrix2,4,&target_wheellines,FXDataTarget::ID_VALUE,FRAME_SUNKEN|FRAME_THICK);
  spinner11->setRange(1,100);

  new FXSeparator(hframe5,SEPARATOR_GROOVE|LAYOUT_FILL_Y);

  FXMatrix* matrix3=new FXMatrix(hframe5,2,LAYOUT_FILL_Y|MATRIX_BY_COLUMNS,0,0,0,0,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING);

  // Maximum colors spinner
  new FXLabel(matrix3,tr("Maximum Colors Allocated"));
  FXSpinner* spinner12=new FXSpinner(matrix3,3,&target_maxcolors,FXDataTarget::ID_VALUE,FRAME_SUNKEN|FRAME_THICK);
  spinner12->setRange(1,256);

  // Gamma correction spinner
  new FXLabel(matrix3,tr("Gamma Correction"));
  FXRealSpinner* spinner13=new FXRealSpinner(matrix3,3,&target_gamma,FXDataTarget::ID_VALUE,FRAME_SUNKEN|FRAME_THICK);
  spinner13->setRange(0.0,5.0);
  spinner13->setIncrement(0.1);

  // Close button etc.
  new FXSeparator(main,SEPARATOR_GROOVE|LAYOUT_FILL_X);
  FXHorizontalFrame *closebox=new FXHorizontalFrame(main,LAYOUT_BOTTOM|LAYOUT_FILL_X,0,0,0,0,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING);
  vendorandapplication=new FXLabel(closebox,FXString::null,NULL,LAYOUT_FILL_X|LAYOUT_CENTER_Y|JUSTIFY_LEFT,0,0,0,0,15);
  vendorandapplication->disable();
  new FXButton(closebox,tr("&Close"),NULL,this,FXTopWindow::ID_CLOSE,BUTTON_INITIAL|BUTTON_DEFAULT|LAYOUT_RIGHT|FRAME_RAISED|FRAME_THICK,0,0,0,0,20,20);

  // Tool tip
  tooltip=new FXToolTip(getApp());

  // Color data targets associations
  target_base.connect(theme_current.base,this,ID_COLORS);
  target_back.connect(theme_current.back,this,ID_COLORS);
  target_border.connect(theme_current.border,this,ID_COLORS);
  target_fore.connect(theme_current.fore,this,ID_COLORS);
  target_selfore.connect(theme_current.selfore,this,ID_COLORS);
  target_selback.connect(theme_current.selback,this,ID_COLORS);
  target_tipfore.connect(theme_current.tipfore,this,ID_COLORS);
  target_tipback.connect(theme_current.tipback,this,ID_COLORS);
  target_menufore.connect(theme_current.menufore,this,ID_COLORS);
  target_menuback.connect(theme_current.menuback,this,ID_COLORS);

  // Miscellaneous data target assocations
  target_typingspeed.connect(typingSpeed);
  target_clickspeed.connect(clickSpeed);
  target_scrollspeed.connect(scrollSpeed);
  target_scrolldelay.connect(scrollDelay);
  target_blinkspeed.connect(blinkSpeed);
  target_animspeed.connect(animSpeed);
  target_menupause.connect(menuPause);
  target_tooltippause.connect(tooltipPause);
  target_tooltiptime.connect(tooltipTime);
  target_dragdelta.connect(dragDelta);
  target_wheellines.connect(wheelLines);
  target_maxcolors.connect(maxcolors);
  target_gamma.connect(gamma);

  // File data target associations
  target_filebinding_description.connect(filebinding.description);
  target_filebinding_command.connect(filebinding.command);
  target_iconpath.connect(iconpath);
  }


// Create window
void FXDesktopSetup::create(){
  FXMainWindow::create();
  show(PLACEMENT_SCREEN);
  }


// Close main window and terminate the application
FXbool FXDesktopSetup::close(FXbool notify){
  FXint result=FXMessageBox::question(this,MBOX_SAVE_CANCEL_DONTSAVE,tr("Save Changes?"),tr("Do you want to save changes to the FOX Registry\nbefore closing?\n\nIf you don't save, your changes will be lost."));
  if(result!=MBOX_CLICKED_CANCEL){
    if(result==MBOX_CLICKED_SAVE){
      saveFileBinding();
      writeSettingsFile(filename);
      }
    return FXMainWindow::close(notify);
    }
  return false;
  }


// Delete window
FXDesktopSetup::~FXDesktopSetup(){
  delete titlefont;
  delete font;
  delete desktopicon;
  delete icon_colors;
  delete icon_settings;
  delete icon_filebinding;
  }


// Search iconpath for given name and load the icon
FXIcon *FXDesktopSetup::createIconFromName(const FXString& name) const {
  FXIconSource iconsource(getApp());
  FXString iconfilename=FXPath::search(iconpath,name);
  if(!iconfilename.empty()){
    FXIcon *icon=iconsource.loadIconFile(iconfilename);
    if(icon){
      icon->blend(getApp()->getBaseColor());
      icon->create();
      return icon;
      }
    }
  return NULL;
  }


// Reflect icon on button
void FXDesktopSetup::setupIconButton(const FXString& name,FXint index){
  FXIcon *icon=iconbutton[index]->getIcon();
  if(icon){
    iconbutton[index]->setIcon(NULL);
    delete icon;
    }
  icon=createIconFromName(name);
  if(icon){
    icon->create();
    iconbutton[index]->setIcon(icon);
    }
  }


// Save file binding entry
void FXDesktopSetup::saveFileBinding(){
  if(!filebinding.key.empty()){
    FXString entry=filebinding.command+";"+filebinding.description+";";
    if(filebinding.iconfile[BIG_ICON_OPEN].empty())
      entry+=filebinding.iconfile[BIG_ICON]+";";
    else
      entry+=filebinding.iconfile[BIG_ICON]+":"+filebinding.iconfile[BIG_ICON_OPEN]+";";
    if(filebinding.iconfile[MINI_ICON_OPEN].empty())
      entry+=filebinding.iconfile[MINI_ICON]+";";
    else
      entry+=filebinding.iconfile[MINI_ICON]+":" + filebinding.iconfile[MINI_ICON_OPEN]+";";
    entry+=filebinding.mime;
    desktopsettings.writeStringEntry("FILETYPES",filebinding.key.text(),entry.text());
    }
  }


// Set command association
long FXDesktopSetup::onCmdSelectCommand(FXObject*,FXSelector,void*){
  FXString path=(filebinding.command.empty()) ? "/usr/bin/" : filebinding.command.text();
  FXString command=FXFileDialog::getOpenFilename(this,"Select Command",path,"*");
  if(!command.empty()){
    filebinding.command=command;
    }
  return 1;
  }


// Selected (new) mimetype
long FXDesktopSetup::onCmdMimeType(FXObject*,FXSelector,void* ptr){
  FXString mime=(const FXchar*)ptr;
  if(!mime.empty() && (mimetypelist->findItem(mime)==-1)){
    mimetypelist->appendItem(mime);
    mimetypelist->sortItems();
    }
  filebinding.mime=mime;
  return 1;
  }


// Selected file binding from list
long FXDesktopSetup::onCmdFileBinding(FXObject*,FXSelector,void* ptr){
  FXint index=(FXint)(FXival)ptr,no;
  FXString association;
  FXString iconname;
  FXIcon *icn;

  // Save old one
  saveFileBinding();

  // New item selected
  if(0<=index){

    // Get extension
    filebinding.key=filebindinglist->getItemText(index);

    // Get associated information
    association=desktopsettings.readStringEntry("FILETYPES",filebinding.key.text());

    // Get command and description names
    filebinding.command=association.section(";",0);
    filebinding.description=association.section(";",1);

    // Big icon closed and open
    iconname=association.section(";",2);
    filebinding.iconfile[BIG_ICON]=iconname.section(":",0);
    filebinding.iconfile[BIG_ICON_OPEN]=iconname.section(":",1);

    // Small icon closed and open
    iconname=association.section(";",3);
    filebinding.iconfile[MINI_ICON]=iconname.section(":",0);
    filebinding.iconfile[MINI_ICON_OPEN]=iconname.section(":",1);

    // Mime type name
    filebinding.mime=association.section(";",4);
    if(!filebinding.mime.empty()){
      no=mimetypelist->findItem(filebinding.mime);
      mimetypelist->setCurrentItem(no);
      }
    else {
      no=mimetypelist->findItem(" ");           // FIXME
      mimetypelist->setCurrentItem(no);
      }

    // Change icons
    setupIconButton(filebinding.iconfile[BIG_ICON],BIG_ICON);
    setupIconButton(filebinding.iconfile[BIG_ICON_OPEN],BIG_ICON_OPEN);
    setupIconButton(filebinding.iconfile[MINI_ICON],MINI_ICON);
    setupIconButton(filebinding.iconfile[MINI_ICON_OPEN],MINI_ICON_OPEN);
    }
  return 1;
  }


// Add new file binding
long FXDesktopSetup::onCmdCreateFileBinding(FXObject*,FXSelector,void*){
  FXString result;
  if(FXInputDialog::getString(result,this,tr("New File Binding"),tr("Please enter filebinding key:"),NULL)){
    if(result.empty()) return 1;
    if((filebindinglist->findItem(result))>=0){
      FXMessageBox::question(this,MBOX_OK,tr("Duplicate Binding"),tr("The given key %s already exists."),result.text());
      return 1;
      }

    // Save Current
    saveFileBinding();

    filebinding.key=result;
    filebinding.command=FXString::null;
    filebinding.description=FXString::null;
    filebinding.iconfile[BIG_ICON]=FXString::null;
    filebinding.iconfile[BIG_ICON_OPEN]=FXString::null;
    filebinding.iconfile[MINI_ICON]=FXString::null;
    filebinding.iconfile[MINI_ICON_OPEN]=FXString::null;
    filebinding.mime=FXString::null;

    // Save New
    saveFileBinding();

    // Add new item
    FXint no=filebindinglist->appendItem(filebinding.key);
    filebindinglist->setCurrentItem(no,true);
    filebindinglist->sortItems();
    filebindinglist->makeItemVisible(filebindinglist->getCurrentItem());
    }
  return 1;
  }


// Remove file binding
long FXDesktopSetup::onCmdRemoveFileBinding(FXObject*,FXSelector,void*){
  if(filebinding.key.empty()) return 1;
  if(FXMessageBox::question(this,MBOX_OK_CANCEL,tr("Delete Filebinding?"),tr("Are you sure you want to delete\nthe filebinding for %s"),filebinding.key.text())==MBOX_CLICKED_OK){
    desktopsettings.deleteEntry("FILETYPES",filebinding.key);
    filebinding.key=FXString::null;
    filebindinglist->removeItem(filebindinglist->getCurrentItem(),true);
    }
  return 1;
  }


// Rename file binding
long FXDesktopSetup::onCmdRenameFileBinding(FXObject*,FXSelector,void*){
  FXint no;
  FXString result=filebinding.key;
  if(FXInputDialog::getString(result,this,tr("Rename File Binding"),tr("Rename File Binding"),NULL)){
    if(result==filebinding.key) return 1;
    if((no=filebindinglist->findItem(result))>=0){
      if(FXMessageBox::question(this,MBOX_YES_NO,tr("Replace Binding?"),tr("Would you like to replace the existing binding?"))!=MBOX_CLICKED_YES){
        return 1;
        }
      }

    // Remove old key
    desktopsettings.deleteEntry("FILETYPES",filebinding.key);
    filebinding.key=result;
    saveFileBinding();
    if(no>=0){
      FXint previtem=filebindinglist->getCurrentItem();
      filebindinglist->setCurrentItem(no,true);
      filebindinglist->removeItem(previtem);
      }
    else {
      filebindinglist->setItemText(filebindinglist->getCurrentItem(),filebinding.key);
      }
    }
  return 1;
  }


// Select new icon
long FXDesktopSetup::onCmdSelectIcon(FXObject*,FXSelector sel,void*){
  FXFileDialog opendialog(this,tr("Select Icon"));
  FXint index=FXSELID(sel)-ID_SELECT_ICON_BIG;
  FXString iconfilename=FXPath::search(iconpath,filebinding.iconfile[index]);
  opendialog.setSelectMode(SELECTFILE_EXISTING);
  opendialog.showImages(true);
  if(iconfilename.empty()) iconfilename=FXPath::search(iconpath,".");
  opendialog.setFilename(iconfilename);
  if(opendialog.execute()){
    iconfilename=opendialog.getFilename();
    filebinding.iconfile[index]=FXPath::relativize(iconpath,iconfilename);
    setupIconButton(filebinding.iconfile[index],index);
    }
  return 1;
  }


// Change font
long FXDesktopSetup::onChooseFont(FXObject*,FXSelector,void*){
  FXFontDialog dialog(this,tr("Select Normal Font"));
  dialog.setFont(fontspec);
  if(dialog.execute(PLACEMENT_SCREEN)){
    fontspec=dialog.getFont();
    setupFont();
    }
  return 1;
  }

/*******************************************************************************/

// Changed color, update sampler display
long FXDesktopSetup::onColorChanged(FXObject*,FXSelector,void*){
  list->setCurrentItem(list->getNumItems()-1);
  setupColors();
  return 1;
  }


// Picked new theme
long FXDesktopSetup::onColorTheme(FXObject*,FXSelector,void* ptr){
  ColorTheme *theme_selected=reinterpret_cast<ColorTheme*>(list->getItemData((FXint)(FXival)ptr));
  if(theme_selected){
    theme_current.base     = theme_selected->base;
    theme_current.border   = theme_selected->border;
    theme_current.back     = theme_selected->back;
    theme_current.fore     = theme_selected->fore;
    theme_current.selfore  = theme_selected->selfore;
    theme_current.selback  = theme_selected->selback;
    theme_current.tipfore  = theme_selected->tipfore;
    theme_current.tipback  = theme_selected->tipback;
    theme_current.menufore = theme_selected->menufore;
    theme_current.menuback = theme_selected->menuback;
    setupColors();
    }
  return 1;
  }

/*******************************************************************************/


// Update dialog according to new font
void FXDesktopSetup::setupFont(){
  FXFont *oldfont=font;
  font=new FXFont(getApp(),fontspec);
  font->create();
  delete oldfont;
  tabitem->setFont(font);
  label1->setFont(font);
  label2->setFont(font);
  label3->setFont(font);
  label4->setFont(font);
  label5->setFont(font);
  menulabels[0]->setFont(font);
  menulabels[1]->setFont(font);
  menulabels[2]->setFont(font);
  menulabels[3]->setFont(font);
  menulabels[4]->setFont(font);
  menulabels[5]->setFont(font);
  textfield1->setFont(font);
  fontbutton->setText(fontspec);
  }


// Update controls of file bindings
void FXDesktopSetup::setupFileBindings(){
  FXStringDict *prefs=desktopsettings.find("FILETYPES");
  filebindinglist->clearItems();
  mimetypelist->clearItems();
  if(prefs){
    FXString string,mime;
    for(FXint e=prefs->first(); e<prefs->size(); e=prefs->next(e)){
      filebindinglist->appendItem(prefs->key(e),NULL,NULL,true);
      string=prefs->data(e);
      mime=string.section(";",4);
      if(!mime.empty() && (mimetypelist->findItem(mime)==-1)){
        mimetypelist->appendItem(mime);
        }
      }
    }
  filebindinglist->sortItems();
  mimetypelist->sortItems();
  }


// Initialize colors
void FXDesktopSetup::initColors(){
  FXint i,scheme=-1;

  // Find the correct current scheme
  for(i=0; i<numThemes; i++){
    if((theme_current.base==ColorThemes[i].base) &&
       (theme_current.border==ColorThemes[i].border) &&
       (theme_current.back==ColorThemes[i].back) &&
       (theme_current.fore==ColorThemes[i].fore) &&
       (theme_current.selfore==ColorThemes[i].selfore) &&
       (theme_current.selback==ColorThemes[i].selback) &&
       (theme_current.menufore==ColorThemes[i].menufore) &&
       (theme_current.menuback==ColorThemes[i].menuback) &&
       (theme_current.tipfore==ColorThemes[i].tipfore) &&
       (theme_current.tipback==ColorThemes[i].tipback)){
      scheme=i;
      break;
      }
    }

  // Clear items
  list->clearItems();

  // Add Standard Themes to List
  for(i=0; i<numThemes; i++){
    list->appendItem(tr(ColorThemes[i].name),NULL,(void*)&ColorThemes[i]);
    }

  // Custom colors were used
  if(scheme==-1){
    theme_user.base=theme_current.base;
    theme_user.border=theme_current.border;
    theme_user.back=theme_current.back;
    theme_user.fore=theme_current.fore;
    theme_user.selfore=theme_current.selfore;
    theme_user.selback=theme_current.selback;
    theme_user.menufore=theme_current.menufore;
    theme_user.menuback=theme_current.menuback;
    theme_user.tipfore=theme_current.tipfore;
    theme_user.tipback=theme_current.tipback;
    scheme=list->getNumItems();
    list->appendItem(tr("Current"),NULL,&theme_user);
    }

  // User defined theme
  list->appendItem(tr("User Defined"));
  list->setCurrentItem(scheme);
  }


// Update sampler
void FXDesktopSetup::setupColors(){
  FXColor shadow=makeShadowColor(theme_current.base);
  FXColor hilite=makeHiliteColor(theme_current.base);

  tabitem->setBorderColor(theme_current.border);
  tabitem->setBaseColor(theme_current.base);
  tabitem->setBackColor(theme_current.base);
  tabitem->setTextColor(theme_current.fore);
  tabitem->setShadowColor(shadow);
  tabitem->setHiliteColor(hilite);

  tabframe->setBorderColor(theme_current.border);
  tabframe->setBaseColor(theme_current.base);
  tabframe->setBackColor(theme_current.base);
  tabframe->setShadowColor(shadow);
  tabframe->setHiliteColor(hilite);


  tabsubframe->setBorderColor(theme_current.border);
  tabsubframe->setBaseColor(theme_current.base);
  tabsubframe->setBackColor(theme_current.base);
  tabsubframe->setShadowColor(shadow);
  tabsubframe->setHiliteColor(hilite);

  menuframe->setBorderColor(theme_current.border);
  menuframe->setBaseColor(theme_current.base);
  menuframe->setBackColor(theme_current.base);
  menuframe->setShadowColor(shadow);
  menuframe->setHiliteColor(hilite);

  grpbox1->setBorderColor(theme_current.border);
  grpbox1->setBaseColor(theme_current.base);
  grpbox1->setBackColor(theme_current.base);
  grpbox1->setShadowColor(shadow);
  grpbox1->setHiliteColor(hilite);
  grpbox1->setTextColor(theme_current.fore);

  grpbox2->setBorderColor(theme_current.border);
  grpbox2->setBaseColor(theme_current.base);
  grpbox2->setBackColor(theme_current.base);
  grpbox2->setShadowColor(shadow);
  grpbox2->setHiliteColor(hilite);
  grpbox2->setTextColor(theme_current.fore);

  sep1->setBorderColor(theme_current.border);
  sep1->setBaseColor(theme_current.base);
  sep1->setBackColor(theme_current.base);
  sep1->setShadowColor(shadow);
  sep1->setHiliteColor(hilite);

  sep2->setBorderColor(theme_current.border);
  sep2->setBaseColor(theme_current.base);
  sep2->setBackColor(theme_current.base);
  sep2->setShadowColor(shadow);
  sep2->setHiliteColor(hilite);

  sep3->setBorderColor(theme_current.border);
  sep3->setBaseColor(theme_current.base);
  sep3->setBackColor(theme_current.base);
  sep3->setShadowColor(shadow);
  sep3->setHiliteColor(hilite);

  labeltextframe1->setBorderColor(theme_current.border);
  labeltextframe1->setBaseColor(theme_current.base);
  labeltextframe1->setBackColor(theme_current.base);
  labeltextframe1->setShadowColor(shadow);
  labeltextframe1->setHiliteColor(hilite);


  labeltextframe2->setBorderColor(theme_current.border);
  labeltextframe2->setBaseColor(theme_current.base);
  labeltextframe2->setBackColor(theme_current.base);
  labeltextframe2->setShadowColor(shadow);
  labeltextframe2->setHiliteColor(hilite);

  label1->setBorderColor(theme_current.border);
  label1->setBaseColor(theme_current.base);
  label1->setBackColor(theme_current.base);
  label1->setTextColor(theme_current.fore);
  label1->setShadowColor(shadow);
  label1->setHiliteColor(hilite);

  label2->setBorderColor(theme_current.tipfore);
  label2->setBaseColor(theme_current.tipback);
  label2->setBackColor(theme_current.tipback);
  label2->setTextColor(theme_current.tipfore);
  label2->setShadowColor(shadow);
  label2->setHiliteColor(hilite);

  label3->setBorderColor(theme_current.border);
  label3->setBaseColor(theme_current.base);
  label3->setBackColor(theme_current.selback);
  label3->setTextColor(theme_current.selfore);
  label3->setShadowColor(shadow);
  label3->setHiliteColor(hilite);

  label4->setBorderColor(theme_current.border);
  label4->setBaseColor(theme_current.base);
  label4->setBackColor(theme_current.base);
  label4->setTextColor(theme_current.fore);
  label4->setShadowColor(shadow);
  label4->setHiliteColor(hilite);

  label5->setBorderColor(theme_current.tipfore);
  label5->setBaseColor(theme_current.tipback);
  label5->setBackColor(theme_current.tipback);
  label5->setTextColor(theme_current.tipfore);
  label5->setShadowColor(shadow);
  label5->setHiliteColor(hilite);

  for(FXint i=0; i<6; i++){
    menulabels[i]->setBorderColor(theme_current.border);
    menulabels[i]->setBaseColor(theme_current.base);
    menulabels[i]->setBackColor(theme_current.base);
    menulabels[i]->setTextColor(theme_current.fore);
    menulabels[i]->setShadowColor(shadow);
    menulabels[i]->setHiliteColor(hilite);
    }

  menulabels[4]->setBorderColor(theme_current.border);
  menulabels[4]->setBaseColor(theme_current.menuback);
  menulabels[4]->setBackColor(theme_current.menuback);
  menulabels[4]->setTextColor(theme_current.menufore);
  menulabels[4]->setShadowColor(shadow);
  menulabels[4]->setHiliteColor(hilite);

  textframe1->setBorderColor(theme_current.border);
  textframe1->setBaseColor(theme_current.base);
  textframe1->setBackColor(theme_current.back);
  textframe1->setShadowColor(shadow);
  textframe1->setHiliteColor(hilite);

  textframe2->setBorderColor(theme_current.border);
  textframe2->setBaseColor(theme_current.base);
  textframe2->setBackColor(theme_current.back);
  textframe2->setShadowColor(shadow);
  textframe2->setHiliteColor(hilite);

  textfield1->setBorderColor(theme_current.border);
  textfield1->setBackColor(theme_current.back);
  textfield1->setBaseColor(theme_current.base);
  textfield1->setTextColor(theme_current.fore);
  textfield1->setSelTextColor(theme_current.selfore);
  textfield1->setSelBackColor(theme_current.selback);
  textfield1->setCursorColor(theme_current.fore);
  textfield1->setShadowColor(shadow);
  textfield1->setHiliteColor(hilite);

  tooltip->setTextColor(theme_current.tipfore);
  tooltip->setBackColor(theme_current.tipback);
  }


// Change vendor and application label
void FXDesktopSetup::setupVendorAndAppLabel(){
  FXString labelname=tr("Desktop Settings");
  if(!applicationname.empty()){
    labelname=applicationname;
    if(!vendorname.empty()){
      labelname+=" [ "+vendorname+" ]";
      }
    labelname+=" Settings";
    }
  vendorandapplication->setText(labelname);
  }

/*******************************************************************************/

// Set filename from vendor and application name
FXbool FXDesktopSetup::setApplicationAndVendor(const FXString& an,const FXString& vn){

  // Path to user config directory
  FXString path=FXPath::absolute(FXPath::expand(getApp()->reg().getUserDirectory()));

  // Append path separator
  path.append(PATHSEPSTRING);

  // Common settings
  if(an.empty()){
    path.append(FXRegistry::foxrc);
    }

  // Vendor/Application settings
  else if(!vn.empty()){
    path.append(vn+PATHSEPSTRING+an+FXRegistry::ext);
    }

  // Application settings
  else{
    path.append(an+FXRegistry::ext);
    }

  // Remember all this
  setApplicationName(an);
  setVendorName(vn);
  setFilename(path);

  // Now try to read
  if(readSettingsFile(path)){
    setupVendorAndAppLabel();
    setupFileBindings();
    setupColors();
    initColors();
    setupFont();
    return true;
    }
  return false;
  }


// Read settings file
FXbool FXDesktopSetup::readSettingsFile(const FXString& file){
  const FXlong milliseconds=1000000L;

  // Read file
  if(desktopsettings.parseFile(file,true)){

    // Normal font
    FXString stockfontspec=getApp()->getNormalFont()->getFont();
    fontspec=desktopsettings.readStringEntry("SETTINGS","normalfont",stockfontspec.text());

    // Color theme
    theme_current.base=desktopsettings.readColorEntry("SETTINGS","basecolor",getApp()->getBaseColor());
    theme_current.border=desktopsettings.readColorEntry("SETTINGS","bordercolor",getApp()->getBorderColor());
    theme_current.back=desktopsettings.readColorEntry("SETTINGS","backcolor",getApp()->getBackColor());
    theme_current.fore=desktopsettings.readColorEntry("SETTINGS","forecolor",getApp()->getForeColor());
    //hilite=desktopsettings.readColorEntry("SETTINGS","hilitecolor",getApp()->getHiliteColor());
    //shadow=desktopsettings.readColorEntry("SETTINGS","shadowcolor",getApp()->getShadowColor());
    theme_current.selfore=desktopsettings.readColorEntry("SETTINGS","selforecolor",getApp()->getSelforeColor());
    theme_current.selback=desktopsettings.readColorEntry("SETTINGS","selbackcolor",getApp()->getSelbackColor());
    theme_current.tipfore=desktopsettings.readColorEntry("SETTINGS","tipforecolor",getApp()->getTipforeColor());
    theme_current.tipback=desktopsettings.readColorEntry("SETTINGS","tipbackcolor",getApp()->getTipbackColor());
    theme_current.menufore=desktopsettings.readColorEntry("SETTINGS","selmenutextcolor",getApp()->getSelMenuTextColor());
    theme_current.menuback=desktopsettings.readColorEntry("SETTINGS","selmenubackcolor",getApp()->getSelMenuBackColor());

    // General Settings
    typingSpeed=desktopsettings.readLongEntry("SETTINGS","typingspeed",getApp()->getTypingSpeed())/milliseconds;
    clickSpeed=desktopsettings.readLongEntry("SETTINGS","clickspeed",getApp()->getClickSpeed())/milliseconds;
    scrollSpeed=desktopsettings.readLongEntry("SETTINGS","scrollspeed",getApp()->getScrollSpeed())/milliseconds;
    scrollDelay=desktopsettings.readLongEntry("SETTINGS","scrolldelay",getApp()->getScrollDelay())/milliseconds;
    blinkSpeed=desktopsettings.readLongEntry("SETTINGS","blinkspeed",getApp()->getBlinkSpeed())/milliseconds;
    animSpeed=desktopsettings.readLongEntry("SETTINGS","animspeed",getApp()->getAnimSpeed())/milliseconds;
    menuPause=desktopsettings.readLongEntry("SETTINGS","menupause",getApp()->getMenuPause())/milliseconds;
    tooltipPause=desktopsettings.readLongEntry("SETTINGS","tippause",getApp()->getToolTipPause())/milliseconds;
    tooltipTime=desktopsettings.readLongEntry("SETTINGS","tiptime",getApp()->getToolTipTime())/milliseconds;

    // Icon search path
    iconpath=desktopsettings.readStringEntry("SETTINGS","iconpath",FXIconDict::defaultIconPath);

    // Mouse tweaks
    dragDelta=desktopsettings.readIntEntry("SETTINGS","dragdelta",getApp()->getDragDelta());
    wheelLines=desktopsettings.readIntEntry("SETTINGS","wheellines",getApp()->getWheelLines());

    // Display tweaks
    maxcolors=desktopsettings.readUIntEntry("SETTINGS","maxcolors",125);
    gamma=desktopsettings.readRealEntry("SETTINGS","displaygamma",1.0);
    return true;
    }
  return false;
  }


// Write settings file
FXbool FXDesktopSetup::writeSettingsFile(const FXString& file){
  const FXlong milliseconds=1000000L;

  // Normal font
  desktopsettings.writeStringEntry("SETTINGS","normalfont",fontspec.text());
  
  // Save Colors
  desktopsettings.writeColorEntry("SETTINGS","basecolor",theme_current.base);
  desktopsettings.writeColorEntry("SETTINGS","bordercolor",theme_current.border);
  desktopsettings.writeColorEntry("SETTINGS","backcolor",theme_current.back);
  desktopsettings.writeColorEntry("SETTINGS","forecolor",theme_current.fore);
  desktopsettings.writeColorEntry("SETTINGS","hilitecolor",makeHiliteColor(theme_current.base));
  desktopsettings.writeColorEntry("SETTINGS","shadowcolor",makeShadowColor(theme_current.base));
  desktopsettings.writeColorEntry("SETTINGS","selforecolor",theme_current.selfore);
  desktopsettings.writeColorEntry("SETTINGS","selbackcolor",theme_current.selback);
  desktopsettings.writeColorEntry("SETTINGS","tipforecolor",theme_current.tipfore);
  desktopsettings.writeColorEntry("SETTINGS","tipbackcolor",theme_current.tipback);
  desktopsettings.writeColorEntry("SETTINGS","selmenutextcolor",theme_current.menufore);
  desktopsettings.writeColorEntry("SETTINGS","selmenubackcolor",theme_current.menuback);

  // Save General Settings
  desktopsettings.writeLongEntry("SETTINGS","typingspeed",typingSpeed*milliseconds);
  desktopsettings.writeLongEntry("SETTINGS","clickspeed",clickSpeed*milliseconds);
  desktopsettings.writeLongEntry("SETTINGS","scrollspeed",scrollSpeed*milliseconds);
  desktopsettings.writeLongEntry("SETTINGS","scrolldelay",scrollDelay*milliseconds);
  desktopsettings.writeLongEntry("SETTINGS","blinkspeed",blinkSpeed*milliseconds);
  desktopsettings.writeLongEntry("SETTINGS","animspeed",animSpeed*milliseconds);
  desktopsettings.writeLongEntry("SETTINGS","menupause",menuPause*milliseconds);
  desktopsettings.writeLongEntry("SETTINGS","tippause",tooltipPause*milliseconds);
  desktopsettings.writeLongEntry("SETTINGS","tiptime",tooltipTime*milliseconds);

  // Icon search path
  desktopsettings.writeStringEntry("SETTINGS","iconpath",iconpath.text());

  // Mouse tweaks
  desktopsettings.writeIntEntry("SETTINGS","dragdelta",dragDelta);
  desktopsettings.writeIntEntry("SETTINGS","wheellines",wheelLines);

  // Display tweaks
  desktopsettings.writeUIntEntry("SETTINGS","maxcolors",maxcolors);
  desktopsettings.writeRealEntry("SETTINGS","displaygamma",gamma);

  // Write file
  if(desktopsettings.unparseFile(file)){
    return true;
    }
  return false;
  }

/*******************************************************************************/

// Start the program
int main(int argc,char **argv){
  FXString appnm,vndnm;

  // Make application
  FXApp application("ControlPanel","FOX-DESKTOP");

  // Open display
  application.init(argc,argv);

  // Check for arguments
  if(1<argc){
    appnm=argv[1];
    if(2<argc){
      vndnm=argv[2];
      }
    }

  // Check for help option
  if(appnm=="-h" || appnm=="--help"){
    fxmessage("Usage: Controlpanel [applicationname [vendorname]].\n");
    return 1;
    }

  // Make main window
  FXDesktopSetup *main=new FXDesktopSetup(&application);

  // Create the application
  application.create();

  // Set application and vendor names
  if(!main->setApplicationAndVendor(appnm,vndnm)){
    fxmessage("Unable to read settings for: %s\n",appnm.text());
    return 1;
    }

  // Start
  return application.run();
  }
