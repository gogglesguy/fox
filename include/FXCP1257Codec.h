#ifndef FXCP1257CODEC_H
#define FXCP1257CODEC_H

#ifndef FXTEXTCODEC_H
#include "FXTextCodec.h"
#endif

namespace FX {

/// CP1257 Codec
class FXAPI FXCP1257Codec : public FXTextCodec {
  FXDECLARE(FXCP1257Codec)
public:
  FXCP1257Codec(){}
  virtual FXint mb2wc(FXwchar& wc,const FXchar* src,FXint nsrc) const override;
  virtual FXint wc2mb(FXchar* dst,FXint ndst,FXwchar wc) const override;
  virtual FXint mibEnum() const override;
  virtual const FXchar* name() const override;
  virtual const FXchar* mimeName() const override;
  virtual const FXchar* const* aliases() const override;
  virtual ~FXCP1257Codec(){}
  };

}

#endif
