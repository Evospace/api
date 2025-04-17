// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Evospace/CoordinateSystem.h"
#include "Public/StorageBlockLogic.h"

#include "ChestBlockLogic.generated.h"

class USolidAccessor;
class UCoreAccessor;
class UInventory;

UCLASS()
class EVOSPACE_API UChestBlockLogic : public UStorageBlockLogic {
  GENERATED_BODY()
  using Self = UChestBlockLogic;
  EVO_CODEGEN_INSTANCE(ChestBlockLogic)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UStorageBlockLogic>("ChestBlockLogic") //@class ChestBlockLogic : StorageBlockLogic
      .addProperty("capacity", &Self::Capacity) //@field integer Chest slot count
      .endClass();
  }

  public:
  UChestBlockLogic();

  virtual void LoadSettings(TSharedPtr<FJsonObject> json, AMainPlayerController *mpc) override;
  virtual void SaveSettings(TSharedPtr<FJsonObject> json, AMainPlayerController *mpc) override;

  UFUNCTION(BlueprintCallable)
  virtual void SetLimit(int32 _l);

  UFUNCTION(BlueprintCallable)
  virtual int32 GetLimit() const;

  virtual int64 GetCapacity() const override;

  virtual void BlockBeginPlay() override;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  int Capacity = 20;

  protected:
  UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  UInventory *mInventory;

  USolidAccessor *input_accessor[Side::NumSides];
};
