/********************************************************************************
*                                                                               *
*                        U n i t - C o n v e r s i o n s                        *
*                                                                               *
*********************************************************************************
* Copyright (C) 2026 by Jeroen van der Zijp.   All Rights Reserved.             *
********************************************************************************/
#include "fx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
  Notes:

  - Units test program.  The new FXUnits feature in FOX provides general
    physics conversion.

  - You can convert units between any arbitrary expression of S.I. and
    other common units, and any other expression.

  - FXUnits tries to match dimensions if both source-units and destination-
    units are specified.

  - When converting from (or to) S.I. units, you should be more careful as
    only the destination (or source) unit's dimensions are determined.

  - FXUnits also knows about common prefixes such as G[iga], M[ega], etc.

  - Data have been obtained from recent N.I.S.T. publications and S.I.
    standards organization.  However, if you spot problems please communicate
    these to jeroen@fox-toolkit.com.

*/

/*******************************************************************************/

// Print options
void printusage(const FXchar* prog){
  fxmessage("%s options:\n",prog);
  fxmessage("  --value <value>          Start value.\n");
  fxmessage("  --from <units>           Increment value .\n");
  fxmessage("  --to <units>             Add increment this many times.\n");
  fxmessage("  --span <units>           Dump every so many times.\n");
  fxmessage("  --trace-topics <topics>  Set trace topics.\n");
  fxmessage("  -h, --help               Print help.\n");
  }


// Build table
int main(int argc,char *argv[]){
  const FXchar* fm_unit=nullptr;
  const FXchar* to_unit=nullptr;
  const FXchar* sp_unit=nullptr;
  FXdouble originalvalue=1.0;
  FXdouble convertedvalue=1.0;
  FXint    length=0;

  // Grab a few arguments
  for(int arg=1; arg<argc; ++arg){
    if(FXString::compare(argv[arg],"-h")==0 || FXString::compare(argv[arg],"--help")==0){
      printusage(argv[0]);
      return 0;
      }
    else if(FXString::compare(argv[arg],"--trace-topics")==0){
      if(++arg>=argc){ fxwarning("Missing argument for --trace-topics option.\n"); return 1; }
      setTraceTopics(argv[arg]);
      }
    else if(FXString::compare(argv[arg],"--value")==0){
      if(++arg>=argc){ fxwarning("Missing argument for --value option.\n"); return 1; }
      originalvalue=strtod(argv[arg],nullptr);
      }
    else if(FXString::compare(argv[arg],"--from")==0){
      if(++arg>=argc){ fxwarning("Missing argument for --from option.\n"); return 1; }
      fm_unit=argv[arg];
      }
    else if(FXString::compare(argv[arg],"--to")==0){
      if(++arg>=argc){ fxwarning("Missing argument for --to option.\n"); return 1; }
      to_unit=argv[arg];
      }
    else if(FXString::compare(argv[arg],"--span")==0){
      if(++arg>=argc){ fxwarning("Missing argument for --span option.\n"); return 1; }
      sp_unit=argv[arg];
      }
    else{
      fxwarning("Bad argument.\n");
      printusage(argv[0]);
      return 1;
      }
    }

  // Conversion
  if(fm_unit || to_unit){
    convertedvalue=originalvalue;
    if(fm_unit && to_unit){
      if(Units::convert(convertedvalue,fm_unit,to_unit)){
        fxmessage("value %.14lg %s -> %.14lg %s\n",originalvalue,fm_unit,convertedvalue,to_unit);
        }
      else{
        fxmessage("failed to convert: %s -> %s\n",fm_unit,to_unit);
        }
      }
    else if(fm_unit){
      if(Units::convertToSIFrom(convertedvalue,fm_unit)){
        fxmessage("value %.14lg %s -> %.14lg S.I.\n",originalvalue,fm_unit,convertedvalue);
        }
      else{
        fxmessage("failed to convert from: %s\n",fm_unit);
        }
      }
    else if(to_unit){
      if(Units::convertFromSITo(convertedvalue,to_unit)){
        fxmessage("value %.14lg S.I. -> %.14lg %s\n",originalvalue,convertedvalue,to_unit);
        }
      else{
        fxmessage("failed to convert to: %s\n",to_unit);
        }
      }
    }

  // Count bytes parsed
  if(sp_unit){
    length=Units::span(sp_unit);
    fxmessage("length(%s) = %d\n",sp_unit,length);
    }
  return 0;
  }
