/********************************************************************************
*                                                                               *
*                          E v e n t   D i s p a t c h e r                      *
*                                                                               *
*********************************************************************************
* Copyright (C) 2019,2026 by Jeroen van der Zijp.   All Rights Reserved.        *
********************************************************************************/
#ifndef FXEVENTDISPATCHER_H
#define FXEVENTDISPATCHER_H

#ifndef FXDISPATCHER_H
#include "FXDispatcher.h"
#endif

namespace FX {


/**
* A FXEventDispatcher extends FXDispatcher, adding graphical user interface
* handling and other display-related events.
*/
class FXAPI FXEventDispatcher : public FXDispatcher {
private:
  FXptr         display;        // Display
private:
  FXEventDispatcher(const FXEventDispatcher&);
  FXEventDispatcher &operator=(const FXEventDispatcher&);
public:

  /// Event callback when GUI has activity
  typedef FXCallback<FXbool(FXEventDispatcher*,FXRawEvent& event)> EventCallback;

public:

  /// Construct event dispatcher object.
  FXEventDispatcher();

  /// Initialize dispatcher with display.
  virtual FXbool init(FXptr dpy);

  /// Initialize dispatcher without display.
  virtual FXbool init() override;

  /// Return display pointer
  FXptr getDisplay() const { return display; }

  /// Dispatch if something happens within given blocking time.
  /// Flags control subsets of events to be dispatched (signals, timers,
  /// idle, and more). Default is to dispatch all events.
  virtual FXbool dispatch(FXTime blocking=forever,FXuint flags=DispatchAll) override;

  /// Dispatch platform-dependent event
  virtual FXbool dispatchEvent(FXRawEvent& event);

  /// Exit dispatcher.
  virtual FXbool exit() override;

  /// Destroy event dispatcher object.
  virtual ~FXEventDispatcher();
  };

}

#endif
