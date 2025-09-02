// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#include "Public/LogicSignal.h"
#include "Qr/JsonHelperCommon.h"
#include "Public/ItemData.h"
#include "Templates/TypeHash.h"

void ULogicSignal::EnsureExportFlagsInitialized() {
  const int32 n = ExportSignals.Num();
  if (ExportEnabled.Num() != n) {
    ExportEnabled.SetNum(n);
    for (int32 i = 0; i < n; ++i) {
      const ULogicExportOption *opt = ExportSignals[i];
      ExportEnabled[i] = (opt && opt->bEnabled);
    }
  }
}

bool ULogicSignal::IsExportEnabled(int32 Index) const {
  if (!ExportSignals.IsValidIndex(Index) || ExportSignals[Index] == nullptr) return false;
  if (ExportEnabled.IsValidIndex(Index)) return ExportEnabled[Index];
  // fallback to option default if flags not initialized
  return ExportSignals[Index]->bEnabled;
}

void ULogicSignal::SetExportEnabled(int32 Index, bool bEnabled) {
  if (!ExportSignals.IsValidIndex(Index)) return;
  if (!ExportEnabled.IsValidIndex(Index)) {
    ExportEnabled.SetNum(ExportSignals.Num());
  }
  ExportEnabled[Index] = bEnabled;
}

bool ULogicSignal::SerializeJson(TSharedPtr<FJsonObject> json) {
  json_helper::TrySet(json, TEXT("Exp"), ExportEnabled);
  return true;
}

bool ULogicSignal::IsModified() const {
  if (!ensure(ExportSignals.Num() == ExportEnabled.Num()))
    return true;

  for (int32 i = 0; i < ExportSignals.Num(); ++i) {
    if (!ExportSignals[i]) continue;
    if (ExportSignals[i]->bEnabled != ExportEnabled[i])
      return true;
  }
  return false;
}

bool ULogicSignal::DeserializeJson(TSharedPtr<FJsonObject> json) {
  json_helper::TryGet(json, TEXT("Exp"), ExportEnabled);
  EnsureExportFlagsInitialized();
  return true;
}

// Performance optimization implementations

int32 ULogicSignal::FInventoryCache::CalculateInventoryHash(const TArray<FItemData> &Slots) const {
  int32 Hash = 0;
  for (const FItemData &Slot : Slots) {
    // Combine item pointer and value into hash
    Hash = HashCombine(Hash, GetTypeHash(Slot.mItem));
    Hash = HashCombine(Hash, GetTypeHash(Slot.mValue));
  }
  return Hash;
}

bool ULogicSignal::FInventoryCache::HasInventoryChanged(const TArray<FItemData> &Slots) const {
  if (!bIsValid) return true;
  return CalculateInventoryHash(Slots) != LastInventoryHash;
}

void ULogicSignal::FInventoryCache::UpdateCache(const TArray<FItemData> &Slots) {
  CachedSlots = Slots;
  LastInventoryHash = CalculateInventoryHash(Slots);
  bIsValid = true;
}

void ULogicSignal::FInventoryCache::InvalidateCache() {
  bIsValid = false;
  CachedSlots.Empty();
  LastInventoryHash = 0;
}

const TArray<FItemData> &ULogicSignal::GetCachedInventoryData(int32 ExportIndex, const TArray<FItemData> &CurrentSlots) const {
  // Ensure cache array is properly sized
  if (ExportCaches.Num() <= ExportIndex) {
    ExportCaches.SetNum(ExportIndex + 1);
  }

  FInventoryCache &Cache = ExportCaches[ExportIndex];

  // Check if we can use cached data
  if (!Cache.HasInventoryChanged(CurrentSlots)) {
    return Cache.CachedSlots;
  }

  // Update cache with new data
  Cache.UpdateCache(CurrentSlots);
  return Cache.CachedSlots;
}

void ULogicSignal::InvalidateAllCaches() {
  for (FInventoryCache &Cache : ExportCaches) {
    Cache.InvalidateCache();
  }
}
