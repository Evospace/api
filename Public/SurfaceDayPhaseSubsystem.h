// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Public/SurfaceDayPhaseTypes.h"
#include "SurfaceDayPhaseSubsystem.generated.h"

class UGameSessionSubsystem;
class UStaticPlanet;

DECLARE_DELEGATE_FourParams(
  FSurfaceDayPhaseNativeSlot,
  const FString &,
  ESurfaceDayPhaseAnchor,
  float /*SessionCosmeticHour*/,
  float /*AnchorPhaseCosmeticHour*/);

/**
 * World-scoped registry of per-save-folder surface IDs (e.g. "Temperate") and bus for cosmetic day-phase moments.
 *
 * Advances are evaluated when the logical session tick increments (typically from ADimension), using TotalGameTicks
 * from UGameSessionData together with surface-specific cycle parameters from UStaticPlanet or session defaults.
 */
UCLASS()
class USurfaceDayPhaseSubsystem : public UWorldSubsystem {
  GENERATED_BODY()

  public:
  virtual void Initialize(FSubsystemCollectionBase &Collection) override;
  virtual void Deinitialize() override;

  /** Registers (or replaces) timing parameters for one surface folder key. Passing nullptr Planet uses GameSessionDefaults. */
  void RegisterSurface(const FString &SurfaceFolderName, const UStaticPlanet *OptionalPlanetCycle);

  UFUNCTION(BlueprintCallable, Category = "Day Phase")
  void UnregisterSurface(const FString &SurfaceFolderName);

  /**
   * Native bus subscription: empty SurfaceFolderName matches all surfaces.
   * Returns an id for UnsubscribeNative; 0 on failure.
   */
  int32 SubscribeNative(const FString &SurfaceFolderName, const FSurfaceDayPhaseNativeSlot &Slot);
  void UnsubscribeNative(int32 SubscriptionId);

  /** Called after UGameSessionSubsystem::IncrementTicks() for a dimension that owns SurfaceFolderName. */
  void NotifyAfterSessionTick(const FString &SurfaceFolderName, UGameSessionSubsystem *GameSession);

  /** Editor / smoke: validates phase crossing extraction on tiny synthetic cycles. */
  static bool RunCrossingLogicSelfTest();

  private:
  struct FSurfaceTrack {
    FSurfaceDayCycleParams Params;
  };

  struct FNativeSubscription {
    int32 Id = 0;
    FString SurfaceFilter;
    FSurfaceDayPhaseNativeSlot Slot;
  };

  TMap<FString, FSurfaceTrack> Tracks;
  TArray<FNativeSubscription> NativeSubscriptions;
  int32 NextSubscriptionId = 1;

  void BroadcastNative(const FString &SurfaceKey, ESurfaceDayPhaseAnchor Anchor, float SessionCosmeticHour, float AnchorPhaseCosmeticHour);
  void EmitLua(const FString &SurfaceKey, ESurfaceDayPhaseAnchor Anchor, float SessionCosmeticHour, float AnchorPhaseCosmeticHour);
};
