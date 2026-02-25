// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#include "Public/GameSessionData.h"

#include "Public/SurfaceDefinition.h"
#include "Public/MainGameInstance.h"
#include "Qr/JsonHelperCommon.h"
#include "Qr/Ensure.h"
#include "Evospace/Misc/StaticSaveHelpers.h"
#include "Qr/GameInstanceHelper.h"

UGameSessionData::UGameSessionData() {}

bool UGameSessionData::GetCreativeAllowed() const {
#ifdef EVOSPACE_DEMO
  return false;
#else
  return CreativeAllowed;
#endif
}

TOptional<FVersionStruct> UGameSessionData::VersionFromString(const FString &ser) {
  FVersionStruct result;
  TArray<FString> parts;

  TArray<FString> dashParts;
  ser.ParseIntoArray(dashParts, TEXT("-"), true);

  if (!expect(dashParts.Num() >= 3, "FVersionStruct::VersionFromString: invalid version string ", ser)) {
    return {};
  }
  {
    TArray<FString> dotParts;
    dashParts[0].ParseIntoArray(dotParts, TEXT("."), true);
    parts.Append(dotParts);
    parts.Add(dashParts[1]);
    parts.Add(dashParts[2]);
  }
  if (!expect(parts.Num() == 5, "FVersionStruct::VersionFromString: invalid version string ", ser)) {
    return {};
  }
  result.Major = FCString::Atoi(*parts[0]);
  result.Minor = FCString::Atoi(*parts[1]);
  result.Patch = FCString::Atoi(*parts[2]);
  result.Build = FCString::Atoi(*parts[3]);
  result.Hash = parts[4];

  return result;
}

void UGameSessionData::Initialize(UObject *WorldContextObject, const FString &saveName, bool CreativeMode, bool InfiniteOre, bool AllResearchCompleted, const FString &seed, FName generatorName) {
  check(WorldContextObject && WorldContextObject->GetWorld());

  SaveName = saveName;
  Seed = seed;
  GeneratorName = generatorName;
  TotalGameTime = 0.f;
  TotalGameTicks = 0;
  CreativeAllowed = CreativeMode;
  Version = UGameSessionData::VersionFromString(UMainGameInstance::GetBuildString()).Get({});
  auto gi = UGameInstanceHelper::GetMainGameInstance(WorldContextObject);
  check(gi);
  Mods = gi->DB->GetMods();

  // TickDelta is computed from TickRate via getter

  UStaticSaveHelpers::SaveGameSessionData(saveName, this);

  for (auto &surface : { "Temperate" }) {
    auto &instance = UGameInstanceHelper::GetGameInstance(WorldContextObject);
    USurfaceDefinition *surfaceDefinition = NewObject<USurfaceDefinition>(&instance);
    surfaceDefinition->GeneratorName = generatorName;
    surfaceDefinition->Initialize();
    UStaticSaveHelpers::SaveSurfaceDefinition(saveName, surface, surfaceDefinition);
  }
}

FString UGameSessionData::VersionToString(const FVersionStruct &version) {
  return FString::Printf(TEXT("%d.%d.%d-%d-%s"), version.Major, version.Minor, version.Patch, version.Build, *version.Hash);
}

bool UGameSessionData::DeserializeJson(TSharedPtr<FJsonObject> json) {
  json_helper::TryGet(json, "Seed", Seed);
  json_helper::TryGet(json, "Generator", GeneratorName);
  json_helper::TryGet(json, "TotalGameTime", TotalGameTime);
  json_helper::TryGet(json, "TotalGameTicks", TotalGameTicks);
  json_helper::TryGet(json, "CreativeMode", CreativeMode);
  json_helper::TryGet(json, "InfiniteOre", InfiniteOre);
  json_helper::TryGet(json, "CreativeAllowed", CreativeAllowed);
  json_helper::TryGet(json, "Mods", Mods);
  FString ser;
  if (json_helper::TryGet(json, "Version", ser)) {
    auto result = UGameSessionData::VersionFromString(ser);
    Version = result.Get({});
  }
  json_helper::TryGet(json, "Cloud", Cloud);
  json_helper::TryGet(json, "AllResearchCompleted", AllResearchCompleted);
  json_helper::TryGet(json, "SaveName", SaveName);
  json_helper::TryGet(json, "WorldTimeOfDayHours", WorldTimeOfDayHours);
  json_helper::TryGet(json, "WorldTimeAutoAdvance", WorldTimeAutoAdvance);

  return true;
}

bool UGameSessionData::SerializeJson(TSharedPtr<FJsonObject> json) const {
  json_helper::TrySet(json, "Seed", Seed);
  json_helper::TrySet(json, "Generator", GeneratorName);
  json_helper::TrySet(json, "TotalGameTime", TotalGameTime);
  json_helper::TrySet(json, "TotalGameTicks", TotalGameTicks);
  json_helper::TrySet(json, "CreativeMode", CreativeMode);
  json_helper::TrySet(json, "InfiniteOre", InfiniteOre);
  json_helper::TrySet(json, "CreativeAllowed", CreativeAllowed);
  json_helper::TrySet(json, "Mods", Mods);
  json_helper::TrySet(json, "Version", UGameSessionData::VersionToString(Version));
  json_helper::TrySet(json, "Cloud", Cloud);
  json_helper::TrySet(json, "AllResearchCompleted", AllResearchCompleted);
  json_helper::TrySet(json, "SaveName", SaveName);
  json_helper::TrySet(json, "WorldTimeOfDayHours", WorldTimeOfDayHours);
  json_helper::TrySet(json, "WorldTimeAutoAdvance", WorldTimeAutoAdvance);
  return true;
}

void UGameSessionData::Reset() {
  Seed = "Default";
  GeneratorName = "";
  TotalGameTime = 0.f;
  TotalGameTicks = 0;
  CreativeMode = false;
  CreativeAllowed = false;
  InfiniteOre = false;
  AllResearchCompleted = false;
  Version = {};
  Mods.Empty();
  Cloud = false;
  SaveName = "Default";
  WorldTimeOfDayHours = 8.0;
  WorldTimeAutoAdvance = true;
}

FString UGameSessionData::GetModsCombined() const {
  FString res;
  for (auto &s : Mods) {
    res += s + "\n";
  }
  return res;
}