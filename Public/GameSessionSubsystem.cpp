// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#include "Public/GameSessionSubsystem.h"

#include "Public/GameSessionData.h"
#include "Public/StaticPlanet.h"
#include "Public/WorldDayCycle.h"
#include "Public/MainGameInstance.h"
#include "Public/ResearchSubsystem.h"
#include "Public/Dimension.h"
#include "Public/SimulationSurfaceSubsystem.h"
#include "Public/DimensionRuntime.h"
#include "Public/SurfaceDefinition.h"
#include "Public/Net/NetSessionSubsystem.h"
#include "Evospace/Misc/StaticSaveHelpers.h"
#include "Evospace/CoordinateSystem.h"
#include "Qr/GameInstanceHelper.h"
#include "Qr/QrFind.h"
#include "Qr/StaticLogger.h"
#include "Engine/World.h"
#include "EngineUtils.h"

void UGameSessionSubsystem::Initialize(FSubsystemCollectionBase &Collection) {
  Super::Initialize(Collection);

  if (!Data) {
    Data = NewObject<UGameSessionData>(this, TEXT("GameSessionData"));
  }
}
void UGameSessionSubsystem::RequestSave(const FString &saveName) {
  OnSaveRequested.Broadcast(saveName);
}

void UGameSessionSubsystem::NotifySaveLoaded(const FString &saveName) {
  OnSaveLoading.Broadcast(saveName);
}

bool UGameSessionSubsystem::SetCreativeMode(bool val) {
  check(Data);
  bool newCreative = false;
#ifndef EVOSPACE_DEMO
  if (Data->CreativeAllowed && val) {
    newCreative = true;
  }
  Data->CreativeMode = newCreative;
#endif
  return newCreative;
}

void UGameSessionSubsystem::SetMenuMuffling(bool bMuffled) {
  OnMenuMuffling.Broadcast(bMuffled);
}

bool UGameSessionSubsystem::IsCreative() const {
  check(Data);
#ifdef EVOSPACE_DEMO
  return false;
#else
  return Data->CreativeMode;
#endif
}

void UGameSessionSubsystem::SetSeed(const FString &seed) {
  check(Data);
  Data->Seed = seed;
  OnDataUpdated.Broadcast(Data);
}

void UGameSessionSubsystem::SetGenerator(const FName &generator) {
  check(Data);
  Data->GeneratorName = generator;
  OnDataUpdated.Broadcast(Data);
}

int64 UGameSessionSubsystem::GetSeed() const {
  check(Data);
  return Data->GetSeed();
}

void UGameSessionSubsystem::SetSaveName(const FString &saveName) {
  check(Data);
  Data->SaveName = saveName;
}

void UGameSessionSubsystem::SetData(UGameSessionData *dat) {
  check(dat);
  Data = dat;
  OnDataUpdated.Broadcast(dat);
}

void UGameSessionSubsystem::SetCreativeAllowed(bool val) {
  check(Data);
  Data->CreativeAllowed = val;
}

void UGameSessionSubsystem::SetSearchItems(const TArray<UStaticItem *> &InItems) {
  TArray<const UStaticItem *> const_items;
  for (auto i : InItems) {
    const_items.Add(i);
  }
  ItemTypeHighlight.SetSearchItems(const_items);
}

void UGameSessionSubsystem::SetHoveredItem(UStaticItem *item) {
  ItemTypeHighlight.SetHoveredItem(item);
}

void UGameSessionSubsystem::SetDataExt(UGameSessionData *dat, const FString &saveName, const FVersionStruct &version, const TArray<FString> &mods) {
  check(dat);
  Data = dat;
  Data->SaveName = saveName;
  Data->Version = version;
  Data->Mods = mods;
  OnDataUpdated.Broadcast(dat);
}

void UGameSessionSubsystem::SetAllResearchCompleted(bool val) {
  check(Data);
  Data->AllResearchCompleted = val;
  OnDataUpdated.Broadcast(Data);
}

float UGameSessionSubsystem::GetTickDelta() const {
  check(Data);
  return Data->TickRate > 0 ? 1.f / static_cast<float>(Data->TickRate) : 0.f;
}

int32 UGameSessionSubsystem::GetTickRate() const {
  check(Data);
  return Data->TickRate;
}

void UGameSessionSubsystem::SetTickRate(int32 rate) {
  check(Data);
  Data->TickRate = rate;
}

int64 UGameSessionSubsystem::IncrementTicks() {
  check(Data);
  return ++Data->TotalGameTicks;
}

void UGameSessionSubsystem::IncrementTime(double delta) {
  check(Data);
  Data->TotalGameTime += delta;
}

void UGameSessionSubsystem::SetPresentationSurfacePlanet(const UStaticPlanet *Planet) {
  PresentationSurfacePlanet = Planet;
}

void UGameSessionSubsystem::ClearPresentationSurfacePlanet() {
  PresentationSurfacePlanet = nullptr;
}

int64 UGameSessionSubsystem::GetWorldDayPhaseTicks() const {
  check(Data);
  const UStaticPlanet *Planet = PresentationSurfacePlanet.Get();
  if (!Planet) {
    return 0;
  }
  const int64 len = FMath::Max<int64>(1, Planet->DayLengthTicks);
  if (!Data->WorldTimeAutoAdvance) {
    return evo::WorldDayCycle::PositiveModInt64(Data->WorldTimeOfDayPhaseTicks, len);
  }
  return evo::WorldDayCycle::PositiveModInt64(Planet->SimulationDayPhaseTicks(Data->TotalGameTicks), len);
}

float UGameSessionSubsystem::GetWorldTimeOfDayHours() const {
  check(Data);
  const UStaticPlanet *Planet = PresentationSurfacePlanet.Get();
  if (!Planet) {
    return 0.f;
  }
  return Planet->ResolveTimeOfDayHours(Data->WorldTimeAutoAdvance, Data->WorldTimeOfDayPhaseTicks, Data->TotalGameTicks);
}

void UGameSessionSubsystem::SetWorldTimeOfDayHours(float Hours) {
  check(Data);
  const UStaticPlanet *Planet = PresentationSurfacePlanet.Get();
  if (!Planet) {
    LOG(ERROR_LL) << "SetWorldTimeOfDayHours: presentation surface planet not set";
    return;
  }
  const int64 len = FMath::Max<int64>(1, Planet->DayLengthTicks);
  Data->WorldTimeOfDayPhaseTicks = evo::WorldDayCycle::LockedHoursToPhaseTicks(Hours, len);
}

void UGameSessionSubsystem::SetLockedWorldTimeOfDayHours(float Hours) {
  SetWorldTimeOfDayHours(Hours);
  SetWorldTimeAutoAdvance(false);
}

bool UGameSessionSubsystem::GetWorldTimeAutoAdvance() const {
  check(Data);
  return Data->WorldTimeAutoAdvance;
}

void UGameSessionSubsystem::SetWorldTimeAutoAdvance(bool bEnable) {
  check(Data);
  Data->WorldTimeAutoAdvance = bEnable;
}

void UGameSessionSubsystem::SetInfiniteOre(bool val) {
  check(Data);
  Data->InfiniteOre = val;
  OnDataUpdated.Broadcast(Data);
}

bool UGameSessionSubsystem::GetInfiniteOre() const {
  check(Data);
  return Data->InfiniteOre;
}

namespace {
FName GeneratorNameForSurface(const FString &SurfaceName, const UGameSessionData *SessionData) {
  if (SurfaceName == TEXT("Moon")) {
    return TEXT("WorldGeneratorMoon");
  }
  if (SessionData) {
    return SessionData->GetGeneratorName();
  }
  return TEXT("WorldGeneratorConfigurable");
}

ADimension *FindDimensionInWorld(UWorld *World) {
  if (!World) {
    return nullptr;
  }
  for (TActorIterator<ADimension> It(World); It; ++It) {
    return *It;
  }
  return nullptr;
}
} // namespace

void UGameSessionSubsystem::TravelToSurface(const FString &SurfaceName) {
  if (SurfaceName.IsEmpty()) {
    LOG(ERROR_LL) << "TravelToSurface: empty surface name";
    return;
  }

  UGameInstance *Gi = GetGameInstance();
  if (!Gi) {
    LOG(ERROR_LL) << "TravelToSurface: no game instance";
    return;
  }

  UWorld *World = Gi->GetWorld();
  if (!World) {
    LOG(ERROR_LL) << "TravelToSurface: no world";
    return;
  }

  ADimension *Dimension = FindDimensionInWorld(World);
  if (!Dimension) {
    LOG(ERROR_LL) << "TravelToSurface: no ADimension in world";
    return;
  }

  if (Dimension->SurfaceFolderName == SurfaceName) {
    LOG(INFO_LL) << "TravelToSurface: already on " << SurfaceName;
    return;
  }

  if (!QrTryFind<UStaticPlanet>(FName(*SurfaceName))) {
    LOG(ERROR_LL) << "TravelToSurface: no StaticPlanet for surface " << SurfaceName;
    return;
  }

  Dimension->SaveDimentionFolder();

  auto &InstanceRef = UGameInstanceHelper::GetGameInstance(Gi);
  USurfaceDefinition *TargetDefinition =
    UStaticSaveHelpers::LoadSurfaceDefinition(&InstanceRef, TEXT("temp"), SurfaceName);
  if (!TargetDefinition) {
    TargetDefinition = NewObject<USurfaceDefinition>(&InstanceRef);
    TargetDefinition->GeneratorName = GeneratorNameForSurface(SurfaceName, Data);
    TargetDefinition->Initialize();
    UStaticSaveHelpers::SaveSurfaceDefinition(TEXT("temp"), SurfaceName, TargetDefinition);
  }

  Dimension->SurfaceFolderName = SurfaceName;
  Dimension->InitializeSurface(TargetDefinition, /*bDestroyPreviousOnSwitch=*/false);

  const float SurfaceHeightBlocks = Dimension->LuaSampleHeight(0.f, 0.f);
  const FVector SpawnLocation(0.f, 0.f, SurfaceHeightBlocks * gCubeSize + 200.f);
  Dimension->BeginTeleport(SpawnLocation);

  if (UNetSessionSubsystem *Net = Gi->GetSubsystem<UNetSessionSubsystem>()) {
    Net->OnLocalSurfaceChanged();
  }

  OnSurfaceChange.Broadcast(SurfaceName);
  LOG(INFO_LL) << "TravelToSurface: switched presentation to " << SurfaceName;
}

void UGameSessionSubsystem::SaveAllSurfaces(UWorld *World) {
  if (!World) {
    return;
  }

  UGameInstance *Gi = World->GetGameInstance();
  if (!Gi) {
    return;
  }

  USimulationSurfaceSubsystem *SurfaceSubsystem = Gi->GetSubsystem<USimulationSurfaceSubsystem>();
  if (!SurfaceSubsystem) {
    return;
  }

  FString ActiveSurface;
  if (ADimension *Dimension = FindDimensionInWorld(World)) {
    ActiveSurface = Dimension->SurfaceFolderName;
  }

  TArray<FString> SurfaceNames;
  SurfaceSubsystem->GetSurfaceFolderNames(SurfaceNames);
  for (const FString &SurfaceName : SurfaceNames) {
    if (SurfaceName == ActiveSurface) {
      continue;
    }
    if (UDimensionRuntime *Runtime = SurfaceSubsystem->GetRuntime(SurfaceName)) {
      UStaticSaveHelpers::SaveRuntimeLogic(TEXT("temp"), SurfaceName, Runtime);
    }
  }
}