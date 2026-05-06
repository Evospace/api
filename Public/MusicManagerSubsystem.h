// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <Subsystems/GameInstanceSubsystem.h>
#include "Tickable.h"

#include "Sound/SoundSubmix.h"
#include "ThirdParty/luabridge/LuaBridge.h"

#include "MusicManagerSubsystem.generated.h"

struct lua_State;

class UAudioComponent;
class USoundBase;
class UGameSessionSubsystem;
class UMusicPlaylist;
class UWorld;

UCLASS()
class UMusicManagerSubsystem : public UGameInstanceSubsystem, public FTickableGameObject {
  GENERATED_BODY()

  using Self = UMusicManagerSubsystem;

  public:
  virtual void Initialize(FSubsystemCollectionBase &Collection) override;
  virtual void Deinitialize() override;

  // FTickableGameObject interface
  virtual void Tick(float DeltaTime) override;
  virtual TStatId GetStatId() const override;
  virtual bool IsTickable() const override { return true; }
  virtual bool IsTickableInEditor() const override { return false; }
  virtual bool IsTickableWhenPaused() const override { return false; }
  virtual UWorld *GetTickableGameObjectWorld() const override { return GetWorld(); }

  UFUNCTION(BlueprintCallable)
  void SetMuffled(bool bInMuffled);

  UFUNCTION(BlueprintCallable)
  void EnsureAudioComponent();

  UFUNCTION(BlueprintCallable)
  void StartCrossfade(USoundBase *NewSound);

  /** Same as StartCrossfade but uses CrossfadeSeconds for this transition (fade-in / fade-out and auto-advance boundary). */
  UFUNCTION(BlueprintCallable, meta = (ClampMin = "0"))
  void StartCrossfadeWithDuration(USoundBase *NewSound, float CrossfadeSeconds);

  UFUNCTION(BlueprintCallable)
  void StartRandomTrack();

  /** Like StartRandomTrack but uses CrossfadeSeconds for this transition only. */
  UFUNCTION(BlueprintCallable, meta = (ClampMin = "0"))
  void StartRandomTrackWithCrossfade(float CrossfadeSeconds);

  UFUNCTION(BlueprintCallable)
  void SetPlaylist(UMusicPlaylist *NewPlaylist);

  /** Stops automatic advance and fades out both music channels (crossfade owned here). */
  UFUNCTION(BlueprintCallable)
  void RequestSilence();

  /** Select built-in or registered playlist by key (e.g. default, cosmos). Returns false if unknown. */
  UFUNCTION(BlueprintCallable)
  bool SetPlaylistByKey(const FString &PlaylistKey);

  /** Play a track from the current playlist by zero-based index (Lua uses same indexing). */
  UFUNCTION(BlueprintCallable)
  void PlayTrackByIndex(int32 TrackIndex);

  /** Like PlayTrackByIndex; fade duration in seconds (also used after request_silence so music can ramp up from quiet). */
  UFUNCTION(BlueprintCallable, meta = (ClampMin = "0"))
  void PlayTrackByIndexWithCrossfade(int32 TrackIndex, float CrossfadeSeconds);

  int32 GetTrackCount() const;

  const FString &GetActivePlaylistKey() const { return ActivePlaylistKey; }

  /** Editor/CI: validates named playlists after subsystem init (requires game instance world). */
  bool ValidateBuiltinPlaylistsForSmokeTest() const;

  static bool RunBindingsSelfTest(UWorld *World);

  // direct:
  //--- Fade out both channels and stop automatic track advance.
  //function Music:request_silence() end
  //---
  //--- Select a named playlist (e.g. default, cosmos). Returns whether the key exists.
  //function Music:set_playlist(key) end
  //---
  //--- Play a track by zero-based index in the current playlist.
  //function Music:play_track(index) end
  //---
  //--- Play a track with explicit crossfade duration in seconds (0 = immediate).
  //function Music:play_track_crossfade(index, crossfade_seconds) end
  //---
  //--- Start a random track from the current playlist (crossfade handled in C++).
  //function Music:play_random() end
  //---
  //--- Start a random track with explicit crossfade duration in seconds.
  //function Music:play_random_crossfade(crossfade_seconds) end

  void lua_reg(lua_State *L) const {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UMusicManagerSubsystem, UObject>("Music") //@class Music : Object
      .addProperty(
        "playlist",
        [](const Self *S) -> std::string { return TCHAR_TO_UTF8(*S->GetActivePlaylistKey()); }) //@field string
      .addProperty("track_count", [](const Self *S) -> int32 { return S->GetTrackCount(); }) //@field integer
      .addFunction("request_silence", &Self::RequestSilence)
      .addFunction(
        "set_playlist",
        [](Self *S, const std::string &Key) -> bool {
          return S->SetPlaylistByKey(FString(UTF8_TO_TCHAR(Key.c_str())));
        })
      .addFunction("play_track", &Self::PlayTrackByIndex)
      .addFunction("play_track_crossfade", &Self::PlayTrackByIndexWithCrossfade)
      .addFunction("play_random", &Self::StartRandomTrack)
      .addFunction("play_random_crossfade", &Self::StartRandomTrackWithCrossfade)
      .endClass();
  }

  private:
  // MetaSound used as a music player graph. The actual track is injected via a "Wave" parameter.
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Music", meta = (AllowPrivateAccess = "true"))
  USoundBase *MusicMetaSoundSource = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Music", meta = (AllowPrivateAccess = "true"))
  UMusicPlaylist *Playlist = nullptr;

  /** Registered playlists addressable by Lua/C++ key (default, cosmos, …). */
  UPROPERTY()
  TMap<FString, TObjectPtr<UMusicPlaylist>> NamedPlaylists;

  /** Empty when SetPlaylist assigned a non-registered list. */
  UPROPERTY()
  FString ActivePlaylistKey;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  UAudioComponent *AudioComponentA = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  UAudioComponent *AudioComponentB = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  float CrossfadeTime = 3.0f;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  float WindTime = 50.0f;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  bool bUseAAsActive = true;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  float AudioComponentADuration = 0.0f;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  float AudioComponentBDuration = 0.0f;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  bool bMuffled = false;

  UPROPERTY(VisibleAnywhere)
  USoundSubmix *FactoryBus;

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
  void ScheduleNextTimer(float DurationSeconds, bool bAddWindTail, float CrossfadeForAdvance);

  UFUNCTION()
  void OnNextTrackTimer();

  FTimerHandle NextTrackTimerHandle;
};
