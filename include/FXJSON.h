/********************************************************************************
*                                                                               *
*                      J S O N   R e a d e r  &  W r i t e r                    *
*                                                                               *
*********************************************************************************
* Copyright (C) 2013 by Jeroen van der Zijp.   All Rights Reserved.             *
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
class FXJSON {
public:
  enum Error {
    ErrOK,              /// No errors
    ErrSave,            /// Unable to save
    ErrLoad,            /// Unable to load
    ErrToken,           /// Illegal token
    ErrColon,           /// Expected color ':'
    ErrComma,           /// Expected comma ','
    ErrBracket,         /// Expected closing bracket
    ErrBrace,           /// Expected closing brace
    ErrQuotes,          /// Expected closing quotes
    ErrNumber,          /// Numeric conversion
    ErrEnd              /// Unexpected end of file
    };
  enum {
    Stream,             /// Stream-of-consciousness output
    Compact,            /// Compact, human readable output (default)
    Pretty              /// Pretty printed, indented output
    };
private:
  FXIO       *dev;              // File IO device
  FXchar     *begptr;           // Text buffer begin ptr
  FXchar     *endptr;           // Text buffer end ptr
  FXchar     *rptr;             // Text buffer read ptr
  FXchar     *wptr;             // Text buffer write ptr
  FXchar     *sptr;             // Text buffer scan ptr
  FXival      size;             // Buffer size to allocate
  FXint       column;           // Column number
  FXint       indent;           // Indent level
  FXint       line;             // Line number
  FXint       token;            // Token
  FXint       wrap;             // Line wrap column
  FXuchar     flow;             // Output flow
  FXuchar     prec;             // Float precision
  FXuchar     fmt;              // Float format
  FXuchar     dent;             // Indentation amount
private:
  FXint next();
  Error loadString(FXString& str);
  Error loadMap(FXVariant& var);
  Error loadArray(FXVariant& var);
  Error loadVariant(FXVariant& var);
  Error saveText(const FXchar* ptr,FXint count);
  Error saveIndent(FXint count);
  Error saveString(const FXString& str);
  Error saveMap(const FXVariant& var);
  Error saveArray(const FXVariant& var);
  Error saveVariant(const FXVariant& var);
private:
  static const FXchar *const errors[];
private:
  FXJSON(const FXJSON&){}
  FXJSON &operator=(const FXJSON&);
public:

  /**
  * Construct JSON serializer.
  * Initialize serializer and set suggested buffer size.
  */
  FXJSON(FXival sz=4096);

  /**
  * Construct JSON serializer and open stream.
  * Initialize serializer, set suggested buffer size, and
  * open the stream for the given direction.
  */
  FXJSON(const FXString& filename,FXuint m=FXIO::Reading,FXuint perm=FXIO::AllReadWrite,FXival sz=4096);

  /**
  * Open stream for the given direction.
  * Allocate buffer of suggested size given in constructor.
  */
  FXbool open(const FXString& filename,FXuint m=FXIO::Reading,FXuint perm=FXIO::AllReadWrite);

  /**
  * Return current io device.
  */
  FXIO* device() const { return dev; }

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

  /**
  * Fill buffer from file.
  * Return false if not open for reading, or fail to read from disk.
  */
  virtual FXbool fill();

  /**
  * Flush buffer to file.
  * Return false if not open for writing, or if fail to write to disk.
  */
  virtual FXbool flush();

  /**
  * Close stream and delete buffers.
  * If writing, flush remaining text from buffer first.
  * Return false if not open, or disk is full.
  */
  FXbool close();

  /**
  * Return current line number.
  */
  FXint getLine() const { return line; }

  /**
  * Return current column number.
  */
  FXint getColumn() const { return column; }

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
  */
  void setOutputFlow(FXint f){ flow=f; }
  FXint getOutputFlow() const { return flow; }

  /**
  * Change indentation level for pretty print flow.
  */
  void setIndentation(FXint d){ dent=d; }
  FXint getIndentation() const { return dent; }

  /**
  * Change column at which lines are wrapped.
  */
  void setLineWrap(FXint w){ wrap=w; }
  FXint getLineWrap() const { return wrap; }

  /**
  * Close stream and clean up.
  */
  virtual ~FXJSON();
  };

}

#endif
