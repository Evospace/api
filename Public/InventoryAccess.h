// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "InventoryReader.h"
#include "Qr/Prototype.h"

#include "InventoryAccess.generated.h"
class UInventoryFilter;

UCLASS(Abstract, BlueprintType)
class EVOSPACE_API UInventoryAccess : public UInventoryReader {
  GENERATED_BODY()
  EVO_CODEGEN_INSTANCE(InventoryAccess)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UInventoryAccess, UInventoryReader>("InventoryAccess") //@class InventoryAccess : InventoryReader
      //direct:
      //--- Add item with count to InventoryAccess
      //--- @param item StaticItem
      //--- @param count integer
      //--- @return integer Remainder
      //function InventoryAccess:add(item, count) end
      .addFunction("add", [](UInventoryAccess *self, const UStaticItem *item, int64 count) {
        return self->_Add({ item, count });
      })
      //direct:
      //--- Remove item with count from InventoryAccess
      //--- @param item StaticItem
      //--- @param count integer
      //--- @return integer Remainder
      //function InventoryAccess:sub(item, count) end
      .addFunction("sub", [](UInventoryAccess *self, const UStaticItem *item, int64 count) {
        return self->_Sub({ item, count });
      })
      .addProperty("zero_slots", &UInventoryAccess::mCanHaveZeroSlot) //@field boolean
      .endClass();
  }

  public:
  virtual int64 _Add(const FItemData &other) {
    checkNoEntry();
    return 0;
  };

  virtual int64 _AddSilent(const FItemData &other) {
    return _Add(other);
  };

  virtual int64 _Add(int32 index, const FItemData &other) {
    checkNoEntry();
    return 0;
  };

  //TODO: remove
  virtual int64 _AddSilent(int32 index, const FItemData &other) {
    return _Add(index, other);
  };

  virtual int64 _AddWithLimit(const FItemData &other, bool arm = false) {
    checkNoEntry();
    return 0;
  };

  virtual int64 _Sub(const FItemData &other) {
    checkNoEntry();
    return 0;
  };

  virtual int64 _SubSilent(const FItemData &other) {
    return _Sub(other);
  };

  virtual bool _Set(int64 index, const FItemData &other) {
    checkNoEntry();
    return false;
  };

  virtual int64 _Sub(int32 index, const FItemData &other) {
    checkNoEntry();
    return 0;
  };

  virtual int64 _SubSilent(int32 index, const FItemData &other) {
    return _Sub(index, other);
  };

  UFUNCTION(BlueprintCallable)
  virtual void SetLimit(int32 _l) { mLimit = _l; };

  UFUNCTION(BlueprintCallable)
  virtual int32 GetLimit() const { return mLimit; };

  UPROPERTY(BlueprintReadOnly, EditAnywhere)
  int32 mLimit = INDEX_NONE;

  UPROPERTY(BlueprintReadOnly, EditAnywhere)
  int32 mHighlight = INDEX_NONE;

  UFUNCTION(BlueprintCallable)
  virtual void SetHighlight(int32 _l) { mHighlight = _l; };

  UFUNCTION(BlueprintCallable)
  virtual int32 GetHighlight() const { return mHighlight; };

  virtual bool GetCanHaveZeroSlot() {
    return mCanHaveZeroSlot;
  }

  virtual void SetCanHaveZeroSlot(bool value) {
    mCanHaveZeroSlot = value;
  }

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  bool Draggable = true;

  virtual UInventoryFilter *GetFilter() const {
    checkNoEntry();
    return nullptr;
  }

  virtual void TrySetFilter(int32 index, UInventoryFilter *filter) {
    checkNoEntry();
  }

  UFUNCTION(BlueprintCallable)
  virtual void SortKeyAZ() {}

  virtual void RemoveAllSlots() { checkNoEntry(); }

  virtual bool IsFrozen() { return false; }

  protected:
  UPROPERTY(EditAnywhere)
  bool mCanHaveZeroSlot = false;
};
