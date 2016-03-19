/********************************************************************************
*                                                                               *
*                             Regular Expression Test                           *
*                                                                               *
*********************************************************************************
* Copyright (C) 1999,2009 by Jeroen van der Zijp.   All Rights Reserved.        *
********************************************************************************/
#include "fx.h"


/*


*/

#define NCAP 10    // Must be less that or equal to 10


/*******************************************************************************/


// Start the whole thing
int main(int argc,char** argv){
  FXRex::Error err;
  FXRex rex;
  FXbool ok;
  FXint i;
  FXint beg[NCAP];
  FXint end[NCAP];
  fxTraceLevel=101;
  if(argc==1){
    fprintf(stderr,"no arguments\n");
    exit(1);
    }
  if(2<=argc){
    err=rex.parse(argv[1],FXRex::Normal|FXRex::Capture);
    fprintf(stderr,"parse(\"%s\") = %s\n",argv[1],FXRex::getError(err));
    }
  if(3<=argc){
    ok=rex.match(argv[2],strlen(argv[2]),beg,end,FXRex::Forward,NCAP);
    if(ok){
      fprintf(stderr,"match at %d:%d\n",beg[0],end[0]);
      for(i=1; i<NCAP; i++){
        fprintf(stderr,"capture at %d:%d\n",beg[i],end[i]);
        }
      for(i=beg[0]; i<end[0]; i++){
        fprintf(stderr,"%c",argv[2][i]);
        }
      fprintf(stderr,"\n");
      }
    else{
      fprintf(stderr,"no match\n");
      }
    }
  return 1;
  }

