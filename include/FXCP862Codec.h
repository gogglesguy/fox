#ifndef FXCP862CODEC_H
#define FXCP862CODEC_H

#ifndef FXTEXTCODEC_H
#include "FXTextCodec.h"
#endif

namespace FX {

/// CP862 Codec
class FXAPI FXCP862Codec : public FXTextCodec {
  FXDECLARE(FXCP862Codec)
public:
  FXCP862Codec(){}
  virtual FXint mb2wc(FXwchar& wc,const FXchar* src,FXint nsrc) const override;
  virtual FXint wc2mb(FXchar* dst,FXint ndst,FXwchar wc) const override;
  virtual FXint mibEnum() const override;
  virtual const FXchar* name() const override;
  virtual const FXchar* mimeName() const override;
  virtual const FXchar* const* aliases() const override;
  virtual ~FXCP862Codec(){}
  };

}

#endif
