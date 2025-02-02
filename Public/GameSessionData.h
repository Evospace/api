// Copyright (c) 2017 - 2024, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Evospace/SerializableJson.h"
#include "Evospace/SerializableJson.h"
#include "ThirdParty/lua/lua.h"
#include "ThirdParty/luabridge/LuaBridge.h"
#include "UObject/Object.h"
#include "GameSessionData.generated.h"

class UValueStorage;

/**
 * 
 */
UCLASS()
class EVOSPACE_API UGameSessionData : public UObject, public ISerializableJson {
  GENERATED_BODY()
  public:
  UGameSessionData();

  static void lua_reg(lua_State * L) {
    luabridge::getGlobalNamespace(L)
    .deriveClass<UGameSessionData, UObject>("GameSessionData") //class: GameSessionData, parent: Object
    .addProperty("infinite_ore", &UGameSessionData::mInfiniteOre) //field: boolean
    .endClass();
  }

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;

  UFUNCTION(BlueprintCallable)
  FString GetModsCombined() const;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  int32 mAutosavePeriod = 600;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  float mTotalGameTime = 0.f;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  int64 mTotalGameTicks = 0;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  FString mSeed;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  FString mGeneratorName;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  bool mCreativeMode;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  bool mCreativeAllowed;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  bool mInfiniteOre;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  UValueStorage *mValueStorage;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  FString mVersion;

  UPROPERTY(VisibleAnywhere)
  TArray<FString> mMods;
};
