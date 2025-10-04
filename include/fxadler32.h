/********************************************************************************
*                                                                               *
*                           A d l e r 3 2   S u p p o r t                       *
*                                                                               *
*********************************************************************************
* Copyright (C) 2024 by Jeroen van der Zijp.   All Rights Reserved.             *
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
#ifndef FXADLER32_H
#define FXADLER32_H

namespace FX {

namespace ADLER32 {


/**
* Calculate ADLER32 checksum of bunch of data:
*
*  1) Start with: sum=1;
*
*  2) Add items:
*
*       sum=ADLER32::SUM(sum,byte),
*       sum=ADLER32::SUM(sum,short),
*       sum=ADLER32::SUM(sum,int),                    (in any order)
*       sum=ADLER32::SUM(sum,long),
*       sum=ADLER32::SUM(sum,buffer,length),
*
*  3) Profit!
*/

/// Calculate ADLER32 checksum of unsigned char
static inline FXuint SUM(FXuint sum,FXuchar byte){
  FXuint s1=(sum&65535)+byte;
  FXuint s2=(sum>>16)+s1;
  return ((s1%65521)<<16)|(s2%65521);
  }


/// Calculate ADLER32 checksum of unsigned short
static inline FXuint SUM(FXuint sum,FXushort x){
  union{ FXushort s; FXuchar b[2]; } z={x};
  FXuint s1=sum&65535;
  FXuint s2=sum>>16;
#if FOX_BIGENDIAN == 1
  s2+=(s1+=z.b[0]);
  s2+=(s1+=z.b[1]);
#else
  s2+=(s1+=z.b[1]);
  s2+=(s1+=z.b[0]);
#endif
  return ((s1%65521)<<16)|(s2%65521);
  }


/// Calculate ADLER32 checksum of unsigned int
static inline FXuint SUM(FXuint sum,FXuint x){
  union{ FXuint i; FXuchar b[4]; } z={x};
  FXuint s1=sum&65535;
  FXuint s2=sum>>16;
#if FOX_BIGENDIAN == 1
  s2+=(s1+=z.b[0]);
  s2+=(s1+=z.b[1]);
  s2+=(s1+=z.b[2]);
  s2+=(s1+=z.b[3]);
#else
  s2+=(s1+=z.b[3]);
  s2+=(s1+=z.b[2]);
  s2+=(s1+=z.b[1]);
  s2+=(s1+=z.b[0]);
#endif
  return ((s1%65521)<<16)|(s2%65521);
  }


/// Calculate ADLER32 checksum of unsigned long
static inline FXuint SUM(FXuint sum,FXulong x){
  union{ FXulong l; FXuchar b[8]; } z={x};
  FXuint s1=sum&65535;
  FXuint s2=sum>>16;
#if FOX_BIGENDIAN == 1
  s2+=(s1+=z.b[0]);
  s2+=(s1+=z.b[1]);
  s2+=(s1+=z.b[2]);
  s2+=(s1+=z.b[3]);
  s2+=(s1+=z.b[4]);
  s2+=(s1+=z.b[5]);
  s2+=(s1+=z.b[6]);
  s2+=(s1+=z.b[7]);
#else
  s2+=(s1+=z.b[7]);
  s2+=(s1+=z.b[6]);
  s2+=(s1+=z.b[5]);
  s2+=(s1+=z.b[4]);
  s2+=(s1+=z.b[3]);
  s2+=(s1+=z.b[2]);
  s2+=(s1+=z.b[1]);
  s2+=(s1+=z.b[0]);
#endif
  return ((s1%65521)<<16)|(s2%65521);
  }


/// Calculate ADLER32 checksum of a array of unsigned chars
static inline FXuint SUM(FXuint sum,const FXuchar *buf,FXival len){
  FXuint s1=sum&65535;
  FXuint s2=sum>>16;
  FXival cnt;
  while(0<len){
    len-=(cnt=FXMIN(len,5552));
    while(8<=cnt){
      s2+=(s1+=*buf++);
      s2+=(s1+=*buf++);
      s2+=(s1+=*buf++);
      s2+=(s1+=*buf++);
      s2+=(s1+=*buf++);
      s2+=(s1+=*buf++);
      s2+=(s1+=*buf++);
      s2+=(s1+=*buf++);
      cnt-=8;
      }
    while(cnt){
      s2+=(s1+=*buf++);
      cnt--;
      }
    s1%=65521;
    s2%=65521;
    }
  return (s2<<16)|s1;
  }

}

}

#endif
