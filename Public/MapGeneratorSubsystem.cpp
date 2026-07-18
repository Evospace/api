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
  MapSettings = GameSessionData->MapSettings;

  for (auto wg : WorldGenerators) {
    wg->SetSeed(GameSessionData->GetSeed());
  }

  if (auto *cfg = Cast<UWorldGeneratorConfigurable>(FindWorldGenerator(GameSessionData->GetGeneratorName()))) {
    cfg->SetMapSettings(GameSessionData->MapSettings);
  }

  ApplyMapSettingsToGenerators();
}

void UMapGeneratorSubsystem::InitializeWorldGenerators() {
  WorldGenerators.Empty();

  WorldGenerators.Add(NewObject<UWorldGeneratorRivers>(this, TEXT("WorldGeneratorRivers")));
  WorldGenerators.Add(NewObject<UWorldGeneratorLegacy>(this, TEXT("WorldGeneratorBiome")));
  WorldGenerators.Add(NewObject<UWorldGeneratorPlains>(this, TEXT("WorldGeneratorPlains")));
  WorldGenerators.Add(NewObject<UWorldGeneratorMoon>(this, TEXT("WorldGeneratorMoon")));
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
    MapSettings = cfg->GetMapSettings();
  }
  ApplyMapSettingsToGenerators();
}

void UMapGeneratorSubsystem::SetMapSettings(const FMapGeneratorSettings &InSettings) {
  MapSettings = InSettings;
  CommitMapSettings();
}

void UMapGeneratorSubsystem::CommitMapSettings() {
  ApplyMapSettingsToGenerators();

  if (auto *gameSessionSubsystem = GetGameInstance()->GetSubsystem<UGameSessionSubsystem>()) {
    if (UGameSessionData *gameSessionData = const_cast<UGameSessionData *>(gameSessionSubsystem->GetData())) {
      gameSessionData->MapSettings = MapSettings;
    }
  }

  OnMapSettingsChanged.Broadcast();
}

void UMapGeneratorSubsystem::ApplyMapSettingsToGenerators() {
  if (auto *cfg = Cast<UWorldGeneratorConfigurable>(FindWorldGenerator(TEXT("WorldGeneratorConfigurable")))) {
    FMapGeneratorSettings configurableSettings = MapSettings;
    configurableSettings.bUseBiomeHeightMaps = true;
    cfg->SetMapSettings(configurableSettings);
  }

  for (auto *wg : WorldGenerators) {
    auto *bg = Cast<UBiomeWorldGenerator>(wg);
    if (!bg || Cast<UWorldGeneratorConfigurable>(bg)) {
      continue;
    }

    FMapGeneratorSettings generatorSettings = bg->GetMapSettings();
    generatorSettings.CarveSettings = MapSettings.CarveSettings;
    bg->SetMapSettings(generatorSettings);
  }
}

void UMapGeneratorSubsystem::Deinitialize() {
  Super::Deinitialize();
}

TArray<UWorldGenerator *> UMapGeneratorSubsystem::GetWorldGeneratorList() {
  TArray<UWorldGenerator *> result;
  result.Reserve(WorldGenerators.Num());

  for (UWorldGenerator *gen : WorldGenerators) {
    if (gen->IsA(UWorldGeneratorRivers::StaticClass()) ||
        gen->IsA(UWorldGeneratorLegacy::StaticClass()) ||
        gen->IsA(UWorldGeneratorPlains::StaticClass()) ||
        gen->IsA(UWorldGeneratorMoon::StaticClass())) {
      continue;
    }
    result.Add(gen);
  }

  return result;
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
