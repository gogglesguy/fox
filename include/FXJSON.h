/********************************************************************************
*                                                                               *
*                      J S O N   R e a d e r  &  W r i t e r                    *
*                                                                               *
*********************************************************************************
* Copyright (C) 2013,2018 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#ifndef FXJSON_H
#define FXJSON_H

namespace FX {


/**
* The FXJSON serializer loads or saves an FXVariant to a json text file.
* Since FXVariant can contain an arbitrarily complex data structure, this
* provides applications with a convenient way to load and save state information
* in a well-defined and human-readable file format.
*/
class FXAPI FXJSON {
public:
  enum Error {
    ErrOK,              /// No errors
    ErrSave,            /// Unable to save
    ErrLoad,            /// Unable to load
    ErrToken,           /// Illegal token
    ErrColon,           /// Expected colon ':'
    ErrComma,           /// Expected comma ','
    ErrBracket,         /// Expected closing bracket
    ErrBrace,           /// Expected closing brace
    ErrQuotes,          /// Expected closing quotes
    ErrQuote,           /// Expected closing quote
    ErrNumber,          /// Numeric conversion
    ErrEnd              /// Unexpected end of file
    };
  enum Flow {
    Stream,             /// Stream-of-consciousness output
    Compact,            /// Compact, human readable output (default)
    Pretty              /// Pretty printed, indented output
    };
  enum Direction {
    Stop = 0,           /// Not active
    Save = 1,           /// Save to device
    Load = 2            /// Load from device
    };
protected:
  FXchar     *begptr;           // Text buffer begin ptr
  FXchar     *endptr;           // Text buffer end ptr
  FXchar     *wptr;             // Text buffer write ptr
  FXchar     *rptr;             // Text buffer read ptr
  FXchar     *sptr;             // Text buffer scan ptr
  FXlong      offset;           // Position from start
  FXint       column;           // Column number
  FXint       indent;           // Indent level
  FXint       line;             // Line number
  Direction   dir;              // Direction
  FXint       token;            // Token
  FXint       wrap;             // Line wrap column
  FXuchar     flow;             // Output flow
  FXuchar     prec;             // Float precision
  FXuchar     fmt;              // Float format
  FXuchar     esc;              // Escape mode
  FXuchar     dent;             // Indentation amount
private:
  FXint next();
  FXbool need(FXival count);
  FXbool emit(const FXchar* str,FXival count);
  FXbool emit(FXchar ch,FXival count);
  Error loadString(FXString& str);
  Error loadIdent(FXString& str);
  Error loadMap(FXVariant& var);
  Error loadArray(FXVariant& var);
  Error loadVariant(FXVariant& var);
  Error saveString(const FXString& str);
  Error saveIdent(const FXString& str);
  Error saveMap(const FXVariant& var);
  Error saveArray(const FXVariant& var);
  Error saveVariant(const FXVariant& var);
private:
  static const FXchar *const errors[];
private:
  FXJSON(const FXJSON&);
  FXJSON &operator=(const FXJSON&);
public:

  /**
  * Initialize JSON serializer with buffer of size and direction.
  * Text location (column, line number, byte offset) is reset.
  */
  FXJSON();

  /**
  * Construct JSON serializer and open for direction d.
  * Use given buffer data of size sz.
  */
  FXJSON(FXchar* buffer,FXuval sz=8192,Direction d=Load);

  /**
  * Open JSON parse buffer with given size and direction.
  */
  FXbool open(FXchar* buffer=NULL,FXuval sz=8192,Direction d=Load);

  /**
  * Return direction in effect.
  */
  Direction direction() const { return dir; }

  /**
  * Return size of parse buffer
  */
  FXuval size() const { return endptr-begptr; }

  /**
  * Return current line number.
  */
  FXint getLine() const { return line; }

  /**
  * Return current column number.
  */
  FXint getColumn() const { return column; }

  /**
  * Return offset from begin of file.
  */
  FXlong getOffset() const { return offset; }

  /**
  * Load a variant from stream.
  * Return false if stream wasn't opened for loading, or syntax error.
  */
  Error load(FXVariant& variant);

  /**
  * Save a variant to stream.
  * Return false if stream wasn't opened for saving, or disk was full.
  */
  Error save(const FXVariant& variant);

  /// Returns error code for given error
  static const FXchar* getError(Error err){ return errors[err]; }

  /**
  * Floating point output precision control.
  */
  void setNumericPrecision(FXint p){ prec=p; }
  FXint getNumericPrecision() const { return prec; }

  /**
  * Floating point output precision control.
  */
  void setNumericFormat(FXint f){ fmt=f; }
  FXint getNumericFormat() const { return fmt; }

  /**
  * Change output flow format (Stream, Compact, Pretty).
  * Stream is the most compact, but pretty much unreadable by humans; it
  * aims to be compact.
  * Compact is very human-readable while at the same time using minimum number
  * of lines to represent the output.
  * Pretty will print one item on each line, with indentation.  It is very easily
  * readable but produces large numbers of text lines.
  */
  void setOutputFlow(FXint f){ flow=f; }
  FXint getOutputFlow() const { return flow; }

  /**
  * Change indentation level for pretty print flow, the amount of
  * indentation applied for each level.
  */
  void setIndentation(FXint d){ dent=d; }
  FXint getIndentation() const { return dent; }

  /**
  * Change column at which lines are wrapped.
  */
  void setLineWrap(FXint w){ wrap=w; }
  FXint getLineWrap() const { return wrap; }

  /**
  * Change string escape mode; 0=don't escape unicode in strings;
  * 1=escape unicode as \xHH, 2=escape unicode as \uHHHH.
  * Default is to escape control characters only.
  */
  void setEscapeMode(FXint e){ esc=e; }
  FXint getEscapeMode() const { return esc; }

  /**
  * Read at least count bytes into buffer; return bytes available, or -1 for error.
  */
  virtual FXival fill(FXival count);

  /**
  * Write at least count bytes from buffer; return space available, or -1 for error.
  */
  virtual FXival flush(FXival count);

  /**
  * Close stream and delete buffer, if owned.
  */
  FXbool close();

  /**
  * Close JSON stream and clean up.
  */
  virtual ~FXJSON();
  };

}

#endif
