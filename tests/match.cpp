/********************************************************************************
*                                                                               *
*                             File Pattern Match Test                           *
*                                                                               *
*********************************************************************************
* Copyright (C) 2007,2012 by Jeroen van der Zijp.   All Rights Reserved.        *
********************************************************************************/
#include "fx.h"

/*
  Notes:
  - Test battery for FXPath::match().
*/


/*******************************************************************************/

static FXbool matchtest(const FXchar* pattern,const FXchar* string,FXbool expected){
  const FXchar *const passorfail[]={"false","true "};
  const FXchar *const okorbad[]={"BAD","   "};
  FXbool result=FXPath::match(string,pattern,FXPath::CaseFold);
  FXbool ok=result==expected;
  fxwarning("%s match: %-10s pattern: %-10s  result: %s expected: %s\n",okorbad[ok],string,pattern,passorfail[result],passorfail[expected]);
  return ok;
  }


// Start
int main(int,char*[]){

  matchtest("*x", "x", true);
  matchtest("*x", "xx", true);
  matchtest("*x", "yyyx", true);
  matchtest("*x", "yyxy", false);
  matchtest("?x", "x", false);
  matchtest("?x", "xx", true);
  matchtest("?x", "yyyx", false);
  matchtest("?x", "yyxy", false);
  matchtest("*?x", "xx", true);
  matchtest("?*x", "xx", true);
  matchtest("*?x", "x", false);
  matchtest("?*x", "x", false);
  matchtest("*?*x", "yx", true);
  matchtest("*?*x", "xxxx", true);
  matchtest("x*??", "xyzw", true);
  matchtest("*x", "\xc3\x84x", true);
  matchtest("?x", "\xc3\x84x", true);
  matchtest("??x", "\xc3\x84x", false);
  matchtest("ab\xc3\xa4\xc3\xb6", "ab\xc3\xa4\xc3\xb6", true);
  matchtest("ab\xc3\xa4\xc3\xb6", "abao", false);
  matchtest("ab?\xc3\xb6", "ab\xc3\xa4\xc3\xb6", true);
  matchtest("ab?\xc3\xb6", "abao", false);
  matchtest("ab\xc3\xa4?", "ab\xc3\xa4\xc3\xb6", true);
  matchtest("ab\xc3\xa4?", "abao", false);
  matchtest("ab??", "ab\xc3\xa4\xc3\xb6", true);
  matchtest("ab*", "ab\xc3\xa4\xc3\xb6", true);
  matchtest("ab*\xc3\xb6", "ab\xc3\xa4\xc3\xb6", true);
  matchtest("ab*\xc3\xb6", "aba\xc3\xb6x\xc3\xb6", true);
  matchtest("*.o", "FXApp.o", true);
  matchtest("A*.o", "AA.o", true);
  matchtest("A*.o", "aaaa.o", true);
  matchtest("A*.o", "B.o", false);
  matchtest("[A-Z].o", "B.o", true);
  matchtest("[0-9].o", "B.o", false);
  matchtest("[!0-9].o", "B.o", true);
  matchtest("[]].o", "].o", true);
  return 1;
  }

