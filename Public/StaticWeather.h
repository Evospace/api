// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Qr/Prototype.h"
#include "WeatherTypes.h"
#include "StaticWeather.generated.h"

/**
 * Static data describing a single weather preset/state.
 * Tunable in the editor and referenced by systems to apply runtime weather.
 */
UCLASS(BlueprintType)
class UStaticWeather : public UPrototype {
  GENERATED_BODY()
  public:
  PROTOTYPE_CODEGEN(StaticWeather, StaticWeather)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UStaticWeather, UPrototype>("StaticWeather") //@class StaticWeather : Prototype
      .addProperty("cloudiness01", &Self::Cloudiness01) //@field number 0..1
      .addProperty("precipitation01", &Self::Precipitation01) //@field number 0..1
      .addProperty("fog01", &Self::Fog01) //@field number 0..1
      .addProperty("storminess01", &Self::Storminess01) //@field number 0..1
      .addProperty("wind_speed", &Self::WindSpeed) //@field number >=0
      .addProperty("selection_weight", &Self::SelectionWeight) //@field integer >=0
      .endClass();
  }
  virtual UClass *GetSuperProto() const override { return StaticClass(); }
  using Self = UStaticWeather;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0.0", ClampMax = "1.0"))
  float Cloudiness01 = 0.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0.0", ClampMax = "1.0"))
  float Precipitation01 = 0.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0.0", ClampMax = "1.0"))
  float Fog01 = 0.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0.0", ClampMax = "1.0"))
  float Storminess01 = 0.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0.0"))
  float WindSpeed = 0.0f;

  /** Base selection weight for random weather picking. Higher values make this preset more likely. Integer. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0"))
  int32 SelectionWeight = 1;

  /** Minimum duration this weather lasts (seconds) */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0.0", ToolTip = "Minimum duration this weather lasts in seconds"))
  float MinDurationSeconds = 60.0f;

  /** Maximum duration this weather lasts (seconds). Random duration will be between MinDurationSeconds and MaxDurationSeconds */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0.0", ToolTip = "Maximum duration this weather lasts in seconds. Random duration will be between MinDurationSeconds and MaxDurationSeconds"))
  float MaxDurationSeconds = 300.0f;

  public:
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
};
