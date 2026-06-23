#include "Public/MapGeneratorSubsystem.h"
#include "Public/GameSessionData.h"
#include "Public/GameSessionSubsystem.h"
#include "Public/GameSessionSubsystem.h"
#include "Qr/ModLoadingSubsystem.h"
#include "Qr/JsonObjectLibrary.h"
#include "Public/WorldGenerator.h"
#include "Subsystems/SubsystemCollection.h"
#include <Engine/Engine.h>
#include "FlatWorldGenerator.h"
#include "BiomeWorldGenerator.h"

void UMapGeneratorSubsystem::Initialize(FSubsystemCollectionBase &Collection) {
  Super::Initialize(Collection);

  Collection.InitializeDependency<UGameSessionSubsystem>();

  auto GameSessionSubsystem = GetGameInstance()->GetSubsystem<UGameSessionSubsystem>();
  if (ensure(GameSessionSubsystem)) {
    GameSessionSubsystem->OnDataUpdated.AddDynamic(this, &UMapGeneratorSubsystem::UpdateSeed_Internal);
  }

  auto mls = GEngine->GetEngineSubsystem<UModLoadingSubsystem>();
  if (ensure(mls)) {
    mls->ModLoadingFinalStep.AddDynamic(this, &UMapGeneratorSubsystem::InitializeWorldGenerators);
  }
}

void UMapGeneratorSubsystem::UpdateSeed_Internal(UGameSessionData *GameSessionData) {
  check(GameSessionData);
  LOG(INFO_LL) << "UMapGeneratorSubsystem::UpdateSeed_Internal";
  CarveSettings = GameSessionData->MapSettings.CarveSettings;

  for (auto wg : WorldGenerators) {
    wg->SetSeed(GameSessionData->GetSeed());
  }

  // Push persisted settings onto the configurable generator only; legacy
  // generators keep the values hardcoded in their constructors.
  if (auto *cfg = Cast<UWorldGeneratorConfigurable>(FindWorldGenerator(GameSessionData->GetGeneratorName()))) {
    cfg->SetMapSettings(GameSessionData->MapSettings);
  }

  ApplyCarveSettingsToBiomeGenerators();
}

void UMapGeneratorSubsystem::InitializeWorldGenerators() {
  WorldGenerators.Empty();

  WorldGenerators.Add(NewObject<UWorldGeneratorRivers>(this, TEXT("WorldGeneratorRivers")));
  WorldGenerators.Add(NewObject<UWorldGeneratorLegacy>(this, TEXT("WorldGeneratorBiome")));
  WorldGenerators.Add(NewObject<UWorldGeneratorPlains>(this, TEXT("WorldGeneratorPlains")));
  WorldGenerators.Add(NewObject<UWorldGeneratorConfigurable>(this, TEXT("WorldGeneratorConfigurable")));
  WorldGenerators.Add(NewObject<UFlatWorldGenerator>(this, TEXT("FlatWorldGenerator")));

  // Add modded world generators registered in DB storage
  if (UDbStorage::Singleton) {
    const TArray<UWorldGenerator *> modGenerators = UDbStorage::Singleton->GetObjects<UWorldGenerator>();
    for (auto *gen : modGenerators) {
      if (ensure(gen)) {
        WorldGenerators.Add(gen);
      }
    }
  }

  for (auto wg : WorldGenerators) {
    wg->Initialize();
    wg->LoadBiomeFamily();
  }

  if (auto *cfg = Cast<UWorldGeneratorConfigurable>(FindWorldGenerator(TEXT("WorldGeneratorConfigurable")))) {
    CarveSettings = cfg->GetMapSettings().CarveSettings;
  }
  ApplyCarveSettingsToBiomeGenerators();
}

void UMapGeneratorSubsystem::SetCarveSettings(const FCarveNoiseSettings &InSettings) {
  CarveSettings = InSettings;
  CommitCarveSettings();
}

void UMapGeneratorSubsystem::CommitCarveSettings() {
  ApplyCarveSettingsToBiomeGenerators();

  if (auto *gameSessionSubsystem = GetGameInstance()->GetSubsystem<UGameSessionSubsystem>()) {
    if (UGameSessionData *gameSessionData = const_cast<UGameSessionData *>(gameSessionSubsystem->GetData())) {
      gameSessionData->MapSettings.CarveSettings = CarveSettings;
    }
  }

  OnCarveSettingsChanged.Broadcast();
}

void UMapGeneratorSubsystem::ApplyCarveSettingsToBiomeGenerators() {
  for (auto *wg : WorldGenerators) {
    if (auto *bg = Cast<UBiomeWorldGenerator>(wg)) {
      FMapGeneratorSettings mapSettings = bg->GetMapSettings();
      mapSettings.CarveSettings = CarveSettings;
      bg->SetMapSettings(mapSettings);
    }
  }
}

void UMapGeneratorSubsystem::Deinitialize() {
  Super::Deinitialize();
}

TArray<UWorldGenerator *> UMapGeneratorSubsystem::GetWorldGeneratorList() {
  return WorldGenerators;
}

UWorldGenerator *UMapGeneratorSubsystem::FindWorldGenerator(FName name) const {
  for (auto gen : WorldGenerators) {
    if (gen->GetName() == name) {
      return gen;
    }
  }

  LOG(ERROR_LL) << "UMainGameInstance::FindWorldGenerator World generator not found: " << name;
  return nullptr;
}