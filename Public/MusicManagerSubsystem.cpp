// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#include "MusicManagerSubsystem.h"

#include <Components/AudioComponent.h>
#include <Sound/SoundBase.h>
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Public/GameSessionSubsystem.h"
#include "Subsystems/SubsystemCollection.h"

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
  if (AudioComponent) {
    if (AudioComponent->IsRegistered()) {
      AudioComponent->UnregisterComponent();
    }
    AudioComponent->DestroyComponent();
    AudioComponent = nullptr;
  }
  FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
  FWorldDelegates::OnWorldCleanup.RemoveAll(this);
  Super::Deinitialize();
}

void UMusicManagerSubsystem::SetMusicSound(USoundBase *InSound) {
  MusicSound = InSound;
  if (AudioComponent) {
    LOG(INFO_LL) << "UMusicManagerSubsystem Setting music sound";
    AudioComponent->SetSound(MusicSound);
    AudioComponent->Play();
  }
}

void UMusicManagerSubsystem::SetMuffled(bool bInMuffled) {
  bMuffled = bInMuffled;
  if (AudioComponent) {
    // Set MetaSound boolean parameter named "Muffled" if supported by the sound/graph.
    AudioComponent->SetBoolParameter(TEXT("Muffled"), bMuffled);
  }
}

void UMusicManagerSubsystem::HandleMenuMuffling(bool bInMuffled) {
  SetMuffled(bInMuffled);
}

void UMusicManagerSubsystem::EnsureAudioComponent() {
  if (AudioComponent)
    return;

  AudioComponent = NewObject<UAudioComponent>(this, TEXT("Music_AudioComponent"), RF_Transient);
  if (ensure(AudioComponent)) {
    AudioComponent->bIsUISound = true;
    if (!MusicSound) {
      MusicSound = LoadObject<USoundBase>(nullptr, TEXT("/Game/Music/Theme"));
    }
  }
}

void UMusicManagerSubsystem::RegisterAudioToWorld(UWorld *NewWorld) {
  if (!AudioComponent || !NewWorld)
    return;

  if (AudioComponent->IsRegistered()) {
    if (AudioComponent->GetWorld() == NewWorld)
      return;
    AudioComponent->UnregisterComponent();
  }

  AudioComponent->RegisterComponentWithWorld(NewWorld);

  if (MusicSound) {
    LOG(INFO_LL) << "UMusicManagerSubsystem Setting music sound";
    AudioComponent->SetSound(MusicSound);
  }
  AudioComponent->SetBoolParameter(TEXT("Muffled"), bMuffled);
  if (MusicSound) {
    LOG(INFO_LL) << "UMusicManagerSubsystem Playing music";
    AudioComponent->Play();
  }
}

void UMusicManagerSubsystem::OnPostLoadMap(UWorld *NewWorld) {
  RegisterAudioToWorld(NewWorld);
}

void UMusicManagerSubsystem::OnWorldCleanup(UWorld *World, bool /*bSessionEnded*/, bool /*bCleanupResources*/) {
  if (AudioComponent && AudioComponent->IsRegistered() && AudioComponent->GetWorld() == World) {
    AudioComponent->Stop();
    AudioComponent->UnregisterComponent();
  }
}
