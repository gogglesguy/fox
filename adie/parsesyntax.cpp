/********************************************************************************
*                                                                               *
*                     P a r s e   S y n t a x   F i l e                         *
*                                                                               *
*********************************************************************************
* Copyright (C) 1998,2009 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "FXRex.h"
#include "FXArray.h"
#include "Hilite.h"
#include "Commands.h"
#include "TextWindow.h"
#include "Adie.h"


/*
  Notes:
  - Parses Adie syntax and style file.
  - Needs to be redone some day!  Current "string" syntax is strange; replace this
    with the one from FXString escape/unescape.
*/

#define MAXLINE         2048

/*******************************************************************************/


// Used while parsing
class Parser {
private:
  FXString  file;
  FXchar   *line;
  FXchar   *tok;
  FILE     *fp;
  FXint     number;
  FXchar    buffer[MAXLINE];
private:
  FXchar* getline();
  FXint integer();
  const FXchar* token();
  const FXchar* string();
public:
  Parser(const FXString& fn):file(fn),line(NULL),tok(NULL),fp(NULL),number(0){}
  FXbool parse(FXSyntaxList& syntaxes);
  FXbool parserules(FXSyntax *syntax,FXint parent);
  ~Parser(){ if(fp) fclose(fp); }
  };


// Get next non-empty line
FXchar* Parser::getline(){
  register FXchar *ptr;
  while((ptr=fgets(buffer,sizeof(buffer),fp))!=NULL){
    number++;
    while(*ptr && isspace((FXuchar)*ptr)) ptr++;
    if(*ptr!='\0' && *ptr!='\n' && *ptr!='\r' && *ptr!='#') break;
    }
  return ptr;
  }


// Get next token from file
const FXchar* Parser::token(){
  line=tok=getline();
  if(!line) return NULL;
  while(*line && isalpha((FXuchar)*line)) line++;
  *line++='\0';
  return tok;
  }


// Parse word from line
FXint Parser::integer(){
  register FXchar *value;
  while(*line && isspace((FXuchar)*line)) line++;
  value=line;
  while(*line && !isspace((FXuchar)*line)) line++;
  *line++='\0';
  return strtol(value,NULL,10);
  }


// Parse escaped string from line
const FXchar* Parser::string(){
  register FXchar *value,*ptr;
  while(*line && *line!='"') line++;
  if(*line=='"'){
    line++;
    value=ptr=line;
    while(*line && *line!='\n' && *line!='\r' && *line!='"'){
      if(*line=='\\' && *(line+1)=='"') line++;
      *ptr++=*line++;
      }
    *ptr='\0';
    return value;
    }
  return "";
  }


// Parse rules and sub rules
FXbool Parser::parserules(FXSyntax *syntax,FXint parent){
  FXString     name,brex,erex,srex;
  FXRex        expression;
  FXRex::Error error;
  FXint        index;

  FXTRACE((1,"parserules begin parent = %d\n",parent));

  // Parse the rules
  while(strcmp(tok,"rule")==0){

    // Parse name
    name=string();

    // Clear to empty
    brex=FXString::null;
    erex=FXString::null;
    srex=FXString::null;

    // Parse rule info
    while(token()){
      if(strcmp(tok,"pattern")==0){             // Simple pattern
        brex=string();
        error=expression.parse(brex,FXRex::Syntax);
        if(error){
          fxwarning("%s:%d: error: %s.\n",file.text(),number,FXRex::getError(error));
          return false;
          }
        continue;
        }
      if(strcmp(tok,"openpattern")==0){         // Open pattern
        brex=string();
        error=expression.parse(brex,FXRex::Syntax);
        if(error){
          fxwarning("%s:%d: error: %s.\n",file.text(),number,FXRex::getError(error));
          return false;
          }
        continue;
        }
      if(strcmp(tok,"closepattern")==0){        // Close pattern
        erex=string();
        error=expression.parse(erex,FXRex::Syntax);
        if(error){
          fxwarning("%s:%d: error: %s.\n",file.text(),number,FXRex::getError(error));
          return false;
          }
        continue;
        }
      if(strcmp(tok,"stoppattern")==0){         // Stop pattern
        srex=string();
        error=expression.parse(srex,FXRex::Syntax);
        if(error){
          fxwarning("%s:%d: error: %s.\n",file.text(),number,FXRex::getError(error));
          return false;
          }
        continue;
        }
      break;
      }

    // Premature end
    if(!tok){
      fxwarning("%s:%d: error: unexpected end of file.\n",file.text(),number);
      return false;
      }

    FXTRACE((1,"brex = %s\n",brex.text()));
    FXTRACE((1,"erex = %s\n",erex.text()));
    FXTRACE((1,"srex = %s\n",srex.text()));

    // Validation
    if(brex.empty()) return false;

    // Create rule
    if(erex.empty() && srex.empty()){
      index=syntax->append(name,brex,parent);
      }
    else if(srex.empty()){
      index=syntax->append(name,brex,erex,parent);
      }
    else{
      index=syntax->append(name,brex,erex,srex,parent);
      }

    // Parse subrules, if any
    if(!parserules(syntax,index)) return false;

    // Check end
    if(strcmp(tok,"end")!=0){
      fxwarning("%s:%d: error: expected 'end'.\n",file.text(),number);
      return false;
      }

    // Next token
    token();
    if(!tok) return false;
    }
  FXTRACE((1,"parserules end parent = %d\n",parent));
  return true;
  }


// Parse file
FXbool Parser::parse(FXSyntaxList& syntaxes){
  FXSyntax *syntax;
  FXString  name;

  FXTRACE((1,"Parser::parse: file = %s\n",file.text()));

  // Open file
  fp=fopen(file.text(),"r");
  if(!fp){
    fxwarning("error: unable to open file: %s.\n",file.text());
    return false;
    }

  // Parse the languages
  while(token()){

    // Parse next language
    if(strcmp(tok,"language")!=0){
      fxwarning("%s:%d: error: expected 'language'.\n",file.text(),number);
      return false;
      }

    // Parse language name
    name=string();

    // Make new language node
    syntax=new FXSyntax(name);

    // Add to list
    syntaxes.append(syntax);

    // Parse language info
    while(token()){
      if(strcmp(tok,"filesmatch")==0){          // File extensions
        syntax->setExtensions(string());
        continue;
        }
      if(strcmp(tok,"contentsmatch")==0){       // File contents
        syntax->setContents(string());
        continue;
        }
      if(strcmp(tok,"delimiters")==0){          // Word delimiters
        syntax->setDelimiters(string());
        continue;
        }
      if(strcmp(tok,"contextlines")==0){        // Context lines
        syntax->setContextLines(integer());
        continue;
        }
      if(strcmp(tok,"contextchars")==0){        // Context chars
        syntax->setContextChars(integer());
        continue;
        }
      break;
      }

    // Premature end
    if(!tok){
      fxwarning("%s:%d: error: unexpected end of file.\n",file.text(),number);
      return false;
      }

    // Parse rules
    if(!parserules(syntax,0)) return false;

    // Check end
    if(strcmp(tok,"end")!=0){
      fxwarning("%s:%d: error: expected 'end'.\n",file.text(),number);
      return false;
      }
    }
  FXTRACE((1,"Parser::parse: OK\n"));
  return true;
  }



// Parse syntax file
FXbool Adie::loadSyntaxFile(const FXString& file){
  Parser parser(file);
  return parser.parse(syntaxes);
  }
