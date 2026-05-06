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
      .addProperty("start_phase_ticks", &Self::StartPhaseTicks) //@field integer
      .addProperty("phase_offset_ticks", &Self::PhaseOffsetTicks) //@field integer
      .addFunction("time_of_day_hours_from_ticks", &Self::LuaTimeOfDayHoursFromTicks)
      .addFunction("resolve_time_of_day_hours", &Self::ResolveTimeOfDayHours)
      .endClass();
  }
  virtual UClass *GetSuperProto() const override { return StaticClass(); }
  using Self = UStaticPlanet;

  /** Number of world ticks per full local day (logical 20 Hz clock). */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Day Cycle", meta = (ClampMin = "1"))
  int64 DayLengthTicks = 144000;

  /** Phase at TotalGameTicks == 0; combined with TotalGameTicks modulo DayLengthTicks. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Day Cycle")
  int64 StartPhaseTicks = 48000;

  /** Added to phase after day wrap (e.g. longitude); integer tick space. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Day Cycle")
  int64 PhaseOffsetTicks = 0;

  /** Solar hour [0, 24) for rendering/curves from world ticks. */
  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Day Cycle")
  float GetTimeOfDayHoursForWorldTicks(int64 TotalGameTicks) const;

  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Day Cycle")
  float ResolveTimeOfDayHours(bool bWorldTimeAutoAdvance, float LockedWorldTimeOfDayHours, int64 TotalGameTicks) const;

  float LuaTimeOfDayHoursFromTicks(int64 TotalGameTicks) const { return GetTimeOfDayHoursForWorldTicks(TotalGameTicks); }

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  private:
  int64 LocalPhaseTicks(int64 TotalGameTicks) const;
};
