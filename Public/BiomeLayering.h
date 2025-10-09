#pragma once

#include "CoreMinimal.h"
#include "Qr/Prototype.h"

#include "map"

#include "BiomeLayering.generated.h"

class UStaticBlock;

USTRUCT()
struct FLayeringNode {
  GENERATED_BODY()

  FLayeringNode(UStaticBlock *_block) : block(_block) {}

  FLayeringNode() {}

  UStaticBlock *block = nullptr;
};

USTRUCT()
struct FLayering {
  GENERATED_BODY()

  std::map<unsigned short, FLayeringNode, std::greater<unsigned short>> layers;

  UStaticBlock *at(int depth) {
    const auto &l_node = layers.lower_bound(depth);

    if (ensure(l_node != layers.end())) {
      return std::get<1>(*l_node).block;
    }

    return nullptr;
  }
};