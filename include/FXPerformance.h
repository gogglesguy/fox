/********************************************************************************
*                                                                               *
*                    P e r f o r m a n c e   C o u n t e r                      *
*                                                                               *
*********************************************************************************
* Copyright (C) 2009,2026 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#ifndef FXPERFORMANCE_H
#define FXPERFORMANCE_H

namespace FX {


// Performance measurement counter
class FXAPI FXCounter {
private:
  const FXchar *const name;
  volatile FXlong     minticks;
  volatile FXlong     maxticks;
  volatile FXlong     totticks;
  volatile FXlong     counter;
public:
  FXCounter(const FXchar *const nm);
  void tally(FXlong ticks);
 ~FXCounter();
  };


// Measure performance of code in scope
template<FXCounter& counter>
class FXPerformanceCounter {
private:
  volatile FXlong ticks;
public:
  FXPerformanceCounter(){ticks=FXThread::ticks();}
 ~FXPerformanceCounter(){counter.tally(FXThread::ticks()-ticks);}
  };



// Invoke counter if enabled
#if defined(PERFORMANCE_LOGGING)
#define PERFORMANCE_RECORDER(counter)    FXCounter perf##counter(#counter)
#define PERFORMANCE_COUNTER(counter)     FXPerformanceCounter< perf##counter > measure##counter
#else
#define PERFORMANCE_RECORDER(counter)
#define PERFORMANCE_COUNTER(counter)
#endif

}

#endif
