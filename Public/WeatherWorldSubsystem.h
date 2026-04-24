// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WeatherTypes.h"
#include "WeatherWorldSubsystem.generated.h"

class UStaticWeather;
class UBiome;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnWeatherChangedNative, const FWeatherState &);

/**
 * Weather world subsystem.
 * Owns and updates current weather state; other systems subscribe and apply.
 */
UCLASS()
class UWeatherWorldSubsystem : public UWorldSubsystem {
  GENERATED_BODY()
  public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
  FWeatherState Current;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ToolTip = "Optional default weather for initialization. If not set, will use first available weather after content load."))
  UStaticWeather *DefaultWeather = nullptr;

  FOnWeatherChangedNative OnWeatherChangedNative;

  // Display current weather asset in editor
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weather", meta = (ToolTip = "Current active weather asset"))
  UStaticWeather *CurrentWeatherAsset = nullptr;

  // Display remaining time until next weather change (seconds)
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weather", meta = (ToolTip = "Remaining time until next weather change in seconds"))
  float RemainingTimeUntilNextWeather = 0.0f;

  // Display all available weather assets in editor
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weather", meta = (ToolTip = "All available weather assets loaded from content"))
  TArray<UStaticWeather *> AvailableWeatherAssets;

  // Current biome for weather selection
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weather", meta = (ToolTip = "Current biome that determines available weather"))
  const UBiome *CurrentBiome = nullptr;

  virtual void Initialize(FSubsystemCollectionBase &Collection) override;
  virtual void Deinitialize() override;

  // Set current biome for weather selection
  UFUNCTION(BlueprintCallable, Category = "Weather")
  void SetCurrentBiome(const UBiome *Biome);

  // Set current weather directly (immediate change)
  UFUNCTION(BlueprintCallable, Category = "Weather")
  void SetWeatherFromAsset(UStaticWeather *Asset);

  // Target state: Current will gradually move towards Target every tick
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
  FWeatherState Target;

  /** Fixed interval between weather-subsystem ticks (must match AddTicker delay in WeatherWorldSubsystem.cpp). */
  static constexpr float WeatherSubsystemTickIntervalSeconds = 0.5f;

  /**
   * How many weather ticks it takes to move normalized scalars across a full 0→1 span (constant |Δ| per tick).
   * Smaller = faster transitions.
   */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "1"))
  int32 TransitionTicks = 240;

  /**
   * Wind speed range that TransitionTicks covers (same per-tick step rate as 0→1 on cloudiness).
   * Raise if your weather assets use larger WindSpeed values.
   */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0.01"))
  float WindSpeedTransitionReferenceSpan = 20.0f;

  // Epsilon for deciding if change is worth broadcasting
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0.0"))
  float BroadcastTolerance = 0.0005f;

  UFUNCTION(BlueprintCallable, Category = "Weather")
  void SetTargetWeatherFromAsset(UStaticWeather *Asset);

  UFUNCTION(BlueprintCallable, Category = "Weather")
  void SetTargetWeatherValues(float Cloudiness01, float Precipitation01, float Fog01, float Storminess01, float WindSpeed);

  UFUNCTION(BlueprintCallable, Category = "Weather")
  void SetTransitionTicks(int32 Ticks) { TransitionTicks = FMath::Max(1, Ticks); }

  /** Sets TransitionTicks from an approximate wall duration (seconds / WeatherSubsystemTickIntervalSeconds). */
  UFUNCTION(BlueprintCallable, Category = "Weather")
  void SetTransitionDurationSeconds(float DurationSeconds);

  /** No-op; kept for Blueprint compatibility. Current always steps toward Target in TickWeather. */
  UFUNCTION(BlueprintCallable, Category = "Weather")
  void BeginWeatherTransition();

  UFUNCTION(BlueprintCallable, Category = "Weather")
  const FWeatherState &GetTarget() const { return Target; }

  // Get a random weather asset from available weathers (or from current biome if set)
  UFUNCTION(BlueprintCallable, Category = "Weather")
  UStaticWeather *GetRandomWeatherAsset() const;

  // Get a random weather asset from all available weathers
  UFUNCTION(BlueprintCallable, Category = "Weather")
  UStaticWeather *GetRandomWeatherAssetFromAll() const;

  // === Save / Load ===
  // Persist current weather state for a given save and surface.
  UFUNCTION(BlueprintCallable, Category = "Weather|Save")
  void SaveWeatherState(const FString &SaveName, const FString &SurfaceName);

  // Restore weather state for a given save and surface if present.
  UFUNCTION(BlueprintCallable, Category = "Weather|Save")
  void LoadWeatherState(const FString &SaveName, const FString &SurfaceName);

  private:
  bool TickWeather(float DeltaTime);
  FTSTicker::FDelegateHandle TickDelegateHandle;
  void Broadcast();

  // Random weather system
  UFUNCTION()
  void OnContentLoaded();
  void LoadAllWeatherAssets();
  void SelectNextRandomWeather();

  float TimeSinceLastWeatherChange = 0.0f;
  float CurrentWeatherDuration = 0.0f; // Current duration for the weather (includes min duration + random duration)
  float TimeInCurrentWeather = 0.0f; // Time spent in current weather state

  // Minimum time between weather changes (even if keeping current weather)
  static constexpr float MinTimeBetweenWeatherChanges = 10.0f; // seconds

  public:
  // Surface / world state (not preset weather scalars): wetness and horizontal wind heading evolve independently of weather assets.
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weather|Surface")
  float SurfaceWetness01 = 0.0f;

  /** Horizontal wind heading in degrees around +Z (0 = +X, 90 = +Y). Random walk; strength comes from Current.WindSpeed. */
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weather|Surface")
  float WindHeadingDegrees = 0.0f;

  // Precipitation threshold above which wetness increases
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Wetness", meta = (ClampMin = "0.0", ClampMax = "1.0"))
  float RainIncreaseThreshold = 0.05f;

  // Time (seconds) to dry fully from 1.0 to 0.0 when below threshold
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Wetness", meta = (ClampMin = "0.1"))
  float DryingTimeSeconds = 60.0f;

  // Scale of wetting speed when raining above threshold
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Wetness", meta = (ClampMin = "0.0"))
  float WettingRate = 1.0f;

  // Minimum seconds between random wind heading target updates.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Wind", meta = (ClampMin = "0.1"))
  float WindDirectionRetargetMinSeconds = 30.0f;

  // Maximum seconds between random wind heading target updates.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Wind", meta = (ClampMin = "0.1"))
  float WindDirectionRetargetMaxSeconds = 200.0f;

  // Max absolute angle step (degrees) for every random wind heading retarget.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Wind", meta = (ClampMin = "0.0", ClampMax = "180.0"))
  float WindDirectionRetargetStepDegrees = 70.0f;

  private:
  /** Smooth interpolation target for WindHeadingDegrees (random retarget). */
  float WindHeadingTargetDegrees = 0.0f;

  float WindDirectionRetargetRemainingSeconds = 0.0f;

  /** Last wall time (FPlatformTime::Seconds) we ran TickWeather; 0 = not yet, use nominal tick interval. */
  double LastTickWeatherWallTimeSec = 0.0;
};
