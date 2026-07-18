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

  // Per-player profile inside a save root. One file per player who ever entered the
  // world, keyed by the stable player id (see PlayerIdentity::GetStableId).
  static FString GetPlayerProfilePath(const FString &SaveRoot, const FString &StableId) {
    return SaveRoot / TEXT("Players") / (StableId + TEXT(".json"));
  }

  // The local player is always the host of their own single-player / hosted world, so the
  // host's own profile uses a fixed key instead of the machine/Steam-dependent stable id:
  // a changing stable id (config reset, new machine, Steam up vs down) must never orphan
  // the host's profile. Only remote guests key by their real stable id (see
  // UNetSessionSubsystem::GetLocalProfileId).
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
