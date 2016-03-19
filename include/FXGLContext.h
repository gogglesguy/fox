/********************************************************************************
*                                                                               *
*                     G L  R e n d e r i n g   C o n t e x t                    *
*                                                                               *
*********************************************************************************
* Copyright (C) 2000,2007 by Jeroen van der Zijp.   All Rights Reserved.        *
*********************************************************************************
* This library is free software; you can redistribute it and/or modify          *
* it under the terms of the GNU Lesser General Public License as published by   *
* the Free Software Foundation; either version 3 of the License, or             *
* (at your option) any later version.                                           *
*                                                                               *
* This library is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                 *
* GNU Lesser General Public License for more details.                           *
*                                                                               *
* You should have received a copy of the GNU Lesser General Public License      *
* along with this program.  If not, see <http://www.gnu.org/licenses/>          *
*********************************************************************************
* $Id: FXGLContext.h,v 1.36 2007/07/09 16:02:44 fox Exp $                       *
********************************************************************************/
#ifndef FXGLCONTEXT_H
#define FXGLCONTEXT_H

//////////////////////////////  UNDER DEVELOPMENT  //////////////////////////////

namespace FX {

class FXApp;
class FXDrawable;

/// OpenGL context
class FXAPI FXGLContext : public FXId {
private:
  FXDrawable  *drawable;        // To render on, if any
  FXGLContext *shared;          // Shared with other
  FXGLConfig   desired;         // Desired configuration
  FXGLConfig   actual;          // Actual onfiguration
  void*        format;          // Configuration format
private:
  FXGLContext(const FXGLContext&);
  FXGLContext &operator=(const FXGLContext&);
protected:
  FXGLContext();
public:

  /**
  * Construct an OpenGL context with default configuration properties;
  * it shares a display list with another context shr.
  */
  FXGLContext(FXApp *a,FXGLContext* shr=NULL);

  /**
  * Construct an OpenGL context with given configuration properties cfg;
  * it shares a display list with another context shr.
  */
  FXGLContext(FXApp *a,const FXGLConfig& cfg,FXGLContext* shr=NULL);

  /// Change configuration
  void setConfig(const FXGLConfig& cfg){ desired=cfg; }

  /// Get configuration
  const FXGLConfig& getConfig() const { return desired; }

  /// Get actual configuration
  const FXGLConfig& getActualConfig() const { return actual; }

  /// Get matched configuration format
  void* getFormat() const { return format; }

  /// Change share context prior to calling create()
  void setShared(FXGLContext *ctx){ shared=ctx; }

  /// Get share context
  FXGLContext* getShared() const { return shared; }

  /// Create context
  virtual void create();

  /// Detach context
  virtual void detach();

  /// Destroy context
  virtual void destroy();

  /// Has double buffering
  FXbool doubleBuffer() const { return actual.doubleBuffer(); }

  /// Has stereo buffering
  FXbool stereoBuffer() const { return actual.stereoBuffer(); }

  /// Is direct rendering context
  FXbool direct() const { return actual.direct(); }

  /// Make OpenGL context current prior to performing OpenGL commands
  FXbool begin(FXDrawable *draw);

  /// Make OpenGL context non current
  FXbool end();

  /// Swap front and back buffer
  void swapBuffers();

  /// Destructor
  virtual ~FXGLContext();
  };

}

#endif

