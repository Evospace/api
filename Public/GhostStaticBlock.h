// Copyright (c) 2017 - 2026, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Public/StaticBlock.h"
#include "Public/BlockActor.h"

#include "GhostStaticBlock.generated.h"

UCLASS()
class AGhostBlockActor : public ABlockActor {
  GENERATED_BODY()

  public:
  AGhostBlockActor();

  protected:
  virtual void BeginPlay() override;

  private:
  UPROPERTY()
  UChildActorComponent *VisualChild = nullptr;
};

UCLASS(BlueprintType)
class UGhostStaticBlock : public UStaticBlock {
  GENERATED_BODY()
  using Self = UGhostStaticBlock;

  public:
  PROTOTYPE_CODEGEN(GhostStaticBlock, StaticBlock)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UStaticBlock>("GhostStaticBlock") //@class GhostStaticBlock : StaticBlock
      .endClass();
  }
  virtual UClass *GetSuperProto() const override { return UStaticBlock::StaticClass(); }

  UGhostStaticBlock();

  virtual ABlockActor *SpawnActorAndLuaDeferred(ADimension *world, UBlockLogic *bloc_logic, const FTransform &tr) const override;
};
