// Copyright (c) 2017 - 2026, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Public/SaveSourceTypes.h"

#include "SaveManagerSubsystem.generated.h"

UCLASS()
class USaveManagerSubsystem : public UGameInstanceSubsystem {
  GENERATED_BODY()

  public:
  UFUNCTION(BlueprintCallable, Category = "Evospace|Save")
  bool StartGameFromSave(const FString &SaveName);

  UFUNCTION(BlueprintCallable, Category = "Evospace|Save")
  bool ResolveAndStage(const FSaveSourceDescriptor &Source, FPreparedSaveContext &OutContext);

  UFUNCTION(BlueprintCallable, Category = "Evospace|Save")
  bool StartGameFromSource(const FSaveSourceDescriptor &Source);

  UFUNCTION(BlueprintCallable, Category = "Evospace|Save")
  bool StartGameFromShippingBuiltIn(const FString &BuiltInPackageId);

  UFUNCTION(BlueprintCallable, Category = "Evospace|Save")
  bool StartGameFromWorkshopPath(const FString &PackagePath);

  UFUNCTION(BlueprintCallable, Category = "Evospace|Save")
  bool StartGameFromWorkshopId(int64 PublishedFileId);

  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Evospace|Save")
  const FPreparedSaveContext &GetLastPreparedSaveContext() const { return LastPreparedSaveContext; }

  private:
  UPROPERTY()
  FPreparedSaveContext LastPreparedSaveContext;
};
