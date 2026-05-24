#ifndef FXCP869CODEC_H
#define FXCP869CODEC_H

#ifndef FXTEXTCODEC_H
#include "FXTextCodec.h"
#endif

namespace FX {

/// CP869 Codec
class FXAPI FXCP869Codec : public FXTextCodec {
  FXDECLARE(FXCP869Codec)
public:
  FXCP869Codec(){}
  virtual FXint mb2wc(FXwchar& wc,const FXchar* src,FXint nsrc) const override;
  virtual FXint wc2mb(FXchar* dst,FXint ndst,FXwchar wc) const override;
  virtual FXint mibEnum() const override;
  virtual const FXchar* name() const override;
  virtual const FXchar* mimeName() const override;
  virtual const FXchar* const* aliases() const override;
  virtual ~FXCP869Codec(){}
  };

}

#endif
