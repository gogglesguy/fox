#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXTextCodec.h"
#include "FX885916Codec.h"

namespace FX {

FXIMPLEMENT(FX885916Codec,FXTextCodec,NULL,0)


//// Created by codec tool on 03/25/2005 from: 8859-16.TXT ////
const unsigned short forward_data[256]={
   0,    1,    2,    3,    4,    5,    6,    7,    8,    9,    10,   11,   12,   13,   14,   15,
   16,   17,   18,   19,   20,   21,   22,   23,   24,   25,   26,   27,   28,   29,   30,   31,
   32,   33,   34,   35,   36,   37,   38,   39,   40,   41,   42,   43,   44,   45,   46,   47,
   48,   49,   50,   51,   52,   53,   54,   55,   56,   57,   58,   59,   60,   61,   62,   63,
   64,   65,   66,   67,   68,   69,   70,   71,   72,   73,   74,   75,   76,   77,   78,   79,
   80,   81,   82,   83,   84,   85,   86,   87,   88,   89,   90,   91,   92,   93,   94,   95,
   96,   97,   98,   99,   100,  101,  102,  103,  104,  105,  106,  107,  108,  109,  110,  111,
   112,  113,  114,  115,  116,  117,  118,  119,  120,  121,  122,  123,  124,  125,  126,  127,
   128,  129,  130,  131,  132,  133,  134,  135,  136,  137,  138,  139,  140,  141,  142,  143,
   144,  145,  146,  147,  148,  149,  150,  151,  152,  153,  154,  155,  156,  157,  158,  159,
   160,  260,  261,  321,  8364, 8222, 352,  167,  353,  169,  536,  171,  377,  173,  378,  379,
   176,  177,  268,  322,  381,  8221, 182,  183,  382,  269,  537,  187,  338,  339,  376,  380,
   192,  193,  194,  258,  196,  262,  198,  199,  200,  201,  202,  203,  204,  205,  206,  207,
   272,  323,  210,  211,  212,  336,  214,  346,  368,  217,  218,  219,  220,  280,  538,  223,
   224,  225,  226,  259,  228,  263,  230,  231,  232,  233,  234,  235,  236,  237,  238,  239,
   273,  324,  242,  243,  244,  337,  246,  347,  369,  249,  250,  251,  252,  281,  539,  255,
  };


const unsigned char reverse_plane[17]={
  0, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
  };

const unsigned char reverse_pages[73]={
  0,  34, 34, 34, 34, 34, 34, 34, 97, 34, 34, 34, 34, 34, 34, 34,
  34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
  34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
  34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
  34, 34, 34, 34, 34, 34, 34, 34, 34,
  };

const unsigned short reverse_block[161]={
  0,   16,  32,  48,  64,  80,  96,  112, 128, 144, 160, 176, 192, 208, 224, 240,
  256, 272, 282, 282, 297, 313, 329, 345, 282, 282, 282, 282, 282, 282, 282, 282,
  282, 360, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282,
  282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282,
  282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282,
  282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282,
  282, 282, 372, 282, 282, 282, 282, 282, 282, 282, 282, 387, 282, 282, 282, 282,
  282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282,
  282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282,
  282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282,
  282,
  };

const unsigned char reverse_data[403]={
   0,    1,    2,    3,    4,    5,    6,    7,    8,    9,    10,   11,   12,   13,   14,   15,
   16,   17,   18,   19,   20,   21,   22,   23,   24,   25,   26,   27,   28,   29,   30,   31,
   32,   33,   34,   35,   36,   37,   38,   39,   40,   41,   42,   43,   44,   45,   46,   47,
   48,   49,   50,   51,   52,   53,   54,   55,   56,   57,   58,   59,   60,   61,   62,   63,
   64,   65,   66,   67,   68,   69,   70,   71,   72,   73,   74,   75,   76,   77,   78,   79,
   80,   81,   82,   83,   84,   85,   86,   87,   88,   89,   90,   91,   92,   93,   94,   95,
   96,   97,   98,   99,   100,  101,  102,  103,  104,  105,  106,  107,  108,  109,  110,  111,
   112,  113,  114,  115,  116,  117,  118,  119,  120,  121,  122,  123,  124,  125,  126,  127,
   128,  129,  130,  131,  132,  133,  134,  135,  136,  137,  138,  139,  140,  141,  142,  143,
   144,  145,  146,  147,  148,  149,  150,  151,  152,  153,  154,  155,  156,  157,  158,  159,
   160,  26,   26,   26,   26,   26,   26,   167,  26,   169,  26,   171,  26,   173,  26,   26,
   176,  177,  26,   26,   26,   26,   182,  183,  26,   26,   26,   187,  26,   26,   26,   26,
   192,  193,  194,  26,   196,  26,   198,  199,  200,  201,  202,  203,  204,  205,  206,  207,
   26,   26,   210,  211,  212,  26,   214,  26,   26,   217,  218,  219,  220,  26,   26,   223,
   224,  225,  226,  26,   228,  26,   230,  231,  232,  233,  234,  235,  236,  237,  238,  239,
   26,   26,   242,  243,  244,  26,   246,  26,   26,   249,  250,  251,  252,  26,   26,   255,
   26,   26,   195,  227,  161,  162,  197,  229,  26,   26,   26,   26,   178,  185,  26,   26,
   208,  240,  26,   26,   26,   26,   26,   26,   221,  253,  26,   26,   26,   26,   26,   26,
   26,   26,   26,   26,   26,   26,   26,   26,   26,   26,   163,  179,  209,  241,  26,   26,
   26,   26,   26,   26,   26,   26,   26,   26,   26,   213,  245,  188,  189,  26,   26,   26,
   26,   26,   26,   215,  247,  26,   26,   26,   26,   166,  168,  26,   26,   26,   26,   26,
   26,   26,   26,   26,   26,   26,   26,   26,   26,   216,  248,  26,   26,   26,   26,   26,
   26,   190,  172,  174,  175,  191,  180,  184,  26,   26,   26,   26,   26,   26,   26,   26,
   170,  186,  222,  254,  26,   26,   26,   26,   26,   26,   26,   26,   26,   26,   26,   26,
   26,   181,  165,  26,   26,   26,   26,   26,   26,   26,   26,   26,   26,   26,   26,   164,
   26,   26,   26,
  };


FXint FX885916Codec::mb2wc(FXwchar& wc,const FXchar* src,FXint nsrc) const {
  if(nsrc<1) return -1;
  wc=forward_data[(FXuchar)src[0]];
  return 1;
  }


FXint FX885916Codec::wc2mb(FXchar* dst,FXint ndst,FXwchar wc) const {
  if(ndst<1) return -1;
  dst[0]=reverse_data[reverse_block[reverse_pages[reverse_plane[wc>>16]+((wc>>10)&63)]+((wc>>4)&63)]+(wc&15)];
  return 1;
  }

FXint FX885916Codec::mibEnum() const {
  return 112;
  }


const FXchar* FX885916Codec::name() const {
  return "ISO-8859-16";
  }


const FXchar* FX885916Codec::mimeName() const {
  return "ISO-8859-16";
  }


const FXchar* const* FX885916Codec::aliases() const {
  static const FXchar *const list[]={"iso8859-16","ISO-8859-16","ISO_8859-16","latin10","iso-ir-226","l10",NULL};
  return list;
  }

}

