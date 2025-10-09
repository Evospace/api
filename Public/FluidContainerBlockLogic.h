#pragma once
#include "ConductorBlockLogic.h"
#include "FluidContainerBlockLogic.generated.h"

UCLASS()
class UFluidContainerBlockLogic : public UConductorBlockLogic {
  using Self = UFluidContainerBlockLogic;
  EVO_CODEGEN_INSTANCE(FluidContainerBlockLogic)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UConductorBlockLogic>(
        "FluidContainerBlockLogic") //@class FluidContainerBlockLogic : ConductorBlockLogic
      .addProperty("capacity", &Self::Capacity) //@field integer
      .addProperty("charge", &Self::GetCharge) //@field integer
      .endClass();
  }
  GENERATED_BODY()
  public:
  UFluidContainerBlockLogic();
  virtual UCoreAccessor *CoreInit() override;
  virtual bool IsResourceStorage() const override { return true; }
  virtual bool IsBatteryContainer() const override { return false; }

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  virtual int64 GetCapacity() const override;
  virtual int64 GetCharge() const override;

  virtual void BlockBeginPlay() override;

  virtual void UpdateSides(UAccessor *except = nullptr) override;

  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;
  virtual EBlockWidgetType GetWidgetType() const override;

  virtual int32 GetChannel() const override;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  int64 Capacity = 32;

  protected:
  UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  UResourceInventory *mInventory;
};