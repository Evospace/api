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

/**
 * Transient, thread-safe store of sector-cell writes waiting for an unloaded column.
 * Filled on the game thread; drained atomically either by the column cell-edit job or by a
 * streaming column load — whichever runs first on the single column IO worker thread, so the
 * two can never both consume the same cells. NOT a session-long delta store: entries live only
 * until the next job/load touches the column.
 */
class EVOSPACE_API FColumnCellEditQueue {
  public:
  /** Last write wins per cell. Game thread. */
  void Enqueue(const FQrVector3i &ColumnPos, const FQrVector3i &CellPos, const FPendingCellWrite &Write) {
    std::lock_guard<std::mutex> lock(Mutex);
    Columns.FindOrAdd(ColumnPos).Add(CellPos, Write);
  }

  /** Atomically removes and returns all pending cells for a column. Any thread. */
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
