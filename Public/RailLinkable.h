// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RailLinkable.generated.h"

class URailNodeBlockLogic;

/**
 * A block that can be an endpoint of a rail segment: a plain rail node, a station,
 * or any future block that embeds a rail node. The Steel Rail tool
 * (ASteelRailItemLogic) links/unlinks blocks through this interface instead of
 * hard-casting to a concrete class. GetRailNode() returns the graph node that
 * represents the block inside the rail network (itself for a rail node / station).
 */
UINTERFACE(NotBlueprintable)
class URailLinkable : public UInterface {
  GENERATED_BODY()
};

class IRailLinkable {
  GENERATED_BODY()

  public:
  /** The rail-network node representing this block; null when it cannot link. */
  virtual URailNodeBlockLogic *GetRailNode() PURE_VIRTUAL(IRailLinkable::GetRailNode, return nullptr;);
};
