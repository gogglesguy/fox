/********************************************************************************
*                                                                               *
*                             String Format I/O Test                            *
*                                                                               *
*********************************************************************************
* Copyright (C) 2007,2019 by Jeroen van der Zijp.   All Rights Reserved.        *
********************************************************************************/
#include "fx.h"

/*
  Notes:
  - Test battery for fxscanf().
*/


/*******************************************************************************/

namespace FX {

extern FXint __sscanf(const FXchar* string,const FXchar* format,...);
extern FXint __snprintf(FXchar* string,FXint length,const FXchar* format,...);

}

const FXchar *intformat[]={
  "%d %d %d",
  "%i %i %i",
  "%u %u %u",
  "%x %x %x",
  "%o %o %o",
  "%b %b %b",
  "%1d %1d %1d",
  "%0d %0d %0d",
  "%3d %3d %3d",
  "%3$i %1$i %2$i"
  };


const FXchar *intnumbers[]={
  "111 222 333",
  "-111 +222 -333",
  "0xff 0377 123456789",
  "11111111 10101010 0b1111111111111111111111111111111",
  "4294967295 2147483647 -2147483648"
  };


const FXchar* floatformat[]={
  "%lf %lf %lf",
  "%3$lf %1$lf %2$lf",
  "%'lf %'lf %'lf",
  };


const FXchar *floatnumbers[]={
  "0.0 1.0 3.1415926535897932384626433833",
  "-0.1 +0.11111 -1.23456789E-99",
  "1.7976931348623157e+308 2.2250738585072014e-308 1.17549435e-38",
  "1.8e+308 4.94065645841246544177e-324 0.0E400",
  "1,000.0 26,345,123 1000.0",
  "0000000000000000000000000000 0.1 1.0E-1",
  "-1. -.1 1.",
  "NaN Inf 3.14",
  "+0.5 -.5 +0x1.ac54p+66",
  };


const FXchar *stringformat[]={
  "%s",
  "%4s",
  "%[0-9.Ee+-]",
  "%[^a-c]",
  "%[]]",
  "%[0-9-]",
  "%[a-a]",
  "%[a-zA-Z0-9_]"
  };

const FXchar *stringinputs[]={
  "1.0E-99",
  "123abc",
  "]]]][[[[",
  "123-1456",
  "aaaaabbbb",
  "Camel_Case_1337"
  };


// Uncomment to revert to native version
//#define __sscanf sscanf


// Start
int main(int,char*[]){
  FXuint   x,y;
  FXint    res;
  FXint    ia,ib,ic;
  FXdouble da,db,dc;
  FXchar   buf[1000];


/*
//  res=__sscanf("+0x1.ac54p+66","%la",&dc); //1.23456789E+20,
//  res=__sscanf("0x1.ac53a7df93d691111p+66","%la",&dc);
//  res=__sscanf("0x003.fffffffffffffp+00","%la",&dc);
  res=__sscanf("0x0.00ffffffp+00","%la",&dc);
  __snprintf(buf,sizeof(buf),"%a 0x%16llx",dc,*((FXulong*)&dc));
  fprintf(stdout,"c=%s\n",buf);
  fprintf(stdout,"c=%.20lg\n",dc);
  fprintf(stdout,"\n");
*/
/*

  fprintf(stdout,"i=0.0000012345678987654321234\n");
  res=__sscanf("0.0000012345678987654321234","%lf",&dc);
  fprintf(stdout,"o=%.15le\n",dc);
  res=__sscanf("0.00012345678987654321234","%lf",&dc);
  fprintf(stdout,"o=%.15le\n",dc);
  res=__sscanf("0.12345678987654321234","%lf",&dc);
  fprintf(stdout,"o=%.15le\n",dc);
  res=__sscanf("1.2345678987654321234","%lf",&dc);
  fprintf(stdout,"o=%.15le\n",dc);
  res=__sscanf("12345678987654321234.","%lf",&dc);
  fprintf(stdout,"o=%.15le\n",dc);
  fprintf(stdout,"\n");
  return 1;
*/
  
  

  // Reading integers
  for(x=0; x<ARRAYNUMBER(intformat); x++){
    for(y=0; y<ARRAYNUMBER(intnumbers); y++){
      ia=ib=ic=0;
      res=__sscanf(intnumbers[y],intformat[x],&ia,&ib,&ic);
      fprintf(stdout,"format=\"%s\" input=\"%s\" res=%d a=%d b=%d c=%d\n",intformat[x],intnumbers[y],res,ia,ib,ic);
      }
    }
  fprintf(stdout,"\n");

  // Reading floats
  for(x=0; x<ARRAYNUMBER(floatformat); x++){
    for(y=0; y<ARRAYNUMBER(floatnumbers); y++){
      da=db=dc=0;
      res=__sscanf(floatnumbers[y],floatformat[x],&da,&db,&dc);
      fprintf(stdout,"format=\"%s\" input=\"%s\" res=%d a=%.20lg b=%.20lg c=%.20lg\n",floatformat[x],floatnumbers[y],res,da,db,dc);
      }
    }
  fprintf(stdout,"\n");

  // Reading strings
  for(x=0; x<ARRAYNUMBER(stringformat); x++){
    for(y=0; y<ARRAYNUMBER(stringinputs); y++){
      memset(buf,0,sizeof(buf));
      res=__sscanf(stringinputs[y],stringformat[x],buf);
      fprintf(stdout,"format=\"%s\" input=\"%s\" res=%d str=%s\n",stringformat[x],stringinputs[y],res,buf);
      }
    }
  fprintf(stdout,"\n");

  return 1;
  }

