// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Public/GameSessionData.h"

#include "SaveMigrationManager.generated.h"

UCLASS()
class USaveMigrationManager : public UObject {
  GENERATED_BODY()

  public:
  // Runs migrators on SaveGames/temp after ResolveAndStage/CopySaveToTemp. DisplaySaveName is for logs only.
  UFUNCTION(BlueprintCallable)
  static void RunMigrationsIfNeeded(const FString &DisplaySaveName, UGameInstance *GameInstance);

  static void RunMigrationsAtRoot(const FString &RootPath, const FString &DisplayName, UGameInstance *GameInstance);

  // Replace block names in Logic.bin files (safe operation, ignores sector .bin files)
  UFUNCTION(BlueprintCallable)
  static bool ReplaceBlocksInLogicJson(const FString &saveName, const FString &oldBlockName, const FString &newBlockName);

  static bool ReplaceBlocksInLogicJsonAtRoot(const FString &RootPath, const FString &DisplayName,
                                             const FString &oldBlockName, const FString &newBlockName);

  // Replace multiple old block names with one new block name in a single pass
  static bool ReplaceBlocksInLogicJson(const FString &saveName, const TArray<FString> &oldBlockNames, const FString &newBlockName);

  static bool ReplaceBlocksInLogicJsonAtRoot(const FString &RootPath, const FString &DisplayName,
                                             const TArray<FString> &oldBlockNames, const FString &newBlockName);

  // Remove "SingleStaticBlock" from StaticBlock values in Logic.bin files
  UFUNCTION(BlueprintCallable)
  static bool RemoveSingleStaticBlocksFromLogicJson(const FString &saveName);

  static bool RemoveSingleStaticBlocksFromLogicJsonAtRoot(const FString &RootPath, const FString &DisplayName);
};
