// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Public/SolidInputAccessor.h"

#include "ConveyorInventorySideAccessor.generated.h"

class UConveyorBlockLogic;
class UInventoryAccess;
class UItemInstancingComponent;

class ConveyorInterface {
  public:
  virtual ~ConveyorInterface() = default;
  virtual void SetPushedSide(const Vec3i &side, RItemInstancingHandle &&item) = 0;
};

UCLASS()
class EVOSPACE_API UConveyorInventorySideAccessor : public USolidInputAccessor {
  GENERATED_BODY()
  using Self = UConveyorInventorySideAccessor;
  EVO_CODEGEN_ACCESSOR(ConveyorInventorySideAccessor)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, USolidInputAccessor>("ConveyorInventorySideAccessor") //@class ConveyorInventorySideAccessor : SolidInputAccessor
      .endClass();
  }

  public:
  virtual bool Push(UInventoryAccess *from, bool by_arm = false) override;
  virtual bool Pop(UInventoryAccess *to) override;

  virtual bool Pop(UInventoryAccess *to, int32 max_count) override;

  virtual bool PushWithData(UInventoryAccess *from, int32 max_count, RItemInstancingHandle &&item) override;

  void SetConveyor(ConveyorInterface *conveyour);

  private:
  ConveyorInterface *mConveyour;
};
