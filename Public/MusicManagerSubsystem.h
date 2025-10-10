// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <Subsystems/GameInstanceSubsystem.h>
#include "MusicManagerSubsystem.generated.h"

class UAudioComponent;
class USoundBase;
class UGameSessionSubsystem;

UCLASS()
class UMusicManagerSubsystem : public UGameInstanceSubsystem {
  GENERATED_BODY()

  public:
  virtual void Initialize(FSubsystemCollectionBase &Collection) override;
  virtual void Deinitialize() override;

  UFUNCTION(BlueprintCallable)
  void SetMusicSound(USoundBase *InSound);

  UFUNCTION(BlueprintCallable)
  void SetMuffled(bool bInMuffled);

  UFUNCTION(BlueprintCallable)
  void EnsureAudioComponent();

  private:
  UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  USoundBase *MusicSound = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  UAudioComponent *AudioComponent = nullptr;

  bool bMuffled = false;

  UFUNCTION()
  void HandleMenuMuffling(bool bInMuffled);

  void RegisterAudioToWorld(UWorld *NewWorld);
  void OnPostLoadMap(UWorld *NewWorld);
  void OnWorldCleanup(UWorld *World, bool bSessionEnded, bool bCleanupResources);
};
