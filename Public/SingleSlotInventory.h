// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.

#pragma once

#include "BaseInventory.h"
#include "Public/StaticItem.h"

#include "SingleSlotInventory.generated.h"

UCLASS()
class EVOSPACE_API USingleSlotInventory : public UBaseInventory {
  GENERATED_BODY()
  using Self = USingleSlotInventory;
  EVO_CODEGEN_INSTANCE(SingleSlotInventory)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<USingleSlotInventory, UBaseInventory>("SingleSlotInventory")
      .addProperty("arm_capacity", &USingleSlotInventory::ArmCapacity)
      .addProperty("capacity", &Self::Capacity)
      .endClass();
  }

  public:
  USingleSlotInventory();
  virtual bool _Set(int64 index, const FItemData &other) override;

  void SetSimpleFilter(const UStaticItem *item);

  virtual void TrySetFilter(int32 index, UInventoryFilter *filter) override;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;

  void SetFreeze(bool cond);
  virtual bool IsFrozen() override;

  virtual bool IsEmpty() const override;

  virtual const FItemData &_SafeGet(int32 index) const override;

  virtual const FItemData &_Get(int32 index) const override;

  private:
  bool mFrozen = false;
};

UCLASS()
class EVOSPACE_API UResourceInventory : public USingleSlotInventory {
  GENERATED_BODY()
  EVO_CODEGEN_INSTANCE(ResourceInventory)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UResourceInventory, USingleSlotInventory>("ResourceInventory")
      .addProperty("item", &UResourceInventory::GetItem, &UResourceInventory::SetItem)
      .addProperty("drain", &UResourceInventory::mDrain)
      .endClass();
  }

  const UStaticItem *GetItem() const;
  void SetItem(UStaticItem *s);

  int64 mDrain = 0;

  //TODO: why in single slot
  void AddResource(int64 a) {
    Data[0].mValue += a;
  }

  //TODO: why in single slot
  void RemoveResource(int64 a) {
    Data[0].mValue -= a;
  }

  int64 GetInput() const {
    return Capacity.value_or(0);
  }

  int64 GetOutput() const {
    return Capacity.value_or(0);
  }

  int64 GetFreeInput() const {
    return FMath::Abs(Capacity.value_or(0) - _Get(0).mValue);
  }

  int64 GetAvailableOutput() const {
    auto slot_value = _Get(0).mValue;
    return FMath::Min(Capacity.value_or(slot_value), slot_value);
  }
};

UCLASS()
class EVOSPACE_API UNonSerializedSingleSlotInventory : public USingleSlotInventory {
  GENERATED_BODY()
};