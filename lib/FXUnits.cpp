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
#include "fxchar.h"
#include "fxmath.h"
#include "fxascii.h"
#include "fxunicode.h"
#include "FXString.h"
#include "FXUnits.h"

/*
  Notes:

  - The NEW units class performs transformations between units.  It parses
    a unit expression, building up the transformation from one unit to another.

  - The resulting FXUnits instance can be used repeatedly to transform units
    in bulk-quantities, as long as the unit expression doesn't change.

  - Apart from units, a unit expression may also be preceeded by magnitude
    prefixes: (y,z,a,f,p,n,μ,u,m,c,d,h,k,M,G,T,P,E,Z,Y), these scale the
    corresponding conversion.

  - The S.I. system recognizes 9 basic units: (kg,m,A,cd,mol,rad,sr,s,K).
    All other units can be expressed in terms of these, via unit expressions.

  - Unlike other such libraries, FXUnits can parse any combination of units,
    as long as unit expression is built up out of the base units or any of the
    pre-defined units currently incorporated in the list.

  - When converting units, it is important to ensure that the dimensions of
    the source-unit and the destination-unit match.  To this end, FXUnits
    not only computes the unit transformation, but also carries a dimenstion-
    vector which describes the respective units in terms of their base-units.

    The dimension-vector is stored into a single 64-bit integer, making
    dimension-checking into a single comparison operation.

  - Convenience APIs are provided in case the source- or destination-unit
    is part of the S.I. system.  For example, convertFromTo(srcunit) converts
    from the given source unit to whatever S.I. unit would be equivalent to
    that.

  - To enforce dimensionality checking, another set of APIs pass the desired
    dimension-vector. For example, convertFromToDims(srcunit,dims) can parse
    any unit expression, but ensures that the dimensionality will match the
    given dimensions.

    This way, you can let a user pick the units he or she wants, but are
    always assured that the program will ultimately read the required
    dimensions.  For example,

    convertFromToDims("lb",0x108421084211) reads mass. Changing "lb" to
    "in" will fail, as "in" has dimension-vector 0x108421084230 which is
    different from 0x108421084211.

  - The dimensions() API returns the dimension-vector corresponding to
    the unit-parameter.  You can then take these into your program and
    let a user pick the units but ensure proper dimensions.

  - The canonical() API takes a dimension-vector and returns a unit-
    string, in S.I. units, that represents the given dimension-vector.

    Thus, we have unit-expression -> dimension-vector -> unit-expression
    roundtrip capability.

  - The invert() API inverts the direction of the FXUnits transformation.

  - Some UTF-8 support is available in this implementation for parsing
    degree-sign (°C), the greek symbol for micro (μ) [1E-6], and minutes
    and seconds of arc, Ångstrom (Å).

  - UTF8 superscripts ^1 '\xC2\xB9' (¹), ^2 '\xC2\xB2' (²) and ^3 '\xC2\xB3' (³).
    We also have superscript minus ^- '\xE2\x81\xBB' (⁻), ^+ '\xE2\x81\xBA' (⁺),
    ^0 '\xE2\x81\xB0' (⁰), ^4 '\xE2\x81\xB4', ... ^9 '\xE2\x81\xB9' (⁴⁵⁶⁷⁸⁹).
    Currently, this does not yet work even though code is in place; it is masked
    by all multi-byte unicode being classified as "word-character".

  - Do we like parentheses in unit-expressions (m*s)^2 v,s, m^2*s^2?  We now
    have an experimental version of this; it is kind of usefull, think about
    being able to suffix numbers like:

      frequency:  10(1/s)

    Without the parentheses, the "1" in 1/s unit expression would be confused
    with the number.  So we'll probably keep this feature.

  - Do we want fractional exponents like √s ? It doesn't happen often; we would
    have exponent with both scale and bias, instead of just bias, in the current
    framework, we could accomodate -8...7, with increments of ½ (1/2).

  - Reference: "Conversion of Units of Measurement," Gordon S. Novak, Jr.,
    IEEE Trans. on Software Engineering, Vol. 21, No. 8, 1995, pp. 651-661.
*/


// Extract exponent from dimension-vector
#define PW(dims,x)  (((FXint)(((dims)>>((x)*5))&31))-16)

using namespace FX;

/*******************************************************************************/

namespace FX {


// Used in unit reduction
struct FXUnits::Conv {
  FXdouble      mult;   // Multiplier
  FXdouble      plus;   // Addend
  FXulong       dims;   // Dimensions (with DIMSBIAS)
  };


// Biased dimensions-vector, 5-bits for each
const FXulong DIMSBIAS=FXULONG(0x108421084210);


// Dimensions with bias 16 for basic unit i
static const FXulong dimmies[]={
  DIMSBIAS+(FXULONG(1)<<(5*0)),         // Mass
  DIMSBIAS+(FXULONG(1)<<(5*1)),         // Length
  DIMSBIAS+(FXULONG(1)<<(5*2)),         // Current
  DIMSBIAS+(FXULONG(1)<<(5*3)),         // Luminous flux
  DIMSBIAS+(FXULONG(1)<<(5*4)),         // Mole
  DIMSBIAS+(FXULONG(1)<<(5*5)),         // Angles
  DIMSBIAS+(FXULONG(1)<<(5*6)),         // Solid angles
  DIMSBIAS+(FXULONG(1)<<(5*7)),         // Time
  DIMSBIAS+(FXULONG(1)<<(5*8)),         // Temperature / Kelvin
  DIMSBIAS+(FXULONG(1)<<(5*8)),         // Temperature / Celsius
  DIMSBIAS+(FXULONG(1)<<(5*8)),         // Temperature / Fahrenheit
  DIMSBIAS+(FXULONG(1)<<(5*8)),         // Temperature / Rankine
  };


// Addends for each unit
static const FXdouble addends[]={
  0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,273.15,459.67,0.0
  };


// Unit symbols
const FXchar FXUnits::symbol[NumUnits][8]={
  "A",                          // Ampere
  "Bq",                         // Becquerel
  "Btu",                        // BritishThermalUnit
  "C",                          // Coulomb
  "Ci",                         // Curie
  "Da",                         // Dalton
  "F",                          // Farad
  "Fdy",                        // Faraday
  "Gy",                         // Gray
  "H",                          // Henry
  "Hz",                         // Hertz
  "J",                          // Joule
  "K",                          // Kelvin
  "L",                          // Liter
  "N",                          // Newton
  "Oe",                         // Oersted
  "Ohm",                        // Ohm
  "P",                          // Poise
  "Pa",                         // Pascal
  "Pdl",                        // Poundal
  "Pica",                       // Pica
  "R",                          // Roentgen
  "S",                          // Siemens
  "St",                         // Stokes
  "Sv",                         // Sievert
  "T",                          // Tesla
  "U",                          // UnifiedAtomicMass
  "V",                          // Volt
  "W",                          // Watt
  "Wb",                         // Weber
  "a",                          // Are
  "acre",                       // Acre
  "arcmin",                     // ArcMinute
  "arcs",                       // ArcSecond
  "atm",                        // Atmosphere
  "au",                         // AstronomicalUnit
  "b",                          // Barn
  "bar",                        // Bar
  "bbl",                        // Barrel
  "bu",                         // Bushel
  "c",                          // Lightspeed
  "cal",                        // Calorie
  "cd",                         // Candela
  "ch",                         // Chain
  "ct",                         // Carat
  "cu",                         // USCup
  "d",                          // Day
  "day",                        // Day
  "deg",                        // Degree
  "dr",                         // Dram
  "dwt",                        // Pennyweight
  "dyn",                        // Dyne
  "eV",                         // ElectronVolt
  "erg",                        // Erg
  "fL",                         // FootLambert
  "fath",                       // Fathom
  "fbm",                        // BoardFoot
  "fc",                         // FootCandle
  "ft",                         // Foot
  "ftUS",                       // SurveyFoot
  "ftn",                        // Fortnight
  "fur",                        // Furlong
  "g",                          // Gram
  "gal",                        // USGallon
  "gee",                        // StandardGravity
  "gf",                         // GramForce
  "gr",                         // Grain
  "grad",                       // Gradian
  "h",                          // Hour
  "ha",                         // Hectare
  "hour",                       // Hour
  "hp",                         // HorsePower
  "in",                         // Inch
  "kat",                        // Katal
  "kip",                        // KiloPoundForce
  "kph",                        // KilometersPerHour
  "kt",                         // Knot
  "lam",                        // Lambert
  "lb",                         // AvoirdupoisPound
  "lbf",                        // PoundForce
  "lbt",                        // TroyPound
  "lm",                         // Lumen
  "lux",                        // Lux
  "lx",                         // Lux
  "ly",                         // Lightyear
  "m",                          // Meter
  "mi",                         // USStatuteMile
  "min",                        // Minute
  "mmH2O",                      // MilimeterOfWater
  "mmHg",                       // MilimeterOfMercury
  "mmH\xE2\x82\x82O",           // MilimeterOfWater
  "mol",                        // Mole
  "mph",                        // MilesPerHour
  "nmi",                        // NauticalMile
  "oz",                         // Ounce
  "ozfl",                       // USFluidOunce
  "ozt",                        // TroyOunce
  "pc",                         // Parsec
  "ph",                         // Phot
  "pk",                         // Peck
  "psi",                        // PoundsPerSquareInch
  "pt",                         // Pint
  "qt",                         // Quart
  "rad",                        // Radian
  "rd",                         // Rod
  "rem",                        // Rem
  "rev",                        // Revolution
  "s",                          // Second
  "sb",                         // Stilb
  "slug",                       // Slug
  "sp",                         // Spat
  "sr",                         // Steradian
  "st",                         // Stone
  "t",                          // MetricTon
  "therm",                      // USTherm
  "tn",                         // ShortTon
  "ton",                        // LongTon
  "torr",                       // Torr
  "tr",                         // Turn
  "yd",                         // Yard
  "yr",                         // Year
  "\xC2\xB0",                   // Degree
  "\xC2\xB0" "C",               // DegreesCelsius
  "\xC2\xB0" "F",               // DegreesFahrenheit
  "\xC2\xB0" "K",               // DegreesKelvin
  "\xC2\xB0" "R",               // DegreesRankine
  "\xC2\xB5",                   // Micron
  "\xC3\x85",                   // Angstrom
  "\xCE\xA9",                   // Ohm
  "\xE2\x80\xB2",               // ArcMinute
  "\xE2\x80\xB3",               // ArcSecond
  "\xE2\x84\x83",               // DegreesCelsius
  "\xE2\x84\x89",               // DegreesFahrenheit
  };


// Conversion factors
const FXdouble FXUnits::factor[NumUnits]={
  1.0,                          // Ampere
  1.0,                          // Becquerel
  1055.05585262,                // BritishThermalUnit
  1.0,                          // Coulomb
  3737.0,                       // Curie
  1.66053906892E-27,            // Dalton
  1.0,                          // Farad
  96487.0,                      // Faraday
  1.0,                          // Gray
  1.0,                          // Henry
  1.0,                          // Hertz
  1.0,                          // Joule
  1.0,                          // Kelvin
  0.001,                        // Liter
  1.0,                          // Newton
  79.57747,                     // Oersted
  1.0,                          // Ohm
  0.1,                          // Poise
  1.0,                          // Pascal
  0.13825495376,                // Poundal
  1.0/72.0,                     // Pica
  0.000258,                     // Roentgen
  1.0,                          // Siemens
  0.0001,                       // Stokes
  1.0,                          // Sievert
  1.0,                          // Tesla
  1.66053906892E-27,            // UnifiedAtomicMass
  1.0,                          // Volt
  1.0,                          // Watt
  1.0,                          // Weber
  100.0,                        // Are
  0.40468564224,                // Acre
  0.000290888208665721596153949,// ArcMinute
  4.84813681109535993589914E-06,// ArcSecond
  101325.0,                     // Atmosphere
  149597870700.0,               // AstronomicalUnit
  1E-28,                        // Barn
  100000.0,                     // Bar
  0.158987294928,               // Barrel
  0.03523907,                   // Bushel
  299792458.0,                  // Lightspeed
  4.1868,                       // Calorie
  1.0,                          // Candela
  20.116840234,                 // Chain
  0.0002,                       // Carat
  2.365882365E-4,               // USCup
  86400.0,                      // Day
  86400.0,                      // Day
  0.0174532925199432957692369,  // Degree
  1.7718451953125,              // Dram
  1.55517384,                   // Pennyweight
  0.00001,                      // Dyne
  1.60217733e-19,               // ElectronVolt
  0.0000001,                    // Erg
  3.42625909963539052691674,    // FootLambert
  1.828803658,                  // Fathom
  0.002359737216,               // BoardFoot
  10.764,                       // FootCandle
  0.3048,                       // Foot
  0.304800609601,               // SurveyFoot
  1209600.0,                    // Fortnight
  201.168402337,                // Furlong
  0.001,                        // Gram
  0.003785411784,               // USGallon
  9.80665,                      // StandardGravity
  0.00980665,                   // GramForce
  64.79891,                     // Grain
  0.015707963267948966192313,   // Gradian
  3600.0,                       // Hour
  10000.0,                      // Hectare
  3600.0,                       // Hour
  745.699871582,                // HorsePower
  0.0254,                       // Inch
  1.0,                          // Katal
  4448.22161526,                // KiloPoundForce
  5.0/18.0,                     // KilometersPerHour
  463.0/900.0,                  // Knot
  3183.09886183790671537768,    // Lambert
  0.45359267,                   // AvoirdupoisPound
  4.44822161526,                // PoundForce
  0.3732417216,                 // TroyPound
  1.0,                          // Lumen
  1.0,                          // Lux
  1.0,                          // Lux
  9460730472580800.0,           // Lightyear
  1.0,                          // Meter
  1609.344,                     // USStatuteMile
  60.0,                         // Minute
  9.80665,                      // MilimeterOfWater
  133.3224,                     // MilimeterOfMercury
  9.80665,                      // MilimeterOfWater
  1.0,                          // Mole
  0.44704,                      // MilesPerHour
  1852.0,                       // NauticalMile
  0.028349523125,               // Ounce
  2.95735295625E-5,             // USFluidOunce
  0.0311034768,                 // TroyOunce
  3.08567758149137E16,          // Parsec
  10000.0,                      // Phot
  8.80976754172,                // Peck
  6894.757,                     // PoundsPerSquareInch
  0.0004731765,                 // Pint
  0.0009463529,                 // Quart
  1.0,                          // Radian
  5.029210058,                  // Rod
  0.01,                         // Rem
  6.283185307179586476925286766,// Revolution
  1.0,                          // Second
  10000.0,                      // Stilb
  14.5939029372,                // Slug
  12.566370614359172954,        // Spat
  1.0,                          // Steradian
  6.35029318,                   // Stone
  1000.0,                       // MetricTon
  105480400.0,                  // USTherm
  907.18,                       // ShortTon
  1016.047,                     // LongTon
  133.3224,                     // Torr
  6.283185307179586476925286766,// Turn
  0.9144,                       // Yard
  31556925.9747,                // Year
  0.0174532925199432957692369,  // Degree
  1.0,                          // DegreesCelsius
  5.0/9.0,                      // DegreesFahrenheit
  1.0,                          // DegreesKelvin
  5.0/9.0,                      // DegreesRankine
  1.0E-06,                      // Micron
  1.0E-10,                      // Angstrom
  1.0,                          // Ohm
  0.000290888208665721596153949,// ArcMinute
  4.84813681109535993589914E-06,// ArcSecond
  1.0,                          // DegreesCelsius
  5.0/9.0,                      // DegreesFahrenheit
  };


// Unit expressions
const FXchar FXUnits::expression[NumUnits][16]={
  {'\0', FXUnits::AMPERE},      // Ampere
  "1/s",                        // Becquerel
  "kg*m^2/s^2",                 // BritishThermalUnit
  "A*s",                        // Coulomb
  "1/s",                        // Curie
  "kg",                         // Dalton
  "A^2*s^4/kg*m^2",             // Farad
  "A*s",                        // Faraday
  "m^2/s^2",                    // Gray
  "kg*m^2/A^2*s^2",             // Henry
  "s^-1",                       // Hertz
  "kg*m^2/s^2",                 // Joule
  {'\0', FXUnits::KELVIN},      // Kelvin
  "m^3",                        // Liter
  "kg*m/s^2",                   // Newton
  "A/m",                        // Oersted
  "kg*m^2/A^2*s^3",             // Ohm
  "kg/m*s",                     // Poise
  "kg/m*s^2",                   // Pascal
  "kg*m/s^2",                   // Poundal
  "in",                         // Pica
  "A*s/kg",                     // Roentgen
  "A^2*s^3/kg*m^2",             // Siemens
  "m^2/s",                      // Stokes
  "m^2/s^2",                    // Sievert
  "kg/A*s^2",                   // Tesla
  "kg",                         // UnifiedAtomicMass
  "kg*m^2/A*s^3",               // Volt
  "kg*m^2/s^3",                 // Watt
  "kg*m^2/A*s^2",               // Weber
  "m^2",                        // Are
  "ha",                         // Acre
  "rad",                        // ArcMinute
  "rad" ,                       // ArcSecond
  "kg/m*s^2",                   // Atmosphere
  "m",                          // AstronomicalUnit
  "m^2",                        // Barn
  "kg/m*s^2",                   // Bar
  "m^3",                        // Barrel
  "m^3",                        // Bushel
  "m/s",                        // Lightspeed
  "kg*m^2/s^2",                 // Calorie
  {'\0', FXUnits::CANDELA},     // Candela
  "m",                          // Chain
  "kg",                         // Carat
  "m^3",                        // USCup
  "s",                          // Day
  "s",                          // Day
  "rad",                        // Degree
  "g",                          // Dram
  "g",                          // Pennyweight
  "kg*m/s^2",                   // Dyne
  "kg*m^2/s^2",                 // ElectronVolt
  "kg*m^2/s^2",                 // Erg
  "cd/m^2",                     // FootLambert
  "m",                          // Fathom
  "m^3",                        // BoardFoot
  "cd*sr/m^2",                  // FootCandle
  "m",                          // Foot
  "m",                          // SurveyFoot
  "s",                          // Fortnight
  "m",                          // Furlong
  {'\0', FXUnits::GRAM},        // Gram
  "m^3",                        // USGallon
  "m/s^2",                      // StandardGravity
  "kg*m/s^2",                   // GramForce
  "mg",                         // Grain
  "rad",                        // Gradian
  "s",                          // Hour
  "m^2",                        // Hectare
  "s",                          // Hour
  "kg*m^2/s^2",                 // HorsePower
  "m",                          // Inch
  "mol/s",                      // Katal
  "kg*m/s^2",                   // KiloPoundForce
  "m/s",                        // KilometersPerHour
  "m/s",                        // Knot
  "cd/m^2",                     // Lambert
  "kg",                         // AvoirdupoisPound
  "kg*m/s^2",                   // PoundForce
  "kg",                         // TroyPound
  "cd*sr",                      // Lumen
  "cd*sr/m^2",                  // Lux
  "cd*sr/m^2",                  // Lux
  "m",                          // Lightyear
  {'\0', FXUnits::METER},       // Meter
  "m",                          // USStatuteMile
  "s",                          // Minute
  "kg/m*s^2",                   // MilimeterOfWater
  "kg/m*s^2",                   // MilimeterOfMercury
  "kg/m*s^2",                   // MilimeterOfWater
  {'\0', FXUnits::MOLE},        // Mole
  "m/s",                        // MilesPerHour
  "m",                          // NauticalMile
  "kg",                         // Ounce
  "m^3",                        // USFluidOunce
  "kg",                         // TroyOunce
  "m",                          // Parsec
  "cd*sr/m^2",                  // Phot
  "L",                          // Peck
  "Pa",                         // PoundsPerSquareInch
  "m^3",                        // Pint
  "m^3",                        // Quart
  {'\0', FXUnits::RADIAN},      // Radian
  "m",                          // Rod
  "m^2/s^2",                    // Rem
  "rad",                        // Revolution
  {'\0', FXUnits::SECOND},      // Second
  "cd/m^2",                     // Stilb
  "kg",                         // Slug
  "sr",                         // Spat
  {'\0', FXUnits::STERADIAN},   // Steradian
  "kg",                         // Stone
  "kg",                         // MetricTon
  "J",                          // USTherm
  "kg",                         // ShortTon
  "kg",                         // LongTon
  "kg/m^2",                     // Torr
  "rad",                        // Turn
  "m",                          // Yard
  "s",                          // Year
  "rad",                        // Degree
  {'\0', FXUnits::CELSIUS},     // DegreesCelsius
  {'\0', FXUnits::FAHRENHEIT},  // DegreesFahrenheit
  {'\0', FXUnits::KELVIN},      // DegreesKelvin
  {'\0', FXUnits::RANKINE},     // DegreesRankine
  "m",                          // Micron
  "m",                          // Angstrom
  "kg*m^2/A^2*s^3",             // Ohm
  "rad",                        // ArcMinute
  "rad",                        // ArcSecond
  {'\0', FXUnits::CELSIUS},     // DegreesCelsius
  {'\0', FXUnits::KELVIN},      // DegreesFahrenheit
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
FXint FXUnits::lookup(const FXchar* unit){
  FXint l=0,h=NumUnits-1,m,c;
  while(l<=h){
    m=(h+l)>>1;
    c=unitCompare(unit,symbol[m]);
    if(c==0) return m;
    if(c<0) h=m-1;
    if(c>0) l=m+1;
    }
  return -1;
  }


// Look up unit string
FXint FXUnits::lookup(const FXString& unit){
  return lookup(unit.text());
  }


// Return dimension-description from a unit-string
FXulong FXUnits::dimensions(const FXchar* unit){
  if(unit){
    Conv u={0.0,0.0,DIMSBIAS};
    if(divex(unit,u)) return u.dims;
    }
  return 0;
  }


// Return dimension-description from a unit-string
FXulong FXUnits::dimensions(const FXString& unit){
  return dimensions(unit.text());
  }


// Names of basic symbols
static const FXchar basicSymbol[][4]={
  "kg","m","A","cd","mol","rad","sr","s","K"
  };


// Map power to string
static const FXchar symbolPower[][4]={
  "","","^2","^3","^4","^5","^6","^7","^8","^9","^10","^11","^12","^13","^14","^15","^16"
  };


// Return canonical unit-string from dimension-description
// It will contain at most one '/' and be of the form:
//
//   kg*m/s^2
//
// A fancy unicode version would use superscripts for the
// exponents; could be in a future version.
FXString FXUnits::canonical(FXulong dims){
  FXString num,den;
  for(FXint x=GRAM; x<=KELVIN; ++x){
    FXint p=PW(dims,x);
    if(0<p){
      if(!num.empty()) num.append('*');
      num.append(basicSymbol[x]);
      num.append(symbolPower[p]);
      }
    if(p<0){
      if(!den.empty()) den.append('*');
      den.append(basicSymbol[x]);
      den.append(symbolPower[-p]);
      }
    }
  if(num.empty()){
    if(den.empty()) return "1";
    return "1/"+den;
    }
  if(den.empty()) return num;
  return num+"/"+den;
  }


// Return number of characters consumed from unit-string
FXint FXUnits::span(const FXchar* unit){
  if(unit){
    Conv u={0.0,0.0,DIMSBIAS};
    const FXchar* end=divex(unit,u);
    if(end){
      return FXint(end-unit);
      }
    }
  return 0;
  }


// Return number of characters consumed from unit-string
FXint FXUnits::span(const FXString& unit){
  return span(unit.text());
  }


// Lookup unit type and populate u
const FXchar* FXUnits::unitex(const FXchar* unit,Conv& u){
  FXint x=lookup(unit);
  FXTRACE(1,"lookup(%s)=%d\n",unit,x);
  if(0<=x){
    if(!expression[x][0]){      // Unit-expression is empty for basic units
      FXint e=expression[x][1]; // This is the basic unit enum
      u.mult=factor[x];         // Set scale factor
      u.plus=addends[e];        // Set addend
      u.dims=dimmies[e];        // Set dimensions
      }
    else{                       // Derived unit: expand it further!
      divex(expression[x],u);   // Parse unit sub-expression
      u.mult*=factor[x];        // And apply scale factor
      }
    unit+=strlen(symbol[x]);    // Advance past unit
    return unit;
    }
  return nullptr;
  }


// Parse scaling prefix
const FXchar* FXUnits::scalex(const FXchar* unit,Conv& u){
  const FXchar* mark=unit;

  // Parse sub-expression (expr)
  if(*mark=='('){
    mark++;
    mark=divex(mark,u);
    if(mark==nullptr) return nullptr;
    if(*mark!=')') return nullptr;
    mark++;
    return mark;
    }

  // For example: Hz = s^-1 = 1/s
  if(*mark=='1'){
    mark++;
    u.mult=1.0;
    u.plus=0.0;
    u.dims=DIMSBIAS;
    return mark;
    }

  // First try w/no scaling prefix
  mark=unitex(mark,u);

  // Maybe try w/scaling prefix
  if(mark==nullptr){
    FXdouble k=1.0;

    mark=unit;

    // Check for scaling prefix
    switch(*mark++){
      case 'Y': k=1.0E+24; break;
      case 'Z': k=1.0E+21; break;
      case 'E': k=1.0E+18; break;
      case 'P': k=1.0E+15; break;
      case 'T': k=1.0E+12; break;
      case 'G': k=1.0E+09; break;
      case 'M': k=1.0E+06; break;
      case 'k': k=1.0E+03; break;
      case 'h': k=1.0E+02; break;
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
      case '\xCE': if(*mark++=='\xBC'){ k=1.0E-6; break; }
      default: return nullptr;
      }

    // Try unit name after scaling prefix
    mark=unitex(mark,u);
    if(mark==nullptr) return nullptr;

    unit=mark;

    // Apply multiplier
    u.mult*=k;
    }
  return unit;
  }


// Parse power-expression
const FXchar* FXUnits::powex(const FXchar* unit,Conv& u){
  const FXchar* mark=unit;
  FXint expo=0;
  FXint sign=0;

  // Parse scaling-expression
  mark=scalex(mark,u);
  if(mark==nullptr) return nullptr;
  unit=mark;

  // Exponentiation syntax like: x^2.
  if(mark[0]=='^'){
    mark++;

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

  // Exponent is not zero
  if(expo){

    // Apply sign
    if(sign) expo=-expo;

    // Perform the power
    u.mult=Math::powi(u.mult,expo);
    u.plus=0.0;
    u.dims=(u.dims-DIMSBIAS)*expo+DIMSBIAS;
    }

  return unit;
  }


// Parse multiply-expression
const FXchar* FXUnits::mulex(const FXchar* unit,Conv& u){
  const FXchar* mark=unit;

  // Parse power-expression
  mark=powex(mark,u);
  if(mark==nullptr) return nullptr;
  unit=mark;

  // A succession of multiply expressions
  while(*mark=='*'){
    Conv mu={0.0,0.0,DIMSBIAS};

    mark++;

    // Parse multiply-expression
    mark=powex(mark,mu);
    if(mark==nullptr) break;
    unit=mark;

    // Perform multiply
    u.mult*=mu.mult;
    u.plus=0.0;
    u.dims+=mu.dims-DIMSBIAS;
    }
  return unit;
  }


// Parse divide-expression
const FXchar* FXUnits::divex(const FXchar* unit,Conv& u){
  const FXchar* mark=unit;

  // Parse multiply-expression
  mark=mulex(mark,u);
  if(mark==nullptr) return nullptr;
  unit=mark;

  // Got more?
  while(*mark=='/'){
    Conv du={0.0,0.0,DIMSBIAS};

    // Eat '/'
    mark++;

    // Parse multiply-expression
    mark=mulex(mark,du);
    if(mark==nullptr) break;
    unit=mark;

    // Perform division
    u.mult/=du.mult;
    u.plus=0.0;
    u.dims-=du.dims-DIMSBIAS;
    }
  return unit;
  }


// Convert from src unit to dst unit; return true if sucess.
// We *do* ensure both srcUnit and dstUnit are actually known units.
FXUnits FXUnits::convertFromTo(const FXchar* srcUnit,const FXchar* dstUnit){
  FXUnits result(0.0,0.0);
  FXTRACE(1,"FXUnits::convertFromTo(%s,%s):\n",srcUnit,dstUnit?dstUnit:"");
  if(srcUnit){
    Conv srcu={0.0,0.0,DIMSBIAS};
    Conv dstu={0.0,0.0,DIMSBIAS};

    // Parse source unit
    srcUnit=divex(srcUnit,srcu);
    if(srcUnit){

      FXTRACE(1,"src: %20.18lf %20.18lf\n",srcu.mult,srcu.plus);
      FXTRACE(1,"src:   g   m   A  cd   mol rad sr  s   K\n    %4d%4d%4d%4d%4d%4d%4d%4d%4d\n",PW(srcu.dims,0),PW(srcu.dims,1),PW(srcu.dims,2),PW(srcu.dims,3),PW(srcu.dims,4),PW(srcu.dims,5),PW(srcu.dims,6),PW(srcu.dims,7),PW(srcu.dims,8));

      if(dstUnit){

        // Parse destination unit
        dstUnit=divex(dstUnit,dstu);
        if(!dstUnit) return result;

        FXTRACE(1,"dst: %20.18lf %20.18lf\n",dstu.mult,dstu.plus);
        FXTRACE(1,"dst:   g   m   A  cd   mol rad sr  s   K\n    %4d%4d%4d%4d%4d%4d%4d%4d%4d\n",PW(dstu.dims,0),PW(dstu.dims,1),PW(dstu.dims,2),PW(dstu.dims,3),PW(dstu.dims,4),PW(dstu.dims,5),PW(dstu.dims,6),PW(dstu.dims,7),PW(dstu.dims,8));

        // Check dimensions
        if(srcu.dims!=dstu.dims) return result;

        // Convert to destination units
        // Conceptually, we're implementing:
        //
        //   v = v + As
        //   v = v * Ms
        //   v = v / Md
        //   v = v - Ad
        //
        // Transform this into:
        //
        //   v = (v + As) * Ms / Md - Ad
        //
        // Which becomes:
        //
        //   v = v * M + A
        //
        // where:
        //
        //   M = Ms / Md
        //
        // and:
        //
        //   A = As * Ms / Md - Ad  =  As * M - Ad
        //
        // We do this transform such that bulk-conversion of numbers becomes
        // a single "fused-multiply-add" which is something modern processors
        // do quite fast, typically in a single instruction.  The FMA is also
        // usually more accurate than a multiply and add, separately.
        result.m=srcu.mult/dstu.mult;
        result.a=srcu.plus*result.m-dstu.plus;
        return result;
        }

      // Convert to S.I. units
      // This would have been implemented as:
      //
      //   v = v + As
      //   v = v * Ms
      //
      // But as above, we prefer "fused-multiply-add" which means we want to
      // do the multiply first, then the add.  So then the above becomes:
      //
      //   v = v * Ms + As * Ms, or:
      //
      //   v = v * M + A
      //
      // where:
      //
      //   M = Ms
      //
      // and:
      //
      //   A = As * Ms
      //
      // This transformation gets us back to FMA, as desired.
      result.m=srcu.mult;
      result.a=srcu.plus*result.m;
      }
    }
  return result;
  }


// Convert from source unit to destination unit; default is convert to S.I.
FXUnits FXUnits::convertFromTo(const FXString& srcUnit,const FXchar* dstUnit){
  return convertFromTo(srcUnit.text(),dstUnit);
  }


// Convert from source unit to destination unit; default is convert to S.I.
FXUnits FXUnits::convertFromTo(const FXchar* srcUnit,const FXString& dstUnit){
  return convertFromTo(srcUnit,dstUnit.text());
  }


// Convert from source unit to destination unit; default is convert to S.I.
FXUnits FXUnits::convertFromTo(const FXString& srcUnit,const FXString& dstUnit){
  return convertFromTo(srcUnit.text(),dstUnit.text());
  }

/*******************************************************************************/


// Convert to destination unit from source unit; default is convert from S.I.
FXUnits FXUnits::convertToFrom(const FXchar* dstUnit,const FXchar* srcUnit){
  FXUnits result(0.0,0.0);
  FXTRACE(1,"FXUnits::convertToFrom(%s,%s):\n",dstUnit,srcUnit?srcUnit:"");
  if(dstUnit){
    Conv srcu={0.0,0.0,DIMSBIAS};
    Conv dstu={0.0,0.0,DIMSBIAS};

    // Parse destination unit
    dstUnit=divex(dstUnit,dstu);
    if(dstUnit){

      FXTRACE(1,"dst: %20.18lf %20.18lf\n",dstu.mult,dstu.plus);
      FXTRACE(1,"dst:   g   m   A  cd   mol rad sr  s   K\n    %4d%4d%4d%4d%4d%4d%4d%4d%4d\n",PW(dstu.dims,0),PW(dstu.dims,1),PW(dstu.dims,2),PW(dstu.dims,3),PW(dstu.dims,4),PW(dstu.dims,5),PW(dstu.dims,6),PW(dstu.dims,7),PW(dstu.dims,8));

      if(srcUnit){

        // Parse source unit
        srcUnit=divex(srcUnit,srcu);
        if(!srcUnit) return result;

        FXTRACE(1,"src: %20.18lf %20.18lf\n",srcu.mult,srcu.plus);
        FXTRACE(1,"src:   g   m   A  cd   mol rad sr  s   K\n    %4d%4d%4d%4d%4d%4d%4d%4d%4d\n",PW(srcu.dims,0),PW(srcu.dims,1),PW(srcu.dims,2),PW(srcu.dims,3),PW(srcu.dims,4),PW(srcu.dims,5),PW(srcu.dims,6),PW(srcu.dims,7),PW(srcu.dims,8));

        // Check dimensions
        if(dstu.dims!=srcu.dims) return result;

        // Convert from source unit
        result.m=srcu.mult/dstu.mult;
        result.a=srcu.plus*result.m-dstu.plus;
        return result;
        }

      // Convert from S.I. units
      result.m=1.0/dstu.mult;
      result.a=-dstu.plus;
      }
    }
  return result;
  }


// Convert to destination unit from source unit; default is convert from S.I.
FXUnits FXUnits::convertToFrom(const FXString& dstUnit,const FXchar* srcUnit){
  return convertToFrom(dstUnit.text(),srcUnit);
  }


// Convert to destination unit from source unit; default is convert from S.I.
FXUnits FXUnits::convertToFrom(const FXchar* dstUnit,const FXString& srcUnit){
  return convertToFrom(dstUnit,srcUnit.text());
  }


// Convert to destination unit from source unit; default is convert from S.I.
FXUnits FXUnits::convertToFrom(const FXString& dstUnit,const FXString& srcUnit){
  return convertToFrom(dstUnit.text(),srcUnit.text());
  }

/*******************************************************************************/

// Convert from source units to S.I., checking dimension-description
FXUnits FXUnits::convertFromToDims(const FXchar* srcUnit,FXulong dims){
  FXUnits result(0.0,0.0);
  if(srcUnit){
    Conv srcu={0.0,0.0,DIMSBIAS};
    srcUnit=divex(srcUnit,srcu);
    if(srcUnit && srcu.dims==dims){
      result.m=srcu.mult;
      result.a=srcu.plus*result.m;
      }
    }
  return result;
  }


// Convert from source units to S.I., checking dimension-description
FXUnits FXUnits::convertFromToDims(const FXString& srcUnit,FXulong dims){
  return convertFromToDims(srcUnit.text(),dims);
  }


// Convert to destimation units from S.I., checking dimension-description
FXUnits FXUnits::convertToFromDims(const FXchar* dstUnit,FXulong dims){
  FXUnits result(0.0,0.0);
  if(dstUnit){
    Conv dstu={0.0,0.0,DIMSBIAS};
    dstUnit=divex(dstUnit,dstu);
    if(dstUnit && dstu.dims==dims){
      result.m=1.0/dstu.mult;
      result.a=-dstu.plus;
      }
    }
  return result;
  }


// Convert to destimation units from S.I., checking dimension-description
FXUnits FXUnits::convertToFromDims(const FXString& dstUnit,FXulong dims){
  return convertToFromDims(dstUnit.text(),dims);
  }

}

