/********************************************************************************
*                                                                               *
*                          B M P   I n p u t / O u t p u t                      *
*                                                                               *
*********************************************************************************
* Copyright (C) 1998,2026 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "fxmath.h"
#include "fxendian.h"
#include "FXElement.h"
#include "FXHash.h"
#include "FXStream.h"

/*
  Notes:

  - Writer should use fxezquantize() and if the number of colors is less than
    256, use 8bpp RLE compressed output; if less that 4, use 4bpp RLE compressed
    output, else if less than 2, use monochrome.
  - Writer should do this only when no loss of fidelity occurs.
  - Now supporting BIH_BITFIELDS and BIH_ALFABITFIELDS.  We grab color masks
    either from place after bitmap info header, unless we have a bitman info
    header that already has color masks.
  - Use ctz32() and pop32() from fxendian.h to find size of color masks and
    how much to shift them.
  - If color masks are <8 bits, we need to scale them.  This can be a bit
    tricky: desired is scale by 255/(2^bits-1).  We want to avoid floating
    point so scaling is done by next closest thing, see maskmult[] table below.
  - If bitmap file header offset, file size and bitmap info header image size
    is known, then we can determine if there's a gap between bitmap info header,
    optional colormap/bitmasks, and the data area.  Skip over this space if
    we can determine its there.
  - Dealing with bitfields, BEXTR instruction may be of use:

      // Preparation [same for green, blue, alpha...]
      rbits=pop32(rmask);
      rshift=ctz32(rmask);
      rext=(rbits<<8)|rshift;

      // Extract and scale
      red=(_bextr_u32(wwww,rext)*rmult)>>24;

      // This replaces:
      red=(((wwww&rmask)>>rshift)*rmult)>>24;

    It may not be worth the trouble if AND + SHLX take same amount of time.
*/

#define TOPIC_DETAIL 1018

using namespace FX;

/*******************************************************************************/

namespace FX {


// File signatures
const FXushort BMP_WNT=0x4d42;
const FXushort BMP_OS2=0x4142;

// Bitmap File Header
const FXuint BFH_SIZE=14;       // Bitmap File Header size

// Bitmap Info Header
const FXuint BIH_WNTV1=40;      // BITMAPINFOHEADER size
const FXuint BIH_WNTV2=52;      // Undocumented
const FXuint BIH_WNTV3=56;      // Undocumented
const FXuint BIH_WNTV4=108;     // BITMAPV4HEADER size
const FXuint BIH_WNTV5=124;     // BITMAPV5HEADER size
const FXuint BIH_OS2V1=12;      // OS21XBITMAPHEADER size
const FXuint BIH_OS2V2=64;      // OS22XBITMAPHEADER size

// Bitmap compression values
const FXuint BIH_RGB=0;         // RGB mode
const FXuint BIH_RLE8=1;        // 8-bit/pixel rle mode
const FXuint BIH_RLE4=2;        // 4-bit/pixel rle mode
const FXuint BIH_BITFIELDS=3;   // Bit field mode
const FXuint BIH_JPEG=4;        // Not supported
const FXuint BIH_PNG=5;         // Not supported
const FXuint BIH_ALFABITFIELDS=6; // RGBA bit field masks
const FXuint BIH_CMYK=11;       // none
const FXuint BIH_CMYKRLE8=12;   // RLE-8
const FXuint BIH_CMYKRLE4=13;   // RLE-4

// RLE codes
const FXuchar RLE_ESC=0;        // RLE escape sequence
const FXuchar RLE_LINE=0;       // RLE end of line
const FXuchar RLE_END=1;        // RLE end of bitmap
const FXuchar RLE_DELTA=2;      // RLE delta

// Icons or cursors
const FXushort IDH_ICO=1;       // ICO
const FXushort IDH_CUR=2;       // CUR

// Check BMP, ICO/CUR file based on contents
extern FXAPI FXbool fxcheckBMP(FXStream& store);
extern FXAPI FXbool fxcheckICO(FXStream& store);

// Load / save BMP
extern FXAPI FXbool fxloadBMP(FXStream& store,FXColor*& data,FXint& width,FXint& height);
extern FXAPI FXbool fxsaveBMP(FXStream& store,const FXColor *data,FXint width,FXint height);

// Load / save DIB
extern FXAPI FXbool fxloadDIB(FXStream& store,FXColor*& data,FXint& width,FXint& height);
extern FXAPI FXbool fxsaveDIB(FXStream& store,const FXColor *data,FXint width,FXint height);

// Load / save ICO or CUR
extern FXAPI FXbool fxloadICO(FXStream& store,FXColor*& data,FXint& width,FXint& height,FXint& xspot,FXint& yspot);
extern FXAPI FXbool fxsaveICO(FXStream& store,const FXColor *data,FXint width,FXint height,FXint xspot=-1,FXint yspot=-1);

extern FXAPI FXbool fxloadICOStream(FXStream& store,FXColor*& data,FXint& width,FXint& height);

// Pixel structure
union RGBAPixel {
  struct {
    FXuchar b;
    FXuchar g;
    FXuchar r;
    FXuchar a;
    };
  struct {
    FXColor c;
    };
  };


// Bitmap File Header
struct BitmapFileHeader {
  FXushort bfType;              // File type
  FXuint   bfSize;              // Size of the file
  FXushort bfReserved1;         // HotspotX
  FXushort bfReserved2;         // HotspotY
  FXuint   bfOffBits;           // Offset to pixels
  };


// Bitmap Info Header
struct BitmapInfoHeader {
  FXuint   biSize;
  FXint    biWidth;             // Width
  FXint    biHeight;            // Height (<0 possible)
  FXushort biPlanes;            // Should be 1
  FXushort biBitCount;          // Number of planes
  FXuint   biCompression;       // Compression
  FXuint   biSizeImage;         // Size of image
  FXuint   biXPelsPerMeter;     // Horizontal pixels/m
  FXuint   biYPelsPerMeter;     // Vertical pixels/m
  FXuint   biClrUsed;           // Used colors
  FXuint   biClrImportant;      // Important colors
  };


// Icon Directory
struct IconDirectory {
  FXushort idReserved;          // Must be 0
  FXushort idType;              // ICO=1, CUR=2
  FXushort idCount;             // Number of images
  };


// Icon Directory Entry
struct IconDirectoryEntry {
  FXuchar  bWidth;
  FXuchar  bHeight;
  FXuchar  bColorCount;
  FXuchar  bReserved;
  FXushort wXHotspot;           // X hotspot if cursor, #planes if icon
  FXushort wYHotspot;           // Y hotspot if cursor, #bits/pixel if icon
  FXuint   dwBytesInRes;
  FXuint   dwImageOffset;
  };


// Multiplier choice when mask has #bits
// Why is this table so large? Because we can now handle really whacky
// color masks like: (R:0xF0000000,G:0x0FFF0000,B:0x0000FFFE,A:0x0000001).
// Its unlikely we encounter these but we handle it if we do!
static const FXuint maskmult[33]={
  0x0000000000,   // #bits            scales by equation
  0x00ff000000,   //   1    (m * 11111111000000000000000000000000) >> 24
  0x0055000000,   //   2    (m * 01010101000000000000000000000000) >> 24
  0x0024800000,   //   3    (m * 00100100100000000000000000000000) >> 24
  0x0011000000,   //   4    (m * 00010001000000000000000000000000) >> 24
  0x0008400000,   //   5    (m * 00001000010000000000000000000000) >> 24
  0x0004100000,   //   6    (m * 00000100000100000000000000000000) >> 24
  0x0002040000,   //   7    (m * 00000010000001000000000000000000) >> 24
  0x0001000000,   //   8    (m * 00000001000000000000000000000000) >> 24
  0x0000800000,   //   9    (m * 00000000100000000000000000000000) >> 24
  0x0000400000,   //  10    (m * 00000000010000000000000000000000) >> 24
  0x0000200000,   //  11    (m * 00000000001000000000000000000000) >> 24
  0x0000100000,   //  12    (m * 00000000000100000000000000000000) >> 24
  0x0000080000,   //  13    (m * 00000000000010000000000000000000) >> 24
  0x0000040000,   //  14    (m * 00000000000001000000000000000000) >> 24
  0x0000020000,   //  15    (m * 00000000000000100000000000000000) >> 24
  0x0000010000,   //  16    (m * 00000000000000010000000000000000) >> 24
  0x0000008000,   //  17    (m * 00000000000000001000000000000000) >> 24
  0x0000004000,   //  18    (m * 00000000000000000100000000000000) >> 24
  0x0000002000,   //  19    (m * 00000000000000000010000000000000) >> 24
  0x0000001000,   //  20    (m * 00000000000000000001000000000000) >> 24
  0x0000000800,   //  21    (m * 00000000000000000000100000000000) >> 24
  0x0000000400,   //  22    (m * 00000000000000000000010000000000) >> 24
  0x0000000200,   //  23    (m * 00000000000000000000001000000000) >> 24
  0x0000000100,   //  24    (m * 00000000000000000000000100000000) >> 24
  0x0000000080,   //  25    (m * 00000000000000000000000010000000) >> 24
  0x0000000040,   //  26    (m * 00000000000000000000000001000000) >> 24
  0x0000000020,   //  27    (m * 00000000000000000000000000100000) >> 24
  0x0000000010,   //  28    (m * 00000000000000000000000000010000) >> 24
  0x0000000008,   //  29    (m * 00000000000000000000000000001000) >> 24
  0x0000000004,   //  30    (m * 00000000000000000000000000000100) >> 24
  0x0000000002,   //  31    (m * 00000000000000000000000000000010) >> 24
  0x0000000001,   //  32    (m * 00000000000000000000000000000001) >> 24
  };

/*******************************************************************************/

// Check if stream contains a BMP
FXbool fxcheckBMP(FXStream& store){
  if(store.direction()==FXStreamLoad){
    FXuchar signature[2];
    store.load(signature,2);
    store.position(-2,FXFromCurrent);
    return signature[0]=='B' && (signature[1]=='M' || signature[1]=='A');
    }
  return false;
  }


// Check if stream contains ICO or CUR
FXbool fxcheckICO(FXStream& store){
  if(store.direction()==FXStreamLoad){
    FXbool swap=store.swapBytes();
    FXshort signature[3];
    store.setBigEndian(false);
    store.load(signature,3);
    store.position(-6,FXFromCurrent);
    store.swapBytes(swap);
    return signature[0]==0 && (signature[1]==IDH_ICO || signature[1]==IDH_CUR) && signature[2]>=1;
    }
  return false;
  }

/*******************************************************************************/

// Load bitmap bits
static FXbool fxloadBMPBits(FXStream& store,FXColor*& data,FXint width,FXint height,RGBAPixel colormap[],FXuint bpp,FXuint enc){
  if(allocElms(data,width*Math::iabs(height))){
    FXColor *dest=data+width*(height-1);
    RGBAPixel pix;
    FXColor  c1,c2;
    FXint    step=-width;
    FXuchar  padding[4],r,g,b,a;
    FXuint   rmask,gmask,bmask,amask;
    FXuint   rshift,gshift,bshift,ashift;
    FXuint   rmult,gmult,bmult,amult;
    FXint    pad,i,x,y;
    FXuchar  u,v,w;
    FXushort ww;
    FXuint   wwww;

    // Flip the vertical
    if(height<0){ height=-height; dest=data; step=width; }

    // We're doing bit fields
    if(enc==BIH_BITFIELDS || enc==BIH_ALFABITFIELDS){
      rmask=colormap[0].c;
      gmask=colormap[1].c;
      bmask=colormap[2].c;
      amask=colormap[3].c;
      rshift=ctz32(rmask);
      gshift=ctz32(gmask);
      bshift=ctz32(bmask);
      ashift=ctz32(amask);
      rmult=maskmult[pop32(rmask)];
      gmult=maskmult[pop32(gmask)];
      bmult=maskmult[pop32(bmask)];
      amult=maskmult[pop32(amask)];
      FXTRACE(TOPIC_DETAIL,"fxloadBMPBits: rmask=%032b rshift=%2d rmult=%032b\n",rmask,rshift,rmult);
      FXTRACE(TOPIC_DETAIL,"fxloadBMPBits: gmask=%032b gshift=%2d gmult=%032b\n",gmask,gshift,gmult);
      FXTRACE(TOPIC_DETAIL,"fxloadBMPBits: bmask=%032b bshift=%2d bmult=%032b\n",bmask,bshift,bmult);
      FXTRACE(TOPIC_DETAIL,"fxloadBMPBits: amask=%032b ashift=%2d amult=%032b\n",amask,ashift,amult);
      }

    // Handle various depths
    switch(bpp){
      case 1:                                   // 1-bit/pixel
        pad=(4-(((width+7)>>3)&3))&3;           // Padded to multiple of DWORD
        for(y=0; y<height; ++y){
          for(x=0; x<width-8; x+=8){
            store >> w;
            dest[y*step+x+0]=colormap[w>>7].c;
            dest[y*step+x+1]=colormap[(w>>6)&1].c;
            dest[y*step+x+2]=colormap[(w>>5)&1].c;
            dest[y*step+x+3]=colormap[(w>>4)&1].c;
            dest[y*step+x+4]=colormap[(w>>4)&1].c;
            dest[y*step+x+5]=colormap[(w>>3)&1].c;
            dest[y*step+x+6]=colormap[(w>>1)&1].c;
            dest[y*step+x+7]=colormap[w&1].c;
            }
          store >> w;
          if(x<width){ dest[y*step+x++]=colormap[w>>7].c;
          if(x<width){ dest[y*step+x++]=colormap[(w>>6)&1].c;
          if(x<width){ dest[y*step+x++]=colormap[(w>>5)&1].c;
          if(x<width){ dest[y*step+x++]=colormap[(w>>4)&1].c;
          if(x<width){ dest[y*step+x++]=colormap[(w>>4)&1].c;
          if(x<width){ dest[y*step+x++]=colormap[(w>>3)&1].c;
          if(x<width){ dest[y*step+x++]=colormap[(w>>1)&1].c;
          if(x<width){ dest[y*step+x++]=colormap[w&1].c; }}}}}}}}
          store.load(padding,pad);
          }
        return true;
      case 4:                                   // 4-bit/pixel
        if(enc==BIH_RLE4){                      // Read RLE4 compressed data
          x=0;
          y=0;
          while(!store.eof()){
            store >> a;
            store >> b;
            if(a==RLE_ESC){                     // Escape code
              if(b==RLE_END){                   // End of data
                break;
                }
              if(b==RLE_LINE){                  // End of line
                x=0;
                y++;
                continue;
                }
              if(b==RLE_DELTA){                 // Delta
                store >> a; x+=a;
                store >> a; y+=a;
                continue;
                }
              if(__unlikely(y>=height)) break;  // Safety check
              for(i=0; i<b; ++i){               // Absolute mode
                if(i&1){
                  c1=colormap[a&15].c;
                  }
                else{
                  store >> a;
                  c1=colormap[a>>4].c;
                  }
                if(__unlikely(x>=width)) continue;
                dest[y*step+x++]=c1;
                }
              if(((b&3)==1) || ((b&3)==2)) store >> a;          // Read pad byte
              }
            else{                               // Repeat mode
              if(__unlikely(y>=height)) break;  // Safety check
              c1=colormap[b>>4].c;
              c2=colormap[b&15].c;
              for(i=0; i<a && x<width; ++i){
                dest[y*step+x++]=(i&1)?c2:c1;
                }
              }
            }
          }
        else{                                   // Read uncompressed data
          pad=(4-(((width+1)>>1)&3))&3;         // Padded to multiple of DWORD
          for(y=0; y<height; y+=1){
            for(x=0; x<width-2; x+=2){
              store >> w;
              dest[y*step+x+0]=colormap[w>>4].c;
              dest[y*step+x+1]=colormap[w&15].c;
              }
            store >> w;
            if(x<width){ dest[y*step+x++]=colormap[w>>4].c;
            if(x<width){ dest[y*step+x++]=colormap[w&15].c; }}
            store.load(padding,pad);
            }
          }
        return true;
      case 8:                                   // 8-bit/pixel
        if(enc==BIH_RLE8){                      // Read RLE8 compressed data
          x=0;
          y=0;
          while(!store.eof()){
            store >> a;
            store >> b;
            if(a==RLE_ESC){                     // Escape code
              if(b==RLE_END){                   // End of data
                break;
                }
              if(b==RLE_LINE){                  // End of line
                x=0;
                y++;
                continue;
                }
              if(b==RLE_DELTA){                 // Delta
                store >> a; x+=a;
                store >> a; y+=a;
                continue;
                }
              if(__unlikely(y>=height)) break;  // Safety check
              for(i=0; i<b && x<width; ++i){    // Absolute mode
                store >> a;
                dest[y*step+x++]=colormap[a].c;
                }
              if(b&1) store >> a;               // Odd length run: read an extra pad byte
              }
            else{                               // Repeat mode
              if(__unlikely(y>=height)) break;  // Safety check
              c1=colormap[b].c;
              for(i=0; i<a && x<width; ++i){
                dest[y*step+x++]=c1;
                }
              }
            }
          }
        else{                                   // Read uncompressed data
          pad=(4-(width&3))&3;                      // Padded to DWORD
          for(y=0; y<height; ++y){
            for(x=0; x<width; ++x){
              store >> w;
              dest[y*step+x]=colormap[w].c;
              }
            store.load(padding,pad);
            }
          }
        return true;
      case 16:                                  // 16-bit/pixel
        pad=(4-((width*2)&3))&3;                // Padded to DWORD
        if(enc==BIH_BITFIELDS){
          for(y=0; y<height; ++y){
            for(x=0; x<width; x++){
              store >> ww;
              pix.r=(((ww&rmask)>>rshift)*rmult)>>24;
              pix.g=(((ww&gmask)>>gshift)*gmult)>>24;
              pix.b=(((ww&bmask)>>bshift)*bmult)>>24;
              pix.a=255;
              dest[y*step+x]=pix.c;
              }
            store.load(padding,pad);
            }
          }
        else if(enc==BIH_ALFABITFIELDS){
          for(y=0; y<height; ++y){
            for(x=0; x<width; x++){
              store >> ww;
              pix.r=(((ww&rmask)>>rshift)*rmult)>>24;
              pix.g=(((ww&gmask)>>gshift)*gmult)>>24;
              pix.b=(((ww&bmask)>>bshift)*bmult)>>24;
              pix.a=(((ww&amask)>>ashift)*amult)>>24;
              dest[y*step+x]=pix.c;
              }
            store.load(padding,pad);
            }
          }
        else{
          for(y=0; y<height; ++y){
            for(x=0; x<width; ++x){
              store >> ww;
              pix.b=(ww<<3)&0xF8; b|=b>>5;
              pix.g=(ww>>2)&0xF8; g|=g>>5;
              pix.r=(ww>>7)&0xF8; r|=r>>5;
              pix.a=255;
              dest[y*step+x]=pix.c;
              }
            store.load(padding,pad);
            }
          }
        return true;
      case 24:                                  // 24-bit/pixel
        pad=(4-((width*3)&3))&3;                // Padded to DWORD
        if(enc==BIH_BITFIELDS){
          for(y=0; y<height; ++y){
            for(x=0; x<width; ++x){
              store >> w;
              store >> v;
              store >> u;
              wwww=w|(v<<8)|(u<<16);
              pix.r=(((wwww&rmask)>>rshift)*rmult)>>24;
              pix.g=(((wwww&gmask)>>gshift)*gmult)>>24;
              pix.b=(((wwww&bmask)>>bshift)*bmult)>>24;
              pix.a=255;
              dest[y*step+x]=pix.c;
              }
            store.load(padding,pad);
            }
          }
        else if(enc==BIH_ALFABITFIELDS){
          for(y=0; y<height; ++y){
            for(x=0; x<width; ++x){
              store >> w;
              store >> v;
              store >> u;
              wwww=w|(v<<8)|(u<<16);
              pix.r=(((wwww&rmask)>>rshift)*rmult)>>24;
              pix.g=(((wwww&gmask)>>gshift)*gmult)>>24;
              pix.b=(((wwww&bmask)>>bshift)*bmult)>>24;
              pix.a=(((wwww&amask)>>ashift)*amult)>>24;
              dest[y*step+x]=pix.c;
              }
            store.load(padding,pad);
            }
          }
        else{
          for(y=0; y<height; ++y){
            for(x=0; x<width; ++x){
              store >> pix.b;
              store >> pix.g;
              store >> pix.r;
              pix.a=255;
              dest[y*step+x]=pix.c;
              }
            store.load(padding,pad);
            }
          }
        return true;
      case 32:                                  // 32-bit/pixel
        if(enc==BIH_BITFIELDS){
          for(y=0; y<height; ++y){
            for(x=0; x<width; ++x){
              store >> wwww;
              pix.r=(((wwww&rmask)>>rshift)*rmult)>>24;
              pix.g=(((wwww&gmask)>>gshift)*gmult)>>24;
              pix.b=(((wwww&bmask)>>bshift)*bmult)>>24;
              pix.a=255;
              dest[y*step+x]=pix.c;
              }
            }
          }
        else if(enc==BIH_ALFABITFIELDS){
          for(y=0; y<height; ++y){
            for(x=0; x<width; ++x){
              store >> wwww;
              pix.r=(((wwww&rmask)>>rshift)*rmult)>>24;
              pix.g=(((wwww&gmask)>>gshift)*gmult)>>24;
              pix.b=(((wwww&bmask)>>bshift)*bmult)>>24;
              pix.a=(((wwww&amask)>>ashift)*amult)>>24;
              dest[y*step+x]=pix.c;
              }
            }
          }
        else{
          for(y=0; y<height; ++y){
            for(x=0; x<width; ++x){
              store >> wwww;
              dest[y*step+x]=wwww;
              }
            }
          }
        return true;
      }
    }
  return false;
  }


// Save bitmap bits
static FXbool fxsaveBMPBits(FXStream& store,const FXColor* data,FXint width,FXint height,FXuint bpp){
  static const FXuchar padding[4]={0,0,0,0};
  if(bpp==32){          // 32-bit/pixel
    for(FXint y=height-1; y>=0; y--){
      for(FXint x=0; x<width; x++){
        FXuchar r=FXREDVAL(data[y*width+x]);
        FXuchar g=FXGREENVAL(data[y*width+x]);
        FXuchar b=FXBLUEVAL(data[y*width+x]);
        FXuchar a=FXALPHAVAL(data[y*width+x]);
        store << b;
        store << g;
        store << r;
        store << a;
        }
      }
    return true;
    }
  if(bpp==24){          // 24-bit/pixel
    FXint pad=(4-((width*3)&3))&3;              // Padded to DWORD
    for(FXint y=height-1; y>=0; y--){
      for(FXint x=0; x<width; x++){
        FXuchar r=FXREDVAL(data[y*width+x]);
        FXuchar g=FXGREENVAL(data[y*width+x]);
        FXuchar b=FXBLUEVAL(data[y*width+x]);
        store << b;
        store << g;
        store << r;
        }
      store.save(padding,pad);
      }
    return true;
    }
  return false;
  }

/*******************************************************************************/

// Load icon bits
static FXbool fxloadICOBits(FXStream& store,FXColor*& data,FXint width,FXint height,RGBAPixel colormap[],FXuint bpp,FXuint enc){

  // Load pixels (XOR bytes)
  if(fxloadBMPBits(store,data,width,height,colormap,bpp,enc)){
    FXuchar c;

    // Use AND bytes to set alpha channel
    if(bpp<32){
      FXint pad=(4-((width+7)>>3))&3;           // Padded to DWORD
      height=Math::iabs(height);                // FIXME and bytes flipped
      for(FXint y=height-1; y>=0; y--){
        for(FXint x=0; x<width; x++){
          if((x&7)==0){ store >> c; }
          if(c&0x80) data[y*width+x]&=FXRGBA(255,255,255,0);
          c<<=1;
          }
        store.position(pad,FXFromCurrent);
        }
      }

    // Got alpha, so skip over AND bytes
    else{
      FXint pad=((width+31)>>5)<<2;             // Width rounded up to DWORD
      store.position(height*pad,FXFromCurrent);
      }
    return true;
    }
  return false;
  }


// Save icon bits
static FXbool fxsaveICOBits(FXStream& store,const FXColor* data,FXint width,FXint height,FXuint bpp){
  static const FXuchar padding[4]={0,0,0,0};

  // Save pixels (XOR bytes)
  if(fxsaveBMPBits(store,data,width,height,bpp)){
    FXuchar bit,c;

    // Write AND bytes from alpha channel
    FXint pad=(4-((width+7)>>3))&3;           // Padded to DWORD
    for(FXint y=height-1; y>=0; y--){
      bit=0x80;
      c=0;
      for(FXint x=0; x<width; x++){
        if((data[y*width+x]&FXRGBA(0,0,0,255))==0) c|=bit;
        bit>>=1;
        if(bit==0){
          store << c;
          bit=0x80;
          c=0;
          }
        }
      store.save(padding,pad);
      }
    return true;
    }
  return false;
  }


// 32 bpp if alpha, 24 bpp otherwise
static FXuint checkBPP(const FXColor *data,FXint width,FXint height){
  for(FXint i=0; i<width*height; ++i){
    if((data[i]&FXRGBA(0,0,0,255))<FXRGBA(0,0,0,255)){ return 32; }
    }
  return 24;
  }


/*******************************************************************************/

// Load BMP image from stream
FXbool fxloadBMP(FXStream& store,FXColor*& data,FXint& width,FXint& height){
  FXbool result=false;

  // Null out
  data=nullptr;
  width=0;
  height=0;

  // Stream must be loading
  if(store.direction()==FXStreamLoad){

    // Old swap state
    FXbool swap=store.swapBytes();

    // Make little-endian
    store.setBigEndian(false);

    // Get size and offset
    BitmapFileHeader bfh;
    store >> bfh.bfType;
    store >> bfh.bfSize;
    store >> bfh.bfReserved1;
    store >> bfh.bfReserved2;
    store >> bfh.bfOffBits;

    FXTRACE(TOPIC_DETAIL,"fxloadBMP: bfType=0x%04x\n",bfh.bfType);
    FXTRACE(TOPIC_DETAIL,"fxloadBMP: bfSize=%u\n",bfh.bfSize);
    FXTRACE(TOPIC_DETAIL,"fxloadBMP: bfOffBits=%u\n",bfh.bfOffBits);
    FXTRACE(TOPIC_DETAIL,"fxloadBMP: bmiStart=%ld\n",store.position());

    // Check signature
    if(bfh.bfType==BMP_WNT || bfh.bfType==BMP_OS2){

      // Read bitmap info header; default masks BGRA
      BitmapInfoHeader bmi={0,0,0,0,0,0,0,0,0,0,0};
      store >> bmi.biSize;

      FXTRACE(TOPIC_DETAIL,"fxloadBMP: biSize=%u\n",bmi.biSize);

      // Known BMP header size?
      if(bmi.biSize==BIH_WNTV1 || bmi.biSize==BIH_WNTV2 || bmi.biSize==BIH_WNTV3 || bmi.biSize==BIH_WNTV4 || bmi.biSize==BIH_WNTV5 || bmi.biSize==BIH_OS2V1 || bmi.biSize==BIH_OS2V2){
        RGBAPixel colormap[256];

        // Older OS/2 1.x BitmapInfoHeader
        if(bmi.biSize==BIH_OS2V1){
          FXushort ss;
          store >> ss; bmi.biWidth=ss;
          store >> ss; bmi.biHeight=ss;
          store >> bmi.biPlanes;
          store >> bmi.biBitCount;
          bmi.biCompression=BIH_RGB;
          bmi.biSizeImage=(((bmi.biPlanes*bmi.biBitCount*bmi.biWidth)+31)>>5)*4*bmi.biHeight;
          bmi.biXPelsPerMeter=0;
          bmi.biYPelsPerMeter=0;
          bmi.biClrUsed=(bfh.bfOffBits-BIH_OS2V1-BFH_SIZE)/3;   // As suggested
          bmi.biClrImportant=0;
          store.position(bmi.biSize-BIH_OS2V1,FXFromCurrent);
          }

        // Newer Windows BitmapInfoHeader
        else{
          store >> bmi.biWidth;
          store >> bmi.biHeight;
          store >> bmi.biPlanes;
          store >> bmi.biBitCount;
          store >> bmi.biCompression;
          store >> bmi.biSizeImage;
          store >> bmi.biXPelsPerMeter;
          store >> bmi.biYPelsPerMeter;
          store >> bmi.biClrUsed;
          store >> bmi.biClrImportant;
          if(bmi.biSize==BIH_WNTV3 || bmi.biSize==BIH_WNTV4 || bmi.biSize==BIH_WNTV5){
            store >> colormap[0].c;       // Red mask
            store >> colormap[1].c;       // Green mask
            store >> colormap[2].c;       // Blue mask
            store >> colormap[3].c;       // Alpha mask
            store.position(bmi.biSize-BIH_WNTV3,FXFromCurrent);
            }
          else{
            store.position(bmi.biSize-BIH_WNTV1,FXFromCurrent);
            }
          }

        FXTRACE(TOPIC_DETAIL,"fxloadBMP: biWidth=%d\n",bmi.biWidth);
        FXTRACE(TOPIC_DETAIL,"fxloadBMP: biHeight=%d\n",bmi.biHeight);
        FXTRACE(TOPIC_DETAIL,"fxloadBMP: biBitCount=%d\n",bmi.biBitCount);
        FXTRACE(TOPIC_DETAIL,"fxloadBMP: biClrUsed=%d\n",bmi.biClrUsed);
        FXTRACE(TOPIC_DETAIL,"fxloadBMP: biSizeImage=%d\n",bmi.biSizeImage);
        FXTRACE(TOPIC_DETAIL,"fxloadBMP: biCompression=%d\n",bmi.biCompression);
        FXTRACE(TOPIC_DETAIL,"fxloadBMP: colormapStart=%ld\n",store.position());

        // Check for sensible inputs
        if(0<bmi.biWidth && 0!=bmi.biHeight && 1<=bmi.biBitCount && bmi.biBitCount<=32){
          FXuint colors=(bmi.biBitCount<=8 && (bmi.biClrUsed==0 || bmi.biClrUsed>256)) ? 1<<bmi.biBitCount : bmi.biClrUsed;
          FXint gapsize;

          FXTRACE(TOPIC_DETAIL,"fxloadBMP: colors=%d\n",colors);


          // Read bitfields if we haven't already
          if(8<bmi.biBitCount && bmi.biSize<BIH_WNTV3){
            if(bmi.biCompression==BIH_BITFIELDS){
              store >> colormap[0].c;     // Red mask
              store >> colormap[1].c;     // Green mask
              store >> colormap[2].c;     // Blue mask
              colormap[3].c=0;            // No alpha
              }
            if(bmi.biCompression==BIH_ALFABITFIELDS){
              store >> colormap[0].c;     // Red mask
              store >> colormap[1].c;     // Green mask
              store >> colormap[2].c;     // Blue mask
              store >> colormap[3].c;     // Alpha mask
              }
            }

          // Read colormap
          if(bmi.biBitCount<=8 && colors<=256){
            if(bmi.biSize!=BIH_OS2V1){
              for(FXuint i=0; i<colors; i++){
                store >> colormap[i].b;   // Blue
                store >> colormap[i].g;   // Green
                store >> colormap[i].r;   // Red
                store >> colormap[i].a;
                colormap[i].a=255;
                }
              }
            else{
              for(FXuint i=0; i<colors; i++){
                store >> colormap[i].b;   // Blue
                store >> colormap[i].g;   // Green
                store >> colormap[i].r;   // Red
                colormap[i].a=255;
                }
              }
            }

          // Start of data is supposedly at bfh.bfOffBits, but sometimes
          // there's a gap between end of bitmap info header and colormap
          // to the pixel data; this gap should be positive or zero.
          gapsize=bfh.bfOffBits-store.position();
          FXTRACE(TOPIC_DETAIL,"fxloadBMP: gapsize=%d\n",gapsize);
          if(0<=gapsize){

            // Jump forward over the gap to get to data field
            store.position(bfh.bfOffBits,FXFromStart);

            // But wait, there's more
            if(!store.eof()){

              // Image dimensions
              width=bmi.biWidth;
              height=Math::iabs(bmi.biHeight);

              FXTRACE(TOPIC_DETAIL,"fxloadBMP: dataStart=%ld\n",store.position());

              // Load the bits
              result=fxloadBMPBits(store,data,bmi.biWidth,bmi.biHeight,colormap,bmi.biBitCount,bmi.biCompression);

              FXTRACE(TOPIC_DETAIL,"fxloadBMP: dataEnd=%ld\n",store.position());
              }
            }
          }
        }
      }

    // Restore byte order
    store.swapBytes(swap);
    }
  FXTRACE(TOPIC_DETAIL,"fxloadBMP: %s\n\n",result?"OK":"FAIL");
  return result;
  }

/*******************************************************************************/

// Load DIB image from stream
FXbool fxloadDIB(FXStream& store,FXColor*& data,FXint& width,FXint& height){
  FXbool result=false;

  // Null out
  data=nullptr;
  width=0;
  height=0;

  // Stream must be loading
  if(store.direction()==FXStreamLoad){

    // Old swap state
    FXbool swap=store.swapBytes();

    // Make little-endian
    store.setBigEndian(false);

    // Read bitmap info header
    BitmapInfoHeader bmi={0,0,0,0,0,0,0,0,0,0,0};
    store >> bmi.biSize;

    // Check bitmap info header size
    if(bmi.biSize==BIH_WNTV1 || bmi.biSize==BIH_WNTV2 || bmi.biSize==BIH_WNTV3 || bmi.biSize==BIH_WNTV4 || bmi.biSize==BIH_WNTV5){
      RGBAPixel colormap[256];
      store >> bmi.biWidth;
      store >> bmi.biHeight;
      store >> bmi.biPlanes;
      store >> bmi.biBitCount;
      store >> bmi.biCompression;
      store >> bmi.biSizeImage;
      store >> bmi.biXPelsPerMeter;
      store >> bmi.biYPelsPerMeter;
      store >> bmi.biClrUsed;
      store >> bmi.biClrImportant;
      if(bmi.biSize==BIH_WNTV3 || bmi.biSize==BIH_WNTV4 || bmi.biSize==BIH_WNTV5){
        store >> colormap[0].c;           // Red mask
        store >> colormap[1].c;           // Green mask
        store >> colormap[2].c;           // Blue mask
        store >> colormap[3].c;           // Alpha mask
        store.position(bmi.biSize-BIH_WNTV3,FXFromCurrent);
        }
      else{
        store.position(bmi.biSize-BIH_WNTV1,FXFromCurrent);
        }

      FXTRACE(TOPIC_DETAIL,"fxloadDIB: biWidth=%d\n",bmi.biWidth);
      FXTRACE(TOPIC_DETAIL,"fxloadDIB: biHeight=%d\n",bmi.biHeight);
      FXTRACE(TOPIC_DETAIL,"fxloadDIB: biBitCount=%d\n",bmi.biBitCount);
      FXTRACE(TOPIC_DETAIL,"fxloadDIB: biClrUsed=%d\n",bmi.biClrUsed);
      FXTRACE(TOPIC_DETAIL,"fxloadDIB: biSizeImage=%d\n",bmi.biSizeImage);
      FXTRACE(TOPIC_DETAIL,"fxloadDIB: biCompression=%d\n",bmi.biCompression);
      FXTRACE(TOPIC_DETAIL,"fxloadDIB: colormapStart=%ld\n",store.position());

      // Check for sensible inputs
      if(0<bmi.biWidth && 0!=bmi.biHeight && 1<=bmi.biBitCount && bmi.biBitCount<=32){
        FXuint colors=(bmi.biBitCount<=8 && (bmi.biClrUsed==0 || bmi.biClrUsed>256)) ? 1<<bmi.biBitCount : bmi.biClrUsed;

        FXTRACE(TOPIC_DETAIL,"fxloadDIB: colors=%d\n",colors);

        // Read bitfields if we haven't already
        if(8<bmi.biBitCount && bmi.biSize<BIH_WNTV3){
          if(bmi.biCompression==BIH_BITFIELDS){
            store >> colormap[0].c;       // Red mask
            store >> colormap[1].c;       // Green mask
            store >> colormap[2].c;       // Blue mask
            colormap[3].c=0;              // No alpha
            }
          if(bmi.biCompression==BIH_ALFABITFIELDS){
            store >> colormap[0].c;       // Red mask
            store >> colormap[1].c;       // Green mask
            store >> colormap[2].c;       // Blue mask
            store >> colormap[3].c;       // Alpha mask
            }
          }

        // Read colormap
        if(bmi.biBitCount<=8){
          for(FXuint i=0; i<colors; i++){
            store >> colormap[i].b;       // Blue
            store >> colormap[i].g;       // Green
            store >> colormap[i].r;       // Red
            store >> colormap[i].a;
            colormap[i].a=255;
            }
          }

        // But wait, there's more
        if(!store.eof()){

          // Image dimensions
          width=bmi.biWidth;
          height=Math::iabs(bmi.biHeight);

          // Load the bits
          result=fxloadBMPBits(store,data,bmi.biWidth,bmi.biHeight,colormap,bmi.biBitCount,bmi.biCompression);
          }
        }
      }

    // Restore byte order
    store.swapBytes(swap);
    }
  FXTRACE(TOPIC_DETAIL,"fxloadDIB: %s\n\n",result?"OK":"FAIL");
  return result;
  }

/*******************************************************************************/

// Save BMP image to file stream
FXbool fxsaveBMP(FXStream& store,const FXColor *data,FXint width,FXint height){
  FXbool result=false;

  // Stream must be saving
  if(store.direction()==FXStreamSave){

    // Must make sense
    if(data && 0<width && 0<height){

      // Save byte order
      FXbool swap=store.swapBytes();

      // Use alpha channel if image not opaque
      FXushort bpp=checkBPP(data,width,height);

      // Make little-endian
      store.setBigEndian(false);

      // BitmapFileHeader
      BitmapFileHeader bfh={BMP_WNT,FXuint(14+BIH_WNTV1+height*(((width*bpp+31)>>5)<<2)),0,0,14+BIH_WNTV1};

      // Initialize bitmap info header
      BitmapInfoHeader bmi={BIH_WNTV1,width,height,1,bpp,BIH_RGB,FXuint(height*(((width*bpp+31)>>5)<<2)),75*39,75*39,0,0};

      // BitmapFileHeader
      store << bfh.bfType;        // Magic number
      store << bfh.bfSize;        // File size
      store << bfh.bfReserved1;   // bfReserved1
      store << bfh.bfReserved2;   // bfReserved2
      store << bfh.bfOffBits;     // bfOffBits

      // Bitmap Info Header
      store << bmi.biSize;
      store << bmi.biWidth;
      store << bmi.biHeight;
      store << bmi.biPlanes;
      store << bmi.biBitCount;            // biBitCount (1,4,8,24, or 32)
      store << bmi.biCompression;         // biCompression:  BIH_RGB, BIH_RLE8, BIH_RLE4, or BIH_BITFIELDS
      store << bmi.biSizeImage;
      store << bmi.biXPelsPerMeter;       // biXPelsPerMeter: (75dpi * 39" per meter)
      store << bmi.biYPelsPerMeter;       // biYPelsPerMeter: (75dpi * 39" per meter)
      store << bmi.biClrUsed;
      store << bmi.biClrImportant;

      // Save pixels
      result=fxsaveBMPBits(store,data,width,height,bpp);

      // Restore byte order
      store.swapBytes(swap);
      }
    }
  FXTRACE(TOPIC_DETAIL,"fxsaveBMP: %s\n\n",result?"OK":"FAIL");
  return result;
  }

/*******************************************************************************/

// Save DIB image to stream
FXbool fxsaveDIB(FXStream& store,const FXColor *data,FXint width,FXint height){
  FXbool result=false;

  // Stream must be saving
  if(store.direction()==FXStreamSave){

    // Must make sense
    if(data && 0<width && 0<height){

      // Save byte order
      FXbool swap=store.swapBytes();

      // Use alpha channel if image not opaque
      FXushort bpp=checkBPP(data,width,height);

      // Make little-endian
      store.setBigEndian(false);

      // Initialize bitmap info header
      BitmapInfoHeader bmi={BIH_WNTV1,width,height,1,bpp,BIH_RGB,FXuint(height*(((width*bpp+31)>>5)<<2)),75*39,75*39,0,0};

      // Bitmap Info Header
      store << bmi.biSize;
      store << bmi.biWidth;
      store << bmi.biHeight;
      store << bmi.biPlanes;
      store << bmi.biBitCount;            // biBitCount (1,4,8,24, or 32)
      store << bmi.biCompression;         // biCompression:  BIH_RGB, BIH_RLE8, BIH_RLE4, or BIH_BITFIELDS
      store << bmi.biSizeImage;           // Image size in bytes
      store << bmi.biXPelsPerMeter;       // biXPelsPerMeter: (75dpi * 39" per meter)
      store << bmi.biYPelsPerMeter;       // biYPelsPerMeter: (75dpi * 39" per meter)
      store << bmi.biClrUsed;
      store << bmi.biClrImportant;

      // Save pixels
      result=fxsaveBMPBits(store,data,width,height,bpp);

      // Restore byte order
      store.swapBytes(swap);
      }
    }
  FXTRACE(TOPIC_DETAIL,"fxsaveDIB: %s\n\n",result?"OK":"FAIL");
  return result;
  }

/*******************************************************************************/

// Load ICO image from stream
FXbool fxloadICO(FXStream& store,FXColor*& data,FXint& width,FXint& height,FXint& xspot,FXint& yspot){
  FXbool result=false;

  // Null out
  data=nullptr;
  width=0;
  height=0;
  xspot=-1;
  yspot=-1;

  // Stream must be loading
  if(store.direction()==FXStreamLoad){
    FXbool swap=store.swapBytes();

    // Make little-endian
    store.setBigEndian(false);

    // Icon Directory Header
    IconDirectory icd;
    store >> icd.idReserved;      // Must be zero
    store >> icd.idType;          // Must be 1 (icon) or 2 (cursor)
    store >> icd.idCount;         // Only one icon

    // Validity of icon directory header
    if(icd.idReserved==0 && 0<icd.idCount && (icd.idType==IDH_ICO || icd.idType==IDH_CUR)){

      // Icon Directory Entry
      IconDirectoryEntry ice;
      store >> ice.bWidth;
      store >> ice.bHeight;
      store >> ice.bColorCount;     // 0 for > 8bit/pixel
      store >> ice.bReserved;       // 0
      store >> ice.wXHotspot;       // X hotspot if cursor, #planes if icon
      store >> ice.wYHotspot;       // Y hotspot if cursor, #bits/pixel if icon
      store >> ice.dwBytesInRes;    // Total number of bytes in images (including palette data)
      store >> ice.dwImageOffset;   // Location of image from the beginning of file

      // Skip to bitmap info header
      store.position(ice.dwImageOffset-22,FXFromCurrent);

      // Initialize bitmap info header
      BitmapInfoHeader bmi={0,0,0,0,0,0,0,0,0,0,0};
      store >> bmi.biSize;
      if(bmi.biSize==BIH_WNTV1 || bmi.biSize==BIH_WNTV2 || bmi.biSize==BIH_WNTV3 || bmi.biSize==BIH_WNTV4 || bmi.biSize==BIH_WNTV5){
        RGBAPixel colormap[256];
        store >> bmi.biWidth;
        store >> bmi.biHeight;
        store >> bmi.biPlanes;
        store >> bmi.biBitCount;
        store >> bmi.biCompression;
        store >> bmi.biSizeImage;
        store >> bmi.biXPelsPerMeter;
        store >> bmi.biYPelsPerMeter;
        store >> bmi.biClrUsed;
        store >> bmi.biClrImportant;
        if(bmi.biSize==BIH_WNTV3 || bmi.biSize==BIH_WNTV4 || bmi.biSize==BIH_WNTV5){
          store >> colormap[0].c;   // Red mask
          store >> colormap[1].c;   // Green mask
          store >> colormap[2].c;   // Blue mask
          store >> colormap[3].c;   // Alpha mask
          store.position(bmi.biSize-BIH_WNTV3,FXFromCurrent);
          }
        else{
          store.position(bmi.biSize-BIH_WNTV1,FXFromCurrent);
          }

        FXTRACE(TOPIC_DETAIL,"fxloadICO: biWidth=%d\n",bmi.biWidth);
        FXTRACE(TOPIC_DETAIL,"fxloadICO: biHeight=%d\n",bmi.biHeight);
        FXTRACE(TOPIC_DETAIL,"fxloadICO: biBitCount=%d\n",bmi.biBitCount);
        FXTRACE(TOPIC_DETAIL,"fxloadICO: biClrUsed=%d\n",bmi.biClrUsed);
        FXTRACE(TOPIC_DETAIL,"fxloadICO: biSizeImage=%d\n",bmi.biSizeImage);
        FXTRACE(TOPIC_DETAIL,"fxloadICO: biCompression=%d\n",bmi.biCompression);
        FXTRACE(TOPIC_DETAIL,"fxloadICO: idCount=%d\n",icd.idCount);
        FXTRACE(TOPIC_DETAIL,"fxloadICO: idType=%d\n",icd.idType);
        FXTRACE(TOPIC_DETAIL,"fxloadICO: wXHotspot=%d\n",ice.wXHotspot);
        FXTRACE(TOPIC_DETAIL,"fxloadICO: wYHotspot=%d\n",ice.wYHotspot);
        FXTRACE(TOPIC_DETAIL,"fxloadICO: colormapStart=%ld\n",store.position());

        // Check for sensible inputs
        if(0<bmi.biWidth && 0!=bmi.biHeight && 1<=bmi.biBitCount && bmi.biBitCount<=32){
          FXuint colors=(bmi.biBitCount<=8 && (bmi.biClrUsed==0 || bmi.biClrUsed>256)) ? 1<<bmi.biBitCount : bmi.biClrUsed;

          FXTRACE(TOPIC_DETAIL,"fxloadICO: colors=%d\n",colors);

          // Read bitfields if we haven't already
          if(8<bmi.biBitCount && bmi.biSize<BIH_WNTV3){
            if(bmi.biCompression==BIH_BITFIELDS){
              store >> colormap[0].c;     // Red mask
              store >> colormap[1].c;     // Green mask
              store >> colormap[2].c;     // Blue mask
              colormap[3].c=0;              // No alpha
              }
            if(bmi.biCompression==BIH_ALFABITFIELDS){
              store >> colormap[0].c;     // Red mask
              store >> colormap[1].c;     // Green mask
              store >> colormap[2].c;     // Blue mask
              store >> colormap[3].c;     // Alpha mask
              }
            }

          // Read colormap
          if(bmi.biBitCount<=8){
            for(FXuint i=0; i<colors; i++){
              store >> colormap[i].b;     // Blue
              store >> colormap[i].g;     // Green
              store >> colormap[i].r;     // Red
              store >> colormap[i].a;
              colormap[i].a=255;
              }
            }

          // But wait, there's more
          if(!store.eof()){

            // Image dimensions
            width=bmi.biWidth;
            height=Math::iabs(bmi.biHeight)/2;

            // Copy hotspot location if cursor
            if(icd.idType==IDH_CUR){
              xspot=ice.wXHotspot;
              yspot=ice.wYHotspot;
              }

            // Load the bits
            result=fxloadICOBits(store,data,bmi.biWidth,bmi.biHeight/2,colormap,bmi.biBitCount,bmi.biCompression);
            }
          }
        }
      }

    // Restore byte order
    store.swapBytes(swap);
    }
  FXTRACE(TOPIC_DETAIL,"fxloadICO: %s\n\n",result?"OK":"FAIL");
  return result;
  }


// Load ICO Image from stream
FXbool fxloadICOStream(FXStream& store,FXColor*& data,FXint& width,FXint& height){
  FXbool result=false;

  // Null out
  data=nullptr;
  width=0;
  height=0;

  // Stream must be loading
  if(store.direction()==FXStreamLoad){

    // Save old byte order
    FXbool swap=store.swapBytes();

    // Bitmaps are little-endian
    store.setBigEndian(false);

    // Read bitmap info header
    BitmapInfoHeader bmi={0,0,0,0,0,0,0,0,0,0,0};
    store >> bmi.biSize;

    // Check bitmap info header size
    if(bmi.biSize==BIH_WNTV1 || bmi.biSize==BIH_WNTV2 || bmi.biSize==BIH_WNTV3 || bmi.biSize==BIH_WNTV4 || bmi.biSize==BIH_WNTV5){
      RGBAPixel colormap[256];
      store >> bmi.biWidth;
      store >> bmi.biHeight;
      store >> bmi.biPlanes;
      store >> bmi.biBitCount;
      store >> bmi.biCompression;
      store >> bmi.biSizeImage;
      store >> bmi.biXPelsPerMeter;
      store >> bmi.biYPelsPerMeter;
      store >> bmi.biClrUsed;
      store >> bmi.biClrImportant;
      if(bmi.biSize==BIH_WNTV3 || bmi.biSize==BIH_WNTV4 || bmi.biSize==BIH_WNTV5){
        store >> colormap[0].c;   // Red mask
        store >> colormap[1].c;   // Green mask
        store >> colormap[2].c;   // Blue mask
        store >> colormap[3].c;   // Alpha mask
        store.position(bmi.biSize-BIH_WNTV3,FXFromCurrent);
        }
      else{
        store.position(bmi.biSize-BIH_WNTV1,FXFromCurrent);
        }

      FXTRACE(TOPIC_DETAIL,"fxloadICOStream: biWidth=%d\n",bmi.biWidth);
      FXTRACE(TOPIC_DETAIL,"fxloadICOStream: biHeight=%d\n",bmi.biHeight);
      FXTRACE(TOPIC_DETAIL,"fxloadICOStream: biBitCount=%d\n",bmi.biBitCount);
      FXTRACE(TOPIC_DETAIL,"fxloadICOStream: biClrUsed=%d\n",bmi.biClrUsed);
      FXTRACE(TOPIC_DETAIL,"fxloadICOStream: biSizeImage=%d\n",bmi.biSizeImage);
      FXTRACE(TOPIC_DETAIL,"fxloadICOStream: biCompression=%d\n",bmi.biCompression);
      FXTRACE(TOPIC_DETAIL,"fxloadICOStream: colormapStart=%ld\n",store.position());

      // Check for sensible inputs
      if(bmi.biPlanes==1 && 0<bmi.biWidth && 0!=bmi.biHeight && bmi.biClrUsed<=256 && bmi.biCompression<=BIH_RLE4){
        FXuint colors=(bmi.biBitCount<=8 && (bmi.biClrUsed==0 || bmi.biClrUsed>256)) ? 1<<bmi.biBitCount : bmi.biClrUsed;

        FXTRACE(TOPIC_DETAIL,"fxloadICOStream: colors=%d\n",colors);

        // Read bitfields if we haven't already
        if(8<bmi.biBitCount && bmi.biSize<BIH_WNTV3){
          if(bmi.biCompression==BIH_BITFIELDS){
            store >> colormap[0].c;       // Red mask
            store >> colormap[1].c;       // Green mask
            store >> colormap[2].c;       // Blue mask
            colormap[3].c=0;              // No alpha
            }
          if(bmi.biCompression==BIH_ALFABITFIELDS){
            store >> colormap[0].c;       // Red mask
            store >> colormap[1].c;       // Green mask
            store >> colormap[2].c;       // Blue mask
            store >> colormap[3].c;       // Alpha mask
            }
          }

        // Read colormap
        if(bmi.biBitCount<=8){
          for(FXuint i=0; i<colors; i++){
            store >> colormap[i].b;       // Blue
            store >> colormap[i].g;       // Green
            store >> colormap[i].r;       // Red
            store >> colormap[i].a;
            colormap[i].a=255;
            }
          }

        // But wait, there's more
        if(!store.eof()){

          // Image dimensions
          width=bmi.biWidth;
          height=Math::iabs(bmi.biHeight)/2;         // Topsy turvy possibility; adjust height also

          // Load the bits
          result=fxloadICOBits(store,data,bmi.biWidth,bmi.biHeight/2,colormap,bmi.biBitCount,bmi.biCompression);
          }
        }
      }


    // Restore byte order
    store.swapBytes(swap);
    }
  FXTRACE(TOPIC_DETAIL,"fxloadICOStream: %s\n\n",result?"OK":"FAIL");
  return result;
  }

/*******************************************************************************/

// Save a ICO file to a stream
FXbool fxsaveICO(FXStream& store,const FXColor *data,FXint width,FXint height,FXint xspot,FXint yspot){
  FXbool result=false;

  // Stream must be saving
  if(store.direction()==FXStreamSave){

    // Must make sense
    if(data && 0<width && 0<height && width<256 && height<256){

      // Save byte order
      FXbool swap=store.swapBytes();

      // Bitmaps are little-endian
      store.setBigEndian(false);

      // Use alpha channel if image not opaque
      FXushort bpp=checkBPP(data,width,height);

      // Initialize icon directory header
      IconDirectory icd={0,IDH_CUR,1};

      // Initialize icon directory entry
      IconDirectoryEntry ice={(FXuchar)width,(FXuchar)height,0,0,(FXushort)xspot,(FXushort)yspot,FXuint(BIH_WNTV1+height*((((width*bpp+31)>>5)<<2)+(((width+31)>>5)<<2))),22};

      // Initialize bitmap info header
      BitmapInfoHeader bmi={BIH_WNTV1,width,height*2,1,bpp,BIH_RGB,FXuint(height*(((width*bpp+31)>>5)<<2)),75*39,75*39,0,0};

      // Save as ico if no hotspot
      if(xspot<0 || yspot<0){
        icd.idType=IDH_ICO;
        ice.wXHotspot=1;
        ice.wYHotspot=bpp;
        }

      // Icon Directory Header
      store << icd.idReserved;      // Must be zero
      store << icd.idType;          // Must be 1 (icon) or 2 (cursor)
      store << icd.idCount;         // Only one icon

      // Icon Directory Entry
      store << ice.bWidth;
      store << ice.bHeight;
      store << ice.bColorCount;     // 0 for > 8bit/pixel
      store << ice.bReserved;       // 0
      store << ice.wXHotspot;       // X hotspot if cursor, #planes if icon
      store << ice.wYHotspot;       // Y hotspot if cursor, #bits/pixel if icon
      store << ice.dwBytesInRes;    // Total number of bytes in images (including palette data)
      store << ice.dwImageOffset;   // Location of image from the beginning of file

      // Bitmap Info Header
      store << bmi.biSize;
      store << bmi.biWidth;
      store << bmi.biHeight;
      store << bmi.biPlanes;
      store << bmi.biBitCount;
      store << bmi.biCompression;
      store << bmi.biSizeImage;
      store << bmi.biXPelsPerMeter;
      store << bmi.biYPelsPerMeter;
      store << bmi.biClrUsed;
      store << bmi.biClrImportant;

      // Save pixels
      result=fxsaveICOBits(store,data,width,height,bpp);

      // Restore byte order
      store.swapBytes(swap);
      }
    }
  FXTRACE(TOPIC_DETAIL,"fxsaveICO: %s\n\n",result?"OK":"FAIL");
  return result;
  }

}
