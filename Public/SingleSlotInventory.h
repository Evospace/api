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
      .deriveClass<USingleSlotInventory, UBaseInventory>("SingleSlotInventory") //class: SingleSlotInventory, parent: BaseInventory
      .addProperty("capacity", &Self::Capacity) //field: integer|nil
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
class EVOSPACE_API UNonSerializedSingleSlotInventory : public USingleSlotInventory {
  GENERATED_BODY()
};