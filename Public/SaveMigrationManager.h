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
  // Applies hardcoded migrators whose target version is less than the save's version
  UFUNCTION(BlueprintCallable)
  static void RunMigrationsIfNeeded(const FString &saveName, UGameInstance *GameInstance);
};
