// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RailLinkable.generated.h"

class URailNodeBlockLogic;

UINTERFACE(NotBlueprintable)
class URailLinkable : public UInterface {
  GENERATED_BODY()
};

class IRailLinkable {
  GENERATED_BODY()

  public:
  virtual URailNodeBlockLogic *GetRailNode() PURE_VIRTUAL(IRailLinkable::GetRailNode, return nullptr;);
};
