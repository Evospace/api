// Copyright (c) 2017 - 2026, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Evospace/World/BlockDensity.h"
#include "Qr/CoordinameMinimal.h"

#include <mutex>

class UStaticBlock;

struct FPendingCellWrite {
  const UStaticBlock *Block = nullptr;
  BlockDensity Density{};
};

class EVOSPACE_API FColumnCellEditQueue {
  public:
  void Enqueue(const FQrVector3i &ColumnPos, const FQrVector3i &CellPos, const FPendingCellWrite &Write) {
    std::lock_guard<std::mutex> lock(Mutex);
    Columns.FindOrAdd(ColumnPos).Add(CellPos, Write);
  }

  bool TakeColumn(const FQrVector3i &ColumnPos, TMap<FQrVector3i, FPendingCellWrite> &Out) {
    std::lock_guard<std::mutex> lock(Mutex);
    return Columns.RemoveAndCopyValue(ColumnPos, Out);
  }

  bool HasColumn(const FQrVector3i &ColumnPos) const {
    std::lock_guard<std::mutex> lock(Mutex);
    return Columns.Contains(ColumnPos);
  }

  bool IsEmpty() const {
    std::lock_guard<std::mutex> lock(Mutex);
    return Columns.IsEmpty();
  }

  private:
  mutable std::mutex Mutex;
  TMap<FQrVector3i, TMap<FQrVector3i, FPendingCellWrite>> Columns;
};
