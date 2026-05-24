#ifndef FXCP1253CODEC_H
#define FXCP1253CODEC_H

#ifndef FXTEXTCODEC_H
#include "FXTextCodec.h"
#endif

namespace FX {

/// CP1253 Codec
class FXAPI FXCP1253Codec : public FXTextCodec {
  FXDECLARE(FXCP1253Codec)
public:
  FXCP1253Codec(){}
  virtual FXint mb2wc(FXwchar& wc,const FXchar* src,FXint nsrc) const override;
  virtual FXint wc2mb(FXchar* dst,FXint ndst,FXwchar wc) const override;
  virtual FXint mibEnum() const override;
  virtual const FXchar* name() const override;
  virtual const FXchar* mimeName() const override;
  virtual const FXchar* const* aliases() const override;
  virtual ~FXCP1253Codec(){}
  };

}

#endif
