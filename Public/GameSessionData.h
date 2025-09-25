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
      .addProperty("total_game_time", &Self::TotalGameTime) //@field double
      .addProperty("total_game_ticks", &Self::TotalGameTicks) //@field integer
      .addProperty("seed", &Self::Seed) //@field string
      .addProperty("generator", QR_STRING_GET_SET(GeneratorName)) //@field string
      .addProperty("save_name", QR_STRING_GET_SET(SaveName)) //@field string
      .addProperty("version", &Self::Version) //@field string
      .endClass();
  }

  public:
  UGameSessionData();

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;

  UFUNCTION(BlueprintCallable, BlueprintPure)
  bool GetCreativeAllowed() const;

  UFUNCTION(BlueprintCallable)
  FString GetModsCombined() const;

  UFUNCTION(BlueprintCallable)
  int64 GetSeed() const { return GetTypeHash(Seed); }

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  double TotalGameTime = 0.f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int64 TotalGameTicks = 0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Seed;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString GeneratorName;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString SaveName;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool CreativeMode = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool CreativeAllowed = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool InfiniteOre = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool AllResearchCompleted = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Version;

  UPROPERTY(VisibleAnywhere)
  TArray<FString> Mods;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool Cloud = false;
};
