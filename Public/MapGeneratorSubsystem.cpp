#include "Public/MapGeneratorSubsystem.h"
#include "Public/GameSessionData.h"
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
  for (auto wg : WorldGenerators) {
    wg->SetSeed(GameSessionData->GetSeed());
  }
}

void UMapGeneratorSubsystem::InitializeWorldGenerators() {
  WorldGenerators.Empty();

  WorldGenerators.Add(NewObject<UWorldGeneratorRivers>(this, TEXT("WorldGeneratorRivers")));
  WorldGenerators.Add(NewObject<UWorldGeneratorLegacy>(this, TEXT("WorldGeneratorBiome")));
  WorldGenerators.Add(NewObject<UWorldGeneratorPlains>(this, TEXT("WorldGeneratorPlains")));
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