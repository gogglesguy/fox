#ifndef FXCP860CODEC_H
#define FXCP860CODEC_H

#ifndef FXTEXTCODEC_H
#include "FXTextCodec.h"
#endif

namespace FX {

/// CP860 Codec
class FXAPI FXCP860Codec : public FXTextCodec {
  FXDECLARE(FXCP860Codec)
public:
  FXCP860Codec(){}
  virtual FXint mb2wc(FXwchar& wc,const FXchar* src,FXint nsrc) const override;
  virtual FXint wc2mb(FXchar* dst,FXint ndst,FXwchar wc) const override;
  virtual FXint mibEnum() const override;
  virtual const FXchar* name() const override;
  virtual const FXchar* mimeName() const override;
  virtual const FXchar* const* aliases() const override;
  virtual ~FXCP860Codec(){}
  };

}

#endif
