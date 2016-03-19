/********************************************************************************
*                                                                               *
*                             String Format I/O Test                            *
*                                                                               *
*********************************************************************************
* Copyright (C) 2007,2013 by Jeroen van der Zijp.   All Rights Reserved.        *
********************************************************************************/
#include "fx.h"

/*
  Notes:
  - Test battery for fxscanf().
*/


/*******************************************************************************/

namespace FX {

extern FXint __sscanf(const FXchar* string,const FXchar* format,...);

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
  "%3$lf %1$lf %2$lf"
  };


const FXchar *floatnumbers[]={
  "0.0 1.0 3.1415926535897932384626433833",
  "-0.1 +0.11111 -1.23456789E-99",
  "1.7976931348623157e+308 2.2250738585072014e-308 1.17549435e-38",
  "1.8e+308 4.94065645841246544177e-324 0.0E400"
  };


// Uncomment to revert to native version
//#define __sscanf sscanf


// Start
int main(int,char*[]){
  FXuint   x,y;
  FXint    res;
  FXint    ia,ib,ic;
  FXdouble da,db,dc;

  // Reading integers
  for(x=0; x<ARRAYNUMBER(intformat); x++){
    for(y=0; y<ARRAYNUMBER(intnumbers); y++){
      ia=ib=ic=0;
      res=__sscanf(intnumbers[y],intformat[x],&ia,&ib,&ic);
      fprintf(stdout,"format=\"%s\" input=\"%s\" res=%d a=%d b=%d c=%d\n",intformat[x],intnumbers[y],res,ia,ib,ic);
      }
    }

  // Reading floats
  for(x=0; x<ARRAYNUMBER(floatformat); x++){
    for(y=0; y<ARRAYNUMBER(floatnumbers); y++){
      da=db=dc=0;
      res=__sscanf(floatnumbers[y],floatformat[x],&da,&db,&dc);
      fprintf(stdout,"format=\"%s\" input=\"%s\" res=%d a=%.16g b=%.16g c=%.16g\n",floatformat[x],floatnumbers[y],res,da,db,dc);
      }
    }

/*
  FXString st;
  char buf[1000];
  FXdouble capacite1,capacite2,capacite3;

  while(gets(buf)){
    st=buf;
    capacite2= st.toDouble();
    fprintf(stderr,"toDouble: %.30lg\n", capacite2);
    capacite1=strtod(buf,NULL);
    fprintf(stderr,"strtod:   %.30lg\n", capacite1);
    st.scan("%lf",&capacite3);
    fprintf(stderr,"fxscanf:  %.30lg\n", capacite3);
    fprintf(stderr,"DIFF %.30lg (rel %.10lg)\n",capacite1-capacite2,fabs(capacite1-capacite2)/fabs(capacite1));
    fprintf(stderr,"DIFF %.30lg (rel %.10lg)\n",capacite1-capacite3,fabs(capacite1-capacite3)/fabs(capacite1));
    }
*/

  return 1;
  }

