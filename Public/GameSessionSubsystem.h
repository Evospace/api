// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Public/GameSessionData.h"
#include "GameSessionSubsystem.generated.h"

UCLASS()
class UGameSessionSubsystem : public UGameInstanceSubsystem {
  GENERATED_BODY()

  public:
  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  UGameSessionData *Data = nullptr;

  UFUNCTION(BlueprintCallable)
  void ReloadData(const FString &saveName);

  virtual void Initialize(FSubsystemCollectionBase &Collection) override;
  virtual void Deinitialize() override {}
};


