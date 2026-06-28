// Copyright (c) 2017 - 2026, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"

struct EVOSPACE_API FSavePathProvider {
  // Writable root for this process. Defaults to the project's Saved dir; a `-SaveRoot=<abs>`
  // command-line override redirects it. That lets two instances on one machine (LAN co-op
  // testing / dogfooding) keep separate SaveGames / temp staging / autosaves instead of racing
  // over the same tree. Resolved once — the command line is fixed for the process lifetime.
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
