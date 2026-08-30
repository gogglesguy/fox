/********************************************************************************
*                                                                               *
*                        U n i t - C o n v e r s i o n s                        *
*                                                                               *
*********************************************************************************
* Copyright (C) 2026 by Jeroen van der Zijp.   All Rights Reserved.             *
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
#include "fxascii.h"
#include "FXString.h"
#include "FXUnits.h"

/*
  Notes:

  - Convert between various units, in arbitrary combinations.

  - We'd like convenience API, when one of the unit is from the S.I. systemm,
    the conversion should just parse the "foreign" system; something like:

        TBool convertToSI(TDouble& value,"lbf");

    and:

        TBool convertFromSI(TDouble& value,"psi");

    the idea is that our software, internally, works almost exclusively with
    S.I. units and we only conver to/frm other units for human presentation
    purposes.

  - To recognize utf8 superscripts like ^2 (\xC2\xB2) or ^3 (\xC2\xB3), we might
    need unicode character-classes.  For now, use nonpunct[] table to map only
    nonpunctuation characters, all the rest maps to '\0'.

  - UTF8 superscripts ^1 '\xC2\xB9' (¹), ^2 '\xC2\xB2' (²) and ^3 '\xC2\xB3' (³).
    We also have superscript minus ^- '\xE2\x81\xBB' (⁻), ^+ '\xE2\x81\xBA' (⁺),
    ^0 '\xE2\x81\xB0' (⁰), ^4 '\xE2\x81\xB4', ... ^9 '\xE2\x81\xB9' (⁴⁵⁶⁷⁸⁹).
    Currently, this does not yet work even though code is in place; it is masked
    by all multi-byte unicode being classified as "word-character".

  - Reference: "Conversion of Units of Measurement," Gordon S. Novak, Jr.,
    IEEE Trans. on Software Engineering, Vol. 21, No. 8, 1995, pp. 651-661.
*/

using namespace FX;

/*******************************************************************************/

namespace FX {


// This is unlikely to change, unless physics does...
const FXuval NumBasicUnits=9;

// We use 5 bits for each dimension, limiting values to -16...15 range.
// There are a potentially 3 slots left for non-physics dimensions.
const FXulong DIMSBIAS=0x108421084210ull;


// Physics units dimension-index
const FXchar* KELVIN=(const FXchar*)0ul;        // Temperature
const FXchar* SECOND=(const FXchar*)1ul;        // Time
const FXchar* GRAM=(const FXchar*)2ul;          // Mass
const FXchar* METER=(const FXchar*)3ul;         // Length
const FXchar* AMPERE=(const FXchar*)4ul;        // Electrical current
const FXchar* MOLE=(const FXchar*)5ul;          // Mole
const FXchar* CANDELA=(const FXchar*)6ul;       // Luminous flux
const FXchar* RADIAN=(const FXchar*)7ul;        // Angles
const FXchar* STERADIAN=(const FXchar*)8ul;     // Solid angles


// Used in unit reduction
struct FXUnitConv {
  FXdouble      mult;   // Multiplier
  FXulong       dims;   // Dimensions (with DIMSBIAS)
  };


// Used in the unit info table
struct FXUnitData {
  const FXchar* abbr;   // Abbreviation
  const FXchar* full;   // Full name
  const FXchar* expr;   // Unit-expression or dimension-index
  FXdouble      mult;   // Multiplier
  };


// Table of all units information; some units in UTF8
static const FXUnitData UnitDataArray[]={
   {"A",            "Ampere",              AMPERE,           1.0},
   {"Bq",           "Becquerel",           "1/s",            1.0},
   {"Btu",          "BritishThermalUnit",  "Kg*m^2/s^2",     1055.05585262},
   {"C",            "Coulomb",             "A*s",            1.0},
   {"Ci",           "Curie",               "1/s",            3737.0},
   {"Da",           "Dalton",              "Kg",             1.66053906892E-27},
   {"F",            "Farad",               "A^2*s^4/Kg*m^2", 1.0},
   {"Fdy",          "Faraday",             "A*s",            96487.0},
   {"Gy",           "Gray",                "m^2/s^2",        1.0},
   {"H",            "Henry",               "Kg*m^2/A^2*s^2", 1.0},
   {"Hz",           "Hertz",               "s^-1",           1.0},
   {"J",            "Joule",               "Kg*m^2/s^2",     1.0},
   {"K",            "Kelvin",              KELVIN,           1.0},
   {"L",            "Liter",               "m^3",            0.001},
   {"N",            "Newton",              "Kg*m/s^2",       1.0},
   {"Oe",           "Oersted",             "A/m",            79.57747},
   {"Ohm",          "Ohm",                 "Kg*m^2/A^2*s^3", 1.0},
   {"P",            "Poise",               "Kg/m*s",         0.1},
   {"Pa",           "Pascal",              "Kg/m*s^2",       1.0},
   {"Pdl",          "Poundal",             "Kg*m/s^2",       0.13825495376},
   {"Pica",         "Pica",                "in",             1.0/72.0},
   {"R",            "Roentgen",            "A*s/Kg",         0.000258},
   {"S",            "Siemens",             "A^2*s^3/Kg*m^2", 1.0},
   {"St",           "Stokes",              "m^2/s",          0.0001},
   {"Sv",           "Sievert",             "m^2/s^2",        1.0},
   {"T",            "Tesla",               "Kg/A*s^2",       1.0},
   {"U",            "UnifiedAtomicMass",   "Kg",             1.66053906892E-27},
   {"V",            "Volt",                "Kg*m^2/A*s^3",   1.0},
   {"W",            "Watt",                "Kg*m^2/s^3",     1.0},
   {"Wb",           "Weber",               "Kg*m^2/A*s^2",   1.0},
   {"a",            "Are",                 "m^2",            100.0},
   {"acre",         "Acre",                "ha",             0.40468564224},
   {"arcmin",       "ArcMinute",           "rad",            0.000290888208665721596153949},
   {"arcs",         "ArcSecond",           "rad" ,           4.84813681109535993589914E-06},
   {"atm",          "Atmosphere",          "Kg/m*s^2",       101325.0},
   {"au",           "AstronomicalUnit",    "m",              149597870700.0},
   {"b",            "Barn",                "m^2",            1E-28},
   {"bar",          "Bar",                 "Kg/m*s^2",       100000.0},
   {"bbl",          "Barrel",              "m^3",            0.158987294928},
   {"bu",           "Bushel",              "m^3",            0.03523907},
   {"c",            "Lightspeed",          "m/s",            299792458.0},
   {"cal",          "Calorie",             "Kg*m^2/s^2",     4.1868},
   {"cd",           "Candela",             CANDELA,          1.0},
   {"ch",           "Chain",               "m",              20.116840234},
   {"ct",           "Carat",               "Kg",             0.0002},
   {"cu",           "USCup",               "m^3",            2.365882365E-4},
   {"d",            "Day",                 "s",              86400.0},
   {"day",          "Day",                 "s",              86400.0},
   {"deg",          "Degree",              "rad",            0.0174532925199432957692369},
   {"dr",           "Dram",                "g",              1.7718451953125},
   {"dwt",          "Pennyweight",         "g",              1.55517384},
   {"dyn",          "Dyne",                "Kg*m/s^2",       0.00001},
   {"eV",           "ElectronVolt",        "Kg*m^2/s^2",     1.60217733e-19},
   {"erg",          "Erg",                 "Kg*m^2/s^2",     0.0000001},
   {"fL",           "FootLambert",         "cd/m^2",         3.42625909963539052691674},
   {"fath",         "Fathom",              "m",              1.828803658},
   {"fbm",          "BoardFoot",           "m^3",            0.002359737216},
   {"fc",           "FootCandle",          "cd*sr/m^2",      10.764},
   {"ft",           "Foot",                "m",              0.3048},
   {"ftUS",         "SurveyFoot",          "m",              0.304800609601},
   {"ftn",          "Fortnight",           "s",              1209600.0},
   {"fur",          "Furlong",             "m",              201.168402337},
   {"g",            "Gram",                GRAM,             0.001},
   {"gal",          "USGallon",            "m^3",            0.003785411784},
   {"gee",          "StandardGravity",     "m/s^2",          9.80665},
   {"gf",           "GramForce",           "Kg*m/s^2",       0.00980665},
   {"gr",           "Grain",               "mg",             64.79891},
   {"grad",         "Gradian",             "rad",            0.015707963267948966192313},
   {"h",            "Hour",                "s",              3600.0},
   {"ha",           "Hectare",             "m^2",            10000.0},
   {"hour",         "Hour",                "s",              3600.0},
   {"hp",           "HorsePower",          "Kg*m^2/s^2",     745.699871582},
   {"in",           "Inch",                "m",              0.0254},
   {"kat",          "Katal",               "mol/s",          1.0},
   {"kip",          "KiloPoundForce",      "Kg*m/s^2",       4448.22161526},
   {"kph",          "KilometersPerHour",   "m/s",            5.0/18.0},
   {"kt",           "Knot",                "m/s",            463.0/900.0},
   {"lam",          "Lambert",             "cd/m^2",         3183.09886183790671537768},
   {"lb",           "AvoirdupoisPound",    "Kg",             0.45359267},
   {"lbf",          "PoundForce",          "Kg*m/s^2",       4.44822161526},
   {"lbt",          "TroyPound",           "Kg",             0.3732417216},
   {"lm",           "Lumen",               "cd*sr",          1.0},
   {"lux",          "Lux",                 "cd*sr/m^2",      1.0},
   {"lx",           "Lux",                 "cd*sr/m^2",      1.0},
   {"ly",           "Lightyear",           "m",              9460730472580800.0},
   {"m",            "Meter",               METER,            1.0},
   {"mi",           "USStatuteMile",       "m",              1609.344},
   {"min",          "Minute",              "s",              60.0},
   {"mmHg",         "MilimeterOfMercury",  "Kg/m*s^2",       133.3224},
   {"mol",          "Mole",                MOLE,             1.0},
   {"mph",          "MilesPerHour",        "m/s",            0.44704},
   {"nmi",          "NauticalMile",        "m",              1852.0},
   {"oz",           "Ounce",               "Kg",             0.028349523125},
   {"ozfl",         "USFluidOunce",        "m^3",            2.95735295625E-5},
   {"ozt",          "TroyOunce",           "Kg",             0.0311034768},
   {"pc",           "Parsec",              "m",              3.08567758149137E16},
   {"ph",           "Phot",                "cd*sr/m^2",      10000.0},
   {"pk",           "Peck",                "L",              8.80976754172},
   {"psi",          "PoundsPerSquareInch", "Pa",             6894.757},
   {"pt",           "Pint",                "m^3",            0.0004731765},
   {"qt",           "Quart",               "m^3",            0.0009463529},
   {"rad",          "Radian",              RADIAN,           1.0},
   {"rd",           "Rod",                 "m",              5.029210058},
   {"rem",          "Rem",                 "m^2/s^2",        0.01},
   {"s",            "Second",              SECOND,           1.0},
   {"sb",           "Stilb",               "cd/m^2",         10000.0},
   {"slug",         "Slug",                "Kg",             14.5939029372},
   {"sr",           "Steradian",           STERADIAN,        1.0},
   {"st",           "ShortTon",            "Kg",             907.18},
   {"t",            "MetricTon",           "Kg",             1000.0},
   {"therm",        "USTherm",             "J",              105480400.0},
   {"tn",           "ShortTon",            "Kg",             907.18},
   {"ton",          "LongTon",             "Kg",             1016.047},
   {"torr",         "Torr",                "Kg/m^2",         133.3224},
   {"yd",           "Yard",                "m",              0.9144},
   {"yr",           "Year",                "s",              31556925.9747},
   {"\xC2\xB0",     "Degree",              "rad",            0.0174532925199432957692369},
   {"\xC2\xB0" "C", "DegreesCelsius",      "K",              1.0},
   {"\xC2\xB0" "F", "DegreesFahrenheit",   "K",              1.0/1.8},
   {"\xC2\xB0" "K", "DegreesKelvin",       KELVIN,           1.0},
   {"\xC2\xB0" "R", "DegreesRankine",      "K",              1.0/1.8},
   {"\xC2\xB5",     "Micron",              "m",              1.0E-06},
   {"\xC3\x85",     "\xC3\x85ngstrom",     "m",              1.0E-10},
   {"\xCE\xA9",     "Ohm",                 "Kg*m^2/A^2*s^3", 1.0},
   {"\xE2\x80\xB2", "ArcMinute",           "rad",            0.000290888208665721596153949},
   {"\xE2\x80\xB3", "ArcSecond",           "rad",            4.84813681109535993589914E-06},
   };

/*******************************************************************************/

// Map all punctuation characters to 0, non-puncuation characters to themselves
static const FXuchar nonpunct[256]={
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
  0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x00,0x00,0x00,0x00,0x5f,
  0x00,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
  0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x00,0x00,0x00,0x00,0x00,
  0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
  0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
  0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
  0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,
  0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
  0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,
  0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,
  0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  };

static const FXuchar nonpunct_[256]={
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
  0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x00,0x00,0x00,0x00,0x5f,
  0x00,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
  0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  };

// Compare unit-string against abbreviation.
// Proper units consist of non-punctuation characters only.
static FXint unitCompare(const FXchar* unit,const FXchar* abbr){
  FXuchar ab,un;
  do{
    ab=*abbr++;
    un=*unit++;
    un=nonpunct[un];
    }
  while((un==ab) && ab);
  return un-ab;
  }


// Find unit in sorted table through binary search;
// return NotFound if no such unit found.
FXuint Units::lookup(const FXchar* unit){
  FXint l=0,h=ARRAYNUMBER(UnitDataArray)-1,m,c;
  while(l<=h){
    m=(h+l)>>1;
    c=unitCompare(unit,UnitDataArray[m].abbr);
    if(c==0) return m;
    if(c<0) h=m-1;
    if(c>0) l=m+1;
    }
  return NotFound;
  }


// Returns the number of units in the internal table.
FXuint Units::number(){
  return ARRAYNUMBER(UnitDataArray);
  }


// Return true if x is a basic unit
FXbool Units::basic(FXuint x){
  return (x<ARRAYNUMBER(UnitDataArray)) && ((FXuval)UnitDataArray[x].expr<NumBasicUnits);
  }


// Return unit symbol (in utf8) if it exists.
const FXchar* Units::symbol(FXuint x){
  if(ARRAYNUMBER(UnitDataArray)<=x) return nullptr;
  return UnitDataArray[x].abbr;
  }


// Return unit name (in utf8) if it exists.
const FXchar* Units::name(FXuint x){
  if(ARRAYNUMBER(UnitDataArray)<=x) return nullptr;
  return UnitDataArray[x].full;
  }


// Return unit conversion factor if it exists.
FXdouble Units::factor(FXuint x){
  if(ARRAYNUMBER(UnitDataArray)<=x) return 0.0;
  return UnitDataArray[x].mult;
  }


// Return units expression, if non-basic
const FXchar* Units::expression(FXuint x){
  if(ARRAYNUMBER(UnitDataArray)<=x) return nullptr;
  if((FXuval)UnitDataArray[x].expr<NumBasicUnits) return nullptr;
  return UnitDataArray[x].expr;
  }

/*******************************************************************************/

// Parse division-expression
static const char* divex(const FXchar* unit,FXUnitConv& u);


// Lookup unit type and populate u
static const FXchar* unitex(const FXchar* unit,FXUnitConv& u){
  FXuint x=Units::lookup(unit);
  if(x<ARRAYNUMBER(UnitDataArray)){
    const FXUnitData& info=UnitDataArray[x];

    // Basic unit: we're done!
    if((FXuval)info.expr<NumBasicUnits){
      u.dims=DIMSBIAS+(1<<(x*5));
      u.mult=info.mult;
      }

    // Derived unit: expand it further!
    else{
      divex(info.expr,u);
      u.mult*=info.mult;
      }

    // Advance past unit
    unit+=strlen(info.abbr);
    return unit;
    }
  return nullptr;
  }


// Parse scaling prefix
static const FXchar* scalex(const FXchar* unit,FXUnitConv& u){
  const FXchar* mark=unit;
  FXdouble k=1.0;

  // Skip white space
  while(Ascii::isSpace(*unit)) unit++;

  // For example: Hz = s^-1 = 1/s
  if(*unit=='1'){
    unit++;

    // Skip white space
    while(Ascii::isSpace(*unit)) unit++;

    u.mult=1;
    u.dims=DIMSBIAS;
    return unit;
    }

  // Parse unit name w/o scaling prefix
  unit=unitex(unit,u);
  if(unit!=nullptr) return unit;
  unit=mark;

  // Parse prefix multiplier
  switch(*unit++){
    case 'Y': k=1.0E+24; break;
    case 'Z': k=1.0E+21; break;
    case 'E': k=1.0E+18; break;
    case 'P': k=1.0E+15; break;
    case 'T': k=1.0E+12; break;
    case 'G': k=1.0E+09; break;
    case 'M': k=1.0E+06; break;
    case 'k': k=1.0E+03; break;
    case 'K': k=1.0E+03; break;
    case 'h': k=1.0E+02; break;
    case 'H': k=1.0E+02; break;
    case 'D': k=1.0E+01; break;
    case 'd': k=1.0E-01; break;
    case 'c': k=1.0E-02; break;
    case 'm': k=1.0E-03; break;
    case 'u': k=1.0E-06; break;
    case 'n': k=1.0E-09; break;
    case 'p': k=1.0E-12; break;
    case 'f': k=1.0E-15; break;
    case 'a': k=1.0E-18; break;
    case 'z': k=1.0E-21; break;
    case 'y': k=1.0E-24; break;
    case '\xCE': if(*unit++=='\xBC'){ k=1.0E-6; break; }
    default: return nullptr;
    }

  // Parse unit name following scaling prefix
  unit=unitex(unit,u);
  if(unit==nullptr) return nullptr;

  // Apply multiplier
  u.mult*=k;

  return unit;
  }


// Parse power-expression
static const FXchar* powex(const FXchar* unit,FXUnitConv& u){
  const FXchar* mark;
  FXint expo=0;
  FXint sign=0;

  // Parse scaling-expression
  mark=scalex(unit,u);
  if(mark==nullptr) return nullptr;

  // Skip white space
  while(Ascii::isSpace(mark[0])) mark++;

  unit=mark;

  // Exponentiation syntax like: x^2.
  if(mark[0]=='^'){
    mark++;

    // Skip white space
    while(Ascii::isSpace(mark[0])) mark++;

    // Sign of exponent
    sign=(mark[0]=='-');

    // Advance past sign
    if(mark[0]=='-' || mark[0]=='+') mark++;

    // Expected a digit
    if(Ascii::isDigit(mark[0])){
      expo=mark[0]-'0';
      mark++;

      // Parse exponent
      while(Ascii::isDigit(mark[0])){
        expo=expo*10+(mark[0]-'0');
        mark++;
        }

      // Extreme exponent won't fit in 5 bits
      if(expo>15) return nullptr;

      // Absorb input
      unit=mark;
      }
    }

  // Exponentiation syntax like: x².
  else{

    // Superscript sign
    if(mark[0]=='\xE2' && mark[1]=='\x81'){
      sign=(mark[2]=='\xBB');
      if(mark[2]=='\xBB' || mark[2]=='\xBA') mark+=3;
      }

    // Superscript 1, 2, 3
    if(mark[0]=='\xC2'){
      if(mark[1]=='\xB9'){              // ^1
        expo=1;
        unit=mark+2;
        }
      else if(mark[1]=='\xB2'){         // ^2
        expo=2;
        unit=mark+2;
        }
      else if(mark[1]=='\xB3'){         // ^3
        expo=3;
        unit=mark+2;
        }
      }
    }

  // Skip white space
  while(Ascii::isSpace(unit[0])) unit++;

  // Exponent is not zero
  if(expo){

    // Apply sign
    if(sign) expo=-expo;

    // Perform the power
    u.dims=(u.dims-DIMSBIAS)*expo+DIMSBIAS;
    u.mult=Math::powi(u.mult,expo);
    }

  return unit;
  }


// Parse multiply-expression
static const FXchar* mulex(const FXchar* unit,FXUnitConv& u){
  const FXchar* mark;

  // Parse power-expression
  unit=powex(unit,u);
  if(unit==nullptr) return nullptr;
  mark=unit;

  // Skip white space
  while(Ascii::isSpace(*unit)) unit++;

  // A succession of multiply expressions
  while(*unit=='*'){
    FXUnitConv m={0.0,DIMSBIAS};

    // Eat '*'
    unit++;

    // Parse multiply-expression
    unit=powex(unit,m);
    if(unit==nullptr) return mark;
    mark=unit;

    // Perform multiply
    u.dims+=m.dims-DIMSBIAS;
    u.mult*=m.mult;

    // Skip white space
    while(Ascii::isSpace(*unit)) unit++;
    }
  return unit;
  }


// Parse divide-expression
static const FXchar* divex(const FXchar* unit,FXUnitConv& u){
  const FXchar* mark;

  // Parse multiply-expression
  unit=mulex(unit,u);
  if(unit==nullptr) return nullptr;
  mark=unit;

  // Skip white space
  while(Ascii::isSpace(*unit)) unit++;

  // Got more?
  while(*unit=='/'){
    FXUnitConv d={0.0,DIMSBIAS};

    // Eat '/'
    unit++;

    // Parse white space
    while(Ascii::isSpace(*unit)) unit++;

    // Parse multiply-expression
    unit=mulex(unit,d);
    if(unit==nullptr) return mark;
    mark=unit;

    // Perform division
    u.dims-=d.dims-DIMSBIAS;
    u.mult/=d.mult;

    // Skip white space
    while(Ascii::isSpace(*unit)) unit++;
    }
  return unit;
  }

/*******************************************************************************/

// Convert from src unit to dst unit; return true if sucess.
// We *do* ensure both srcUnit and dstUnit are actually known units.
FXbool Units::convert(FXdouble& value,const FXchar* srcUnit,const FXchar* dstUnit){
  if(srcUnit && dstUnit){
    FXUnitConv srcu={0.0,DIMSBIAS};
    FXUnitConv dstu={0.0,DIMSBIAS};

    // Skip past spaces
    while(Ascii::isSpace(*srcUnit)) srcUnit++;
    while(Ascii::isSpace(*dstUnit)) dstUnit++;

    // Parse source unit into
    srcUnit=divex(srcUnit,srcu);
    if(srcUnit==nullptr) return false;

    // Parse destination unit info
    dstUnit=divex(dstUnit,dstu);
    if(dstUnit==nullptr) return false;

    // Dimensional difference?
    if(srcu.dims!=dstu.dims) return false;

    // Compute conversion
    value*=srcu.mult;
    value/=dstu.mult;
    return true;
    }
  return false;
  }


// Convert from src unit to S.I. unit; return true if success
FXbool Units::convertToSIFrom(FXdouble& value,const FXchar* srcUnit){
  if(srcUnit){
    FXUnitConv srcu={0.0,DIMSBIAS};
    while(Ascii::isSpace(*srcUnit)) srcUnit++;
    if(divex(srcUnit,srcu)){
      value*=srcu.mult;
      return true;
      }
    }
  return false;
  }


// Convert from S.I. unit to dst unit; return true if success
FXbool Units::convertFromSITo(FXdouble& value,const FXchar* dstUnit){
  if(dstUnit){
    FXUnitConv dstu={0.0,DIMSBIAS};
    while(Ascii::isSpace(*dstUnit)) dstUnit++;
    if(divex(dstUnit,dstu)){
      value/=dstu.mult;
      return true;
      }
    }
  return false;
  }


// Return unit's dimensions
FXulong Units::dimensions(FXuint x){
  if(x<ARRAYNUMBER(UnitDataArray)){
    FXUnitConv u={0.0,DIMSBIAS};
    if((FXuval)UnitDataArray[x].expr<NumBasicUnits){
      FXuint z=(FXuint)(FXuval)UnitDataArray[x].expr;
      return DIMSBIAS+(1<<(z*5));
      }
    if(divex(UnitDataArray[x].expr,u)){
      return u.dims;
      }
    }
  return 0;
  }


// Return unit's dimensions
FXulong Units::dimensions(const FXchar* unit){
  if(unit){
    FXUnitConv u={0.0,DIMSBIAS};
    if(divex(unit,u)){
      return u.dims;
      }
    }
  return 0;
  }


// Scan one unit description, return nullptr or number of characters.
FXival Units::span(const FXchar* unit){
  FXUnitConv u={0.0,DIMSBIAS};
  if(unit){
    const FXchar* end=divex(unit,u);
    if(end){
      return (end-unit);
      }
    }
  return 0;
  }

}


