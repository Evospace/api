// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "Containers/Array.h"
#include "CoreMinimal.h"
#include "ItemData.h"
#include "Qr/Prototype.h"

#include "InventoryFilter.generated.h"
class UInventoryAccess;
class UStaticItem;

UCLASS(Abstract, BlueprintType)
class EVOSPACE_API UInventoryFilter : public UInstance {
  GENERATED_BODY()
  EVO_CODEGEN_INSTANCE(InventoryFilter)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UInventoryFilter, UInstance>("InventoryFilter") //@class InventoryFilter : Instance
      .endClass();
  }

  public:
  virtual bool Check(const FItemData &data);
};
