// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.
#pragma once
#include "Public/TieredBlockLogic.h"

#include "StorageBlockLogic.generated.h"

class UResourceComponent;

UCLASS()
class EVOSPACE_API UStorageBlockLogic : public UBlockLogic {
  using Self = UStorageBlockLogic;
  EVO_CODEGEN_INSTANCE(StorageBlockLogic)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UBlockLogic>("StorageBlockLogic") //class: StorageBlockLogic, parent: BlockLogic
      .endClass();
  }
  GENERATED_BODY()
  public:
  UStorageBlockLogic();

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  UInventoryAccess *mStorageAccess = nullptr;

  UFUNCTION(BlueprintCallable, BlueprintPure)
  virtual int64 GetCapacity() const;

  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;
};