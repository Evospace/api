// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Evospace/Props/StaticIndexedHierarchicalInstancedStaticMeshComponent.h"
#include "Public/AccessorComponent.h"
#include "Public/ConveyorInterface.h"
#include "Evospace/Blocks/Accessors/InventoryAccessor.h"
#include "BaseInventoryAccessor.generated.h"

class UInventoryReader;
class UBaseInventoryAccessor;
class UInventoryAccess;
class UItemInstancingComponent;
class UStaticItem;

UCLASS()
class UBaseInventoryAccessor : public UAccessor, public IInventoryAccessor {
  GENERATED_BODY()
  using Self = UBaseInventoryAccessor;
  EVO_CODEGEN_ACCESSOR(BaseInventoryAccessor)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UAccessor>("BaseInventoryAccessor") //@class BaseInventoryAccessor : Accessor
      .addProperty("auto_output", &UBaseInventoryAccessor::mAutoOutput) //@field boolean
      .addProperty("input", &UBaseInventoryAccessor::mInput) //@field InventoryAccess
      .addProperty("output", &UBaseInventoryAccessor::mOutput) //@field InventoryAccess
      .endClass();
  }

  public:
  UBaseInventoryAccessor();

  void SetAutoOutput(bool value);

  virtual void Bind(UInventoryAccess *inventory) override;
  virtual void BindInput(UInventoryAccess *inventory) override;
  virtual void BindOutput(UInventoryAccess *inventory) override;

  void Unbind() override;
  void UnbindInput() override;
  void UnbindOutput() override;

  bool Push();

  virtual bool Push(UInventoryAccess *from, bool by_arm = false) override;
  virtual bool PushWithData(UInventoryAccess *from, int32 max_count, RItemInstancingHandle &&item);

  virtual bool Pop(UInventoryAccess *to) override;
  virtual bool Pop(UInventoryAccess *to, int32 max_count) override;

  virtual FItemData PopItem(UInventoryAccess *to, const UStaticItem *item) override;
  virtual FItemData PopItem(UInventoryAccess *to, const FItemData &data) override;

  virtual UInventoryAccess *GetInput() override;
  virtual UInventoryAccess *GetOutput() override;

  UBaseInventoryAccessor *GetOutsideNeighborSameTypeCached();

  void SetConveyor(TScriptInterface<IConveyorInterface> conveyour) {
    mConveyor = conveyour;
  }

  template <typename _Ty>
  _Ty *_GetInput() {
    return Cast<_Ty>(mInput);
  }

  template <typename _Ty>
  _Ty *_GetOutput() {
    return Cast<_Ty>(mOutput);
  }

  protected:
  virtual void TickComponent() override;

  protected:
  UPROPERTY(VisibleAnywhere)
  bool mAutoOutput = false;

  UPROPERTY(VisibleAnywhere)
  UInventoryAccess *mInput = nullptr;

  UPROPERTY(VisibleAnywhere)
  UInventoryAccess *mOutput = nullptr;

  private:
  UPROPERTY()
  TScriptInterface<IConveyorInterface> mConveyor;
};