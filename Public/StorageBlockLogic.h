// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "Public/TieredBlockLogic.h"

#include "StorageBlockLogic.generated.h"

class UResourceComponent;

UCLASS()
class UStorageBlockLogic : public UBlockLogic {
  using Self = UStorageBlockLogic;
  EVO_CODEGEN_INSTANCE(StorageBlockLogic)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UBlockLogic>("StorageBlockLogic") //@class StorageBlockLogic : BlockLogic
      .addProperty("storage_access", &Self::mStorageAccess) //field InventoryAccess
      .endClass();
  }
  GENERATED_BODY()
  public:
  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  UInventoryAccess *mStorageAccess = nullptr;

  UFUNCTION(BlueprintCallable, BlueprintPure)
  virtual int64 GetCapacity() const;

  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;
};