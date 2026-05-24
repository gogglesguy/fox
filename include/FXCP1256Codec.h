#ifndef FXCP1256CODEC_H
#define FXCP1256CODEC_H

#ifndef FXTEXTCODEC_H
#include "FXTextCodec.h"
#endif

namespace FX {

/// CP1256 Codec
class FXAPI FXCP1256Codec : public FXTextCodec {
  FXDECLARE(FXCP1256Codec)
public:
  FXCP1256Codec(){}
  virtual FXint mb2wc(FXwchar& wc,const FXchar* src,FXint nsrc) const override;
  virtual FXint wc2mb(FXchar* dst,FXint ndst,FXwchar wc) const override;
  virtual FXint mibEnum() const override;
  virtual const FXchar* name() const override;
  virtual const FXchar* mimeName() const override;
  virtual const FXchar* const* aliases() const override;
  virtual ~FXCP1256Codec(){}
  };

}

#endif
