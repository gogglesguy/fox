/********************************************************************************
*                                                                               *
*                   S y n t a x   H i g h l i g h t   E n g i n e               *
*                                                                               *
*********************************************************************************
* Copyright (C) 2002,2014 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#ifndef SYNTAX_H
#define SYNTAX_H

class Rule;
class Syntax;


// List of syntax rules
typedef FXObjectListOf<Rule> RuleList;


// List of syntaxes
typedef FXObjectListOf<Syntax> SyntaxList;


// Highlight node
class Rule : public FXObject {
  FXDECLARE(Rule)
  friend class Syntax;
protected:
  FXString      name;   // Name of rule
  RuleList      rules;  // Subrules
  FXint         parent; // Parent style index
  FXint         style;  // Own style index
protected:
  Rule(){}
private:
  Rule(const Rule&);
  Rule &operator=(const Rule&);
public:

  // Construct node
  Rule(const FXString& nm,FXint p,FXint s):name(nm),parent(p),style(s){}

  // Get number of child rules
  FXint getNumRules() const { return rules.no(); }

  // Get child rule
  Rule* getRule(FXint index) const { return rules[index]; }

  // Rule name
  const FXString& getName() const { return name; }
  void setName(const FXString& nm){ name=nm; }

  // Get parent
  FXint getParent() const { return parent; }

  // Get style
  FXint getStyle() const { return style; }

  // Stylize text
  virtual FXbool stylize(const FXchar* text,FXchar *textstyle,FXint fm,FXint to,FXint& start,FXint& stop) const;

  // Stylize body, i.e. after begin pattern has been seen
  virtual FXbool stylizeBody(const FXchar* text,FXchar *textstyle,FXint fm,FXint to,FXint& start,FXint& stop) const;
  };


// Simple highlight node
class SimpleRule : public Rule {
  FXDECLARE(SimpleRule)
protected:
  FXRex pat;            // Pattern to match
protected:
  SimpleRule(){ }
private:
  SimpleRule(const SimpleRule&);
  SimpleRule &operator=(const SimpleRule&);
public:

  // Construct node
  SimpleRule(const FXString& nm,const FXString& rex,FXint p,FXint s):Rule(nm,p,s),pat(rex,FXRex::Newline|FXRex::NotEmpty){ }

  // Stylize text
  virtual FXbool stylize(const FXchar* text,FXchar *textstyle,FXint fm,FXint to,FXint& start,FXint& stop) const;

  // Stylize body, i.e. after begin pattern has been seen
  virtual FXbool stylizeBody(const FXchar* text,FXchar *textstyle,FXint fm,FXint to,FXint& start,FXint& stop) const;
  };


// Bracketed highlight node
class BracketRule : public Rule {
  FXDECLARE(BracketRule)
protected:
  FXRex beg;            // Beginning pattern
  FXRex end;            // Ending pattern
protected:
  BracketRule(){ }
private:
  BracketRule(const BracketRule&);
  BracketRule &operator=(const BracketRule&);
public:

  // Construct node
  BracketRule(const FXString& nm,const FXString& brex,const FXString& erex,FXint p,FXint s):Rule(nm,p,s),beg(brex,FXRex::Newline),end(erex,FXRex::Newline){ }

  // Stylize text
  virtual FXbool stylize(const FXchar* text,FXchar *textstyle,FXint fm,FXint to,FXint& start,FXint& stop) const;

  // Stylize body, i.e. after begin pattern has been seen
  virtual FXbool stylizeBody(const FXchar* text,FXchar *textstyle,FXint fm,FXint to,FXint& start,FXint& stop) const;
  };


// Bracketed highlight node with termination
class SafeBracketRule : public BracketRule {
  FXDECLARE(SafeBracketRule)
protected:
  FXRex esc;           // Termination pattern
protected:
  SafeBracketRule(){ }
private:
  SafeBracketRule(const SafeBracketRule&);
  SafeBracketRule &operator=(const SafeBracketRule&);
public:

  // Construct node
  SafeBracketRule(const FXString& nm,const FXString& brex,const FXString& erex,const FXString& srex,FXint p,FXint s):BracketRule(nm,brex,erex,p,s),esc(srex,FXRex::Newline){ }

  // Stylize text
  virtual FXbool stylize(const FXchar* text,FXchar *textstyle,FXint fm,FXint to,FXint& start,FXint& stop) const;

  // Stylize body, i.e. after begin pattern has been seen
  virtual FXbool stylizeBody(const FXchar* text,FXchar *textstyle,FXint fm,FXint to,FXint& start,FXint& stop) const;
  };


// Default highlight node
class DefaultRule : public Rule {
  FXDECLARE(DefaultRule)
protected:
  DefaultRule(){ }
private:
  DefaultRule(const DefaultRule&);
  DefaultRule &operator=(const DefaultRule&);
public:

  // Construct node
  DefaultRule(const FXString& nm,FXint p,FXint s):Rule(nm,p,s){ }

  // Stylize text
  virtual FXbool stylize(const FXchar* text,FXchar *textstyle,FXint fm,FXint to,FXint& start,FXint& stop) const;

  // Stylize body, i.e. after begin pattern has been seen
  virtual FXbool stylizeBody(const FXchar* text,FXchar *textstyle,FXint fm,FXint to,FXint& start,FXint& stop) const;
  };


// Syntax for a language
class Syntax : public FXObject {
  FXDECLARE(Syntax)
protected:
  RuleList      rules;          // Highlight rules
  FXString      language;       // Language name
  FXString      extensions;     // File extensions to recognize language
  FXString      contents;       // Contents to recognize language
  FXString      delimiters;     // Word delimiters in this language
  FXint         contextLines;   // Context lines needed for restyle
  FXint         contextChars;   // Context characters needed for restyle
protected:
  Syntax(){}
private:
  Syntax(const Syntax&);
  Syntax &operator=(const Syntax&);
public:

  // New language
  Syntax(const FXString& lang);

  // Get number of child rules
  FXint getNumRules() const { return rules.no(); }

  // Get rule
  Rule* getRule(FXint rule) const { return rules[rule]; }

  // Return true if toplevel rule
  FXbool isRoot(FXint rule) const;

  // Return true if p is ancestor of c
  FXbool isAncestor(FXint p,FXint c) const;

  // Return common ancestor of a and b
  FXint commonAncestor(FXint a,FXint b) const;

  // Language name
  const FXString& getName() const { return language; }
  void setName(const FXString& lang){ language=lang; }

  // Extensions
  const FXString& getExtensions() const { return extensions; }
  void setExtensions(const FXString& exts){ extensions=exts; }

  // Contents
  const FXString& getContents() const { return contents; }
  void setContents(const FXString& cont){ contents=cont; }

  // Delimiters
  const FXString& getDelimiters() const { return delimiters; }
  void setDelimiters(const FXString& delims){ delimiters=delims; }

  // Context lines
  FXint getContextLines() const { return contextLines; }
  void setContextLines(FXint num){ contextLines=num; }

  // Context characters
  FXint getContextChars() const { return contextChars; }
  void setContextChars(FXint num){ contextChars=num; }

  // Match filename against wildcards
  FXbool matchFilename(const FXString& name) const;

  // Match contents against regular expression
  FXbool matchContents(const FXString& text) const;

  // Append default rule
  FXint appendDefault(const FXString& name,FXint parent=0);

  // Append simple rule
  FXint appendSimple(const FXString& name,const FXString& rex,FXint parent=0);

  // Append bracket rule
  FXint appendBracket(const FXString& name,const FXString& brex,const FXString& erex,FXint parent=0);

  // Append safe bracket rule
  FXint appendSafeBracket(const FXString& name,const FXString& brex,const FXString& erex,const FXString& srex,FXint parent=0);

  // Wipes the rules
  virtual ~Syntax();
  };


#endif
