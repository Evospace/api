// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#include "Public/LogicSettings.h"
#include "Qr/JsonHelperCommon.h"
#include "Public/ItemData.h"
#include "Templates/TypeHash.h"

void ULogicSettings::EnsureExportFlagsInitialized() {
  const int32 n = ExportSignals.Num();
  if (ExportEnabled.Num() != n) {
    ExportEnabled.SetNum(n);
    for (int32 i = 0; i < n; ++i) {
      const ULogicExportOption *opt = ExportSignals[i];
      ExportEnabled[i] = (opt && opt->bEnabled);
    }
  }
}

bool ULogicSettings::IsExportEnabled(int32 Index) const {
  if (!ExportSignals.IsValidIndex(Index) || ExportSignals[Index] == nullptr)
    return false;
  if (ExportEnabled.IsValidIndex(Index))
    return ExportEnabled[Index];
  // fallback to option default if flags not initialized
  return ExportSignals[Index]->bEnabled;
}

void ULogicSettings::SetExportEnabled(int32 Index, bool bEnabled) {
  if (!ExportSignals.IsValidIndex(Index))
    return;
  if (!ExportEnabled.IsValidIndex(Index)) {
    ExportEnabled.SetNum(ExportSignals.Num());
  }
  ExportEnabled[Index] = bEnabled;
}

bool ULogicSettings::SerializeJson(TSharedPtr<FJsonObject> json) const {
  json_helper::TrySet(json, TEXT("Exp"), ExportEnabled);
  return true;
}

bool ULogicSettings::DeserializeJson(TSharedPtr<FJsonObject> json) {
  json_helper::TryGet(json, TEXT("Exp"), ExportEnabled);
  EnsureExportFlagsInitialized();
  return true;
}