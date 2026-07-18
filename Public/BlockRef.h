// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "UObject/WeakObjectPtrTemplates.h"

template <class T>
struct TBlockRef {
  TBlockRef() = default;
  TBlockRef(T *Object) : Ptr(Object) {}

  TBlockRef &operator=(T *Object) {
    Ptr = Object;
    return *this;
  }

  T *Get() const {
    T *Object = Ptr.Get();
    return (Object && Object->IsInWorld()) ? Object : nullptr;
  }

  void Reset() { Ptr.Reset(); }

  private:
  TWeakObjectPtr<T> Ptr;
};
