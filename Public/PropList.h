// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "BlockLogic.h"
#include "PropListData.h"
#include "Qr/Prototype.h"
#include "Qr/CommonConverter.h"

#include "PropList.generated.h"

class UStaticProp;

UCLASS()
class UStaticPropList : public UPrototype {
  GENERATED_BODY()
  using Self = UStaticPropList;
  EVO_CODEGEN_DB(StaticPropList, StaticPropList)
  virtual void lua_reg(lua_State *L) const override {
    FPropListData::lua_reg(L);
    luabridge::getGlobalNamespace(L)
      //@comment Prototype asset that owns several PropListData records
      .deriveClass<Self, UPrototype>("StaticPropList") //@class StaticPropList : Prototype
      .addProperty("data", QR_ARRAY_GET_SET(PropListDatas)) //@field PropListData[]
      .endClass();
  }
  virtual UClass *GetSuperProto() const override {
    return StaticClass();
  }

  public:
  UPROPERTY()
  TArray<FPropListData> PropListDatas;

  virtual bool ProtoValidCheck() override;

  const UStaticProp *PickOne(FRandomStream &rnd, const Vec2i &start_point);

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  // int32 CullStart = 6000;
  // int32 CullEnd = 8000;
};