// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "Qr/CoordinameMinimal.h"
#include "ConveyorInterface.generated.h"

class UInventoryAccess;
struct RItemInstancingHandle;

UINTERFACE()
class UConveyorInterface : public UInterface {
  GENERATED_BODY()
};

class IConveyorInterface {
  GENERATED_BODY()
  public:
  virtual void SetPushedSide(const Vec3i &side, RItemInstancingHandle &&item) = 0;
  virtual UInventoryAccess *GetInputAccess() const = 0;
  virtual UInventoryAccess *GetOutputAccess() const = 0;
};
