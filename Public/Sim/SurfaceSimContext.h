// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ISimSurface.h"
#include "SurfaceSimContext.generated.h"

class USurfaceDefinition;

UCLASS()
class USurfaceSimContext : public UObject, public ISimSurface {
  GENERATED_BODY()
public:
  // Initialization
  void Init(const FGuid& InSurfaceId, USurfaceDefinition* InSurfaceDef);

  // Tick simulation (no-op for now)
  void TickContext(float DeltaSeconds);

  // ISimSurface
  virtual const FGuid& GetSurfaceId() const override { return SurfaceId; }
  virtual bool GetBlock(const Vec3i& pos, const UStaticBlock*& outBlock, BlockDensity& outDensity) const override;
  virtual void SetBlock(const Vec3i& pos, const UStaticBlock* block, const BlockDensity& density) override;

  // Accessors
  USurfaceDefinition* GetSurfaceDefinition() const { return SurfaceDef; }

private:
  UPROPERTY()
  USurfaceDefinition* SurfaceDef = nullptr;

  UPROPERTY()
  FGuid SurfaceId;
};


