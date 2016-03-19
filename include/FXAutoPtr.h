/********************************************************************************
*                                                                               *
*                    A u t o m a t i c   P o i n t e r                          *
*                                                                               *
*********************************************************************************
* Copyright (C) 2007 by Jeroen van der Zijp.   All Rights Reserved.             *
*********************************************************************************
* This library is free software; you can redistribute it and/or                 *
* modify it under the terms of the GNU Lesser General Public                    *
* License as published by the Free Software Foundation; either                  *
* version 2.1 of the License, or (at your option) any later version.            *
*                                                                               *
* This library is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             *
* Lesser General Public License for more details.                               *
*                                                                               *
* You should have received a copy of the GNU Lesser General Public              *
* License along with this library; if not, write to the Free Software           *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.    *
*********************************************************************************
* $Id: FXAutoPtr.h,v 1.7 2007/04/22 18:42:09 fox Exp $                          *
********************************************************************************/
#ifndef FXAUTOPTR_H
#define FXAUTOPTR_H

namespace FX {


/// Automatic pointer
template<class TYPE> class FXAutoPtr {
private:
  TYPE* ptr;
private:
  FXAutoPtr(const FXAutoPtr&);
  FXAutoPtr &operator=(const FXAutoPtr&);
public:

  /// Construct with optional pointer
  FXAutoPtr(TYPE* p=NULL):ptr(p){ }

  /// Assign from pointer
  FXAutoPtr& operator=(TYPE *p){ ptr=p; return *this; }

  /// Conversion operators
  operator TYPE*(){ return ptr; }
  operator const TYPE*() const { return ptr; }

  /// Dereference operator
  TYPE& operator*() const { return *ptr; }

  /// Follow pointer operator
  TYPE* operator->() const { return ptr; }

  /// Destruction deletes pointer
  ~FXAutoPtr(){ delete ptr; }
  };

}

#endif

