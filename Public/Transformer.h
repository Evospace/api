// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Evospace/Blocks/ItemHopperInterface.h"
#include "Public/BlockLogic.h"
#include "Transformer.generated.h"

class UResourceInventory;
class UResourceAccessor;
/**
 * 
 */
UCLASS()
class EVOSPACE_API UTransformerLVMV : public UBlockLogic, public ISwitchInterface {
  GENERATED_BODY()
  using Self = UTransformerLVMV;
  EVO_CODEGEN_INSTANCE_ALIAS(TransformerLVMV, Transformer)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UTransformerLVMV, UBlockLogic>("Transformer") //@class Transformer : BlockLogic
      .addProperty("capacity", &Self::Capacity) //@field integer
      .endClass();
  }

  public:
  UTransformerLVMV();

  virtual bool IsBlockTicks() const override { return true; }

  virtual void Tick() override;

  virtual void BlockBeginPlay() override;

  virtual bool HasAction() const override { return true; }
  virtual void OnAction(const FHitResult &hit, const Vec3i &side, AItemLogic *item) override;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;

  virtual void SetSwitch_Implementation(bool val) override;
  virtual bool GetSwitch_Implementation() const override;

  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;

  UPROPERTY(VisibleAnywhere)
  UResourceInventory *LowStorage;

  UPROPERTY(VisibleAnywhere)
  UResourceInventory *HighStorage;

  UPROPERTY(VisibleAnywhere)
  TArray<UResourceAccessor *> LowAcc;

  UPROPERTY(VisibleAnywhere)
  UResourceAccessor *HighAcc;

  UPROPERTY(EditAnywhere)
  int64 Capacity = 400;

  UPROPERTY(EditAnywhere)
  bool SwitchOn = true;
};

UCLASS()
class EVOSPACE_API UTransformerMVLV : public UTransformerLVMV {
  GENERATED_BODY()
  EVO_CODEGEN_INSTANCE(BlockLogic)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UTransformerLVMV, UBlockLogic>("TransformerMVLV") //@class Transformer : BlockLogic
      .endClass();
  }

  public:
  UTransformerMVLV();
  void OnAction(const FHitResult &hit, const Vec3i &side, AItemLogic *item);
  virtual void Tick() override;
};
