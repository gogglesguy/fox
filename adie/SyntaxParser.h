/********************************************************************************
*                                                                               *
*                         S y n t a x   P a r s e r                             *
*                                                                               *
*********************************************************************************
* Copyright (C) 1998,2015 by Jeroen van der Zijp.   All Rights Reserved.        *
*********************************************************************************
* This program is free software: you can redistribute it and/or modify          *
* it under the terms of the GNU General Public License as published by          *
* the Free Software Foundation, either version 3 of the License, or             *
* (at your option) any later version.                                           *
*                                                                               *
* This program is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                 *
* GNU General Public License for more details.                                  *
*                                                                               *
* You should have received a copy of the GNU General Public License             *
* along with this program.  If not, see <http://www.gnu.org/licenses/>.         *
********************************************************************************/
#ifndef SYNTAXPARSER_H
#define SYNTAXPARSER_H


// Parser for syntax file
class SyntaxParser {
private:
  const FXchar *from;           // Source of text being parsed
  const FXchar *head;           // Start of token
  const FXchar *tail;           // End of token+1
  FXuint        token;          // Token type
  FXint         line;           // Line number
public:
  FXuint gettok();
  FXbool parseString(FXString& value);
  FXbool parseRule(Syntax *syntax,FXint parent);
  FXbool parseLanguage(SyntaxList& syntaxes);
  FXbool parse(SyntaxList& syntaxes);
public:
  enum {
    TK_ERROR=0,
    TK_EOF=1,
    TK_INTEGER=2,
    TK_STRING=3,
    TK_RULE=4120910,
    TK_LANGUAGE=1350891346,
    TK_FILESMATCH=1228350630,
    TK_CONTENTSMATCH=586315177,
    TK_DELIMITERS=2185620912,
    TK_CONTEXTLINES=2311868482,
    TK_CONTEXTCHARS=2319640212,
    TK_PATTERN=3508332936,
    TK_OPENPATTERN=3516757596,
    TK_CLOSEPATTERN=2586583262,
    TK_STOPPATTERN=3111100176,
    TK_GROUP=121469983,  
    TK_STYLE=134930039,
    TK_END=113327
    };
public:

  // Construct parser
  SyntaxParser(const FXchar* pat,const FXchar* frm="input");

  // Parse string and return syntaxes found in it; return false if problem.
  static FXbool parse(SyntaxList& syntaxes,const FXchar* patterns);

  // Parse string and return syntaxes found in it; return false if problem.
  static FXbool parse(SyntaxList& syntaxes,const FXString& patterns);

  // Parse file and return syntaxes found in it; return false if problem.
  static FXbool parseFile(SyntaxList& syntaxes,const FXString& filename);

  // Destroy parser
 ~SyntaxParser();
  };


#endif
