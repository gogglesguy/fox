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
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "fxascii.h"
#include "FXElement.h"
#include "FXArray.h"
#include "FXString.h"
#include "FXIO.h"
#include "FXIODevice.h"
#include "FXStat.h"
#include "FXFile.h"
#include "FXException.h"
#include "FXVariant.h"
#include "FXVariantArray.h"
#include "FXVariantMap.h"
#include "FXJSON.h"

/*
  Notes:

  - Load and save FXVariant object from/to JSON format files.
  - We read over C and C++-style comment; yes, this is not in the spec;
    but it was, once.  Plus, its convenient.
  - Closing the file does not reset comment and line number (in case of error, this
    point to the problem).
  - To do:

      o Add better control of output format, range from "stream of consciousness"
        to compact to pretty printed human friendly output.

      o Error codes returned in API for better problem diagnosis.

      o Facade over FXIO.

      o Floating point formatting options (Sander).

  - JSON syntax is very simple:

         value     : object
                   | array
                   | string
                   | number
                   | 'true'
                   | 'false'
                   | 'null'
                   ;

         object    : '{'   '}'
                   | '{' members '}'
                   ;

         members   : pair
                   | pair ',' members
                   ;

         pair      : string ':' value

         array     : '[' ']'
                   | '[' elements ']'
                   ;

         elements  : value
                   | value ',' elements
                   :

         string    : '"' '"'
                   | '"' chars '"'
                   ;

         chars     : char
                   | char chars
                   ;

         char      : unicode_except_esc_or_quotes
                   | '\"'
                   | '\\'
                   | '\/'
                   | '\b'
                   | '\f'
                   | '\n'
                   | '\r'
                   | '\t'
                   | '\' octdigit octdigit octdigit
                   | '\x' hxdigit hxdigit
                   | '\u' hxdigit hxdigit hxdigit hxdigit
                   ;

         number    : int
                   | int frac
                   | int exp
                   | int frac exp
                   ;

         int       : digit
                   | nzdigit digits
                   | "-" digit
                   | "-" nzdigit digits
                   ;

         digits    : digit | digit digits
                   ;


         frac      : "." digits
                   ;

         exp       : e digits
                   ;

         e         : "e" | "e+" | "e-" | "E" | "E-" | "E+"
                   ;

  - Flow controls the looks of the output.  The current values supported are:

      0 No formatting whatsoever.  This is the most compact format but is essentially
        not human-readable.

      1 Compact.  Try to cram as much as possible on a line, but break lines beyond
        a certain size.

      2 Nicely indented pretty printed output.

    Future values can be considered.

*/


#define MINBUFFER 4096          // Minimum buffer size
#define MAXTOKEN  256           // Maximum token size

using namespace FX;

namespace FX {

enum {
  TK_ERROR   = 0,       // Syntax error
  TK_EOF     = 1,       // End of file
  TK_SPACE   = 2,       // White space
  TK_COMMA   = 3,       // Element separator
  TK_COLON   = 4,       // Key:value pair separator
  TK_NULL    = 5,       // NULL value
  TK_FALSE   = 6,       // Truth value
  TK_TRUE    = 7,
  TK_INT     = 8,       // Integer value
  TK_REAL    = 9,       // Real value
  TK_STRING  = 10,      // String
  TK_LBRACK  = 11,      // Start of array
  TK_LBRACE  = 12,      // Start of map
  TK_RBRACK  = 13,      // End of array
  TK_RBRACE  = 14       // End of map
  };


/*******************************************************************************/


// Error messages
const FXchar *const FXJSON::errors[]={
  "OK",
  "Unable to save",
  "Unable to load",
  "Illegal token",
  "Expected a ':'",
  "Expected a ','",
  "Unmatched ']'",
  "Unmatched '}'",
  "Bad number",
  "Unexpected end of file"
  };


// Construct JSON serializer
FXJSON::FXJSON(FXival sz):begptr(NULL),endptr(NULL),rptr(NULL),wptr(NULL),sptr(NULL),size(FXMAX(sz,MINBUFFER)),column(0),indent(0),line(1),token(TK_EOF),wrap(80),flow(Compact),prec(16),fmt(2),dent(2){
  FXTRACE((1,"FXJSON::FXJSON\n"));
  }


// Construct and open for loading
FXJSON::FXJSON(const FXString& filename,FXuint m,FXuint perm,FXival sz):begptr(NULL),endptr(NULL),rptr(NULL),wptr(NULL),sptr(NULL),size(FXMAX(sz,MINBUFFER)),column(0),indent(0),line(1),token(TK_EOF),wrap(80),flow(Compact),prec(16),fmt(2),dent(2){
  FXTRACE((1,"FXJSON::FXJSON\n"));
  open(filename,m,perm);
  }


// Open archive for operation
FXbool FXJSON::open(const FXString& filename,FXuint m,FXuint perm){
  FXTRACE((2,"FXJSON::open(\"%s\",%o,%o)\n",filename.text(),m,perm));
  if(!file.isOpen()){
    if(file.open(filename,m,perm)){
      if(callocElms(begptr,size)){
        endptr=begptr+size;
        if(m==FXIO::Reading){
          wptr=endptr;
          rptr=endptr;
          sptr=endptr;
          }
        else{
          wptr=begptr;
          rptr=begptr;
          sptr=begptr;
          }
        column=0;
        indent=0;
        line=1;
        token=TK_ERROR;
        return true;
        }
      file.close();
      }
    }
  return false;
  }

/*******************************************************************************/

// Skip spaces and comments
FXint FXJSON::space(){
  register FXint comment=0;
  while(1){

    // Tentative token start
    rptr=sptr;

    // Fill buffer if near the end, and not at end of file
    if((sptr+MAXTOKEN>wptr) && (wptr==endptr)){
      if(!fill()){ FXTRACE((1,"%s:%d: fill() failed!\n",__FILE__,__LINE__)); return TK_ERROR; }
      }

    // End of file
    if(sptr>=wptr) return TK_EOF;

    // End of stream
    if(sptr[0]=='\0') return TK_EOF;

    // Space
    if(sptr[0]==' '){
      column++;
      sptr++;
      continue;
      }

    // Tabs
    if(sptr[0]=='\t'){
      column+=(8-column%8);
      sptr++;
      continue;
      }

    // Newline
    if(sptr[0]=='\n'){
      if(comment<0) comment=0;          // End C++-comment, if any
      column=0;
      line++;
      sptr++;
      continue;
      }

    // Other whitespace
    if(sptr[0]=='\v' || sptr[0]=='\f' || sptr[0]=='\r'){
      sptr++;
      continue;
      }

    // C++-Comment start
    if(sptr+1<wptr && sptr[0]=='/' && sptr[1]=='/' && comment==0){
      comment=-1;
      column+=2;
      sptr+=2;
      continue;
      }

    // C-Comment start
    if(sptr+1<wptr && sptr[0]=='/' && sptr[1]=='*' && comment>=0){
      comment++;
      column+=2;
      sptr+=2;
      continue;
      }

    // C-Comment end
    if(sptr+1<wptr && sptr[0]=='*' && sptr[1]=='/' && comment>0){
      comment--;
      column+=2;
      sptr+=2;
      continue;
      }
      
    // Skip BOM, if any
    if(sptr+2<wptr && sptr[0]=='\xef' && sptr[1]=='\xbb' && sptr[2]=='\xbf'){
      sptr+=3;
      continue;
      }

    // Token found?
    if(comment==0) break;

    // Skip characters in comments
    column++;
    sptr++;
    }
  return TK_SPACE;
  }


// Get next token
FXint FXJSON::next(){
  register FXint tok;

  while(1){

    // Eat space and comments
    if((tok=space())!=TK_SPACE) return tok;

    // But handle contents
    switch(sptr[0]){
      case '\0':                // End of file
        return TK_EOF;
      case '\t':                // Tab hops to next tabstop
        column+=(8-column%8);
        sptr++;
        break;
      case '\n':                // Newline increases line number, resets column
        column=-1;
        line++;
      case ' ':                 // Space
        column++;
      case '\v':                // Other white space not incrementing column
      case '\f':
      case '\r':
        sptr++;
        break;
      case '{':                 // Begin of map
        column++;
        sptr++;
        return TK_LBRACE;
      case '}':                 // End of map
        column++;
        sptr++;
        return TK_RBRACE;
      case '[':                 // Begin of array
        column++;
        sptr++;
        return TK_LBRACK;
      case ']':                 // End of array
        column++;
        sptr++;
        return TK_RBRACK;
      case ',':                 // Element separator
        column++;
        sptr++;
        return TK_COMMA;
      case ':':                 // Key:value separator
        column++;
        sptr++;
        return TK_COLON;
      case '"':                 // String value
        column++;
        sptr++;
        while(sptr<wptr && sptr[0]!='"'){
          if((sptr+MAXTOKEN>wptr) && (wptr==endptr)){   // Strings may get long; so fill buffer if needed
            if(!fill()){ FXTRACE((1,"%s:%d: fill() failed!\n",__FILE__,__LINE__)); return TK_ERROR; }
            }
          if(sptr[0]=='\\' && sptr+1<wptr){
            column++;
            sptr++;
            }
          column++;
          sptr++;
          }
        if(sptr>=wptr){ FXTRACE((1,"%s:%d: Unexpected end of file!\n",__FILE__,__LINE__)); return TK_EOF; }
        if(sptr[0]!='"'){ FXTRACE((1,"%s:%d: Expected '\"'!\n",__FILE__,__LINE__)); return TK_ERROR; }
        column++;
        sptr++;
        return TK_STRING;
      case '+':                 // Number value
      case '-':
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        tok=TK_INT;
        if(sptr[0]=='-' || sptr[0]=='+'){
          column++;
          sptr++;
          }
        if(sptr<wptr && sptr[0]=='0'){
          column++;
          sptr++;
          if(sptr<wptr && sptr[0]=='x'){
            sptr++;
            if(sptr>=wptr || !Ascii::isHexDigit(sptr[0])){ FXTRACE((1,"%s:%d: Expected hex digit!\n",__FILE__,__LINE__));  return TK_ERROR; }
            sptr++;
            while(sptr<wptr && Ascii::isHexDigit(sptr[0])) sptr++;
            return tok;
            }
          if(sptr<wptr && '0'<=sptr[0] && sptr[0]<='7'){
            sptr++;
            while(sptr<wptr && '0'<=sptr[0] && sptr[0]<='7') sptr++;
            return tok;
            }
          }
        while(sptr<wptr && Ascii::isDigit(sptr[0])) sptr++;
        if(sptr<wptr && sptr[0]=='.'){
          sptr++;
          while(sptr<wptr && Ascii::isDigit(sptr[0])) sptr++;
          tok=TK_REAL;
          }
        if(sptr<wptr && (sptr[0]=='e' || sptr[0]=='E')){
          sptr++;
          if(sptr<wptr && (sptr[0]=='-' || sptr[0]=='+')) sptr++;
          if(sptr>=wptr || !Ascii::isDigit(sptr[0])){ FXTRACE((1,"%s:%d: Expected digit!\n",__FILE__,__LINE__));  return TK_ERROR; }
          sptr++;
          while(sptr<wptr && Ascii::isDigit(sptr[0])) sptr++;
          tok=TK_REAL;
          }
        return tok;
      case 'n':                 // Null value
        if(sptr+4>wptr || sptr[1]!='u' || sptr[2]!='l' || sptr[3]!='l'){ FXTRACE((1,"%s:%d: Expected \"null\"!\n",__FILE__,__LINE__));  return TK_ERROR; }
        column+=4;
        sptr+=4;
        return TK_NULL;
      case 't':                 // True value
        if(sptr+4>wptr || sptr[1]!='r' || sptr[2]!='u' || sptr[3]!='e'){ FXTRACE((1,"%s:%d: Expected \"true\"!\n",__FILE__,__LINE__));  return TK_ERROR; }
        column+=4;
        sptr+=4;
        return TK_TRUE;
      case 'f':                 // False value
        if(sptr+5>wptr || sptr[1]!='a' || sptr[2]!='l' || sptr[3]!='s' || sptr[4]!='e'){ FXTRACE((1,"%s:%d: Expected \"false\"!\n",__FILE__,__LINE__));  return TK_ERROR; }
        column+=5;
        sptr+=5;
        return TK_FALSE;
      default:
        FXTRACE((1,"%s:%d: Unexpected token!\n",__FILE__,__LINE__));
        return TK_ERROR;
      }
    }
  return TK_EOF;
  }


// Load map elements int o var
FXJSON::Error FXJSON::loadMap(FXVariant& var){
  FXString value,key;
  Error err;

  // Make it into an array now
  var.setType(FXVariant::VMap);

  // While token is string
  while(TK_STRING==token){

    // Keep key value
    value.assign(rptr,sptr-rptr);

    // Next token
    token=next();

    // Missing colon
    if(token!=TK_COLON) return ErrColon;

    // Unescape to get key
    key=unescape(value,'"','"');

    // Next token
    token=next();

    // Load item directly into associated slot
    if((err=loadVariant(var[key]))!=ErrOK) return err;

    // Expect another key-value pair
    if(token!=TK_COMMA) break;

    // Next token
    token=next();
    }
  return ErrOK;
  }


// Load array elements into var
FXJSON::Error FXJSON::loadArray(FXVariant& var){
  FXint index=0;
  Error err;

  // Make it into an array now
  var.setType(FXVariant::VArray);

  // While possible item start token
  while(TK_NULL<=token && token<=TK_LBRACE){

    // Load item directly into array slot
    if((err=loadVariant(var[index]))!=ErrOK) return err;

    // Expect another value
    if(token!=TK_COMMA) break;

    // Next token
    token=next();

    // Next array index
    index++;
    }
  return ErrOK;
  }


// Load variant
FXJSON::Error FXJSON::loadVariant(FXVariant& var){
  FXString value(rptr,sptr-rptr);
  FXbool ok=false;
  Error err;
  switch(token){
  case TK_EOF:                                  // Unexpected end of file
    var=FXVariant::null;
    return ErrEnd;
  case TK_NULL:                                 // Null
    var=FXVariant::null;
    token=next();
    return ErrOK;
  case TK_FALSE:                                // False
    var=false;
    token=next();
    return ErrOK;
  case TK_TRUE:                                 // True
    var=true;
    token=next();
    return ErrOK;
  case TK_INT:
    var=value.toLong(0,&ok);
    if(!ok) return ErrNumber;                   // Numeric conversion error
    token=next();
    return ErrOK;
  case TK_REAL:
    var=value.toDouble(&ok);
    if(!ok) return ErrNumber;                   // Numeric conversion error
    token=next();
    return ErrOK;
  case TK_STRING:                               // Quoted string
    var=unescape(value,'"','"');
    token=next();
    return ErrOK;
  case TK_LBRACK:                               // Array
    token=next();
    if((err=loadArray(var))!=ErrOK) return err;
    if(token!=TK_RBRACK) return ErrBracket;     // Expected closing bracket
    token=next();
    return ErrOK;
  case TK_LBRACE:                               // Map
    token=next();
    if((err=loadMap(var))!=ErrOK) return err;
    if(token!=TK_RBRACE) return ErrBrace;       // Expected closing brace
    token=next();
    return ErrOK;
  default:                                      // Illegal token
    var=FXVariant::null;
    return ErrToken;
    }
  return ErrToken;
  }

/*******************************************************************************/

#ifdef WIN32
#ifndef va_copy
#define va_copy(arg,list) ((arg)=(list))
#endif
#endif

// Furnish our own versions
extern FXAPI FXint __vsnprintf(FXchar* string,FXint length,const FXchar* format,va_list args);


// Print to buffer; flush if no fit
FXint FXJSON::vformat(const FXchar* fmt,va_list args){
  va_list arguments;
  FXint result=0;
  va_copy(arguments,args);
  result=__vsnprintf(wptr,endptr-wptr,fmt,arguments);
  va_end(arguments);
  if(result>endptr-wptr){
    if(!flush()) return -1;                             // Flush buffer and try again
    result=__vsnprintf(wptr,endptr-wptr,fmt,args);
    if(result>=endptr-wptr) return -1;                  // Failed again, give up
    }
  wptr+=result;
  va_end(arguments);
  return result;
  }


// Print to buffer; flush if no fit
FXint FXJSON::format(const FXchar* fmt,...){
  va_list arguments;
  va_start(arguments,fmt);
  FXint result=vformat(fmt,arguments);
  va_end(arguments);
  return result;
  }


// Write indentation
FXint FXJSON::writeindent(FXint d){
  if(format(ENDLINE "%*s",d,"")<0) return ErrSave;
  column=d;
  line++;
  return ErrOK;
  }


// Commas and separators as function of flow type
static const FXchar comma[4][3]={",",", ",", ",", "};
static const FXchar separator[4][4]={":"," : "," : "," : "};


// Save map elements from var
FXJSON::Error FXJSON::saveMap(const FXVariant& var){
  register FXint count=var.asMap().used();

  FXASSERT(var.getType()==FXVariant::VMap);

  // Object start
  if(format("{")<0) return ErrSave;
  column+=1;

  // Skip the whole thing if no items
  if(count){
    register FXint oldindent=indent;
    register FXint n;
    FXString string;

    // Figure indent
    indent=(flow==Pretty)?indent+dent:(flow==Compact)?column:0;

    // Write indent
    if(flow==Pretty && writeindent(indent)!=ErrOK) return ErrSave;

    // Loop over the items
    for(FXint i=0; i<var.asMap().no(); ++i){

      // Skip empty slots
      if(var.asMap().key(i).empty()) continue;

      // Escape magic characters in the string
      string=escape(var.asMap().key(i),'"','"',2);

      // Write string followed by separator
      if((n=format("%s%s",string.text(),separator[flow]))<0) return ErrSave;
      column+=n;

      // Write variant
      if(!saveVariant(var.asMap().data(i))<0) return ErrSave;

      // Another item to follow?
      if(--count>0){

        // Write comma
        if((n=format(comma[flow]))<0) return ErrSave;
        column+=n;

        // Write newline and indent
        if(flow || wrap<column){
          if(writeindent(indent)!=ErrOK) return ErrSave;
          }
        }
      }

    indent=oldindent;

    // Write indent
    if(flow==Pretty && writeindent(indent)!=ErrOK) return ErrSave;
    }

  // Object end
  if(format("}")<0) return ErrSave;
  column+=1;

  return ErrOK;
  }


// Save array elements from var
FXJSON::Error FXJSON::saveArray(const FXVariant& var){

  FXASSERT(var.getType()==FXVariant::VArray);

  // Array start
  if(format("[")<0) return ErrSave;
  column+=1;

  // Skip the whole thing if no items
  if(var.asArray().no()){
    register FXint oldindent=indent;
    register FXint n;

    // Figure indent
    indent=(flow==Pretty)?indent+dent:(flow==Compact)?column:0;

    // Write indent
    if(flow==Pretty && writeindent(indent)!=ErrOK) return ErrSave;

    // Loop over the items
    for(FXint i=0; i<var.asArray().no(); ++i){

      // Write variant
      if(!saveVariant(var.asArray().at(i))<0) return ErrSave;

      // Another item to follow?
      if(i+1<var.asArray().no()){

        // Write comma
        if((n=format(comma[flow]))<0) return ErrSave;
        column+=n;

        // Write newline and indent
        if(flow==Pretty || wrap<column || (flow==Compact && FXVariant::VMap==var.asArray().at(i).getType())){
          if(writeindent(indent)!=ErrOK) return ErrSave;
          }
        }
      }

    indent=oldindent;

    // Write indent
    if(flow==Pretty && writeindent(indent)!=ErrOK) return ErrSave;
    }

  // Array end
  if(format("]")<0) return ErrSave;
  column+=1;
  return ErrOK;
  }


// Recursively save variant var
FXJSON::Error FXJSON::saveVariant(const FXVariant& var){
  const char floatformat[4][6]={"%.*lf","%.*lE","%.*lG","%.*lf"};
  const FXchar truth[2][6]={"false","true"};
  register FXint n;
  FXString string;
  switch(var.getType()){
  case FXVariant::VNull:
    if((n=format("null"))<0) return ErrSave;
    column+=n;
    break;
  case FXVariant::VBool:
    if((n=format("%s",truth[var.asULong()&1]))<0) return ErrSave;
    column+=n;
    break;
  case FXVariant::VChar:
    if((n=format("%d",(FXchar)var.asULong()))<0) return ErrSave;
    column+=n;
    break;
  case FXVariant::VInt:
  case FXVariant::VLong:
    if((n=format("%lld",var.asLong()))<0) return ErrSave;
    column+=n;
    break;
  case FXVariant::VUInt:
  case FXVariant::VULong:
    if((n=format("%llu",var.asULong()))<0) return ErrSave;
    column+=n;
    break;
  case FXVariant::VFloat:
  case FXVariant::VDouble:
    if((n=format(floatformat[fmt&3],prec,var.asDouble()))<0) return ErrSave;
    column+=n;
    break;
  case FXVariant::VPointer:
    if((n=format("%p",var.asPtr()))<0) return ErrSave;
    column+=n;
    break;
  case FXVariant::VString:
    string=escape(var.asString(),'"','"',2);            // Escape utf8 and specials
    if((n=format("%s",string.text()))<0) return ErrSave;
    column+=n;
    break;
  case FXVariant::VArray:
    if(saveArray(var)!=ErrOK) return ErrSave;
    break;
  case FXVariant::VMap:
    if(saveMap(var)!=ErrOK) return ErrSave;
    break;
    }
  return ErrOK;
  }


/*******************************************************************************/

// Load a variant
FXJSON::Error FXJSON::load(FXVariant& variant){
  FXTRACE((2,"FXJSON::load(variant)\n"));
  if(file.isReadable()){
    Error err;

    // Grab next token
    token=next();

    // Get a variant
    if((err=loadVariant(variant))!=ErrOK) return err;

    // Good
    return ErrOK;
    }
  return ErrLoad;
  }


// Save a variant
FXJSON::Error FXJSON::save(const FXVariant& variant){
  FXTRACE((2,"FXJSON::save(variant)\n"));
  if(file.isWritable()){
    Error err;

    // Save a variant
    if((err=saveVariant(variant))!=ErrOK) return err;

    // Good
    return ErrOK;
    }
  return ErrSave;
  }


// Fill buffer from file
FXbool FXJSON::fill(){
  register FXival n;
  if(file.isReadable()){
    if(rptr<wptr){ memmove(begptr,rptr,wptr-rptr); }
    wptr=begptr+(wptr-rptr);
    sptr-=rptr-begptr;
    rptr=begptr;
    n=file.readBlock(wptr,endptr-wptr);
    if(0<=n){
      wptr+=n;
      return rptr<wptr;
      }
    }
  return false;
  }


// Flush buffer to file
FXbool FXJSON::flush(){
  register FXival n;
  if(file.isWritable()){
    n=file.writeBlock(rptr,wptr-rptr);
    if(0<=n){
      rptr+=n;
      if(rptr<wptr){ memmove(begptr,rptr,wptr-rptr); }
      wptr=begptr+(wptr-rptr);
      rptr=begptr;
      return wptr<=rptr;
      }
    }
  return false;
  }


// Close stream and delete buffers
FXbool FXJSON::close(){
  FXTRACE((2,"FXJSON::close()\n"));
  if(file.isOpen()){
    if(file.isWritable()) flush();
    freeElms(begptr);
    endptr=NULL;
    wptr=NULL;
    rptr=NULL;
    sptr=NULL;
    token=TK_ERROR;
    return file.close();
    }
  return false;
  }


// Close stream and clean up
FXJSON::~FXJSON(){
  FXTRACE((1,"FXJSON::~FXJSON\n"));
  close();
  }

}
