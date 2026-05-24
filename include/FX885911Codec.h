#ifndef FX885911CODEC_H
#define FX885911CODEC_H

#ifndef FXTEXTCODEC_H
#include "FXTextCodec.h"
#endif

namespace FX {

/// ISO-8859-11 Codec
class FXAPI FX885911Codec : public FXTextCodec {
  FXDECLARE(FX885911Codec)
public:
  FX885911Codec(){}
  virtual FXint mb2wc(FXwchar& wc,const FXchar* src,FXint nsrc) const override;
  virtual FXint wc2mb(FXchar* dst,FXint ndst,FXwchar wc) const override;
  virtual FXint mibEnum() const override;
  virtual const FXchar* name() const override;
  virtual const FXchar* mimeName() const override;
  virtual const FXchar* const* aliases() const override;
  virtual ~FX885911Codec(){}
  };

}

#endif
