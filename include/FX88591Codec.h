#ifndef FX88591CODEC_H
#define FX88591CODEC_H

#ifndef FXTEXTCODEC_H
#include "FXTextCodec.h"
#endif

namespace FX {

/// ISO-8859-1 Codec
class FXAPI FX88591Codec : public FXTextCodec {
  FXDECLARE(FX88591Codec)
public:
  FX88591Codec(){}
  virtual FXint mb2wc(FXwchar& wc,const FXchar* src,FXint nsrc) const override;
  virtual FXint wc2mb(FXchar* dst,FXint ndst,FXwchar wc) const override;
  virtual FXint mibEnum() const override;
  virtual const FXchar* name() const override;
  virtual const FXchar* mimeName() const override;
  virtual const FXchar* const* aliases() const override;
  virtual ~FX88591Codec(){}
  };

}

#endif
