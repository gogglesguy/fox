#ifndef FXCP866CODEC_H
#define FXCP866CODEC_H

#ifndef FXTEXTCODEC_H
#include "FXTextCodec.h"
#endif

namespace FX {

/// CP866 Codec
class FXAPI FXCP866Codec : public FXTextCodec {
  FXDECLARE(FXCP866Codec)
public:
  FXCP866Codec(){}
  virtual FXint mb2wc(FXwchar& wc,const FXchar* src,FXint nsrc) const override;
  virtual FXint wc2mb(FXchar* dst,FXint ndst,FXwchar wc) const override;
  virtual FXint mibEnum() const override;
  virtual const FXchar* name() const override;
  virtual const FXchar* mimeName() const override;
  virtual const FXchar* const* aliases() const override;
  virtual ~FXCP866Codec(){}
  };

}

#endif
