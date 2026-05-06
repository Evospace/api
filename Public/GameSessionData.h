// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Qr/CommonConverter.h"
#include "Qr/Prototype.h"
#include "ThirdParty/luabridge/LuaBridge.h"
#include "Qr/SerializableJson.h"
#include "ThirdParty/lua/lua.h"
#include "UObject/Object.h"
#include "Qr/CommonConverter.h"
#include "GameSessionData.generated.h"

class USetting;
class UValueStorage;

USTRUCT(BlueprintType)
struct FVersionStruct {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 Major = 0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 Minor = 0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 Patch = 0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 Build = 0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Hash = "";

  std::strong_ordering operator<=>(const FVersionStruct &other) const {
    if (auto r = Major <=> other.Major; r != 0) return r;
    if (auto r = Minor <=> other.Minor; r != 0) return r;
    if (auto r = Patch <=> other.Patch; r != 0) return r;
    return Build <=> other.Build;
  }
};

UCLASS(BlueprintType)
class UGameSessionData : public UInstance {
  using Self = UGameSessionData;
  GENERATED_BODY()
  EVO_CODEGEN_INSTANCE(GameSessionData)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UInstance>("GameSessionData") //@class GameSessionData : Instance
      .addProperty("infinite_ore", &Self::InfiniteOre) //@field boolean
      .addProperty("all_research_completed", &Self::AllResearchCompleted) //@field boolean
      .addProperty("creative_mode", &Self::CreativeMode) //@field boolean
      .addProperty("creative_allowed", &Self::CreativeAllowed) //@field boolean
      .addProperty("infinite_ore", &Self::InfiniteOre) //@field boolean
      .addProperty("total_game_time", &Self::TotalGameTime) //@field number
      .addProperty("total_game_ticks", &Self::TotalGameTicks) //@field integer
      .addProperty("seed", &Self::Seed) //@field string
      .addProperty("version", [](Self *self) -> std::string { return TCHAR_TO_UTF8(*Self::VersionToString(self->Version)); }) //@field string
      .addProperty("generator", QR_NAME_GET_SET(GeneratorName)) //@field string
      .addProperty("save_name", QR_STRING_GET_SET(SaveName)) //@field string
      .addProperty("cloud", &Self::GetCloud, &Self::SetCloud) //@field boolean
      .addProperty("world_time", &Self::WorldTimeOfDayHours) //@field number cosmetic when frozen
      .addProperty("world_time_freeze", &Self::WorldTimeAutoAdvance) //@field boolean
      .addProperty("day_length_ticks", &Self::DayLengthTicks) //@field integer
      .addProperty("start_phase_ticks", &Self::StartPhaseTicks) //@field integer
      .addProperty("tick_rate", &Self::TickRate) //@field integer
      .endClass();
  }

  public:
  UGameSessionData();

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) const override;

  UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
  void Initialize(UObject *WorldContextObject, const FString &saveName, bool CreativeMode, bool InfiniteOre, bool AllResearchCompleted, const FString &seed, FName generatorName);

  UFUNCTION(BlueprintCallable, BlueprintPure)
  bool GetCreativeAllowed() const;

  UFUNCTION(BlueprintCallable, BlueprintPure)
  bool GetCloud() const { return Cloud; }

  UFUNCTION(BlueprintCallable)
  void SetCloud(bool value) { Cloud = value; }

  static TOptional<FVersionStruct> VersionFromString(const FString &ser);

  UFUNCTION(BlueprintCallable, BlueprintPure)
  static FString VersionToString(const FVersionStruct &version);

  UFUNCTION(BlueprintCallable)
  FString GetModsCombined() const;

  UFUNCTION(BlueprintCallable)
  int64 GetSeed() const { return GetTypeHash(Seed); }

  UFUNCTION(BlueprintCallable)
  void Reset();

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  double TotalGameTime = 0.f;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  int64 TotalGameTicks = 0;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  FString SaveName = "Default";

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  bool CreativeMode = false;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  bool CreativeAllowed = false;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  bool AllResearchCompleted = false;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  FVersionStruct Version = {};

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  TArray<FString> Mods = {};

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  bool Cloud = false;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  int32 TickRate = 400;

  // Day cycle: deterministically TotalGameTicks + DayLengthTicks + StartPhaseTicks (WorldDayCycle.h).
  // WorldTimeOfDayHours is cosmetic only — used when WorldTimeAutoAdvance is false (UI / creative lock).
  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  float WorldTimeOfDayHours = 8.0f;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  bool WorldTimeAutoAdvance = true;

  /** World ticks per full day (20 Hz logical clock). */
  UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1"))
  int64 DayLengthTicks = 144000;

  /** Phase added to TotalGameTicks before modulo DayLengthTicks. */
  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  int64 StartPhaseTicks = 48000;

  private:
  friend class UGameSessionSubsystem;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  FString Seed = "Default";

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  FName GeneratorName = "";

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  bool InfiniteOre = false;
};
