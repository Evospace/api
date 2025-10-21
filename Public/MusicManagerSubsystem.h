// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <Subsystems/GameInstanceSubsystem.h>
#include "Tickable.h"
#include "MusicManagerSubsystem.generated.h"

class UAudioComponent;
class USoundBase;
class UGameSessionSubsystem;
class UMusicPlaylist;
class AMainPlayerController;
struct FReverbParameters;

UCLASS()
class UMusicManagerSubsystem : public UGameInstanceSubsystem, public FTickableGameObject {
  GENERATED_BODY()

  public:
  virtual void Initialize(FSubsystemCollectionBase &Collection) override;
  virtual void Deinitialize() override;

  // FTickableGameObject interface
  virtual void Tick(float DeltaTime) override;
  virtual TStatId GetStatId() const override;
  virtual bool IsTickable() const override { return true; }
  virtual bool IsTickableInEditor() const override { return false; }
  virtual bool IsTickableWhenPaused() const override { return false; }
  virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }

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
  float NextTrackDelay = 50.0f;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  bool bUseAAsActive = true;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  float AudioComponentADuration = 0.0f;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  float AudioComponentBDuration = 0.0f;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  bool bMuffled = false;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  float CurrentReverbAmount = 0.0f;

  UFUNCTION()
  void ResetTimers();

  // Update reverb from player controller's ambient tracing component
  void UpdateReverbFromAmbientTracing();

  // Apply all reverb parameters to an audio component
  void ApplyReverbParameters(UAudioComponent *AudioComponent, const FReverbParameters &Params);

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
