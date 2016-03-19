/********************************************************************************
*                                                                               *
*                         S y n t a x   P a r s e  r                            *
*                                                                               *
*********************************************************************************
* Copyright (C) 1998,2011 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "fx.h"
#include "Syntax.h"
#include "SyntaxParser.h"


/*
  Notes:
  - The is the new, more strict parser for the syntax coloring files.
  - New feature is DefaultRule.  The default rule matches all text, so 
    is typically used as toplevel rule to contain the other rules.
  - Anything not colored by the regular subrules is colored by the
    DefaultRule.
  - Use of DefaultRule is actually optional; there already is a DefaultRule
    created automatically, corresponding to the fallback style (default-style).
  - A DefaultRule is used in a language style to override the editor's default
    setting with some custom coloring scheme; this will override the editor's
    normal style settings.  
  - Inside string, '\' escapes only a '"'.  Otherwise it stands for itself;
    for instance "\"quoted\"" means "quoted" but "line\n" means line\n, not
    line<NL>.  This is to keep patterns moderately sane and avoid multiple
    levels of escaping.
*/

/*******************************************************************************/


// Initialize parser
SyntaxParser::SyntaxParser(const FXchar* src):head(src),tail(src),token(TK_END),line(1){
  }


// Obtain next token from input
FXuint SyntaxParser::gettok(){
  FXuint tok;
  while(*tail){
    head=tail;
    if(*tail=='\n'){                    // End of line
      tail++;
      line++;
      continue;
      }
    if(Ascii::isSpace(*tail)){          // Space
      tail++;
      continue;
      }
    if(*tail=='#'){                     // Comment
      tail++;
      while(*tail && *tail!='\n' && *tail!='\r') tail++;
      continue;
      }
    if(*tail=='"'){                     // String
      tail++;
      while(*tail && *tail!='\n' && *tail!='\r' && *tail!='"'){
        if(*tail=='\\' && *(tail+1)=='"') tail++;
        tail++;
        }
      if(*tail=='"'){
        tail++;
        return TK_STRING;
        }
      return TK_ERROR;                  // Error
      }
    if(Ascii::isDigit(*tail)){          // Number
      tail++;
      while(Ascii::isDigit(*tail)) tail++;
      return TK_INTEGER;
      }
    if(Ascii::isLetter(*tail)){         // Keyword
      tail++;
      tok=(FXuchar)*tail++;
      while(Ascii::isAlphaNumeric(*tail)){
        tok=((tok<<5)+tok) ^ (FXuchar)*tail++;
        }
      FXTRACE((20,"hash(%s) = %u\n",FXString(head,tail-head).text(),tok));
      return tok;
      }
    return TK_ERROR;                    // Error
    }
  head=tail;
  return TK_EOF;                        // End of input
  }


// Parse integer value
FXbool SyntaxParser::parseInteger(FXint& value){
  if(token!=TK_INTEGER){ fxwarning("%d: error: expected integer.\n",line); return false; }
  value=strtol(head,NULL,0);
  token=gettok();
  return true;
  }


// Parse (escaped) string value
FXbool SyntaxParser::parseString(FXString& value){
  if(token!=TK_STRING){ fxwarning("%d: error: expected string.\n",line); return false; }
  value.assign(head+1,tail-head-2);
  value.substitute("\\\"","\"",true);
  token=gettok();
  return true;
  }


// Parse color
FXbool SyntaxParser::parseColor(FXColor& value){
  FXString colorname;
  if(!parseString(colorname)) return false;
  value=colorFromName(colorname);
  if(!value){ fxwarning("%d: error: unkown color name.\n",line); return false; }
  return true;
  }


// Parse (escaped) string value and check its syntax
FXbool SyntaxParser::parseRegex(FXString& value){
  FXRex::Error error;
  FXRex expression;
  if(!parseString(value)) return false;
  error=expression.parse(value,FXRex::Syntax);
  if(error){ fxwarning("%d: error: %s.\n",line,FXRex::getError(error)); return false; }
  return true;
  }


// Parse rule and sub rules
FXbool SyntaxParser::parseRule(Syntax *syntax,FXint parent){
  FXString name,openpat,clospat,stoppat;
  FXint index;
  if(token==TK_RULE){
    token=gettok();

    // Rule name
    if(!parseString(name)) return false;

    // Parse various features
    while(1){
      switch(token){
        case TK_PATTERN:                // Simple pattern
          token=gettok();
          if(!parseRegex(openpat)) return false;
          continue;
        case TK_OPENPATTERN:            // Open pattern
          token=gettok();
          if(!parseRegex(openpat)) return false;
          continue;
        case TK_CLOSEPATTERN:           // Close pattern
          token=gettok();
          if(!parseRegex(clospat)) return false;
          continue;
        case TK_STOPPATTERN:            // Stop pattern
          token=gettok();
          if(!parseRegex(stoppat)) return false;
          continue;
        }
      break;
      }

    // Create rule
    if(openpat.empty()){
      index=syntax->appendDefault(name,parent);
      }
    else if(clospat.empty()){
      index=syntax->appendSimple(name,openpat,parent);
      }
    else if(stoppat.empty()){
      index=syntax->appendBracket(name,openpat,clospat,parent);
      }
    else{
      index=syntax->appendSafeBracket(name,openpat,clospat,stoppat,parent);
      }

    // Parse subrules, if any
    while(token==TK_RULE){
      if(!parseRule(syntax,index)) return false;
      }

    // Check end
    if(token!=TK_END){ fxwarning("%d: error: expected 'end'.\n",line); return false; }
    token=gettok();
    return true;
    }
  return false;
  }


// Parse language
FXbool SyntaxParser::parseLanguage(SyntaxList& syntaxes){
  FXString name,filesmatch,contentsmatch,delimiters;
  FXint    contextlines=1;
  FXint    contextchars=1;
  Syntax  *syntax;

  // Expect language
  if(token==TK_LANGUAGE){
    token=gettok();

    // Language name
    if(!parseString(name)) return false;

    // Parse various features
    while(1){
      switch(token){
        case TK_FILESMATCH:             // File extensions
          token=gettok();
          if(!parseString(filesmatch)) return false;
          continue;
        case TK_CONTENTSMATCH:          // File contents
          token=gettok();
          if(!parseString(contentsmatch)) return false;
          continue;
        case TK_DELIMITERS:             // Word delimiters
          token=gettok();
          if(!parseString(delimiters)) return false;
          continue;
        case TK_CONTEXTLINES:           // Context lines
          token=gettok();
          if(!parseInteger(contextlines))  return false;
          continue;
        case TK_CONTEXTCHARS:           // Context chars
          token=gettok();
          if(!parseInteger(contextchars))  return false;
          continue;
        }
      break;
      }

    // Create language
    syntax=new Syntax(name);
    syntax->setExtensions(filesmatch);
    syntax->setContents(contentsmatch);
    syntax->setDelimiters(delimiters);
    syntax->setContextLines(contextlines);
    syntax->setContextChars(contextchars);

    // Add new syntax to list
    syntaxes.append(syntax);

    // Parse rules
    while(token==TK_RULE){
      if(!parseRule(syntax,0)) return false;
      }

    // Check end
    if(token!=TK_END){ fxwarning("%d: error: expected 'end'.\n",line); return false; }
    token=gettok();
    return true;
    }
  return false;
  }


// Parse file
FXbool SyntaxParser::parse(SyntaxList& syntaxes){
  token=gettok();
  while(token==TK_LANGUAGE){
    if(!parseLanguage(syntaxes)) return false;
    }
  return true;
  }


// Parse string and return syntaxes found in it; return false if problem.
FXbool SyntaxParser::parse(const FXchar* string,SyntaxList& syntaxes){
  SyntaxParser parser(string);
  return parser.parse(syntaxes);
  }


// Parse string and return syntaxes found in it; return false if problem.
FXbool SyntaxParser::parse(const FXString& string,SyntaxList& syntaxes){
  return parse(string.text(),syntaxes);
  }


// Parse file and return syntaxes found in it; return false if problem.
FXbool SyntaxParser::parseFile(const FXString& filename,SyntaxList& syntaxes){
  FXFile file(filename,FXIO::Reading);
  FXTRACE((10,"SyntaxParser::parseFile(%s)\n",filename.text()));
  if(file.isOpen()){
    FXString string;
    string.length(file.size());
    if(file.readBlock(string.text(),string.length())==string.length()){
      return parse(string,syntaxes);
      }
    }
  return false;
  }


// Clean up
SyntaxParser::~SyntaxParser(){
  }
