// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Public/SolidAccessor.h"

#include "SolidOutputAccessor.generated.h"

UCLASS()
class EVOSPACE_API USolidOutputAccessor : public USolidAccessor {
  GENERATED_BODY()
  using Self = USolidOutputAccessor;
  EVO_CODEGEN_ACCESSOR(SolidOutputAccessor)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, USolidAccessor>("SolidOutputAccessor") //@class SolidOutputAccessor : SolidAccessor
      .endClass();
  }

  public:
  USolidOutputAccessor();
};

inline USolidOutputAccessor::USolidOutputAccessor() {
  SetAutoOutput(true);
}