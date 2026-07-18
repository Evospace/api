// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "Public/TieredBlockLogic.h"
#include "Public/BlockLogic.h"

#include "StorageBlockLogic.generated.h"

class UResourceComponent;
class UItemMap;
class ULogicContext;

UCLASS()
class UStorageBlockLogic : public UBlockLogic {
  using Self = UStorageBlockLogic;
  EVO_CODEGEN_INSTANCE(StorageBlockLogic)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UBlockLogic>("StorageBlockLogic") //@class StorageBlockLogic : BlockLogic
      .addProperty("storage_access", &Self::mStorageAccess) // field InventoryAccess
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

  // Default storage->logic export: publishes mStorageAccess contents (item -> count)
  // as logic signals when export option `exportIndex` is enabled. Shared by every
  // storage block (Chest, ItemRack, ...) so the version-cached export lives in one place.
  virtual void PopulateLogicOutput(class ULogicContext *ctx) const override;

  protected:
  // Publish storage contents to ctx->Output, rebuilding the cache only when the
  // backing inventory version changed. No-op when the export option is disabled
  // or the block has no storage / no logic settings.
  void ExportStorageContents(class ULogicContext *ctx, int32 exportIndex = 0) const;

  // Force the next ExportStorageContents call to rebuild from scratch.
  // Call after load / resize / contents reset so stale signals are not exported.
  void ResetStorageExportCache();

  // Cached snapshot of exported signals, avoids rebuilding every tick.
  UPROPERTY()
  UItemMap *mCachedStorageOutput = nullptr;

  // Inventory version the cache was last built from; transient, not serialized.
  mutable int64 mLastExportedStorageVersion = INDEX_NONE;
};