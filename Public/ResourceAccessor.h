// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Public/AccessorComponent.h"
#include "Qr/CommonConverter.h"

#include "ResourceAccessor.generated.h"

class USingleSlotInventory;
class UResourceComponent;

UCLASS()
class UResourceAccessor : public UAccessor {
  using Self = UResourceAccessor;
  GENERATED_BODY()
  EVO_CODEGEN_ACCESSOR(ResourceAccessor)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UAccessor>("ResourceAccessor") //@class ResourceAccessor : Accessor
      .addProperty("inventory", &Self::mInventory) //@field ResourceInventory
      .addProperty("is_input", &Self::IsInput) //@field boolean
      .addProperty("is_output", &Self::IsOutput) //@field boolean
      .addProperty("channel", QR_NAME_GET_SET(Channel)) //@field string
      .addFunction("init", &Self::Init)
      .endClass();
  }

  public:
  static FName Electricity() { return TEXT("Electricity"); }
  static FName Kinetic() { return TEXT("Kinetic"); }
  static FName Fluid() { return TEXT("Fluid"); }
  static FName Heat() { return TEXT("Heat"); }
  static FName Data() { return TEXT("Data"); }
  static FName Plasma() { return TEXT("Plasma"); }

  UResourceAccessor *GetOutsideNeighborSameTypeCached();

  void AddInput(const UStaticItem *item, int64 a) const;
  void RemoveOutput(int64 a) const;

  int64 GetFreeInput() const;
  int64 GetAvailableOutput() const;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  class UResourceInventory *mInventory = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool IsInput = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool IsOutput = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FName Channel = TEXT("Error");

  void Init(const Vec3i &side, const Vec3i &pos, UResourceInventory *inv, bool is_input, bool is_output, FName channel);

  bool IsFrozen() const { return false; }

  const UStaticItem *GetResource() const;

  virtual void TickComponent() override;

  void Bind(UResourceInventory *inv) { mInventory = inv; }
};