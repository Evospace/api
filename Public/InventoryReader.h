// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Prototype.h"
#include "Public/ItemData.h"
#include <Templates/SubclassOf.h>

#include "InventoryReader.generated.h"

class UBaseInventoryWidget;
class UStaticItem;

UCLASS(Abstract, BlueprintType)
class EVOSPACE_API UInventoryReader : public UInstance {
  GENERATED_BODY()
  using Self = UInventoryReader;
  EVO_CODEGEN_INSTANCE(InventoryReader)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UInventoryReader, UInstance>("InventoryReader") //class: InventoryReader, parent: Instance
      .endClass();
  }

  // Add interface functions to this class. This is the class that will be
  // inherited to implement this interface.
  virtual int32 _Min() const {
    checkNoEntry();
    return 0;
  };

  //      - .
  virtual int32 _Max() const {
    checkNoEntry();
    return 0;
  };

  //      .
  virtual int32 _Find(const UStaticItem *item) const {
    checkNoEntry();
    return 0;
  };

  //    .
  virtual int32 _FindEmpty() const {
    checkNoEntry();
    return 0;
  };

  //     .
  virtual int32 _FindNotEmpty() const {
    checkNoEntry();
    return 0;
  };

  virtual bool IsEmpty() const {
    checkNoEntry();
    return true;
  };

  virtual int64 _Sum(const UStaticItem *item) const {
    checkNoEntry();
    return 0;
  };

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
  virtual int64 GetCapacity(int32 index) const {
    checkNoEntry();
    return 0;
  };

  UFUNCTION(BlueprintCallable)
  virtual int64 GetArmCapacity(int32 index) const {
    checkNoEntry();
    return 0;
  }

  //   .
  virtual int32 _Num() const {
    checkNoEntry();
    return 0;
  };

  UFUNCTION(BlueprintCallable)
  virtual TSubclassOf<UBaseInventoryWidgetBase> GetWidgetClass() const {
    checkNoEntry();
    return nullptr;
  };

  UFUNCTION(BlueprintCallable)
  int32 Num() const { return _Num(); }

  virtual const TArray<FItemData> &GetSlots() const {
    checkNoEntry();
    static TArray<FItemData> dummy;
    return dummy;
  }

  /*UFUNCTION(BlueprintCallable)
    bool IsEmpty() const { return IsEmpty(); }*/
};
