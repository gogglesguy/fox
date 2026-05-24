#ifndef FXCP861CODEC_H
#define FXCP861CODEC_H

#ifndef FXTEXTCODEC_H
#include "FXTextCodec.h"
#endif

namespace FX {

/// CP861 Codec
class FXAPI FXCP861Codec : public FXTextCodec {
  FXDECLARE(FXCP861Codec)
public:
  FXCP861Codec(){}
  virtual FXint mb2wc(FXwchar& wc,const FXchar* src,FXint nsrc) const override;
  virtual FXint wc2mb(FXchar* dst,FXint ndst,FXwchar wc) const override;
  virtual FXint mibEnum() const override;
  virtual const FXchar* name() const override;
  virtual const FXchar* mimeName() const override;
  virtual const FXchar* const* aliases() const override;
  virtual ~FXCP861Codec(){}
  };

}

#endif
