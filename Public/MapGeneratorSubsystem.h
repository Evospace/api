// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include <Subsystems/GameInstanceSubsystem.h>
#include "MapGeneratorSettings.h"
#include "MapGeneratorSubsystem.generated.h"

class UWorldGenerator;
class UGameSessionData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMapSettingsChanged);

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
  FMapGeneratorSettings GetMapSettings() const { return MapSettings; }

  UFUNCTION(BlueprintCallable)
  void SetMapSettings(const FMapGeneratorSettings &InSettings);

  UFUNCTION(BlueprintCallable)
  void SetMapSettingsWithNotify(const FMapGeneratorSettings &InSettings, bool bNotifyListeners);

  UFUNCTION(BlueprintCallable)
  void NotifyMapSettingsChanged();

  UFUNCTION(BlueprintCallable)
  void CommitMapSettings();

  UPROPERTY(BlueprintAssignable)
  FOnMapSettingsChanged OnMapSettingsChanged;

  private:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Map", meta = (AllowPrivateAccess = "true"))
  FMapGeneratorSettings MapSettings;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  TArray<UWorldGenerator *> WorldGenerators;

  void ApplyMapSettingsToGenerators();

  UFUNCTION()
  void UpdateSeed_Internal(UGameSessionData *GameSessionData);
};
