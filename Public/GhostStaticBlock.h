// Copyright (c) 2017 - 2026, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Public/StaticBlock.h"
#include "Public/BlockActor.h"

#include "GhostStaticBlock.generated.h"

/**
 * Actor of a placed ghost block: renders the target block's build hologram (selector or
 * actor class meshes with the hologram material) and never collides. The live machine
 * actor class is intentionally not used as-is — its blueprint may expect a real logic type.
 */
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

/**
 * Prototype of the single universal "Ghost" block. Logic and actor classes are fixed in
 * C++; the visual comes from the per-instance UGhostBlockLogic::Target, so one prototype
 * covers every machine.
 */
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

  /** Spawns AGhostBlockActor; skips lua actor hooks (actor_init expects a live machine actor). */
  virtual ABlockActor *SpawnActorAndLuaDeferred(ADimension *world, UBlockLogic *bloc_logic, const FTransform &tr) const override;
};
