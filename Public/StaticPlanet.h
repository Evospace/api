// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Qr/Prototype.h"
#include "WorldDayCycle.h"
#include "StaticPlanet.generated.h"

/**
 * Static definition of a celestial body / playable surface with its own day cycle in world ticks.
 * Shares TotalGameTicks with GameSessionData; day length and phase offsets are int64.
 */
UCLASS(BlueprintType)
class UStaticPlanet : public UPrototype {
  GENERATED_BODY()

  public:
  PROTOTYPE_CODEGEN(StaticPlanet, StaticPlanet)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UStaticPlanet, UPrototype>("StaticPlanet") //@class StaticPlanet : Prototype
      .addProperty("day_length_ticks", &Self::DayLengthTicks) //@field integer
      .addProperty("phase_offset_ticks", &Self::PhaseOffsetTicks) //@field integer
      .addProperty("dawn_phase_ticks", &Self::DawnPhaseTicks) //@field integer
      .addProperty("solar_noon_phase_ticks", &Self::SolarNoonPhaseTicks) //@field integer
      .addProperty("sunset_phase_ticks", &Self::SunsetPhaseTicks) //@field integer
      .addFunction("cosmetic_hours_from_day_phase_ticks", &Self::GetCosmeticDayHoursFromDayPhaseTicks)
      .addFunction("time_of_day_hours_from_ticks", &Self::LuaTimeOfDayHoursFromTicks)
      .addFunction("resolve_time_of_day_hours", &Self::ResolveTimeOfDayHours)
      .endClass();
  }
  virtual UClass *GetSuperProto() const override { return StaticClass(); }
  using Self = UStaticPlanet;

  /** Number of world ticks per full local day (logical 20 Hz clock). */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Day Cycle", meta = (ClampMin = "1"))
  int64 DayLengthTicks = 144000;

  /** Added to phase after day wrap (e.g. longitude); integer tick space. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Day Cycle")
  int64 PhaseOffsetTicks = 0;

  /** Phase within one local day [0, DayLengthTicks) for dawn anchor (bus / scripts). */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Day Cycle", meta = (ClampMin = "0"))
  int64 DawnPhaseTicks = 24000;

  /** Phase within one local day [0, DayLengthTicks) for solar noon anchor (bus / scripts). */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Day Cycle", meta = (ClampMin = "0"))
  int64 SolarNoonPhaseTicks = 72000;

  /** Phase within one local day [0, DayLengthTicks) for sunset anchor (bus / scripts). */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Day Cycle", meta = (ClampMin = "0"))
  int64 SunsetPhaseTicks = 120000;

  /** Local simulation day-phase tick for TotalGameTicks (same space as dawn/sunset anchors). */
  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Day Cycle")
  int64 SimulationDayPhaseTicks(int64 TotalGameTicks) const;

  /** Cosmetic hours [0, 24) for a tick in local day-phase space (after day length wrap). */
  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Day Cycle")
  float GetCosmeticDayHoursFromDayPhaseTicks(int64 DayPhaseTick) const;

  /** Solar hour [0, 24) for rendering/curves from world ticks. */
  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Day Cycle")
  float GetTimeOfDayHoursForWorldTicks(int64 TotalGameTicks) const;

  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Day Cycle")
  float ResolveTimeOfDayHours(bool bWorldTimeAutoAdvance, int64 LockedWorldTimeOfDayPhaseTicks, int64 TotalGameTicks) const;

  float LuaTimeOfDayHoursFromTicks(int64 TotalGameTicks) const { return GetTimeOfDayHoursForWorldTicks(TotalGameTicks); }

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  private:
  int64 LocalPhaseTicks(int64 TotalGameTicks) const;
};
