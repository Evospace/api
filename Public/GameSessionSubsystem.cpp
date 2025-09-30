// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#include "Public/GameSessionSubsystem.h"

#include "Public/GameSessionData.h"
#include "Public/MainGameInstance.h"
#include "Qr/StaticSaveHelpers.h"
#include <Engine/World.h>

void UGameSessionSubsystem::Initialize(FSubsystemCollectionBase &Collection) {
  Super::Initialize(Collection);

  if (!Data) {
    Data = NewObject<UGameSessionData>(this, TEXT("GameSessionData"));
  }
}

bool UGameSessionSubsystem::SetCreativeMode(bool val) {
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
#ifdef EVOSPACE_DEMO
  return false;
#else
  return Data->CreativeMode;
#endif
}

void UGameSessionSubsystem::SetSeed(const FString &seed) {
  Data->Seed = seed;
  OnDataUpdated.Broadcast(Data);
}

void UGameSessionSubsystem::SetGenerator(const FName &generator) {
  Data->GeneratorName = generator;
  OnDataUpdated.Broadcast(Data);
}

int64 UGameSessionSubsystem::GetSeed() const {
  return Data->GetSeed();
}

void UGameSessionSubsystem::SetData(UGameSessionData *dat) {
  Data = dat;
  OnDataUpdated.Broadcast(dat);
}

void UGameSessionSubsystem::SetCreativeAllowed(bool val) {
  Data->CreativeAllowed = val;
}

void UGameSessionSubsystem::SetDataExt(UGameSessionData *dat, const FString &saveName, const FVersionStruct &version, const TArray<FString> & mods) {
  Data = dat;
  Data->SaveName = saveName;
  Data->Version = version;
  Data->Mods = mods;
  OnDataUpdated.Broadcast(dat);
}

int64 UGameSessionSubsystem::IncrementTicks() {
  return ++Data->TotalGameTicks;
}

void UGameSessionSubsystem::IncrementTime(double delta) {
  Data->TotalGameTime += delta;
}