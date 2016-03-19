/********************************************************************************
*                                                                               *
*                            W o r k e r   T h r e a d                          *
*                                                                               *
*********************************************************************************
* Copyright (C) 2006,2013 by Jeroen van der Zijp.   All Rights Reserved.        *
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
********************************************************************************/
#ifndef FXWORKER_H
#define FXWORKER_H

#ifndef FXTHREAD_H
#include "FXThread.h"
#endif

namespace FX {


/**
* A Worker is a thread that performs a Runnable.
* When the worker thread completes the execution of the runnable, the
* worker thread and its memory are automatically reclaimed.
* The runnable itself is not deleted by the worker; it will thus outlive the
* worker that runs it.
* Exceptions raised by the runnable cause early termination of the
* runnable, and are caught by the worker.
*/
class FXAPI FXWorker : public FXThread {
private:
  FXRunnable *runnable;
private:
  FXWorker(const FXWorker&);
  FXWorker &operator=(const FXWorker&);
public:

  /// Create worker for runnable
  FXWorker(FXRunnable* rn=NULL);

  /// Change runnable if not started yet
  void setRunnable(FXRunnable* rn){ runnable=rn; }

  /// Return runnable
  FXRunnable* getRunnable() const { return runnable; }

  /// Run worker
  virtual FXint run();

  /// Create and start a worker on a given runnable.
  static FXWorker* execute(FXRunnable* rn,FXuval stacksize=0);

  /// Destroy
  virtual ~FXWorker();
  };

}

#endif
