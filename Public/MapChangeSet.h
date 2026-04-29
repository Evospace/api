// Copyright (c) 2017 - 2026, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Evospace/World/BlockDensity.h"
#include "Qr/CoordinameMinimal.h"
#include "Serialization/JsonTypes.h"

class UStaticBlock;

/** Direction for applying a map change (undo uses Before, redo uses After). */
enum class EMapChangeDirection : uint8 {
  Before = 0,
  After = 1
};

/**
 * Serialized snapshot of one block cell — world static mesh data plus optional root block logic payload.
 * Part positions store StaticBlock/Density matching the blueprint; LogicJson is only set at the logic root cell.
 */
struct EVOSPACE_API FMapCellState {
  Vec3i Pos{};
  const UStaticBlock *StaticBlock = nullptr;
  BlockDensity Density{};
  FQuat Rotation = FQuat::Identity;

  /** When set, restores full UBlockLogic from JSON using the same format as Logic.bin saves. Root cell only. */
  TSharedPtr<FJsonObject> LogicJson;

  bool IsEmptyTerrain() const { return StaticBlock == nullptr && Density.Sum() == 0 && !LogicJson.IsValid(); }

  bool Equals(const FMapCellState &O) const;
};

/** One edited cell — keeps both endpoints for reversible undo/redo. */
struct EVOSPACE_API FMapCellChange {
  Vec3i Pos{};
  FMapCellState Before;
  FMapCellState After;
};

/** Atomic map patch: apply After for the action, Before to roll back. */
struct EVOSPACE_API FMapChangeSet {
  FGuid Id{};
  int32 Sequence = 0;
  TArray<FMapCellChange> Cells;
};
