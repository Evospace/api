// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.
#pragma once
#include "BaseInventory.h"
#include "CoreMinimal.h"
#include "Prototype.h"

#include <Containers/Array.h>
#include <Templates/SubclassOf.h>

#include "Inventory.generated.h"
class UInventoryFilter;

UCLASS(BlueprintType)
class EVOSPACE_API UInventory : public UBaseInventory {
  GENERATED_BODY()
  EVO_CODEGEN_INSTANCE(Inventory)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UInventory, UBaseInventory>("Inventory") //class: Inventory, parent: BaseInventory
      .endClass();
  }

  virtual const TArray<FItemData> &GetSlots() const override;

  void Resize(int32 size);

  virtual void SortKeyAZ() override;

  void Sort(const TFunction<bool(const FItemData &l, const FItemData &r)> &f);

  void SetAutoSize(bool autosize);

  bool GetAutoSize() const;

  void PopSlot();

  //    .
  int32 _AddEmpty();

  //    .
  FItemData &_AddEmpty_GetRef();

  virtual void TrySetFilter(int32 index, UInventoryFilter *filter) override;

  virtual bool _Set(int64 index, const FItemData &other) override;
};