// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "BaseInventory.h"
#include "Public/StaticItem.h"

#include "SingleSlotInventory.generated.h"

UCLASS()
class USingleSlotInventory : public UBaseInventory {
  GENERATED_BODY()
  using Self = USingleSlotInventory;
  EVO_CODEGEN_INSTANCE(SingleSlotInventory)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<USingleSlotInventory, UBaseInventory>(
        "SingleSlotInventory") //@class SingleSlotInventory : BaseInventory
      .addProperty("capacity", &Self::Capacity) //@field integer
      // direct:
      //---@field item StaticItem
      // function SingleSlotInventory:set_simple_filter(item) end
      .addFunction("set_simple_filter", &Self::SetSimpleFilter)
      .endClass();
  }

  public:
  USingleSlotInventory();
  virtual bool _Set(int64 index, const FItemData &other) override;

  void SetSimpleFilter(const UStaticItem *item);

  virtual void TrySetFilter(int32 index, UInventoryFilter *filter) override;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) const override;

  void SetFreeze(bool cond);
  virtual bool IsFrozen() override;

  virtual bool IsEmpty() const override;

  virtual const FItemData &_SafeGet(int32 index) const override;

  virtual const FItemData &_Get(int32 index) const override;

  private:
  bool mFrozen = false;
};

UCLASS()
class UNonSerializedSingleSlotInventory : public USingleSlotInventory {
  GENERATED_BODY()
};