// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include <Subsystems/GameInstanceSubsystem.h>
#include "MapGeneratorSubsystem.generated.h"

class UWorldGenerator;
class UGameSessionData;

UCLASS()
class UMapGeneratorSubsystem : public UGameInstanceSubsystem {
  GENERATED_BODY()

  public:
  virtual void Initialize(FSubsystemCollectionBase &Collection) override;
  virtual void Deinitialize() override;

  UFUNCTION(BlueprintCallable)
  UWorldGenerator *FindWorldGenerator(FName name) const;

  UFUNCTION(BlueprintCallable)
  TArray<UWorldGenerator *> GetWorldGeneratorList();

  UFUNCTION(BlueprintCallable)
  void InitializeWorldGenerators();

  private:
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  TArray<UWorldGenerator *> WorldGenerators;

  void UpdateSeed_Internal(UGameSessionData *GameSessionData);
};