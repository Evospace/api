// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.

#pragma once

#include "SingleSlotInventory.h"
#include "ResourceInventory.generated.h"

UCLASS()
class EVOSPACE_API UResourceInventory : public USingleSlotInventory {
  GENERATED_BODY()
  EVO_CODEGEN_INSTANCE(ResourceInventory)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UResourceInventory, USingleSlotInventory>("ResourceInventory") //class: ResourceInventory, parent: SingleSlotInventory
      .addProperty("item", &UResourceInventory::GetItem, &UResourceInventory::SetItem) //field: StaticItem
      .addProperty("drain", &UResourceInventory::mDrain) //field: integer
      .endClass();
  }

  const UStaticItem *GetItem() const;
  void SetItem(UStaticItem *s);

  UPROPERTY(VisibleAnywhere)
  int64 mDrain = 0;

  UPROPERTY(VisibleAnywhere)
  FColor Color = FColor::White;

  //TODO: why in single slot
  void AddResource(int64 a) {
    Data[0].mValue += a;
  }

  //TODO: why in single slot
  void RemoveResource(int64 a) {
    Data[0].mValue -= a;
  }

  int64 GetInput() const {
    return Capacity != INDEX_NONE ? Capacity : 0;
  }

  int64 GetOutput() const {
    return Capacity != INDEX_NONE ? Capacity : 0;
  }

  int64 GetFreeInput() const {
    const auto v = (Capacity != INDEX_NONE ? Capacity : 0) * 2 - _Get(0).mValue;
    return v > 0 ? v : 0;
  }

  int64 GetAvailableOutput() const {
    auto slot_value = _Get(0).mValue;
    return FMath::Min(Capacity != INDEX_NONE ? Capacity : slot_value, slot_value);
  }
};