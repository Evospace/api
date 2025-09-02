// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Qr/Prototype.h"
#include "Public/ItemData.h"
#include <Templates/SubclassOf.h>

#include "InventoryReader.generated.h"

class UBaseInventoryWidget;
class UStaticItem;

UCLASS(Abstract, BlueprintType)
class UInventoryReader : public UInstance {
  GENERATED_BODY()
  using Self = UInventoryReader;
  EVO_CODEGEN_INSTANCE(InventoryReader)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UInventoryReader, UInstance>("InventoryReader") //@class InventoryReader : Instance
      .addProperty("size", [](const UInventoryReader *self) { return self->_Num(); }) //@field integer Number of slots in this InventoryReader
      //direct:
      //--- Get ItemData with index from InventoryReader
      //--- @param index integer
      //--- @return ItemData
      //function InventoryReader:get(index) end
      .addFunction("get", [](const UInventoryReader *self, int32 index) {
        return self->_SafeGet(index);
      })
      //direct:
      //--- Looking for index of StaticItem in InventoryReader
      //--- @param item StaticItem
      //--- @return integer index of found item or -1
      //function InventoryReader:find(item) end
      .addFunction("find", [](const UInventoryReader *self, const UStaticItem *item) {
        return self->_Find(item);
      })
      .endClass();
  }

  virtual int32 _Find(const UStaticItem *item) const PURE_VIRTUAL(UInventoryReader::_Find, return 0;);

  virtual bool IsEmpty() const PURE_VIRTUAL(UInventoryReader::IsEmpty, return 0;);

  virtual int64 _Sum(const UStaticItem *item) const PURE_VIRTUAL(UInventoryReader::_Sum, return 0;);

  virtual FItemData &_GetMut(int32 index) {
    checkNoEntry();
    static FItemData dummy;
    return dummy;
  };

  UFUNCTION(BlueprintCallable)
  virtual const FItemData &_Get(int32 index) const {
    checkNoEntry();
    const static FItemData dummy;
    return dummy;
  };

  UFUNCTION(BlueprintCallable)
  virtual const FItemData &_SafeGet(int32 index) const {
    checkNoEntry();
    const static FItemData dummy;
    return dummy;
  };

  UFUNCTION(BlueprintCallable)
  virtual int64 GetCapacity(int32 index) const PURE_VIRTUAL(UInventoryReader::GetCapacity, return 0;);

  virtual int32 _Num() const PURE_VIRTUAL(UInventoryReader::_Num, return 0;);

  UFUNCTION(BlueprintCallable)
  virtual TSubclassOf<UBaseInventoryWidgetBase> GetWidgetClass() const {
    checkNoEntry();
    return nullptr;
  };

  UFUNCTION(BlueprintCallable)
  int32 Num() const { return _Num(); }

  virtual TArray<FItemData> &GetSlotsMut() {
    checkNoEntry();
    static TArray<FItemData> dummy;
    return dummy;
  }

  virtual const TArray<FItemData> &GetSlots() const {
    checkNoEntry();
    static TArray<FItemData> dummy;
    return dummy;
  }

  /*UFUNCTION(BlueprintCallable)
    bool IsEmpty() const { return IsEmpty(); }*/
};
