#pragma once
#include "Evospace/CoordinameMinimal.h"

class SimpleNoise {
  struct HashVec2 {
    std::size_t operator()(const Vec2i &v) const {
      std::size_t hashX = std::hash<int>()(v.X);
      std::size_t hashY = std::hash<int>()(v.Y);
      return hashX ^ (hashY << 1);
    }
  };

  public:
  static float generateOffset(const Vec2i &v) {
    HashVec2 hasher;
    std::size_t hashValue = hasher(v);
    return static_cast<float>(std::abs(static_cast<int>(hashValue) % 1000)) / 1000.0f;
  }
};