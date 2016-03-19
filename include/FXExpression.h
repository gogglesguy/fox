/********************************************************************************
*                                                                               *
*                      E x p r e s s i o n   E v a l u a t o r                  *
*                                                                               *
*********************************************************************************
* Copyright (C) 1998,2013 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#ifndef FXEXPRESSION_H
#define FXEXPRESSION_H


namespace FX {


/// Expression
class FXAPI FXExpression {
private:
  FXuchar *code;
private:
  static const FXuchar initial[];
  static const FXchar *const errors[];
public:

  /// Expression error codes
  enum Error {
    ErrOK,              /// No errors
    ErrEmpty,           /// Empty input
    ErrMemory,          /// Out of memory
    ErrParent,          /// Unmatched parentheses
    ErrToken,           /// Illegal token
    ErrComma,           /// Expected comma
    ErrIdent,           /// Unknown identifier
    ErrColon            /// Expected colon
    };

public:

  /// Construct empty expression object
  FXExpression();

  /// Copy expression object
  FXExpression(const FXExpression& orig);

  /// Compile expression; if error is not NULL, error code is returned
  FXExpression(const FXchar* expression,const FXchar* variables=NULL,Error* error=NULL);

  /// Compile expression; if error is not NULL, error code is returned
  FXExpression(const FXString& expression,const FXString& variables=FXString::null,Error* error=NULL);

  /// Assign another expression to this one
  FXExpression& operator=(const FXExpression& orig);

  /// See if expression is empty
  FXbool empty() const { return (code==initial); }

  /// Evaluate expression with given arguments, if any
  FXdouble evaluate(const FXdouble *args=NULL) const;

  /// Parse expression, return error code if syntax error is found
  Error parse(const FXchar* expression,const FXchar* variables=NULL);

  /// Parse expression, return error code if syntax error is found
  Error parse(const FXString& expression,const FXString& variables=FXString::null);

  /// Returns error code for given error
  static const FXchar* getError(Error err){ return errors[err]; }

  /// Saving and loading
  friend FXAPI FXStream& operator<<(FXStream& store,const FXExpression& s);
  friend FXAPI FXStream& operator>>(FXStream& store,FXExpression& s);

  /// Delete
 ~FXExpression();
  };


// Serialization
extern FXAPI FXStream& operator<<(FXStream& store,const FXExpression& s);
extern FXAPI FXStream& operator>>(FXStream& store,FXExpression& s);

}

#endif
