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
}

void UMusicManagerSubsystem::Deinitialize() {
  if (auto session = GetGameInstance()->GetSubsystem<UGameSessionSubsystem>()) {
    check(session);
    session->OnMenuMuffling.RemoveDynamic(this, &UMusicManagerSubsystem::HandleMenuMuffling);
  }
  if (auto *World = GetWorld()) {
    World->GetTimerManager().ClearTimer(NextTrackTimerHandle);
  }
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
  if (!NewWorld)
    return;

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
  if (!World)
    return;
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

void UMusicManagerSubsystem::StartCrossfade(USoundBase *NewSound) {
  EnsureAudioComponent();
  if (!NewSound)
    return;

  // Components should already be registered by map load hooks; avoid re-registering here.

  UAudioComponent *Active = GetActiveComponent();
  UAudioComponent *Inactive = GetInactiveComponent();
  if (!Inactive)
    return;

  // Load metasound source if not set
  if (!MusicMetaSoundSource) {
    MusicMetaSoundSource = LoadObject<USoundBase>(nullptr, TEXT("/Script/MetasoundEngine.MetaSoundSource'/Game/Sounds/MS_MusicPlayer.MS_MusicPlayer'"));
  }

  // Prepare the inactive component with MetaSound source and set parameters
  if (MusicMetaSoundSource) {
    Inactive->SetSound(MusicMetaSoundSource);
  } else {
    Inactive->SetSound(NewSound); // Fallback
  }
  Inactive->SetBoolParameter(TEXT("Muffled"), bMuffled);
  if (USoundWave *AsWave = Cast<USoundWave>(NewSound)) {
    Inactive->SetWaveParameter(TEXT("Wave"), AsWave);
  }

  // Capture and store duration at start
  const float Duration = NewSound->GetDuration();
  if (Inactive == AudioComponentA) {
    AudioComponentADuration = Duration;
  } else {
    AudioComponentBDuration = Duration;
  }

  // Start playback and crossfade
  if (!Inactive->IsPlaying()) {
    Inactive->Play(0.0f);
  }
  Inactive->FadeIn(CrossfadeTime, 1.0f);
  if (Active) {
    Active->FadeOut(CrossfadeTime, 0.0f);
  }

  // Swap active flag so the new one becomes active
  bUseAAsActive = (Inactive == AudioComponentA);

  // Schedule next crossfade 1 second before end
  ScheduleNextTimer(Duration);
}

void UMusicManagerSubsystem::ScheduleNextTimer(float DurationSeconds) {
  if (DurationSeconds <= 0.0f)
    return;
  if (auto *World = GetWorld()) {
    const float TimeUntilFire = FMath::Max(0.01f, DurationSeconds - 1.0f);
    World->GetTimerManager().ClearTimer(NextTrackTimerHandle);
    World->GetTimerManager().SetTimer(NextTrackTimerHandle, this, &UMusicManagerSubsystem::OnNextTrackTimer, TimeUntilFire, false);
  }
}

void UMusicManagerSubsystem::OnNextTrackTimer() {
  if (!Playlist)
    return;
  if (USoundBase *Next = Playlist->GetRandomSound()) {
    StartCrossfade(Next);
  }
}
