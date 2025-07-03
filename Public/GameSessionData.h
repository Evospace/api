// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Prototype.h"
#include "ThirdParty/luabridge/LuaBridge.h"
#include "Qr/SerializableJson.h"
#include "ThirdParty/lua/lua.h"
#include "UObject/Object.h"
#include "GameSessionData.generated.h"

class USetting;
class UValueStorage;

UCLASS(BlueprintType)
class EVOSPACE_API UGameSessionData : public UInstance {
  GENERATED_BODY()
  EVO_CODEGEN_INSTANCE(GameSessionData)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UGameSessionData, UInstance>("GameSessionData") //@class GameSessionData : Instance
      .addProperty("infinite_ore", &UGameSessionData::InfiniteOre) //@field boolean
      .endClass();
  }

  public:
  UGameSessionData();

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;

  UFUNCTION(BlueprintCallable)
  FString GetModsCombined() const;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 AutosavePeriod = 600;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float TotalGameTime = 0.f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int64 TotalGameTicks = 0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Seed;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString GeneratorName;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool CreativeMode;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool CreativeAllowed;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool InfiniteOre;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  UValueStorage *ValueStorage;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Version;

  UPROPERTY(VisibleAnywhere)
  TArray<FString> Mods;
};
