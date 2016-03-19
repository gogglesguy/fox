/********************************************************************************
*                                                                               *
*                             String Format I/O Test                            *
*                                                                               *
*********************************************************************************
* Copyright (C) 2007,2008 by Jeroen van der Zijp.   All Rights Reserved.        *
*********************************************************************************
* $Id: format.cpp,v 1.34 2008/01/04 15:18:33 fox Exp $                          *
********************************************************************************/
#include "fx.h"

/*
  Notes:

*/

/*******************************************************************************/

namespace FX {

extern FXint __sscanf(const FXchar* string,const FXchar* format,...);
extern FXint __snprintf(FXchar* string,FXint length,const FXchar* format,...);

}

const FXchar *floatformat[]={
  "%.10e",
  "%10.5f",
  "%-10.5f",
  "%+10.5f",
  "% 10.5f",
  "%123.9f",
  "%+22.9f",
  "%+4.9f",
  "%01.3f",
  "%4f",
  "%3.1f",
  "%3.2f",
  "%.0f",
  "%.1f",
  "%+.3g",
  "%#.3g",
  "%.g",
  "%#.g",
  "%g",
  "%#g"
  };


const double floatnumbers[]={
  0.000000001,
  -1.5,
  0.8,
  1.0,
  10.0,
  100.0,
  1000.0,
  10000.0,
  999.0,
  1010.0,
  134.21,
  91340.2,
  341.1234,
  0203.9,
  0.96,
  0.996,
  0.9996,
  1.996,
  4.136,
  6442452944.1234,
  1.23456789E+20,
  0.00000123456,
  2.2250738585072014e-308,
  1.7976931348623157e+308,
  0.0
  };

const FXchar *intformat[]={
  "%d",
  "%02x",
  "%0.2x",
  "%-8d",
  "%8d",
  "%08d",
  "%.6d",
  "%u",
  "%+i",
  "% i",
  "%x",
  "%#x",
  "%#08x",
  "%o",
  "%#o",
  "%.32b"
  };

const FXint intnumbers[]={
  0,
  1,
  0x90,
  -34,
  2147483647,
  4294967295u
  };


// Start
int main(int,char*[]){
  FXchar   buffer[1024];
  FXchar   str[100];
  FXdouble dbl;
  FXfloat  flt;
  FXint    num;
  FXshort  sht;
  FXchar   chr;
  FXint    res;
  FXlong   lng;
  void*    ptr;
  FXuint    x,y;

/*
  while(fgets(buffer,sizeof(buffer),stdin)){
    buffer[strlen(buffer)-1]='\0';

    // Test
    res=__sscanf(buffer,"%2lf\n",&dbl);
    fprintf(stdout,"_sscanf(%s)=%d dbl=%.16G\n",buffer,res,dbl);
    res=sscanf(buffer,"%2lf\n",&dbl);
    fprintf(stdout," sscanf(%s)=%d dbl=%.16G\n",buffer,res,dbl);
    }
*/

//  res=snprintf(buffer,sizeof(buffer),"%g",12345E12);
//  fprintf(stdout,"%d \"%s\"\n",res,buffer);
//  res=__snprintf(buffer,sizeof(buffer),"%g",12345E12);
//  fprintf(stdout,"%d \"%s\"\n",res,buffer);


  // Testing int formats
  for(x=0; x<ARRAYNUMBER(intformat); x++){
    for(y=0; y<ARRAYNUMBER(intnumbers); y++){
      __snprintf(buffer,sizeof(buffer),intformat[x],intnumbers[y]);
      fprintf(stdout,"format=\"%s\" output=\"%s\"\n",intformat[x],buffer);
      }
    }

  // Testing double formats
  for(x=0; x<ARRAYNUMBER(floatformat); x++){
    for(y=0; y<ARRAYNUMBER(floatnumbers); y++){
      __snprintf(buffer,sizeof(buffer),floatformat[x],floatnumbers[y]);
      fprintf(stdout,"format=\"%s\" output=\"%s\"\n",floatformat[x],buffer);
      }
    }

  return 1;
  }

