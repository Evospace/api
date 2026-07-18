// Copyright (c) 2017 - 2026, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"

struct EVOSPACE_API FSavePathProvider {
  static const FString &GetWritableRoot() {
    static const FString Root = []() -> FString {
      FString Override;
      if (FParse::Value(FCommandLine::Get(), TEXT("SaveRoot="), Override) && !Override.IsEmpty()) {
        return FPaths::ConvertRelativePathToFull(Override);
      }
      return FPaths::ProjectSavedDir();
    }();
    return Root;
  }

  static FString GetSaveGamesRoot() {
    return GetWritableRoot() / TEXT("SaveGames");
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

  static FString GetPlayerProfilePath(const FString &SaveRoot, const FString &StableId) {
    return SaveRoot / TEXT("Players") / (StableId + TEXT(".json"));
  }

  static const FString &GetHostProfileId() {
    static const FString Id = TEXT("host");
    return Id;
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
