// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#include "Public/GameSessionSubsystem.h"

#include "Public/GameSessionData.h"
#include "Public/MainGameInstance.h"
#include "Public/ResearchSubsystem.h"
#include "Qr/StaticSaveHelpers.h"
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

void UGameSessionSubsystem::SetDataExt(UGameSessionData *dat, const FString &saveName, const FVersionStruct &version, const TArray<FString> & mods) {
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

int64 UGameSessionSubsystem::IncrementTicks() {
  check(Data);
  return ++Data->TotalGameTicks;
}

void UGameSessionSubsystem::IncrementTime(double delta) {
  check(Data);
  Data->TotalGameTime += delta;
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