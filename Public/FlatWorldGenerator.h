// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "Public/WorldGenerator.h"
#include "CoreMinimal.h"

#include "FlatWorldGenerator.generated.h"

class UStaticBlock;
class USurfaceDefinition;
class UBiome;

UCLASS(BlueprintType)
class UFlatWorldGenerator : public UWorldGenerator {
  GENERATED_BODY()

  public:
  UFlatWorldGenerator();

  virtual void Generate(FColumnLoaderData &data, const Vec3i &sector_pos, USurfaceDefinition *surfaceDefinition) const override;
  virtual void GenerateGlobal(FColumnMaps &data, const FVector &pos, USurfaceDefinition *surfaceDefinition, float Scale = 1.f, int32 height_width = 16, bool no_biom = false) const;
  virtual int32 GetHeight(const FVector2i &where) const;

  protected:
  virtual void Initialize() override;

  private:
  TArray<UStaticBlock *> mBlocks;
  UBiome *FlatWorldBiome = nullptr;
};
