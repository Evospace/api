#pragma once

#include "Qr/CoordinameMinimal.h"

class UStaticProp;
class UStaticPropList;

enum class EPropPlacementMode : uint8 {
  Scatter,
  Cluster,
};

struct FPropClusterSettings {
  float MinRadius = 2.f;
  float MaxRadius = 6.f;
  int32 MinCount = 3;
  int32 MaxCount = 8;
  float CenterSpacing = 16.f;
  float Jitter = 1.f;
  float DensityThreshold = 0.55f;
  float CellFrequency = 0.f;
};

struct FPropLayer {
  EPropPlacementMode Mode = EPropPlacementMode::Scatter;
  UStaticPropList *PropList = nullptr;
  float Density = 0.f;
  FPropClusterSettings Cluster;
  int32 SeedOffset = 0;
};

struct FPropCandidate {
  Vec2i WorldCell{ 0, 0 };
  const UStaticProp *Prop = nullptr;
  int32 LayerIndex = 0;
};

inline int32 HashPropCell(int32 seed, int32 x, int32 y) {
  return seed ^ (x * 73856093) ^ (y * 19349663);
}
