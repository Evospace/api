// Copyright (c) 2017 - 2026, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Public/BlockLogic.h"

#include "GhostBlockLogic.generated.h"

class UStaticBlock;
class ADimension;
class UInventoryAccess;
class AMainPlayerController;

/**
 * Placeholder block placed instead of a real machine when the player has no item.
 * Holds only the target StaticBlock and rotation (base mQuat); no accessors, no core,
 * no ticking — neighbors and networks never see it as a machine. Materialized into the
 * target block by the ghost replacer tool.
 */
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
  /** Block this ghost stands for; the ghost materializes into it. */
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  const UStaticBlock *Target = nullptr;

  /** Target footprint is free (except this ghost's own cell) and Target is buildable in `dim`. */
  bool CanMaterialize(ADimension *dim) const;

  /** Build this ghost into Target by the normal build rules: consumes Target's item from
   *  `inventory` unless `creative`, replaces the ghost with the real block, and records the edit
   *  for undo/replication via `mpc`. Returns the spawned block, or nullptr when it cannot build. */
  UBlockLogic *Materialize(ADimension *dim, UInventoryAccess *inventory, bool creative, AMainPlayerController *mpc);

  // Target travels with settings so both column saves (SerializeJson) and
  // undo/replication snapshots (CaptureCellState) restore it before the actor spawns.
  virtual void SaveSettings(TSharedPtr<FJsonObject> json, AMainPlayerController *mpc = nullptr) const override;
  virtual void LoadSettings(TSharedPtr<FJsonObject> json, AMainPlayerController *mpc = nullptr) override;
};
