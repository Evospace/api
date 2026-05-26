// Copyright (c) 2017 - 2026, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Misc/Paths.h"

struct EVOSPACE_API FSavePathProvider {
  static FString GetSaveGamesRoot() {
    return FPaths::ProjectSavedDir() / TEXT("SaveGames");
  }

  static FString GetLocalSlotRoot(const FString &SlotName) {
    return GetSaveGamesRoot() / SlotName;
  }

  static const FString &GetStagingSlotName() {
    static const FString StagingSlotName = TEXT("temp");
    return StagingSlotName;
  }

  static FString GetStagingRoot() {
    return GetLocalSlotRoot(GetStagingSlotName());
  }

  static bool IsReservedSlotName(const FString &SlotName) {
    if (SlotName.Equals(GetStagingSlotName(), ESearchCase::IgnoreCase)) {
      return true;
    }
    if (SlotName.Equals(TEXT("net"), ESearchCase::IgnoreCase)) {
      return true;
    }
    for (int32 i = 1; i <= 5; ++i) {
      if (SlotName.Equals(FString::Printf(TEXT("Autosave %d"), i), ESearchCase::IgnoreCase)) {
        return true;
      }
    }
    return false;
  }
};
