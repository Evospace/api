// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Qr/Prototype.h"
#include "HeightGenerator.generated.h"

class UNoiseGenerator;
/**
 *
 */
UCLASS()
class UHeightGenerator : public UPrototype {
  GENERATED_BODY()

  public:
  UPROPERTY()
  TArray<UNoiseGenerator *> mNoises;

  PROTOTYPE_CODEGEN(HeightGenerator, HeightGenerator)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UHeightGenerator, UPrototype>("HeightGenerator") //@class HeightGenerator : Prototype
      // direct:
      //---Add a noise generator to the height generator
      //---@param noise NoiseGenerator Noise generator to add
      // function HeightGenerator:add_noise(noise) end
      .addFunction("add_noise",
                   [](UHeightGenerator *self, UNoiseGenerator *noise) {
                     if (!noise) {
                       LOG(ERROR_LL) << "Trying to add nullptr noise generator to " << self->GetName();
                       return;
                     }
                     self->mNoises.Add(noise);
                   })
      .endClass();
  }
  virtual UClass *GetSuperProto() const override { return StaticClass(); }
};
