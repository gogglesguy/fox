/********************************************************************************
*                                                                               *
*                   S y n t a x   H i g h l i g h t   E n g i n e               *
*                                                                               *
*********************************************************************************
* Copyright (C) 2002,2011 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include <new>
#include "Syntax.h"


/*
  Notes:
  - Restart position: place in text which is default style, a few
    lines of context before the change.
  - Language mode: use wildcard on filename, or forced explicitly.
  - Either simple pattern, or begin/end pattern. Special stop pattern
    to prevent scanning indefinitely.  Patterns may have sub-patterns.
  - Simple pattern must be non-empty; begin/end patterns of a complex
    pattern may be zero-width assertions.
  - Capturing parenthesis are disabled, for speed reasons.
  - Sample text in Syntax is for displaying inside interactive
    style setup dialog; it is supposed to contain on instance of
    each pattern matched by the rule base.
  - FIXME nested style rules should inherit style settings from parent
    rules; even if it comes from registry!
*/

/*******************************************************************************/


FXIMPLEMENT(Rule,FXObject,NULL,0)


// Fill textstyle with style, returns position of last change+1
static inline void fillstyle(FXchar* textstyle,FXchar style,FXint f,FXint t){
  while(f<t) textstyle[f++]=style;
  }


// Stylize text
FXbool Rule::stylize(const FXchar*,FXchar*,FXint,FXint,FXint&,FXint&) const {
  return false;
  }


// Stylize body, i.e. after begin pattern has been seen
FXbool Rule::stylizeBody(const FXchar*,FXchar*,FXint,FXint,FXint&,FXint&) const {
  return false;
  }


FXIMPLEMENT(SimpleRule,Rule,NULL,0)


// Stylize simple expression
FXbool SimpleRule::stylize(const FXchar* text,FXchar *textstyle,FXint fm,FXint to,FXint& start,FXint& stop) const {
  if(pat.match(text,to,&start,&stop,FXRex::NotEmpty|FXRex::Forward,1,fm,fm)){
    fillstyle(textstyle,style,start,stop);
    return true;
    }
  return false;
  }


FXIMPLEMENT(BracketRule,Rule,NULL,0)


// Stylize complex recursive expression
FXbool BracketRule::stylizeBody(const FXchar* text,FXchar *textstyle,FXint fm,FXint to,FXint& start,FXint& stop) const {
  FXint head,tail,node;
  start=fm;
  while(fm<to){
    for(node=0; node<rules.no(); node++){
      if(rules[node]->stylize(text,textstyle,fm,to,head,tail)){
        fm=tail;
        goto nxt;
        }
      }
    if(end.match(text,to,&head,&stop,FXRex::Forward,1,fm,fm)){
      fillstyle(textstyle,style,head,stop);
      return true;
      }
    textstyle[fm++]=style;
nxt:continue;
    }
  stop=fm;
  return true;
  }


// Stylize complex recursive expression
FXbool BracketRule::stylize(const FXchar* text,FXchar *textstyle,FXint fm,FXint to,FXint& start,FXint& stop) const {
  FXint head,tail;
  if(beg.match(text,to,&start,&tail,FXRex::Forward,1,fm,fm)){
    fillstyle(textstyle,style,start,tail);
    BracketRule::stylizeBody(text,textstyle,tail,to,head,stop);
    return true;
    }
  return false;
  }


FXIMPLEMENT(SafeBracketRule,BracketRule,NULL,0)


// Stylize complex recursive expression with termination pattern
FXbool SafeBracketRule::stylizeBody(const FXchar* text,FXchar *textstyle,FXint fm,FXint to,FXint& start,FXint& stop) const {
  FXint head,tail,node;
  start=fm;
  while(fm<to){
    if(esc.match(text,to,&head,&stop,FXRex::Forward,1,fm,fm)){          // Each time around, check stop pattern (changed from old method!)
      fillstyle(textstyle,style,head,stop);
      return true;
      }
    for(node=0; node<rules.no(); node++){
      if(rules[node]->stylize(text,textstyle,fm,to,head,tail)){
        fm=tail;
        goto nxt;
        }
      }
    if(end.match(text,to,&head,&stop,FXRex::Forward,1,fm,fm)){
      fillstyle(textstyle,style,head,stop);
      return true;
      }
    textstyle[fm++]=style;
nxt:continue;
    }
  stop=fm;
  return true;
  }


// Stylize complex recursive expression with termination pattern
FXbool SafeBracketRule::stylize(const FXchar* text,FXchar *textstyle,FXint fm,FXint to,FXint& start,FXint& stop) const {
  FXint head,tail;
  if(beg.match(text,to,&start,&tail,FXRex::Forward,1,fm,fm)){
    fillstyle(textstyle,style,start,tail);
    SafeBracketRule::stylizeBody(text,textstyle,tail,to,head,stop);
    return true;
    }
  return false;
  }


FXIMPLEMENT(DefaultRule,Rule,NULL,0)


// Stylize body
FXbool DefaultRule::stylizeBody(const FXchar* text,FXchar *textstyle,FXint fm,FXint to,FXint& start,FXint& stop) const {
  FXint head,tail,node;
  start=fm;
  while(fm<to){
    for(node=0; node<rules.no(); node++){
      if(rules[node]->stylize(text,textstyle,fm,to,head,tail)){
        fm=tail;
        goto nxt;
        }
      }
    textstyle[fm++]=style;
nxt:continue;
    }
  stop=to;
  return true;
  }


// Stylize text
FXbool DefaultRule::stylize(const FXchar* text,FXchar *textstyle,FXint fm,FXint to,FXint& start,FXint& stop) const {
  return DefaultRule::stylizeBody(text,textstyle,fm,to,start,stop);
  }


FXIMPLEMENT(Syntax,FXObject,NULL,0)


// Construct syntax object; needs at least one master rule
Syntax::Syntax(const FXString& lang):language(lang){
  rules.append(new DefaultRule("Default",-1,0));
  delimiters=FXText::textDelimiters;
  contextLines=1;
  contextChars=1;
  }


// Match filename against wildcards
FXbool Syntax::matchFilename(const FXString& name) const {
  return FXPath::match(name,extensions);
  }


// Match contents against regular expression
FXbool Syntax::matchContents(const FXString& text) const {
  return FXRex(contents).match(text);
  }


// Append default rule
FXint Syntax::appendDefault(const FXString& name,FXint parent){
  register FXint index=rules.no();
  FXASSERT(0<=parent && parent<rules.no());
  DefaultRule *rule=new DefaultRule(name,parent,index);
  rules.append(rule);
  rules[parent]->rules.append(rule);
  return index;
  }


// Append simple rule
FXint Syntax::appendSimple(const FXString& name,const FXString& rex,FXint parent){
  register FXint index=rules.no();
  FXASSERT(0<=parent && parent<rules.no());
  SimpleRule *rule=new SimpleRule(name,rex,parent,index);
  rules.append(rule);
  rules[parent]->rules.append(rule);
  return index;
  }


// Append bracket rule
FXint Syntax::appendBracket(const FXString& name,const FXString& brex,const FXString& erex,FXint parent){
  register FXint index=rules.no();
  FXASSERT(0<=parent && parent<rules.no());
  BracketRule *rule=new BracketRule(name,brex,erex,parent,index);
  rules.append(rule);
  rules[parent]->rules.append(rule);
  return index;
  }


// Append safe bracket rule
FXint Syntax::appendSafeBracket(const FXString& name,const FXString& brex,const FXString& erex,const FXString& srex,FXint parent){
  register FXint index=rules.no();
  FXASSERT(0<=parent && parent<rules.no());
  SafeBracketRule *rule=new SafeBracketRule(name,brex,erex,srex,parent,index);
  rules.append(rule);
  rules[parent]->rules.append(rule);
  return index;
  }


// Return true if toplevel rule
FXbool Syntax::isRoot(FXint rule) const {
  FXASSERT(0<=rule && rule<rules.no());
  return rule==0 || rules[rule]->parent==0;
  }


// Return true if p is ancestor of c
FXbool Syntax::isAncestor(FXint p,FXint c) const {
  FXASSERT(0<=p && 0<=c);
  while(c>0){
    c=rules[c]->getParent();
    if(c==p) return true;
    }
  return false;
  }


// Clean up
Syntax::~Syntax(){
  for(int i=0; i<rules.no(); i++) delete rules[i];
  }



