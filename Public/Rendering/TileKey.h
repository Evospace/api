#pragma once

#include "TileKey.generated.h"

class UStaticMesh;
class UMaterialInterface;

USTRUCT()
struct FTileKey {
  GENERATED_BODY()

  UPROPERTY()
  int16 TileX = 0; // 0–3 внутри региона
  UPROPERTY()
  int16 TileY = 0; // 0–3
  UPROPERTY()
  TObjectPtr<UStaticMesh> Mesh = nullptr;
  UPROPERTY()
  TObjectPtr<UMaterialInterface> Mat = nullptr;

  friend bool operator==(const FTileKey &A, const FTileKey &B) {
    return A.TileX == B.TileX && A.TileY == B.TileY && A.Mesh == B.Mesh && A.Mat == B.Mat;
  }
};