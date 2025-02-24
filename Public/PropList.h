// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.

#pragma once

#include "BlockLogic.h"
#include "Prototype.h"
#include "../SerializableJson.h"
#include "Evospace/Misc/AssetOwner.h"

#include "PropList.generated.h"

class UStaticProp;

USTRUCT()
struct FPropListData {
  GENERATED_BODY()

  public:
  UPROPERTY()
  TArray<const UStaticProp *> Props;

  UPROPERTY()
  float Chance = 1.0;
};

UCLASS()
class UStaticPropList : public UPrototype {
  GENERATED_BODY()
  EVO_OWNER(StaticPropList)
  EVO_CODEGEN_DB(StaticPropList, StaticPropList)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UStaticPropList, UPrototype>("StaticPropList") //@class StaticPropList : Prototype
      .endClass();
  }

  public:
  UPROPERTY()
  TArray<FPropListData> PropListDatas;

  const UStaticProp *PickOne(const Vec2i &start_point);

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  // int32 CullStart = 6000;
  // int32 CullEnd = 8000;
};