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
#ifndef FXUNITS_H
#define FXUNITS_H

namespace FX {

namespace Units {

/// Returned by lookup if unit not found.
const FXuint NotFound=0xFFFFFFFF;

/// Lookup unit, if it exists.
extern FXuint lookup(const FXchar* unit);

/// Returns the number supported of units.
extern FXuint number();

/// Return true if x is a basic unit
extern FXbool basic(FXuint x);

/// Return unit symbol (in utf8) if it exists.
extern const FXchar* symbol(FXuint x);

/// Return unit name (in utf8) if it exists.
extern const FXchar* name(FXuint x);

/// Return units expression, if exists and non-non-basic
extern const FXchar* expression(FXuint x);

/// Return unit conversion factor if it exists.
extern FXdouble factor(FXuint x);

/// Return unit's (encoded) dimensions
extern FXulong dimensions(FXuint x);

/// Return unit's (encoded) dimensions
extern FXulong dimensions(const FXchar* unit);

/// Convert from src unit to dst unit; return true if success.
extern FXbool convert(FXdouble& value,const FXchar* srcUnit,const FXchar* dstUnit);

/// Convert from src unit to S.I. unit; return true if success
extern FXbool convertToSIFrom(FXdouble& value,const FXchar* srcUnit);

/// Convert from S.I. unit to dst unit; return true if success
extern FXbool convertFromSITo(FXdouble& value,const FXchar* dstUnit);

/// Scan unit to return the number of characters.
extern FXival span(const FXchar* unit);

}
}

#endif
