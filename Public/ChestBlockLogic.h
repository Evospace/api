// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Evospace/CoordinateSystem.h"
#include "Public/StorageBlockLogic.h"
#include "LogicInterface.h"
#include "Public/LogicSignal.h"
#include "ChestBlockLogic.generated.h"

class USolidAccessor;
class UCoreAccessor;
class UInventory;
class UItemMap;

UCLASS()
class UChestBlockLogic : public UStorageBlockLogic, public ILogicInterface {
  GENERATED_BODY()
  using Self = UChestBlockLogic;
  EVO_CODEGEN_INSTANCE(ChestBlockLogic)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UStorageBlockLogic>("ChestBlockLogic") //@class ChestBlockLogic : StorageBlockLogic
      .addProperty("capacity", &Self::Capacity) //@field integer Chest slot count
      .addProperty("signal", &Self::Signal) //@field LogicSignal
      .endClass();
  }

  public:
  UChestBlockLogic();

  virtual void LoadSettings(TSharedPtr<FJsonObject> json, AMainPlayerController *mpc) override;
  virtual void SaveSettings(TSharedPtr<FJsonObject> json, AMainPlayerController *mpc) const override;

  virtual TArray<FLogicExport> GetExport_Implementation() override;
  virtual void PopulateLogicOutput(class ULogicContext *ctx) const override;

  virtual void ApplyLogicInput(const class ULogicContext *ctx) override {}

  UFUNCTION(BlueprintCallable)
  virtual void SetLimit(int32 _l);

  UFUNCTION(BlueprintCallable)
  virtual int32 GetLimit() const;

  virtual int64 GetCapacity() const override;

  virtual void BlockBeginPlay() override;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  int Capacity = 20;

  // Logic I/O configuration: which signals to export/import and control rules
  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  ULogicSignal *Signal = nullptr;

  mutable int64 LastKnownInventoryVersion = INDEX_NONE;

  // Cached snapshot of exported signals to avoid rebuilding every tick
  UPROPERTY()
  UItemMap *CachedOutput = nullptr;

  protected:
  UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  UInventory *mInventory;

  USolidAccessor *input_accessor[Side::NumSides];
};
