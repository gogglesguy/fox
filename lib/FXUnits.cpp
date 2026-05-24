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

  - To recognize utf8 superscripts like ^2 (\xC2\xB2) or ^3 (\xC2\xB3), we will
    need unicode character-classes; we don't have those in TL but we do have them
    in FOX [to replace ISIN(nonpunct,c)].

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
   {"V",            "Volt",                "Kg*m^2/A*s^2",   1.0},
   {"W",            "Watt",                "Kg*m^2/s^3",     1.0},
   {"Wb",           "Weber",               "Kg*m^2/A*s^2",   1.0},
   {"a",            "Are",                 "m^2",            100.0},
   {"acre",         "Acre",                "ha",             0.40468564224},
   {"arcmin",       "ArcMinute",           "r",              2.9088820866E-4},
   {"arcs",         "ArcSecond",           "r" ,             4.84813681109535993589914E-06},
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
   {"dyn",          "Dyne",                "Kg*m/s^2",       0.00001},
   {"eV",           "ElectronVolt",        "Kg*m^2/s^2",     1.60217733e-19},
   {"erg",          "Erg",                 "Kg*m^2/s^2",     0.0000001},
   {"fath",         "Fathom",              "m",              1.828803658},
   {"fbm",          "BoardFoot",           "m^3",            0.002359737216},
   {"fc",           "FootCandle",          "cd*sr/m^2",      10.7639104167},
   {"flam",         "FootLambert",         "cd/m^2",         3.42625909964},
   {"ft",           "InternationalFoot",   "m",              0.3048},
   {"ftUS",         "SurveyFoot",          "m",              0.304800609601},
   {"ftn",          "Fortnight",           "s",              1209600.0},
   {"fur",          "Furlong",             "m",              201.168402337},
   {"g",            "Gram",                GRAM,             0.001},
   {"gal",          "USGallon",            "m^3",            0.003785411784},
   {"gee",          "StandardGravity",     "m/s^2",          9.80665},
   {"gf",           "GramForce",           "Kg*m/s^2",       0.00980665},
   {"grad",         "Grade",               "rad",            0.015707963267948966192313},
   {"gr",           "Grain",               "mg",             64.79891},
   {"h",            "Hour",                "s",              3600.0},
   {"ha",           "Hectare",             "m^2",            10000.0},
   {"hour",         "Hour",                "s",              3600.0},
   {"hp",           "Horsepower",          "Kg*m^2/s^2",     745.699871582},
   {"in",           "Inch",                "m",              0.0254},
   {"kat",          "Katal",               "mol/s",          1.0},
   {"kip",          "KiloPoundForce",      "Kg*m/s^2",       4448.22161526},
   {"kph",          "KilometersPerHour",   "m/s",            5.0/18.0},
   {"kt",           "Knot",                "m/s",            463.0/900.0},
   {"lam",          "Lambert",             "cd/m^2",         3183.09886184},
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

// Set of non-punctation characters that may legitimately
// appear in a unit abbreviation.
static const FXuchar nonpunct[32]={
  0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x03,0xfe,0xff,0xff,0x87,0xfe,0xff,0xff,0x07,
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,
  };


// Check if character is in set
static inline FXuchar ISIN(const FXuchar set[],FXuchar ch){
  return (set[ch>>3]>>(ch&7))&1;
  }


// Compare unit-string against abbreviation.
static FXint unitCompare(const FXchar* unit,const FXchar* abbr){
  while(*unit && *abbr){
    if(*unit != *abbr) return (FXint)((FXuchar)*unit - (FXuchar)*abbr);
    unit++;
    abbr++;
    }
  if(*abbr) return -1;
  if(ISIN(nonpunct,*unit)) return 1;
  return 0;
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


// Divide unit
static void divUnit(FXUnitConv& u1,const FXUnitConv& u2){
  u1.dims-=u2.dims-DIMSBIAS;
  u1.mult/=u2.mult;
  }


// Multiply unit
static void mulUnit(FXUnitConv& u1,const FXUnitConv& u2){
  u1.dims+=u2.dims-DIMSBIAS;
  u1.mult*=u2.mult;
  }


// Set factor to power
static void powUnit(FXUnitConv& u,FXint p){
  u.dims=(u.dims-DIMSBIAS)*p+DIMSBIAS;
  u.mult=Math::powi(u.mult,p);
  }


// Set basic unit in u
static void setunit(FXUnitConv& u,FXdouble k,FXuint x){
  FXASSERT(x<NumBasicUnits);
  u.dims=DIMSBIAS+(1<<(x*5));
  u.mult=k;
  }


// Expand derived unit in u
static void expunit(FXUnitConv& u,FXdouble k,const FXchar* expr){
  divex(expr,u);
  u.mult*=k;
  }


// Lookup unit type and populate u
static const FXchar* unitex(const FXchar* unit,FXUnitConv& u){
  FXuint x=Units::lookup(unit);
  if(x<ARRAYNUMBER(UnitDataArray)){
    const FXUnitData& info=UnitDataArray[x];

    // Basic unit: we're done!
    if((FXuval)info.expr<NumBasicUnits){
      setunit(u,info.mult,(FXuint)(FXuval)info.expr);
      }

    // Derived unit: expand it further!
    else{
      expunit(u,info.mult,info.expr);
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

  // Parse scaling-expression
  unit=scalex(unit,u);
  if(unit==nullptr) return nullptr;
  mark=unit;

  // Skip white space
  while(Ascii::isSpace(*unit)) unit++;

  // Exponentiation
  if(*unit=='^'){
    unit++;

    // Skip white space
    while(Ascii::isSpace(*unit)) unit++;

    // Sign of exponent
    FXint sign=(*unit=='-');

    // Advance past sign
    if(*unit=='-' || *unit=='+') unit++;

    // Expected a digit
    if(Ascii::isDigit(*unit)){
      FXint expo=*unit-'0';
      unit++;

      // Parse exponent
      while(Ascii::isDigit(*unit)){
        expo=expo*10+(*unit-'0');
        unit++;
        }

      // Extreme exponent won't fit in 5 bits
      if(expo>15) return nullptr;

      // Skip white space
      while(Ascii::isSpace(*unit)) unit++;

      // Apply sign
      if(sign) expo=-expo;

      // Perform the power
      powUnit(u,expo);

      // Absorb input
      mark=unit;
      }
    }
  return mark;
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
    mulUnit(u,m);

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
    divUnit(u,d);

    // Skip white space
    while(Ascii::isSpace(*unit)) unit++;
    }
  return unit;
  }

/*******************************************************************************/

// Convert from src unit to dst unit; return true if sucess.
FXbool Units::convert(FXdouble& value,const FXchar* srcUnit,const FXchar* dstUnit){
  if(srcUnit && dstUnit){

    // Pointers match?
    if(srcUnit!=dstUnit){

      // Skip past spaces
      while(Ascii::isSpace(*srcUnit)) srcUnit++;
      while(Ascii::isSpace(*dstUnit)) dstUnit++;

      // Do unit strings match?
      if(FXString::compare(srcUnit,dstUnit)){
        FXUnitConv srcu={0.0,DIMSBIAS};
        FXUnitConv dstu={0.0,DIMSBIAS};

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
        }
      }
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


