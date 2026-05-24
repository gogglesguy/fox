#ifndef FXCP1258CODEC_H
#define FXCP1258CODEC_H

#ifndef FXTEXTCODEC_H
#include "FXTextCodec.h"
#endif

namespace FX {

/// CP1258 Codec
class FXAPI FXCP1258Codec : public FXTextCodec {
  FXDECLARE(FXCP1258Codec)
public:
  FXCP1258Codec(){}
  virtual FXint mb2wc(FXwchar& wc,const FXchar* src,FXint nsrc) const override;
  virtual FXint wc2mb(FXchar* dst,FXint ndst,FXwchar wc) const override;
  virtual FXint mibEnum() const override;
  virtual const FXchar* name() const override;
  virtual const FXchar* mimeName() const override;
  virtual const FXchar* const* aliases() const override;
  virtual ~FXCP1258Codec(){}
  };

}

#endif
