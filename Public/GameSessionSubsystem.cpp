// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#include "Public/GameSessionSubsystem.h"

#include "Public/GameSessionData.h"
#include "Public/StaticPlanet.h"
#include "Public/WorldDayCycle.h"
#include "Public/MainGameInstance.h"
#include "Public/ResearchSubsystem.h"
#include "Qr/GameInstanceHelper.h"
#include "Evospace/Misc/StaticSaveHelpers.h"
#include <Engine/World.h>

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

int64 UGameSessionSubsystem::GetWorldDayPhaseTicks(const UStaticPlanet *Planet) const {
  check(Data);
  check(Planet);
  const int64 len = FMath::Max<int64>(1, Planet->DayLengthTicks);
  if (!Data->WorldTimeAutoAdvance) {
    return evo::WorldDayCycle::PositiveModInt64(Data->WorldTimeOfDayPhaseTicks, len);
  }
  return evo::WorldDayCycle::PositiveModInt64(Planet->SimulationDayPhaseTicks(Data->TotalGameTicks), len);
}

float UGameSessionSubsystem::GetWorldTimeOfDayHours(const UStaticPlanet *Planet) const {
  check(Data);
  check(Planet);
  return Planet->ResolveTimeOfDayHours(Data->WorldTimeAutoAdvance, Data->WorldTimeOfDayPhaseTicks, Data->TotalGameTicks);
}

void UGameSessionSubsystem::SetWorldTimeOfDayHours(float Hours, const UStaticPlanet *Planet) {
  check(Data);
  check(Planet);
  const int64 len = FMath::Max<int64>(1, Planet->DayLengthTicks);
  Data->WorldTimeOfDayPhaseTicks = evo::WorldDayCycle::LockedHoursToPhaseTicks(Hours, len);
}

void UGameSessionSubsystem::SetLockedWorldTimeOfDayHours(float Hours, const UStaticPlanet *Planet) {
  SetWorldTimeOfDayHours(Hours, Planet);
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