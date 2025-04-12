// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "BlockLogic.h"
#include "PropListData.h"
#include "Prototype.h"
#include "Evospace/Misc/AssetOwner.h"

#include "PropList.generated.h"

class UStaticProp;

UCLASS()
class UStaticPropList : public UPrototype {
  GENERATED_BODY()
  EVO_OWNER(StaticPropList)
  EVO_CODEGEN_DB(StaticPropList, StaticPropList)
  virtual void lua_reg(lua_State *L) const override {
    FPropListData::lua_reg(L);
    luabridge::getGlobalNamespace(L)
      .deriveClass<UStaticPropList, UPrototype>("StaticPropList") //@class StaticPropList : Prototype
      .addProperty("data", [](const UStaticPropList * self) //@field PropListData[]
      {
        std::vector<const FPropListData *> arr;
        for (auto & p :self->PropListDatas) {
          arr.push_back(&p);
        }
        return arr;
      })
      .endClass();
  }

  public:
  UPROPERTY()
  TArray<FPropListData> PropListDatas;

  virtual bool ProtoValidCheck() override;

  const UStaticProp *PickOne(const Vec2i &start_point);

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  // int32 CullStart = 6000;
  // int32 CullEnd = 8000;
};