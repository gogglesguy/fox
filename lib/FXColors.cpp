/********************************************************************************
*                                                                               *
*                      C o l o r   N a m e   F u n c t i o n s                  *
*                                                                               *
*********************************************************************************
* Copyright (C) 1997,2020 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "fxmath.h"
#include "fxkeys.h"
#include "fxascii.h"
#include "FXArray.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXString.h"
#include "FXColors.h"

/*
  Notes:

  - Color constants inside FXColors namespace so as to avoid potential name clashes
    in application code.
  - Added color name to FXString and vice versa.  Is more convenient.
  - API of nameFromColor() is not safe; would prefer to pass size parameter.
  - The old color named "None" was renamed to "Clear".  X11 defines a preprocessor
    constant called None and namespaces can't protect against #defines [one more
    reason we like C++].
  - At some point, we would like faster color -> name mapping [name -> color is
    OK, its a binary search].
*/

#define MAXCOLORNAME 24


using namespace FX;

/*******************************************************************************/

namespace FX {

// Names of commonly used colors
const FXchar *const colorName[683]={
  "AliceBlue",
  "AntiqueWhite",
  "AntiqueWhite1",
  "AntiqueWhite2",
  "AntiqueWhite3",
  "AntiqueWhite4",
  "Aqua",
  "Aquamarine",
  "Aquamarine1",
  "Aquamarine2",
  "Aquamarine3",
  "Aquamarine4",
  "Azure",
  "Azure1",
  "Azure2",
  "Azure3",
  "Azure4",
  "Banana",
  "Beige",
  "Bisque",
  "Bisque1",
  "Bisque2",
  "Bisque3",
  "Bisque4",
  "Black",
  "BlanchedAlmond",
  "Blue",
  "Blue1",
  "Blue2",
  "Blue3",
  "Blue4",
  "BlueViolet",
  "Brick",
  "Brown",
  "Brown1",
  "Brown2",
  "Brown3",
  "Brown4",
  "Burlywood",
  "Burlywood1",
  "Burlywood2",
  "Burlywood3",
  "Burlywood4",
  "BurnedSienna",
  "BurnedUmber",
  "CadetBlue",
  "CadetBlue1",
  "CadetBlue2",
  "CadetBlue3",
  "CadetBlue4",
  "CadmiumOrange",
  "CadmiumRed",
  "CadmiumYellow",
  "Carrot",
  "Chartreuse",
  "Chartreuse1",
  "Chartreuse2",
  "Chartreuse3",
  "Chartreuse4",
  "Chocolate",
  "Chocolate1",
  "Chocolate2",
  "Chocolate3",
  "Chocolate4",
  "Cobalt",
  "CobaltGreen",
  "ColdGrey",
  "Coral",
  "Coral1",
  "Coral2",
  "Coral3",
  "Coral4",
  "CornflowerBlue",
  "Cornsilk",
  "Cornsilk1",
  "Cornsilk2",
  "Cornsilk3",
  "Cornsilk4",
  "Crimson",
  "Cyan",
  "Cyan1",
  "Cyan2",
  "Cyan3",
  "Cyan4",
  "DarkBlue",
  "DarkCyan",
  "DarkGoldenrod",
  "DarkGoldenrod1",
  "DarkGoldenrod2",
  "DarkGoldenrod3",
  "DarkGoldenrod4",
  "DarkGray",
  "DarkGreen",
  "DarkGrey",
  "DarkKhaki",
  "DarkMagenta",
  "DarkOliveGreen",
  "DarkOliveGreen1",
  "DarkOliveGreen2",
  "DarkOliveGreen3",
  "DarkOliveGreen4",
  "DarkOrange",
  "DarkOrange1",
  "DarkOrange2",
  "DarkOrange3",
  "DarkOrange4",
  "DarkOrchid",
  "DarkOrchid1",
  "DarkOrchid2",
  "DarkOrchid3",
  "DarkOrchid4",
  "DarkRed",
  "DarkSalmon",
  "DarkSeaGreen",
  "DarkSeaGreen1",
  "DarkSeaGreen2",
  "DarkSeaGreen3",
  "DarkSeaGreen4",
  "DarkSlateBlue",
  "DarkSlateGray",
  "DarkSlateGray1",
  "DarkSlateGray2",
  "DarkSlateGray3",
  "DarkSlateGray4",
  "DarkSlateGrey",
  "DarkTurquoise",
  "DarkViolet",
  "DeepPink",
  "DeepPink1",
  "DeepPink2",
  "DeepPink3",
  "DeepPink4",
  "DeepSkyBlue",
  "DeepSkyBlue1",
  "DeepSkyBlue2",
  "DeepSkyBlue3",
  "DeepSkyBlue4",
  "DimGray",
  "DimGrey",
  "DodgerBlue",
  "DodgerBlue1",
  "DodgerBlue2",
  "DodgerBlue3",
  "DodgerBlue4",
  "Eggshell",
  "EmeraldGreen",
  "Firebrick",
  "Firebrick1",
  "Firebrick2",
  "Firebrick3",
  "Firebrick4",
  "FloralWhite",
  "ForestGreen",
  "Fuchsia",
  "Gainsboro",
  "GhostWhite",
  "Gold",
  "Gold1",
  "Gold2",
  "Gold3",
  "Gold4",
  "Goldenrod",
  "Goldenrod1",
  "Goldenrod2",
  "Goldenrod3",
  "Goldenrod4",
  "Gray",
  "Gray0",
  "Gray1",
  "Gray10",
  "Gray100",
  "Gray11",
  "Gray12",
  "Gray13",
  "Gray14",
  "Gray15",
  "Gray16",
  "Gray17",
  "Gray18",
  "Gray19",
  "Gray2",
  "Gray20",
  "Gray21",
  "Gray22",
  "Gray23",
  "Gray24",
  "Gray25",
  "Gray26",
  "Gray27",
  "Gray28",
  "Gray29",
  "Gray3",
  "Gray30",
  "Gray31",
  "Gray32",
  "Gray33",
  "Gray34",
  "Gray35",
  "Gray36",
  "Gray37",
  "Gray38",
  "Gray39",
  "Gray4",
  "Gray40",
  "Gray41",
  "Gray42",
  "Gray43",
  "Gray44",
  "Gray45",
  "Gray46",
  "Gray47",
  "Gray48",
  "Gray49",
  "Gray5",
  "Gray50",
  "Gray51",
  "Gray52",
  "Gray53",
  "Gray54",
  "Gray55",
  "Gray56",
  "Gray57",
  "Gray58",
  "Gray59",
  "Gray6",
  "Gray60",
  "Gray61",
  "Gray62",
  "Gray63",
  "Gray64",
  "Gray65",
  "Gray66",
  "Gray67",
  "Gray68",
  "Gray69",
  "Gray7",
  "Gray70",
  "Gray71",
  "Gray72",
  "Gray73",
  "Gray74",
  "Gray75",
  "Gray76",
  "Gray77",
  "Gray78",
  "Gray79",
  "Gray8",
  "Gray80",
  "Gray81",
  "Gray82",
  "Gray83",
  "Gray84",
  "Gray85",
  "Gray86",
  "Gray87",
  "Gray88",
  "Gray89",
  "Gray9",
  "Gray90",
  "Gray91",
  "Gray92",
  "Gray93",
  "Gray94",
  "Gray95",
  "Gray96",
  "Gray97",
  "Gray98",
  "Gray99",
  "Green",
  "Green1",
  "Green2",
  "Green3",
  "Green4",
  "GreenYellow",
  "Grey",
  "Grey0",
  "Grey1",
  "Grey10",
  "Grey100",
  "Grey11",
  "Grey12",
  "Grey13",
  "Grey14",
  "Grey15",
  "Grey16",
  "Grey17",
  "Grey18",
  "Grey19",
  "Grey2",
  "Grey20",
  "Grey21",
  "Grey22",
  "Grey23",
  "Grey24",
  "Grey25",
  "Grey26",
  "Grey27",
  "Grey28",
  "Grey29",
  "Grey3",
  "Grey30",
  "Grey31",
  "Grey32",
  "Grey33",
  "Grey34",
  "Grey35",
  "Grey36",
  "Grey37",
  "Grey38",
  "Grey39",
  "Grey4",
  "Grey40",
  "Grey41",
  "Grey42",
  "Grey43",
  "Grey44",
  "Grey45",
  "Grey46",
  "Grey47",
  "Grey48",
  "Grey49",
  "Grey5",
  "Grey50",
  "Grey51",
  "Grey52",
  "Grey53",
  "Grey54",
  "Grey55",
  "Grey56",
  "Grey57",
  "Grey58",
  "Grey59",
  "Grey6",
  "Grey60",
  "Grey61",
  "Grey62",
  "Grey63",
  "Grey64",
  "Grey65",
  "Grey66",
  "Grey67",
  "Grey68",
  "Grey69",
  "Grey7",
  "Grey70",
  "Grey71",
  "Grey72",
  "Grey73",
  "Grey74",
  "Grey75",
  "Grey76",
  "Grey77",
  "Grey78",
  "Grey79",
  "Grey8",
  "Grey80",
  "Grey81",
  "Grey82",
  "Grey83",
  "Grey84",
  "Grey85",
  "Grey86",
  "Grey87",
  "Grey88",
  "Grey89",
  "Grey9",
  "Grey90",
  "Grey91",
  "Grey92",
  "Grey93",
  "Grey94",
  "Grey95",
  "Grey96",
  "Grey97",
  "Grey98",
  "Grey99",
  "Honeydew",
  "Honeydew1",
  "Honeydew2",
  "Honeydew3",
  "Honeydew4",
  "HotPink",
  "HotPink1",
  "HotPink2",
  "HotPink3",
  "HotPink4",
  "IndianRed",
  "IndianRed1",
  "IndianRed2",
  "IndianRed3",
  "IndianRed4",
  "Indigo",
  "Ivory",
  "Ivory1",
  "Ivory2",
  "Ivory3",
  "Ivory4",
  "Khaki",
  "Khaki1",
  "Khaki2",
  "Khaki3",
  "Khaki4",
  "Lavender",
  "LavenderBlush",
  "LavenderBlush1",
  "LavenderBlush2",
  "LavenderBlush3",
  "LavenderBlush4",
  "LawnGreen",
  "LemonChiffon",
  "LemonChiffon1",
  "LemonChiffon2",
  "LemonChiffon3",
  "LemonChiffon4",
  "LightBlue",
  "LightBlue1",
  "LightBlue2",
  "LightBlue3",
  "LightBlue4",
  "LightCoral",
  "LightCyan",
  "LightCyan1",
  "LightCyan2",
  "LightCyan3",
  "LightCyan4",
  "LightGoldenrod",
  "LightGoldenrod1",
  "LightGoldenrod2",
  "LightGoldenrod3",
  "LightGoldenrod4",
  "LightGoldenrodYellow",
  "LightGray",
  "LightGreen",
  "LightGrey",
  "LightPink",
  "LightPink1",
  "LightPink2",
  "LightPink3",
  "LightPink4",
  "LightSalmon",
  "LightSalmon1",
  "LightSalmon2",
  "LightSalmon3",
  "LightSalmon4",
  "LightSeaGreen",
  "LightSkyBlue",
  "LightSkyBlue1",
  "LightSkyBlue2",
  "LightSkyBlue3",
  "LightSkyBlue4",
  "LightSlateBlue",
  "LightSlateGray",
  "LightSlateGrey",
  "LightSteelBlue",
  "LightSteelBlue1",
  "LightSteelBlue2",
  "LightSteelBlue3",
  "LightSteelBlue4",
  "LightYellow",
  "LightYellow1",
  "LightYellow2",
  "LightYellow3",
  "LightYellow4",
  "Lime",
  "LimeGreen",
  "Linen",
  "Magenta",
  "Magenta1",
  "Magenta2",
  "Magenta3",
  "Magenta4",
  "Maroon",
  "Maroon1",
  "Maroon2",
  "Maroon3",
  "Maroon4",
  "MediumAquamarine",
  "MediumBlue",
  "MediumOrchid",
  "MediumOrchid1",
  "MediumOrchid2",
  "MediumOrchid3",
  "MediumOrchid4",
  "MediumPurple",
  "MediumPurple1",
  "MediumPurple2",
  "MediumPurple3",
  "MediumPurple4",
  "MediumSeaGreen",
  "MediumSlateBlue",
  "MediumSpringGreen",
  "MediumTurquoise",
  "MediumVioletRed",
  "MidnightBlue",
  "MintCream",
  "MistyRose",
  "MistyRose1",
  "MistyRose2",
  "MistyRose3",
  "MistyRose4",
  "Moccasin",
  "NavajoWhite",
  "NavajoWhite1",
  "NavajoWhite2",
  "NavajoWhite3",
  "NavajoWhite4",
  "Navy",
  "NavyBlue",
  "None",
  "OldLace",
  "Olive",
  "OliveDrab",
  "OliveDrab1",
  "OliveDrab2",
  "OliveDrab3",
  "OliveDrab4",
  "Orange",
  "Orange1",
  "Orange2",
  "Orange3",
  "Orange4",
  "OrangeRed",
  "OrangeRed1",
  "OrangeRed2",
  "OrangeRed3",
  "OrangeRed4",
  "Orchid",
  "Orchid1",
  "Orchid2",
  "Orchid3",
  "Orchid4",
  "PaleGoldenrod",
  "PaleGreen",
  "PaleGreen1",
  "PaleGreen2",
  "PaleGreen3",
  "PaleGreen4",
  "PaleTurquoise",
  "PaleTurquoise1",
  "PaleTurquoise2",
  "PaleTurquoise3",
  "PaleTurquoise4",
  "PaleVioletRed",
  "PaleVioletRed1",
  "PaleVioletRed2",
  "PaleVioletRed3",
  "PaleVioletRed4",
  "PapayaWhip",
  "PeachPuff",
  "PeachPuff1",
  "PeachPuff2",
  "PeachPuff3",
  "PeachPuff4",
  "Peru",
  "Pink",
  "Pink1",
  "Pink2",
  "Pink3",
  "Pink4",
  "Plum",
  "Plum1",
  "Plum2",
  "Plum3",
  "Plum4",
  "PowderBlue",
  "Purple",
  "Purple1",
  "Purple2",
  "Purple3",
  "Purple4",
  "Raspberry",
  "Red",
  "Red1",
  "Red2",
  "Red3",
  "Red4",
  "RosyBrown",
  "RosyBrown1",
  "RosyBrown2",
  "RosyBrown3",
  "RosyBrown4",
  "RoyalBlue",
  "RoyalBlue1",
  "RoyalBlue2",
  "RoyalBlue3",
  "RoyalBlue4",
  "SaddleBrown",
  "Salmon",
  "Salmon1",
  "Salmon2",
  "Salmon3",
  "Salmon4",
  "SandyBrown",
  "SeaGreen",
  "SeaGreen1",
  "SeaGreen2",
  "SeaGreen3",
  "SeaGreen4",
  "Seashell",
  "Seashell1",
  "Seashell2",
  "Seashell3",
  "Seashell4",
  "Sepia",
  "Sienna",
  "Sienna1",
  "Sienna2",
  "Sienna3",
  "Sienna4",
  "Silver",
  "SkyBlue",
  "SkyBlue1",
  "SkyBlue2",
  "SkyBlue3",
  "SkyBlue4",
  "SlateBlue",
  "SlateBlue1",
  "SlateBlue2",
  "SlateBlue3",
  "SlateBlue4",
  "SlateGray",
  "SlateGray1",
  "SlateGray2",
  "SlateGray3",
  "SlateGray4",
  "SlateGrey",
  "Snow",
  "Snow1",
  "Snow2",
  "Snow3",
  "Snow4",
  "SpringGreen",
  "SpringGreen1",
  "SpringGreen2",
  "SpringGreen3",
  "SpringGreen4",
  "SteelBlue",
  "SteelBlue1",
  "SteelBlue2",
  "SteelBlue3",
  "SteelBlue4",
  "Tan",
  "Tan1",
  "Tan2",
  "Tan3",
  "Tan4",
  "Teal",
  "Thistle",
  "Thistle1",
  "Thistle2",
  "Thistle3",
  "Thistle4",
  "Tomato",
  "Tomato1",
  "Tomato2",
  "Tomato3",
  "Tomato4",
  "Turquoise",
  "Turquoise1",
  "Turquoise2",
  "Turquoise3",
  "Turquoise4",
  "Ultramarine",
  "Violet",
  "VioletRed",
  "VioletRed1",
  "VioletRed2",
  "VioletRed3",
  "VioletRed4",
  "WarmGrey",
  "Wheat",
  "Wheat1",
  "Wheat2",
  "Wheat3",
  "Wheat4",
  "White",
  "WhiteSmoke",
  "Yellow",
  "Yellow1",
  "Yellow2",
  "Yellow3",
  "Yellow4",
  "YellowGreen",
  };


// Values of corresponding colors
const FXColor colorValue[683]={
  FXColors::AliceBlue,
  FXColors::AntiqueWhite,
  FXColors::AntiqueWhite1,
  FXColors::AntiqueWhite2,
  FXColors::AntiqueWhite3,
  FXColors::AntiqueWhite4,
  FXColors::Aqua,
  FXColors::Aquamarine,
  FXColors::Aquamarine1,
  FXColors::Aquamarine2,
  FXColors::Aquamarine3,
  FXColors::Aquamarine4,
  FXColors::Azure,
  FXColors::Azure1,
  FXColors::Azure2,
  FXColors::Azure3,
  FXColors::Azure4,
  FXColors::Banana,
  FXColors::Beige,
  FXColors::Bisque,
  FXColors::Bisque1,
  FXColors::Bisque2,
  FXColors::Bisque3,
  FXColors::Bisque4,
  FXColors::Black,
  FXColors::BlanchedAlmond,
  FXColors::Blue,
  FXColors::Blue1,
  FXColors::Blue2,
  FXColors::Blue3,
  FXColors::Blue4,
  FXColors::BlueViolet,
  FXColors::Brick,
  FXColors::Brown,
  FXColors::Brown1,
  FXColors::Brown2,
  FXColors::Brown3,
  FXColors::Brown4,
  FXColors::Burlywood,
  FXColors::Burlywood1,
  FXColors::Burlywood2,
  FXColors::Burlywood3,
  FXColors::Burlywood4,
  FXColors::BurnedSienna,
  FXColors::BurnedUmber,
  FXColors::CadetBlue,
  FXColors::CadetBlue1,
  FXColors::CadetBlue2,
  FXColors::CadetBlue3,
  FXColors::CadetBlue4,
  FXColors::CadmiumOrange,
  FXColors::CadmiumRed,
  FXColors::CadmiumYellow,
  FXColors::Carrot,
  FXColors::Chartreuse,
  FXColors::Chartreuse1,
  FXColors::Chartreuse2,
  FXColors::Chartreuse3,
  FXColors::Chartreuse4,
  FXColors::Chocolate,
  FXColors::Chocolate1,
  FXColors::Chocolate2,
  FXColors::Chocolate3,
  FXColors::Chocolate4,
  FXColors::Cobalt,
  FXColors::CobaltGreen,
  FXColors::ColdGray,
  FXColors::Coral,
  FXColors::Coral1,
  FXColors::Coral2,
  FXColors::Coral3,
  FXColors::Coral4,
  FXColors::CornflowerBlue,
  FXColors::Cornsilk,
  FXColors::Cornsilk1,
  FXColors::Cornsilk2,
  FXColors::Cornsilk3,
  FXColors::Cornsilk4,
  FXColors::Crimson,
  FXColors::Cyan,
  FXColors::Cyan1,
  FXColors::Cyan2,
  FXColors::Cyan3,
  FXColors::Cyan4,
  FXColors::DarkBlue,
  FXColors::DarkCyan,
  FXColors::DarkGoldenrod,
  FXColors::DarkGoldenrod1,
  FXColors::DarkGoldenrod2,
  FXColors::DarkGoldenrod3,
  FXColors::DarkGoldenrod4,
  FXColors::DarkGray,
  FXColors::DarkGreen,
  FXColors::DarkGray,
  FXColors::DarkKhaki,
  FXColors::DarkMagenta,
  FXColors::DarkOliveGreen,
  FXColors::DarkOliveGreen1,
  FXColors::DarkOliveGreen2,
  FXColors::DarkOliveGreen3,
  FXColors::DarkOliveGreen4,
  FXColors::DarkOrange,
  FXColors::DarkOrange1,
  FXColors::DarkOrange2,
  FXColors::DarkOrange3,
  FXColors::DarkOrange4,
  FXColors::DarkOrchid,
  FXColors::DarkOrchid1,
  FXColors::DarkOrchid2,
  FXColors::DarkOrchid3,
  FXColors::DarkOrchid4,
  FXColors::DarkRed,
  FXColors::DarkSalmon,
  FXColors::DarkSeaGreen,
  FXColors::DarkSeaGreen1,
  FXColors::DarkSeaGreen2,
  FXColors::DarkSeaGreen3,
  FXColors::DarkSeaGreen4,
  FXColors::DarkSlateBlue,
  FXColors::DarkSlateGray,
  FXColors::DarkSlateGray1,
  FXColors::DarkSlateGray2,
  FXColors::DarkSlateGray3,
  FXColors::DarkSlateGray4,
  FXColors::DarkSlateGray,
  FXColors::DarkTurquoise,
  FXColors::DarkViolet,
  FXColors::DeepPink,
  FXColors::DeepPink1,
  FXColors::DeepPink2,
  FXColors::DeepPink3,
  FXColors::DeepPink4,
  FXColors::DeepSkyBlue,
  FXColors::DeepSkyBlue1,
  FXColors::DeepSkyBlue2,
  FXColors::DeepSkyBlue3,
  FXColors::DeepSkyBlue4,
  FXColors::DimGray,
  FXColors::DimGray,
  FXColors::DodgerBlue,
  FXColors::DodgerBlue1,
  FXColors::DodgerBlue2,
  FXColors::DodgerBlue3,
  FXColors::DodgerBlue4,
  FXColors::Eggshell,
  FXColors::EmeraldGreen,
  FXColors::Firebrick,
  FXColors::Firebrick1,
  FXColors::Firebrick2,
  FXColors::Firebrick3,
  FXColors::Firebrick4,
  FXColors::FloralWhite,
  FXColors::ForestGreen,
  FXColors::Fuchsia,
  FXColors::Gainsboro,
  FXColors::GhostWhite,
  FXColors::Gold,
  FXColors::Gold1,
  FXColors::Gold2,
  FXColors::Gold3,
  FXColors::Gold4,
  FXColors::Goldenrod,
  FXColors::Goldenrod1,
  FXColors::Goldenrod2,
  FXColors::Goldenrod3,
  FXColors::Goldenrod4,
  FXColors::Gray,
  FXColors::Gray0,
  FXColors::Gray1,
  FXColors::Gray10,
  FXColors::Gray100,
  FXColors::Gray11,
  FXColors::Gray12,
  FXColors::Gray13,
  FXColors::Gray14,
  FXColors::Gray15,
  FXColors::Gray16,
  FXColors::Gray17,
  FXColors::Gray18,
  FXColors::Gray19,
  FXColors::Gray2,
  FXColors::Gray20,
  FXColors::Gray21,
  FXColors::Gray22,
  FXColors::Gray23,
  FXColors::Gray24,
  FXColors::Gray25,
  FXColors::Gray26,
  FXColors::Gray27,
  FXColors::Gray28,
  FXColors::Gray29,
  FXColors::Gray3,
  FXColors::Gray30,
  FXColors::Gray31,
  FXColors::Gray32,
  FXColors::Gray33,
  FXColors::Gray34,
  FXColors::Gray35,
  FXColors::Gray36,
  FXColors::Gray37,
  FXColors::Gray38,
  FXColors::Gray39,
  FXColors::Gray4,
  FXColors::Gray40,
  FXColors::Gray41,
  FXColors::Gray42,
  FXColors::Gray43,
  FXColors::Gray44,
  FXColors::Gray45,
  FXColors::Gray46,
  FXColors::Gray47,
  FXColors::Gray48,
  FXColors::Gray49,
  FXColors::Gray5,
  FXColors::Gray50,
  FXColors::Gray51,
  FXColors::Gray52,
  FXColors::Gray53,
  FXColors::Gray54,
  FXColors::Gray55,
  FXColors::Gray56,
  FXColors::Gray57,
  FXColors::Gray58,
  FXColors::Gray59,
  FXColors::Gray6,
  FXColors::Gray60,
  FXColors::Gray61,
  FXColors::Gray62,
  FXColors::Gray63,
  FXColors::Gray64,
  FXColors::Gray65,
  FXColors::Gray66,
  FXColors::Gray67,
  FXColors::Gray68,
  FXColors::Gray69,
  FXColors::Gray7,
  FXColors::Gray70,
  FXColors::Gray71,
  FXColors::Gray72,
  FXColors::Gray73,
  FXColors::Gray74,
  FXColors::Gray75,
  FXColors::Gray76,
  FXColors::Gray77,
  FXColors::Gray78,
  FXColors::Gray79,
  FXColors::Gray8,
  FXColors::Gray80,
  FXColors::Gray81,
  FXColors::Gray82,
  FXColors::Gray83,
  FXColors::Gray84,
  FXColors::Gray85,
  FXColors::Gray86,
  FXColors::Gray87,
  FXColors::Gray88,
  FXColors::Gray89,
  FXColors::Gray9,
  FXColors::Gray90,
  FXColors::Gray91,
  FXColors::Gray92,
  FXColors::Gray93,
  FXColors::Gray94,
  FXColors::Gray95,
  FXColors::Gray96,
  FXColors::Gray97,
  FXColors::Gray98,
  FXColors::Gray99,
  FXColors::Green,
  FXColors::Green1,
  FXColors::Green2,
  FXColors::Green3,
  FXColors::Green4,
  FXColors::GreenYellow,
  FXColors::Gray,
  FXColors::Gray0,
  FXColors::Gray1,
  FXColors::Gray10,
  FXColors::Gray100,
  FXColors::Gray11,
  FXColors::Gray12,
  FXColors::Gray13,
  FXColors::Gray14,
  FXColors::Gray15,
  FXColors::Gray16,
  FXColors::Gray17,
  FXColors::Gray18,
  FXColors::Gray19,
  FXColors::Gray2,
  FXColors::Gray20,
  FXColors::Gray21,
  FXColors::Gray22,
  FXColors::Gray23,
  FXColors::Gray24,
  FXColors::Gray25,
  FXColors::Gray26,
  FXColors::Gray27,
  FXColors::Gray28,
  FXColors::Gray29,
  FXColors::Gray3,
  FXColors::Gray30,
  FXColors::Gray31,
  FXColors::Gray32,
  FXColors::Gray33,
  FXColors::Gray34,
  FXColors::Gray35,
  FXColors::Gray36,
  FXColors::Gray37,
  FXColors::Gray38,
  FXColors::Gray39,
  FXColors::Gray4,
  FXColors::Gray40,
  FXColors::Gray41,
  FXColors::Gray42,
  FXColors::Gray43,
  FXColors::Gray44,
  FXColors::Gray45,
  FXColors::Gray46,
  FXColors::Gray47,
  FXColors::Gray48,
  FXColors::Gray49,
  FXColors::Gray5,
  FXColors::Gray50,
  FXColors::Gray51,
  FXColors::Gray52,
  FXColors::Gray53,
  FXColors::Gray54,
  FXColors::Gray55,
  FXColors::Gray56,
  FXColors::Gray57,
  FXColors::Gray58,
  FXColors::Gray59,
  FXColors::Gray6,
  FXColors::Gray60,
  FXColors::Gray61,
  FXColors::Gray62,
  FXColors::Gray63,
  FXColors::Gray64,
  FXColors::Gray65,
  FXColors::Gray66,
  FXColors::Gray67,
  FXColors::Gray68,
  FXColors::Gray69,
  FXColors::Gray7,
  FXColors::Gray70,
  FXColors::Gray71,
  FXColors::Gray72,
  FXColors::Gray73,
  FXColors::Gray74,
  FXColors::Gray75,
  FXColors::Gray76,
  FXColors::Gray77,
  FXColors::Gray78,
  FXColors::Gray79,
  FXColors::Gray8,
  FXColors::Gray80,
  FXColors::Gray81,
  FXColors::Gray82,
  FXColors::Gray83,
  FXColors::Gray84,
  FXColors::Gray85,
  FXColors::Gray86,
  FXColors::Gray87,
  FXColors::Gray88,
  FXColors::Gray89,
  FXColors::Gray9,
  FXColors::Gray90,
  FXColors::Gray91,
  FXColors::Gray92,
  FXColors::Gray93,
  FXColors::Gray94,
  FXColors::Gray95,
  FXColors::Gray96,
  FXColors::Gray97,
  FXColors::Gray98,
  FXColors::Gray99,
  FXColors::Honeydew,
  FXColors::Honeydew1,
  FXColors::Honeydew2,
  FXColors::Honeydew3,
  FXColors::Honeydew4,
  FXColors::HotPink,
  FXColors::HotPink1,
  FXColors::HotPink2,
  FXColors::HotPink3,
  FXColors::HotPink4,
  FXColors::IndianRed,
  FXColors::IndianRed1,
  FXColors::IndianRed2,
  FXColors::IndianRed3,
  FXColors::IndianRed4,
  FXColors::Indigo,
  FXColors::Ivory,
  FXColors::Ivory1,
  FXColors::Ivory2,
  FXColors::Ivory3,
  FXColors::Ivory4,
  FXColors::Khaki,
  FXColors::Khaki1,
  FXColors::Khaki2,
  FXColors::Khaki3,
  FXColors::Khaki4,
  FXColors::Lavender,
  FXColors::LavenderBlush,
  FXColors::LavenderBlush1,
  FXColors::LavenderBlush2,
  FXColors::LavenderBlush3,
  FXColors::LavenderBlush4,
  FXColors::LawnGreen,
  FXColors::LemonChiffon,
  FXColors::LemonChiffon1,
  FXColors::LemonChiffon2,
  FXColors::LemonChiffon3,
  FXColors::LemonChiffon4,
  FXColors::LightBlue,
  FXColors::LightBlue1,
  FXColors::LightBlue2,
  FXColors::LightBlue3,
  FXColors::LightBlue4,
  FXColors::LightCoral,
  FXColors::LightCyan,
  FXColors::LightCyan1,
  FXColors::LightCyan2,
  FXColors::LightCyan3,
  FXColors::LightCyan4,
  FXColors::LightGoldenrod,
  FXColors::LightGoldenrod1,
  FXColors::LightGoldenrod2,
  FXColors::LightGoldenrod3,
  FXColors::LightGoldenrod4,
  FXColors::LightGoldenrodYellow,
  FXColors::LightGray,
  FXColors::LightGreen,
  FXColors::LightGray,
  FXColors::LightPink,
  FXColors::LightPink1,
  FXColors::LightPink2,
  FXColors::LightPink3,
  FXColors::LightPink4,
  FXColors::LightSalmon,
  FXColors::LightSalmon1,
  FXColors::LightSalmon2,
  FXColors::LightSalmon3,
  FXColors::LightSalmon4,
  FXColors::LightSeaGreen,
  FXColors::LightSkyBlue,
  FXColors::LightSkyBlue1,
  FXColors::LightSkyBlue2,
  FXColors::LightSkyBlue3,
  FXColors::LightSkyBlue4,
  FXColors::LightSlateBlue,
  FXColors::LightSlateGray,
  FXColors::LightSlateGray,
  FXColors::LightSteelBlue,
  FXColors::LightSteelBlue1,
  FXColors::LightSteelBlue2,
  FXColors::LightSteelBlue3,
  FXColors::LightSteelBlue4,
  FXColors::LightYellow,
  FXColors::LightYellow1,
  FXColors::LightYellow2,
  FXColors::LightYellow3,
  FXColors::LightYellow4,
  FXColors::Lime,
  FXColors::LimeGreen,
  FXColors::Linen,
  FXColors::Magenta,
  FXColors::Magenta1,
  FXColors::Magenta2,
  FXColors::Magenta3,
  FXColors::Magenta4,
  FXColors::Maroon,
  FXColors::Maroon1,
  FXColors::Maroon2,
  FXColors::Maroon3,
  FXColors::Maroon4,
  FXColors::MediumAquamarine,
  FXColors::MediumBlue,
  FXColors::MediumOrchid,
  FXColors::MediumOrchid1,
  FXColors::MediumOrchid2,
  FXColors::MediumOrchid3,
  FXColors::MediumOrchid4,
  FXColors::MediumPurple,
  FXColors::MediumPurple1,
  FXColors::MediumPurple2,
  FXColors::MediumPurple3,
  FXColors::MediumPurple4,
  FXColors::MediumSeaGreen,
  FXColors::MediumSlateBlue,
  FXColors::MediumSpringGreen,
  FXColors::MediumTurquoise,
  FXColors::MediumVioletRed,
  FXColors::MidnightBlue,
  FXColors::MintCream,
  FXColors::MistyRose,
  FXColors::MistyRose1,
  FXColors::MistyRose2,
  FXColors::MistyRose3,
  FXColors::MistyRose4,
  FXColors::Moccasin,
  FXColors::NavajoWhite,
  FXColors::NavajoWhite1,
  FXColors::NavajoWhite2,
  FXColors::NavajoWhite3,
  FXColors::NavajoWhite4,
  FXColors::Navy,
  FXColors::NavyBlue,
  FXColors::Clear,
  FXColors::OldLace,
  FXColors::Olive,
  FXColors::OliveDrab,
  FXColors::OliveDrab1,
  FXColors::OliveDrab2,
  FXColors::OliveDrab3,
  FXColors::OliveDrab4,
  FXColors::Orange,
  FXColors::Orange1,
  FXColors::Orange2,
  FXColors::Orange3,
  FXColors::Orange4,
  FXColors::OrangeRed,
  FXColors::OrangeRed1,
  FXColors::OrangeRed2,
  FXColors::OrangeRed3,
  FXColors::OrangeRed4,
  FXColors::Orchid,
  FXColors::Orchid1,
  FXColors::Orchid2,
  FXColors::Orchid3,
  FXColors::Orchid4,
  FXColors::PaleGoldenrod,
  FXColors::PaleGreen,
  FXColors::PaleGreen1,
  FXColors::PaleGreen2,
  FXColors::PaleGreen3,
  FXColors::PaleGreen4,
  FXColors::PaleTurquoise,
  FXColors::PaleTurquoise1,
  FXColors::PaleTurquoise2,
  FXColors::PaleTurquoise3,
  FXColors::PaleTurquoise4,
  FXColors::PaleVioletRed,
  FXColors::PaleVioletRed1,
  FXColors::PaleVioletRed2,
  FXColors::PaleVioletRed3,
  FXColors::PaleVioletRed4,
  FXColors::PapayaWhip,
  FXColors::PeachPuff,
  FXColors::PeachPuff1,
  FXColors::PeachPuff2,
  FXColors::PeachPuff3,
  FXColors::PeachPuff4,
  FXColors::Peru,
  FXColors::Pink,
  FXColors::Pink1,
  FXColors::Pink2,
  FXColors::Pink3,
  FXColors::Pink4,
  FXColors::Plum,
  FXColors::Plum1,
  FXColors::Plum2,
  FXColors::Plum3,
  FXColors::Plum4,
  FXColors::PowderBlue,
  FXColors::Purple,
  FXColors::Purple1,
  FXColors::Purple2,
  FXColors::Purple3,
  FXColors::Purple4,
  FXColors::Raspberry,
  FXColors::Red,
  FXColors::Red1,
  FXColors::Red2,
  FXColors::Red3,
  FXColors::Red4,
  FXColors::RosyBrown,
  FXColors::RosyBrown1,
  FXColors::RosyBrown2,
  FXColors::RosyBrown3,
  FXColors::RosyBrown4,
  FXColors::RoyalBlue,
  FXColors::RoyalBlue1,
  FXColors::RoyalBlue2,
  FXColors::RoyalBlue3,
  FXColors::RoyalBlue4,
  FXColors::SaddleBrown,
  FXColors::Salmon,
  FXColors::Salmon1,
  FXColors::Salmon2,
  FXColors::Salmon3,
  FXColors::Salmon4,
  FXColors::SandyBrown,
  FXColors::SeaGreen,
  FXColors::SeaGreen1,
  FXColors::SeaGreen2,
  FXColors::SeaGreen3,
  FXColors::SeaGreen4,
  FXColors::Seashell,
  FXColors::Seashell1,
  FXColors::Seashell2,
  FXColors::Seashell3,
  FXColors::Seashell4,
  FXColors::Sepia,
  FXColors::Sienna,
  FXColors::Sienna1,
  FXColors::Sienna2,
  FXColors::Sienna3,
  FXColors::Sienna4,
  FXColors::Silver,
  FXColors::SkyBlue,
  FXColors::SkyBlue1,
  FXColors::SkyBlue2,
  FXColors::SkyBlue3,
  FXColors::SkyBlue4,
  FXColors::SlateBlue,
  FXColors::SlateBlue1,
  FXColors::SlateBlue2,
  FXColors::SlateBlue3,
  FXColors::SlateBlue4,
  FXColors::SlateGray,
  FXColors::SlateGray1,
  FXColors::SlateGray2,
  FXColors::SlateGray3,
  FXColors::SlateGray4,
  FXColors::SlateGray,
  FXColors::Snow,
  FXColors::Snow1,
  FXColors::Snow2,
  FXColors::Snow3,
  FXColors::Snow4,
  FXColors::SpringGreen,
  FXColors::SpringGreen1,
  FXColors::SpringGreen2,
  FXColors::SpringGreen3,
  FXColors::SpringGreen4,
  FXColors::SteelBlue,
  FXColors::SteelBlue1,
  FXColors::SteelBlue2,
  FXColors::SteelBlue3,
  FXColors::SteelBlue4,
  FXColors::Tan,
  FXColors::Tan1,
  FXColors::Tan2,
  FXColors::Tan3,
  FXColors::Tan4,
  FXColors::Teal,
  FXColors::Thistle,
  FXColors::Thistle1,
  FXColors::Thistle2,
  FXColors::Thistle3,
  FXColors::Thistle4,
  FXColors::Tomato,
  FXColors::Tomato1,
  FXColors::Tomato2,
  FXColors::Tomato3,
  FXColors::Tomato4,
  FXColors::Turquoise,
  FXColors::Turquoise1,
  FXColors::Turquoise2,
  FXColors::Turquoise3,
  FXColors::Turquoise4,
  FXColors::Ultramarine,
  FXColors::Violet,
  FXColors::VioletRed,
  FXColors::VioletRed1,
  FXColors::VioletRed2,
  FXColors::VioletRed3,
  FXColors::VioletRed4,
  FXColors::WarmGray,
  FXColors::Wheat,
  FXColors::Wheat1,
  FXColors::Wheat2,
  FXColors::Wheat3,
  FXColors::Wheat4,
  FXColors::White,
  FXColors::WhiteSmoke,
  FXColors::Yellow,
  FXColors::Yellow1,
  FXColors::Yellow2,
  FXColors::Yellow3,
  FXColors::Yellow4,
  FXColors::YellowGreen
  };


// Furnish our own version
extern FXAPI FXint __sscanf(const FXchar* string,const FXchar* format,...);
extern FXAPI FXint __snprintf(FXchar* string,FXint length,const FXchar* format,...);


// Get RGB value from color name
FXColor colorFromName(const FXchar* name){
  FXchar temp[MAXCOLORNAME],*tail=temp,c;
  FXint l,h,m,eq,r,g,b,a;
  if(name){
    while((c=*name++)!='\0' && tail<&temp[MAXCOLORNAME-1]){     // Squeeze out embedded spaces
      if(!Ascii::isSpace(c)) *tail++=c;
      }
    *tail='\0';
    if(temp[0]=='#'){
      switch(tail-temp-1){
        case 3:
          __sscanf(temp+1,"%01x%01x%01x",&r,&g,&b);
          return FXRGB((r*17),(g*17),(b*17));
        case 4:
          __sscanf(temp+1,"%01x%01x%01x%01x",&r,&g,&b,&a);
          return FXRGBA((r*17),(g*17),(b*17),(a*17));
        case 6:
          __sscanf(temp+1,"%02x%02x%02x",&r,&g,&b);
          return FXRGB(r,g,b);
        case 8:
          __sscanf(temp+1,"%02x%02x%02x%02x",&r,&g,&b,&a);
          return FXRGBA(r,g,b,a);
        case 9:
          __sscanf(temp+1,"%03x%03x%03x",&r,&g,&b);
          return FXRGB((r/16),(g/16),(b/16));
        case 12:
          __sscanf(temp+1,"%04x%04x%04x",&r,&g,&b);
          return FXRGB((r/257),(g/257),(b/257));
        case 16:
          __sscanf(temp+1,"%04x%04x%04x%04x",&r,&g,&b,&a);
          return FXRGBA((r/257),(g/257),(b/257),(a/257));
        }
      }
    else{
      l=0;
      h=ARRAYNUMBER(colorName)-1;
      do{
        m=(h+l)>>1;
        eq=comparecase(temp,colorName[m]);
        if(eq==0) return colorValue[m];
        if(eq<0) h=m-1; else l=m+1;
        }
      while(h>=l);
      }
    }
  return FXRGBA(0,0,0,0);
  }


// Get rgb value from color name
FXColor colorFromName(const FXString& name){
  return colorFromName(name.text());
  }


// Get color name from rgb value
FXchar* nameFromColor(FXchar* name,FXColor color){
  if(!name){ fxerror("FXColorName::nameFromColor: NULL name argument.\n"); }
  if(color && FXALPHAVAL(color)<255){
    __snprintf(name,MAXCOLORNAME,"#%02x%02x%02x%02x",FXREDVAL(color),FXGREENVAL(color),FXBLUEVAL(color),FXALPHAVAL(color));
    }
  else{
    for(FXuint i=0; i<ARRAYNUMBER(colorValue); i++){
      if(colorValue[i]==color){
        fxstrlcpy(name,colorName[i],MAXCOLORNAME);
        return name;
        }
      }
    __snprintf(name,MAXCOLORNAME,"#%02x%02x%02x",FXREDVAL(color),FXGREENVAL(color),FXBLUEVAL(color));
    }
  return name;
  }


// Get color name from rgb value
FXString nameFromColor(FXColor color){
  FXchar temp[100];
  return FXString(nameFromColor(temp,color));
  }


// Blend source color over background color
FXColor blendOverBackground(FXColor back,FXColor clr){
  FXint as=FXALPHAVAL(clr);
  FXint rb=FXREDVAL(back);
  FXint gb=FXGREENVAL(back);
  FXint bb=FXBLUEVAL(back);
  FXint rs=(FXREDVAL(clr)-rb)*as;
  FXint gs=(FXGREENVAL(clr)-gb)*as;
  FXint bs=(FXBLUEVAL(clr)-bb)*as;
  rb+=((rs+(rs>>8)+128)>>8);
  gb+=((gs+(gs>>8)+128)>>8);
  bb+=((gs+(bs>>8)+128)>>8);
  return FXRGB(rb,gb,bb);
  }


// Blend source color over black background color
FXColor blendOverBlack(FXColor clr){
  FXint as=FXALPHAVAL(clr);
  FXint rs=FXREDVAL(clr)*as;
  FXint gs=FXGREENVAL(clr)*as;
  FXint bs=FXBLUEVAL(clr)*as;
  FXint rd=((rs+(rs>>8)+128)>>8);
  FXint gd=((gs+(gs>>8)+128)>>8);
  FXint bd=((bs+(bs>>8)+128)>>8);
  return FXRGB(rd,gd,bd);
  }


// Blend source color over white background color
FXColor blendOverWhite(FXColor clr){
  FXint as=FXALPHAVAL(clr);
  FXint rs=(FXREDVAL(clr)-255)*as;
  FXint gs=(FXGREENVAL(clr)-255)*as;
  FXint bs=(FXBLUEVAL(clr)-255)*as;
  FXint rd=255+((rs+(rs>>8)+128)>>8);
  FXint gd=255+((gs+(gs>>8)+128)>>8);
  FXint bd=255+((bs+(bs>>8)+128)>>8);
  return FXRGB(rd,gd,bd);
  }


// Blend color src toward color dst by a given percentage
FXColor makeBlendColor(FXColor src,FXColor dst,FXint percent){
  FXuint r,g,b,tnecrep=100-percent;
  r=(FXREDVAL(dst)*percent+FXREDVAL(src)*tnecrep+50)/100;
  g=(FXGREENVAL(dst)*percent+FXGREENVAL(src)*tnecrep+50)/100;
  b=(FXBLUEVAL(dst)*percent+FXBLUEVAL(src)*tnecrep+50)/100;
  return FXRGB(r,g,b);
  }


// Get highlight color
FXColor makeHiliteColor(FXColor clr,FXint percent){
  FXuint r,g,b,tnecrep=100-percent;
  r=(255*percent+FXREDVAL(clr)*tnecrep+50)/100;
  g=(255*percent+FXGREENVAL(clr)*tnecrep+50)/100;
  b=(255*percent+FXBLUEVAL(clr)*tnecrep+50)/100;
  return FXRGB(r,g,b);
  }


// Get shadow color
FXColor makeShadowColor(FXColor clr,FXint percent){
  FXuint r,g,b,tnecrep=100-percent;
  r=(FXREDVAL(clr)*tnecrep+50)/100;
  g=(FXGREENVAL(clr)*tnecrep+50)/100;
  b=(FXBLUEVAL(clr)*tnecrep+50)/100;
  return FXRGB(r,g,b);
  }


}
