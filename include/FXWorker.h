/********************************************************************************
*                                                                               *
*                            W o r k e r   T h r e a d                          *
*                                                                               *
*********************************************************************************
* Copyright (C) 2006,2012 by Jeroen van der Zijp.   All Rights Reserved.        *
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
* A Worker is a thread that runs a runnable.
* When the runnable is done, the worker terminates itself.
* Thus this facility allows a job to outlive the thread that
* computes it [the idea being that a thread may be far more
* resources-expensive than the job itself].
* Exceptions raised in the runnable are caught in the worker,
* so that the worker will properly terminate regarless how the
* runnable ended.
*/
class FXAPI FXWorker : public FXThread {
private:
  FXRunnable *runnable;
private:
  FXWorker(const FXWorker&);
  FXWorker &operator=(const FXWorker&);
public:

  /// Create worker for runnable
  FXWorker(FXRunnable* r=NULL);

  /// Run worker
  virtual FXint run();

  /// Destroy worker
  virtual ~FXWorker();
  };

}

#endif
