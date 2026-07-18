// Copyright (c) 2017 - 2026, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Public/BlockLogic.h"

#include "GhostBlockLogic.generated.h"

class UStaticBlock;
class ADimension;
class UInventoryAccess;
class AMainPlayerController;

UCLASS(BlueprintType)
class UGhostBlockLogic : public UBlockLogic {
  GENERATED_BODY()
  using Self = UGhostBlockLogic;
  EVO_CODEGEN_INSTANCE(GhostBlockLogic)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UBlockLogic>("GhostBlockLogic") //@class GhostBlockLogic : BlockLogic
      .addProperty("target", &Self::Target) //@field StaticBlock Block this ghost stands for
      .endClass();
  }

  public:
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  const UStaticBlock *Target = nullptr;

  bool CanMaterialize(ADimension *dim) const;

  UBlockLogic *Materialize(ADimension *dim, UInventoryAccess *inventory, bool creative, AMainPlayerController *mpc);

  virtual void SaveSettings(TSharedPtr<FJsonObject> json, AMainPlayerController *mpc = nullptr) const override;
  virtual void LoadSettings(TSharedPtr<FJsonObject> json, AMainPlayerController *mpc = nullptr) override;
};
