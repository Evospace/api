// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#include "MusicManagerSubsystem.h"

#include <Components/AudioComponent.h>
#include <Sound/SoundBase.h>
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Public/GameSessionSubsystem.h"
#include "Subsystems/SubsystemCollection.h"
#include "MusicPlaylist.h"
#include "TimerManager.h"
#include <Sound/SoundWave.h>
#include "Evospace/Player/MainPlayerController.h"
#include "Evospace/Player/AmbientTracingComponent.h"

void UMusicManagerSubsystem::Initialize(FSubsystemCollectionBase &Collection) {
  Super::Initialize(Collection);

  auto session = Collection.InitializeDependency<UGameSessionSubsystem>();
  check(session);

  session->OnMenuMuffling.AddDynamic(this, &UMusicManagerSubsystem::HandleMenuMuffling);

  EnsureAudioComponent();
  RegisterAudioToWorld(GetWorld());
  FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UMusicManagerSubsystem::OnPostLoadMap);
  FWorldDelegates::OnWorldCleanup.AddUObject(this, &UMusicManagerSubsystem::OnWorldCleanup);

  {
    auto music_playlist = NewObject<UMusicPlaylist>();
    const TArray<FString> music_paths = {
      TEXT("/Script/Engine.SoundWave'/Game/Music/evospace_main_menu.evospace_main_menu'"),
      TEXT("/Script/Engine.SoundWave'/Game/Music/Stellar_Serenity.Stellar_Serenity'"),
      TEXT("/Script/Engine.SoundWave'/Game/Music/DowntempoNeon.DowntempoNeon'"),
      TEXT("/Script/Engine.SoundWave'/Game/Music/Celestial_Machinery.Celestial_Machinery'"),
      TEXT("/Script/Engine.SoundWave'/Game/Music/Celestial_Serenity.Celestial_Serenity'"),
      TEXT("/Script/Engine.SoundWave'/Game/Music/Temperate/Automatic_Rhythms.Automatic_Rhythms'"),
      TEXT("/Script/Engine.SoundWave'/Game/Music/Temperate/Factory_Must_Grow.Factory_Must_Grow'"),
      TEXT("/Script/Engine.SoundWave'/Game/Music/Temperate/Pababam_Must_Grow.Pababam_Must_Grow'"),
      TEXT("/Script/Engine.SoundWave'/Game/Music/Temperate/Celestial_Machinations.Celestial_Machinations'"),
      TEXT("/Script/Engine.SoundWave'/Game/Music/Cosmic_Machinery.Cosmic_Machinery'"),
      TEXT("/Script/Engine.SoundWave'/Game/Music/Celestial_Synchronization.Celestial_Synchronization'")
    };

    for (const auto &path : music_paths) {
      auto wave = LoadObject<USoundBase>(nullptr, *path);
      if (expect(wave, "Failed to load music wave: " + path)) {
        music_playlist->Tracks.Add(wave);
      }
    }

    SetPlaylist(music_playlist);
  }

  {
    auto music_playlist = NewObject<UMusicPlaylist>();
    const TArray<FString> music_paths = {
  TEXT("/Script/Engine.SoundWave'/Game/Music/Cosmos/Cosmic_Echoes.Cosmic_Echoes'"),
  TEXT("/Script/Engine.SoundWave'/Game/Music/Cosmos/Echoes_of_the_Unknown.Echoes_of_the_Unknown'"),
  TEXT("/Script/Engine.SoundWave'/Game/Music/Cosmos/Frozen_Echoes.Frozen_Echoes'"),
  TEXT("/Script/Engine.SoundWave'/Game/Music/Cosmos/Galactic_Ghosts.Galactic_Ghosts'"),
  TEXT("/Script/Engine.SoundWave'/Game/Music/Cosmos/Pew_Pew_Echoes.Pew_Pew_Echoes'"),
  TEXT("/Script/Engine.SoundWave'/Game/Music/Cosmos/Ppmp_Aah_Echoes.Ppmp_Aah_Echoes'"),
  TEXT("/Script/Engine.SoundWave'/Game/Music/Cosmos/Starlight_Voyager.Starlight_Voyager'"),
  TEXT("/Script/Engine.SoundWave'/Game/Music/Cosmos/Tidim_Echoes.Tidim_Echoes'")
    };

    for (const auto &path : music_paths) {
      auto wave = LoadObject<USoundBase>(nullptr, *path);
      if (expect(wave, "Failed to load music wave: " + path)) {
        music_playlist->Tracks.Add(wave);
      }
    }

    //SetPlaylist(music_playlist);
  }

  FactoryBus = LoadObject<USoundSubmix>(nullptr, TEXT("/Game/Audio/Submixes/FactoryBus.FactoryBus"));
  
  // Get the reverb preset from the first submix effect in the chain
  if (FactoryBus && FactoryBus->SubmixEffectChain.Num() > 0) {
    ReverbPreset = Cast<USubmixEffectReverbPreset>(FactoryBus->SubmixEffectChain[0]);
    if (!ReverbPreset) {
      LOG(WARN_LL) << "FactoryBus first submix effect is not a USubmixEffectReverbPreset";
    }
  } else {
    LOG(WARN_LL) << "FactoryBus has no submix effects in chain";
  }

  MusicMetaSoundSource = LoadObject<USoundBase>(nullptr, TEXT("/Script/MetasoundEngine.MetaSoundSource'/Game/Audio/MS_MusicPlayer.MS_MusicPlayer'"));
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

TStatId UMusicManagerSubsystem::GetStatId() const {
  RETURN_QUICK_DECLARE_CYCLE_STAT(UMusicManagerSubsystem, STATGROUP_Tickables);
}

void UMusicManagerSubsystem::Tick(float DeltaTime) {
  UpdateReverbFromAmbientTracing();
}

void UMusicManagerSubsystem::UpdateReverbFromAmbientTracing() {
  UWorld *World = GetWorld();
  if (!World) {
    return;
  }

  // Get the first player controller
  APlayerController *PC = World->GetFirstPlayerController();
  if (!PC) {
    return;
  }

  // Cast to our main player controller
  AMainPlayerController *MainPC = Cast<AMainPlayerController>(PC);
  if (!MainPC || !MainPC->mAmbientTracing) {
    // If ambient tracing is not available, disable reverb
    if (CurrentReverbAmount > 0.001f) {
      CurrentReverbAmount = 0.0f;
      
      // Create disabled reverb settings
      FSubmixEffectReverbSettings DisabledSettings;
      DisabledSettings.bBypass = true;
      DisabledSettings.WetLevel = 0.0f;
      DisabledSettings.DryLevel = 1.0f;
      
      ApplyReverbSettings(DisabledSettings);
    }
    return;
  }

  // Get the reverb settings from ambient tracing
  const FSubmixEffectReverbSettings &ReverbSettings = MainPC->mAmbientTracing->ReverbSettings;
  float NewReverbAmount = MainPC->mAmbientTracing->ReverbAmount;
  
  // Update if the value has changed meaningfully
  if (FMath::Abs(NewReverbAmount - CurrentReverbAmount) > 0.001f) {
    CurrentReverbAmount = NewReverbAmount;

    // Apply reverb settings to the submix
    ApplyReverbSettings(ReverbSettings);
  }
}

void UMusicManagerSubsystem::ApplyReverbSettings(const FSubmixEffectReverbSettings &Settings) {
  if (!ReverbPreset) {
    return;
  }

  // Directly apply the settings calculated in AmbientTracingComponent
  ReverbPreset->SetSettings(Settings);
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
      
      // Route to FactoryBus submix for reverb processing
      if (FactoryBus) {
        AudioComponentA->SetSubmixSend(FactoryBus, 1.0f);
      }
    }
  }
  if (!AudioComponentB) {
    AudioComponentB = NewObject<UAudioComponent>(this, TEXT("Music_AudioComponent_B"), RF_Transient);
    if (ensure(AudioComponentB)) {
      AudioComponentB->bIsUISound = true;
      AudioComponentB->bAutoActivate = false;
      AudioComponentB->SetVolumeMultiplier(1.0f);
      
      // Route to FactoryBus submix for reverb processing
      if (FactoryBus) {
        AudioComponentB->SetSubmixSend(FactoryBus, 1.0f);
      }
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
#if WITH_EDITOR
  if (GEngine && NewSound) {
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Starting crossfade: %s"), *NewSound->GetName()));
  }
#endif
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
