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
  /** Cellular noise gate in [0, 1]; cluster center spawns when cell value >= threshold. */
  float DensityThreshold = 0.55f;
  /** Cellular noise frequency (1 / world cells). Defaults to 1 / CenterSpacing when unset. */
  float CellFrequency = 0.f;
};

struct FPropLayer {
  EPropPlacementMode Mode = EPropPlacementMode::Scatter;
  UStaticPropList *PropList = nullptr;
  /** Scatter: per-cell spawn probability in [0, 1]. */
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
