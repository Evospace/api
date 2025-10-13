// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <Subsystems/GameInstanceSubsystem.h>
#include "MusicManagerSubsystem.generated.h"

class UAudioComponent;
class USoundBase;
class UGameSessionSubsystem;
class UMusicPlaylist;

UCLASS()
class UMusicManagerSubsystem : public UGameInstanceSubsystem {
  GENERATED_BODY()

  public:
  virtual void Initialize(FSubsystemCollectionBase &Collection) override;
  virtual void Deinitialize() override;

  UFUNCTION(BlueprintCallable)
  void SetMuffled(bool bInMuffled);

  UFUNCTION(BlueprintCallable)
  void EnsureAudioComponent();

  UFUNCTION(BlueprintCallable)
  void StartCrossfade(USoundBase *NewSound);

  UFUNCTION(BlueprintCallable)
  void StartRandomTrack();

  UFUNCTION(BlueprintCallable)
  void SetPlaylist(UMusicPlaylist *NewPlaylist);
  

  private:
  // MetaSound used as a music player graph. The actual track is injected via a "Wave" parameter.
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Music", meta = (AllowPrivateAccess = "true"))
  USoundBase *MusicMetaSoundSource = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Music", meta = (AllowPrivateAccess = "true"))
  UMusicPlaylist *Playlist = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  UAudioComponent *AudioComponentA = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  UAudioComponent *AudioComponentB = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  float CrossfadeTime = 3.0f;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  float NextTrackDelay = 70.0f;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  bool bUseAAsActive = true;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  float AudioComponentADuration = 0.0f;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  float AudioComponentBDuration = 0.0f;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  bool bMuffled = false;

  UFUNCTION()
  void ResetTimers();

  UFUNCTION()
  void HandleMenuMuffling(bool bInMuffled);

  void RegisterAudioToWorld(UWorld *NewWorld);
  
  UFUNCTION()
  void OnPostLoadMap(UWorld *NewWorld);

  void OnWorldCleanup(UWorld *World, bool bSessionEnded, bool bCleanupResources);
  

  UAudioComponent *GetActiveComponent() const;
  UAudioComponent *GetInactiveComponent() const;
  void ScheduleNextTimer(float DurationSeconds);
  
  UFUNCTION()
  void OnNextTrackTimer();

  FTimerHandle NextTrackTimerHandle;
};
