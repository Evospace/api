// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#include "MusicManagerSubsystem.h"

#include <Components/AudioComponent.h>
#include <Sound/SoundBase.h>
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Public/GameSessionSubsystem.h"
#include "Subsystems/SubsystemCollection.h"
#include "MusicPlaylist.h"
#include "TimerManager.h"
#include <Sound/SoundWave.h>

void UMusicManagerSubsystem::Initialize(FSubsystemCollectionBase &Collection) {
  Super::Initialize(Collection);

  auto session = Collection.InitializeDependency<UGameSessionSubsystem>();
  check(session);

  session->OnMenuMuffling.AddDynamic(this, &UMusicManagerSubsystem::HandleMenuMuffling);

  EnsureAudioComponent();
  RegisterAudioToWorld(GetWorld());
  FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UMusicManagerSubsystem::OnPostLoadMap);
  FWorldDelegates::OnWorldCleanup.AddUObject(this, &UMusicManagerSubsystem::OnWorldCleanup);

  auto music_playlist = NewObject<UMusicPlaylist>();
  const TArray<FString> music_paths = {
    TEXT("/Script/Engine.SoundWave'/Game/Music/evospace_main_menu.evospace_main_menu'"),
    TEXT("/Script/Engine.SoundWave'/Game/Music/Binary_Serenity.Binary_Serenity'"),
    TEXT("/Script/Engine.SoundWave'/Game/Music/Elven_Realm.Elven_Realm'"),
    TEXT("/Script/Engine.SoundWave'/Game/Music/7__classic_ОК.7__classic_ОК'"),
    TEXT("/Script/Engine.SoundWave'/Game/Music/Stellar_Serenity.Stellar_Serenity'"),
    TEXT("/Script/Engine.SoundWave'/Game/Music/DowntempoNeon.DowntempoNeon'"),
    TEXT("/Script/Engine.SoundWave'/Game/Music/Epic_Fantasy.Epic_Fantasy'"),
    TEXT("/Script/Engine.SoundWave'/Game/Music/Celestial_Machinery.Celestial_Machinery'"),
    TEXT("/Script/Engine.SoundWave'/Game/Music/11_blues_chill_.11_blues_chill_'"),
    TEXT("/Script/Engine.SoundWave'/Game/Music/Celestial_Serenity.Celestial_Serenity'"),
    TEXT("/Script/Engine.SoundWave'/Game/Music/16_Various.16_Various'"),
    TEXT("/Script/Engine.SoundWave'/Game/Music/Automatic_Rhythms.Automatic_Rhythms'"),
    TEXT("/Script/Engine.SoundWave'/Game/Music/Magical_Fantasy.Magical_Fantasy'"),
    TEXT("/Script/Engine.SoundWave'/Game/Music/Elven_Harmonies__no_percussion_.Elven_Harmonies__no_percussion_'"),
    TEXT("/Script/Engine.SoundWave'/Game/Music/Celestial_Machinations.Celestial_Machinations'"),
    TEXT("/Script/Engine.SoundWave'/Game/Music/Cosmic_Machinery.Cosmic_Machinery'"),
    TEXT("/Script/Engine.SoundWave'/Game/Music/Celestial_Synchronization.Celestial_Synchronization'"),
    TEXT("/Script/Engine.SoundWave'/Game/Music/Cosmic_Tranquility_Comb.Cosmic_Tranquility_Comb'"),
    TEXT("/Script/Engine.SoundWave'/Game/Music/21_2_amnbient_.21_2_amnbient_'")
  };

  for (const auto& path : music_paths) {
    auto wave = LoadObject<USoundBase>(nullptr, *path);
    if (ensure(wave)) {
      music_playlist->Tracks.Add(wave);
    }
  }

  SetPlaylist(music_playlist);

  MusicMetaSoundSource = LoadObject<USoundBase>(nullptr, TEXT("/Script/MetasoundEngine.MetaSoundSource'/Game/Sounds/MS_MusicPlayer.MS_MusicPlayer'"));
}

void UMusicManagerSubsystem::ResetTimers() {
  if (auto *World = GetWorld()) {
    World->GetTimerManager().ClearTimer(NextTrackTimerHandle);
  }
}

void UMusicManagerSubsystem::Deinitialize() {
  if (auto session = GetGameInstance()->GetSubsystem<UGameSessionSubsystem>()) {
    check(session);
    session->OnMenuMuffling.RemoveDynamic(this, &UMusicManagerSubsystem::HandleMenuMuffling);
  }

  ResetTimers();
  
  if (AudioComponentA) {
    if (AudioComponentA->IsRegistered()) {
      AudioComponentA->UnregisterComponent();
    }
    AudioComponentA->DestroyComponent();
    AudioComponentA = nullptr;
  }
  if (AudioComponentB) {
    if (AudioComponentB->IsRegistered()) {
      AudioComponentB->UnregisterComponent();
    }
    AudioComponentB->DestroyComponent();
    AudioComponentB = nullptr;
  }
  FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
  FWorldDelegates::OnWorldCleanup.RemoveAll(this);
  Super::Deinitialize();
}

void UMusicManagerSubsystem::SetMuffled(bool bInMuffled) {
  bMuffled = bInMuffled;
  if (AudioComponentA) {
    AudioComponentA->SetBoolParameter(TEXT("Muffled"), bMuffled);
  }
  if (AudioComponentB) {
    AudioComponentB->SetBoolParameter(TEXT("Muffled"), bMuffled);
  }
}

void UMusicManagerSubsystem::HandleMenuMuffling(bool bInMuffled) {
  SetMuffled(bInMuffled);
}

void UMusicManagerSubsystem::EnsureAudioComponent() {
  if (!AudioComponentA) {
    AudioComponentA = NewObject<UAudioComponent>(this, TEXT("Music_AudioComponent_A"), RF_Transient);
    if (ensure(AudioComponentA)) {
      AudioComponentA->bIsUISound = true;
      AudioComponentA->bAutoActivate = false;
      AudioComponentA->SetVolumeMultiplier(1.0f);
    }
  }
  if (!AudioComponentB) {
    AudioComponentB = NewObject<UAudioComponent>(this, TEXT("Music_AudioComponent_B"), RF_Transient);
    if (ensure(AudioComponentB)) {
      AudioComponentB->bIsUISound = true;
      AudioComponentB->bAutoActivate = false;
      AudioComponentB->SetVolumeMultiplier(1.0f);
    }
  }
}

void UMusicManagerSubsystem::RegisterAudioToWorld(UWorld *NewWorld) {
  check(NewWorld);

  if (AudioComponentA) {
    if (AudioComponentA->IsRegistered() && AudioComponentA->GetWorld() != NewWorld) {
      AudioComponentA->UnregisterComponent();
    }
    if (!AudioComponentA->IsRegistered()) {
      AudioComponentA->RegisterComponentWithWorld(NewWorld);
    }
    AudioComponentA->SetBoolParameter(TEXT("Muffled"), bMuffled);
  }

  if (AudioComponentB) {
    if (AudioComponentB->IsRegistered() && AudioComponentB->GetWorld() != NewWorld) {
      AudioComponentB->UnregisterComponent();
    }
    if (!AudioComponentB->IsRegistered()) {
      AudioComponentB->RegisterComponentWithWorld(NewWorld);
    }
    AudioComponentB->SetBoolParameter(TEXT("Muffled"), bMuffled);
  }

}

void UMusicManagerSubsystem::OnPostLoadMap(UWorld *NewWorld) {
  RegisterAudioToWorld(NewWorld);
  UAudioComponent *Active = GetActiveComponent();
  if (Active && Active->GetWorld() == NewWorld) {
    if (Active->GetSound()) {
      Active->Play(0.0f);
    }
    Active->SetBoolParameter(TEXT("Muffled"), bMuffled);
  }
  // Ensure inactive does not start automatically
  if (UAudioComponent *Inactive = GetInactiveComponent()) {
    if (Inactive->IsPlaying()) {
      Inactive->Stop();
    }
  }
}

void UMusicManagerSubsystem::OnWorldCleanup(UWorld *World, bool /*bSessionEnded*/, bool /*bCleanupResources*/) {
  check(World);
  if (AudioComponentA && AudioComponentA->IsRegistered() && AudioComponentA->GetWorld() == World) {
    AudioComponentA->Stop();
    AudioComponentA->UnregisterComponent();
  }
  if (AudioComponentB && AudioComponentB->IsRegistered() && AudioComponentB->GetWorld() == World) {
    AudioComponentB->Stop();
    AudioComponentB->UnregisterComponent();
  }
}

UAudioComponent *UMusicManagerSubsystem::GetActiveComponent() const {
  return bUseAAsActive ? AudioComponentA : AudioComponentB;
}

UAudioComponent *UMusicManagerSubsystem::GetInactiveComponent() const {
  return bUseAAsActive ? AudioComponentB : AudioComponentA;
}

void UMusicManagerSubsystem::StartRandomTrack() {
  if (!Playlist)
    return;
  if (USoundBase *Next = Playlist->GetRandomSound()) {
    StartCrossfade(Next);
  }
}

void UMusicManagerSubsystem::SetPlaylist(UMusicPlaylist *NewPlaylist) {
  Playlist = NewPlaylist;
}

// Wind crossfade removed; wind handled in MetaSound graph now

void UMusicManagerSubsystem::StartCrossfade(USoundBase *NewSound) {
  EnsureAudioComponent();
  ResetTimers();

  UAudioComponent *Active = GetActiveComponent();
  UAudioComponent *Inactive = GetInactiveComponent();
  check(Inactive);
  check(MusicMetaSoundSource);


  if (USoundWave *AsWave = Cast<USoundWave>(NewSound)) {
    Inactive->SetSound(MusicMetaSoundSource);
    Inactive->SetWaveParameter(TEXT("Wave"), AsWave);
    Inactive->SetBoolParameter(TEXT("Muffled"), bMuffled);
  } else {
    Inactive->SetSound(NewSound);
    Inactive->SetBoolParameter(TEXT("Muffled"), bMuffled);
  }

  // Capture and store duration at start
  const float Duration = NewSound ? NewSound->GetDuration() : 0.0f;
  if (Inactive == AudioComponentA) {
    AudioComponentADuration = Duration;
  } else {
    AudioComponentBDuration = Duration;
  }
  
  Inactive->Play(0.0f);
  Inactive->FadeIn(CrossfadeTime, 1.0f);
  if (Active) {
    Active->FadeOut(CrossfadeTime, 0.0f);
  }

  // Swap active flag so the new one becomes active
  bUseAAsActive = (Inactive == AudioComponentA);

  // Schedule next random track before the current one ends
  ScheduleNextTimer(Duration);
}

void UMusicManagerSubsystem::ScheduleNextTimer(float DurationSeconds) {
  if (DurationSeconds <= 0.0f)
    return;
  if (auto *World = GetWorld()) {
    const float TimeUntilNext = FMath::Max(0.01f, DurationSeconds - CrossfadeTime + NextTrackDelay);
    World->GetTimerManager().SetTimer(NextTrackTimerHandle, this, &UMusicManagerSubsystem::OnNextTrackTimer, TimeUntilNext, false);
  }
}

void UMusicManagerSubsystem::OnNextTrackTimer() {
  if (!Playlist)
    return;
  if (USoundBase *Next = Playlist->GetRandomSound()) {
    StartCrossfade(Next);
  }
}

// Wind timer removed; wind handled in MetaSound graph now
