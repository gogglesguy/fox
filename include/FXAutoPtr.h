/********************************************************************************
*                                                                               *
*                      A u t o m a t i c   P o i n t e r                        *
*                                                                               *
*********************************************************************************
* Copyright (C) 2007,2010 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#ifndef FXAUTOPTR_H
#define FXAUTOPTR_H

namespace FX {


/// Automatic pointer
template <class EType> class FXAutoPtr {
private:
  EType* ptr;
public:

  /// Construct from optional pointer
  FXAutoPtr(EType* src=NULL):ptr(src){ }

  /// Construct from another automatic pointer
  FXAutoPtr(FXAutoPtr& src):ptr(src.release()){ }

  /// Construct from another automatic pointer of compatible type
  template <class T> FXAutoPtr(FXAutoPtr<T>& src):ptr(src.release()){ }

  /// Assign from pointer
  FXAutoPtr& operator=(EType *src){ ptr=src; return *this; }

  /// Assign from an another automatic pointer
  FXAutoPtr& operator=(FXAutoPtr& src){ return reset(src.release()); }

  /// Assign from an automatic pointer with compatible type
  template <class T> FXAutoPtr& operator=(FXAutoPtr<T>& src){ return reset(src.release()); }

  /// Convert to true/false
  operator FXbool() const { return !!ptr; }

  /// Conversion operators
  operator EType*() const { return ptr; }

  /// Dereference operator
  EType& operator*() const { return *ptr; }

  /// Follow pointer operator
  EType* operator->() const { return ptr; }

  /// Release hold on the pointer
  EType* release(){ EType* tmp=ptr; ptr=NULL; return tmp; }

  /// Delete old object, replace by new, if any
  FXAutoPtr& reset(EType* p=NULL){ if(p!=ptr){ delete ptr; ptr=p; } return *this; }

  /// Destruction deletes pointer
  ~FXAutoPtr(){ delete ptr; }
  };


/// Serialize of automatic pointer
template <class EType> FXStream& operator<<(FXStream& store,const FXAutoPtr<EType>& obj){
  EType *temp=obj; store << temp; return store;
  }


/// Deserialize of automatic pointer
template <class EType> FXStream& operator>>(FXStream& store,FXAutoPtr<EType>& obj){
  EType *temp; store >> temp; obj=temp; return store;
  }

}

#endif

