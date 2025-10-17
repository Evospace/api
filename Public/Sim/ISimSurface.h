// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Qr/CoordinameMinimal.h"

class UStaticBlock;
struct BlockDensity;

// Lightweight interface exposed by a simulation surface to game systems and logics.
class ISimSurface {
  public:
  virtual ~ISimSurface() = default;

  // Identity
  virtual const FGuid &GetSurfaceId() const = 0;

  // Block data access (authoritative in simulation)
  virtual bool GetBlock(const Vec3i &pos, const UStaticBlock *&outBlock, BlockDensity &outDensity) const = 0;
  virtual void SetBlock(const Vec3i &pos, const UStaticBlock *block, const BlockDensity &density) = 0;
};
