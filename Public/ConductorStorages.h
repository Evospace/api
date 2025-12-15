// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "ConductorBlockLogic.h"

#include "ConductorStorages.generated.h"

UCLASS()
class UElectricityContainerBlockLogic : public UConductorBlockLogic {
  GENERATED_BODY()

  using Self = UElectricityContainerBlockLogic;

  EVO_CODEGEN_INSTANCE(ElectricityContainerBlockLogic)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UConductorBlockLogic>("ElectricityContainerBlockLogic") //@class ElectricityContainerBlockLogic : ConductorBlockLogic
      .endClass();
  }

  public:
  UElectricityContainerBlockLogic();

  virtual void BlockBeginPlay() override;

  virtual int32 GetChannel() const override;
};