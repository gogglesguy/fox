#ifndef FX885915CODEC_H
#define FX885915CODEC_H

#ifndef FXTEXTCODEC_H
#include "FXTextCodec.h"
#endif

namespace FX {

/// ISO-8859-15 Codec
class FXAPI FX885915Codec : public FXTextCodec {
  FXDECLARE(FX885915Codec)
public:
  FX885915Codec(){}
  virtual FXint mb2wc(FXwchar& wc,const FXchar* src,FXint nsrc) const override;
  virtual FXint wc2mb(FXchar* dst,FXint ndst,FXwchar wc) const override;
  virtual FXint mibEnum() const override;
  virtual const FXchar* name() const override;
  virtual const FXchar* mimeName() const override;
  virtual const FXchar* const* aliases() const override;
  virtual ~FX885915Codec(){}
  };

}

#endif
