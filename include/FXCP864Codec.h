#ifndef FXCP864CODEC_H
#define FXCP864CODEC_H

#ifndef FXTEXTCODEC_H
#include "FXTextCodec.h"
#endif

namespace FX {

/// CP864 Codec
class FXAPI FXCP864Codec : public FXTextCodec {
  FXDECLARE(FXCP864Codec)
public:
  FXCP864Codec(){}
  virtual FXint mb2wc(FXwchar& wc,const FXchar* src,FXint nsrc) const override;
  virtual FXint wc2mb(FXchar* dst,FXint ndst,FXwchar wc) const override;
  virtual FXint mibEnum() const override;
  virtual const FXchar* name() const override;
  virtual const FXchar* mimeName() const override;
  virtual const FXchar* const* aliases() const override;
  virtual ~FXCP864Codec(){}
  };

}

#endif
