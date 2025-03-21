// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Evospace/Blocks/Accessors/BaseInventorySideAccessor.h"

#include "ResourceAccessor.generated.h"

class USingleSlotInventory;
class UResourceComponent;
UCLASS()
class EVOSPACE_API UElectricAccessor : public UBaseInventoryAccessor {
  GENERATED_BODY()
  public:
  UElectricAccessor *GetOutsideAccessorCached();
  const UElectricAccessor *GetOutsideAccessorCached() const;

  protected:
  virtual UBaseInventoryAccessor *GetAutoOutsideAccessor() override;
};

UCLASS()
class EVOSPACE_API UElectricConductorAccessor : public UAccessor {
  GENERATED_BODY()
};
UCLASS()
class EVOSPACE_API UElectricInputAccessor : public UElectricAccessor {
  GENERATED_BODY()
};
UCLASS()
class EVOSPACE_API UElectricOutputAccessor : public UElectricAccessor {
  GENERATED_BODY()
  public:
  UElectricOutputAccessor();
};

UCLASS()
class EVOSPACE_API UFluidAccessor : public UBaseInventoryAccessor {
  GENERATED_BODY()
  public:
  UFluidAccessor *GetOutsideAccessorCached();
  const UFluidAccessor *GetOutsideAccessorCached() const;

  protected:
  virtual UBaseInventoryAccessor *GetAutoOutsideAccessor() override;
};

UCLASS()
class EVOSPACE_API UFluidConductorAccessor : public UAccessor {
  GENERATED_BODY()
};
UCLASS()
class EVOSPACE_API UFluidInputAccessor : public UFluidAccessor {
  GENERATED_BODY()
};
UCLASS()
class EVOSPACE_API UFluidOutputAccessor : public UFluidAccessor {
  GENERATED_BODY()
  public:
  UFluidOutputAccessor();
};

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
      .addProperty(
        "channel", [](Self *self) -> std::string { return TCHAR_TO_UTF8(*self->Channel.ToString()); }, [](Self *self, const std::string &s) { self->Channel = UTF8_TO_TCHAR(s.data()); })
      .addFunction("init", &Self::Init)
      .endClass();
  }

  public:
  static FName LV() { return TEXT("LV"); }
  static FName MV() { return TEXT("MV"); }
  static FName HV() { return TEXT("HV"); }
  static FName Kinetic() { return TEXT("Kinetic"); }
  static FName Fluid() { return TEXT("Fluid"); }
  static FName Heat() { return TEXT("Heat"); }
  static FName Data() { return TEXT("Data"); }
  static FName Plasma() { return TEXT("Plasma"); }

  UResourceAccessor *GetOutsideAccessorCached();
  const UResourceAccessor *GetOutsideAccessorCached() const;

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

  void Init(const Vec3i &side, const Vec3i &pos, UResourceInventory *inv, bool is_input, bool is_output, const FName &channel);

  bool IsFrozen() const { return false; }

  const UStaticItem *GetResource() const;

  virtual void TickComponent() override;

  void Bind(UResourceInventory *inv) {
    mInventory = inv;
  }
};

UCLASS()
class EVOSPACE_API UKineticAccessor : public UBaseInventoryAccessor {
  GENERATED_BODY()

  public:
  UKineticAccessor *GetOutsideAccessorCached();
  const UKineticAccessor *GetOutsideAccessorCached() const;

  protected:
  virtual UBaseInventoryAccessor *GetAutoOutsideAccessor() override;
};

UCLASS()
class EVOSPACE_API UKineticConductorAccessor : public UAccessor {
  GENERATED_BODY()
};

UCLASS()
class EVOSPACE_API UKineticInputAccessor : public UKineticAccessor {
  GENERATED_BODY()
};

UCLASS()
class EVOSPACE_API UKineticOutputAccessor : public UKineticAccessor {
  GENERATED_BODY()

  public:
  UKineticOutputAccessor();
};

UCLASS()
class EVOSPACE_API UDataAccessor : public UDataInventoryAccessor {
  GENERATED_BODY()
  public:
  UDataAccessor *GetOutsideAccessorCached();
  const UDataAccessor *GetOutsideAccessorCached() const;

  protected:
  virtual UBaseInventoryAccessor *GetAutoOutsideAccessor() override;
};

UCLASS()
class EVOSPACE_API UDataConductorAccessor : public UAccessor {
  GENERATED_BODY()
};
UCLASS()
class EVOSPACE_API UDataInputAccessor : public UDataAccessor {
  GENERATED_BODY()
};
UCLASS()
class EVOSPACE_API UDataOutputAccessor : public UDataAccessor {
  GENERATED_BODY()
  public:
  UDataOutputAccessor();
};