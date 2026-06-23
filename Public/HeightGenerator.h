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

  // Seed all owned noises (each noise applies its own SeedOffset).
  void SetSeed(int32 seed);

  // Batched height-detail fill over a [w x h] region anchored at world
  // (originX, originY). Sums every noise in mNoises, each mapped from its raw
  // [-1,1] output into its authored [min, max] range. Writes into out[w*h]
  // (row-major, matching FastNoiseSIMD layout). This is the primary path used
  // by the sector generator; the result is pure per-biome detail (no global
  // base elevation), so callers add the reference elevation themselves.
  void FillHeight(float *out, int32 originX, int32 originY, int32 w, int32 h, float scale = 1.f) const;

  // Scalar spot query (single cell) of the same detail field as FillHeight.
  float GetHeight(float x, float y) const;

  // JSON authoring: "Noises" = array of NoiseGenerator names.
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

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
