// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include <Subsystems/GameInstanceSubsystem.h>
#include "CarveSettings.h"
#include "MapGeneratorSubsystem.generated.h"

class UWorldGenerator;
class UGameSessionData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCarveSettingsChanged);

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

  UFUNCTION(BlueprintCallable, BlueprintPure)
  FCarveNoiseSettings GetCarveSettings() const { return CarveSettings; }

  UFUNCTION(BlueprintCallable)
  void SetCarveSettings(const FCarveNoiseSettings &InSettings);

  UFUNCTION(BlueprintCallable)
  void CommitCarveSettings();

  UPROPERTY(BlueprintAssignable)
  FOnCarveSettingsChanged OnCarveSettingsChanged;

  private:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Caves", meta = (AllowPrivateAccess = "true"))
  FCarveNoiseSettings CarveSettings;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  TArray<UWorldGenerator *> WorldGenerators;

  void ApplyCarveSettingsToBiomeGenerators();

  UFUNCTION()
  void UpdateSeed_Internal(UGameSessionData *GameSessionData);
};