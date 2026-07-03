// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "UObject/WeakObjectPtrTemplates.h"

// The single mechanism for holding a reference into another block: a weak pointer
// validated at the point of use via T::IsInWorld() (UBlockLogic checks its own flag,
// UAccessor checks its owner block). The flag is flipped exclusively by
// UDimensionRuntime::SetBlockLogic. There is no event-driven invalidation and no
// removal hook for reference safety; on a null Get() the caller skips this tick or
// re-resolves by position.
template <class T>
struct TBlockRef {
  TBlockRef() = default;
  TBlockRef(T *Object) : Ptr(Object) {}

  TBlockRef &operator=(T *Object) {
    Ptr = Object;
    return *this;
  }

  // nullptr if the object is dead or its block is no longer in the world.
  T *Get() const {
    T *Object = Ptr.Get();
    return (Object && Object->IsInWorld()) ? Object : nullptr;
  }

  void Reset() { Ptr.Reset(); }

  private:
  TWeakObjectPtr<T> Ptr;
};
