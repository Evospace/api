// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Public/SolidAccessor.h"

#include "SolidInputAccessor.generated.h"

UCLASS()
class USolidInputAccessor : public USolidAccessor {
  GENERATED_BODY()
  using Self = USolidInputAccessor;
  EVO_CODEGEN_ACCESSOR(SolidInputAccessor)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, USolidAccessor>("SolidInputAccessor") //@class SolidInputAccessor : SolidAccessor
      .endClass();
  }
};
