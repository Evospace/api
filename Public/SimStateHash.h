// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Hash/xxhash.h"
#include "Qr/Vector.h"

class UDimensionRuntime;
class UInventoryReader;

struct FSimHashWriter {
  FXxHash64Builder Builder;

  FORCEINLINE void AppendBytes(const void *Data, uint64 Size) { Builder.Update(Data, Size); }

  template <typename T>
  FORCEINLINE void Append(const T &Value) {
    static_assert(std::is_trivially_copyable_v<T>, "hash input must be raw value bits");
    static_assert(!std::is_pointer_v<T>, "pointers are not stable across runs; hash a stable id instead");
    Builder.Update(&Value, sizeof(T));
  }

  FORCEINLINE void Append(const FQrVector3i &V) {
    Append(V.X);
    Append(V.Y);
    Append(V.Z);
  }

  void AppendString(const FString &S);
  void AppendPlainName(const FName &N);
  void AppendObjectName(const UObject *Object);
  void AppendInventory(const UInventoryReader *Inventory);

  FORCEINLINE uint64 Finalize() { return Builder.Finalize().Hash; }
};

struct FSimStateHashRecord {
  int64 Tick = 0;
  uint64 World = 0;
  uint64 Blocks = 0;
  uint64 Networks = 0;
  uint64 Trains = 0;
};

struct FSimStateHashBlockTrace {
  int64 Tick = 0;
  FQrVector3i Pos = FQrVector3i(0);
  FName Class;
  uint64 Hash = 0;
};

class FSimStateHash {
  public:
  static FORCEINLINE bool IsEnabled() { return GEnabled; }

  static void Begin(int32 Period);
  static void End();

  static void SetTraceBlocks(bool bEnabled) { GTraceBlocks = bEnabled; }
  static bool IsTracingBlocks() { return GTraceBlocks; }
  static TArray<FSimStateHashBlockTrace> &GetBlockTraces() { return BlockTraces; }
  static bool WriteBlockTracesJson(const FString &Path);

  static void OnSubstepEnd(const UDimensionRuntime &Runtime);

  static const TArray<FSimStateHashRecord> &GetRecords() { return Records; }
  static uint64 FinalCombined();
  static bool WriteRecordsJson(const FString &Path);

  static uint64 StableObjectKey(const UObject *Object);

  private:
  static bool GEnabled;
  static bool GTraceBlocks;
  static int32 HashPeriod;
  static int64 SubstepCounter;
  static TArray<FSimStateHashRecord> Records;
  static TArray<FSimStateHashBlockTrace> BlockTraces;
};
