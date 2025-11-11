// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WeatherTypes.h"
#include "WeatherWorldSubsystem.generated.h"

class UStaticWeather;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeatherChanged, const FWeatherState&, State);

/**
 * Weather world subsystem.
 * Owns and updates current weather state; other systems subscribe and apply.
 */
UCLASS()
class UWeatherWorldSubsystem : public UWorldSubsystem {
  GENERATED_BODY()
public:
  UPROPERTY(BlueprintReadOnly, Category = "Weather")
  FWeatherState Current;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ToolTip = "Optional default weather for initialization. If not set, will use first available weather after content load."))
  UStaticWeather* DefaultWeather = nullptr;

  UPROPERTY(BlueprintAssignable, Category = "Weather")
  FOnWeatherChanged OnWeatherChanged;
  
  // Display current weather asset in editor
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weather", meta = (ToolTip = "Current active weather asset"))
  UStaticWeather* CurrentWeatherAsset = nullptr;
  
  // Display remaining time until next weather change (seconds)
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weather", meta = (ToolTip = "Remaining time until next weather change in seconds"))
  float RemainingTimeUntilNextWeather = 0.0f;
  
  // Display all available weather assets in editor
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weather", meta = (ToolTip = "All available weather assets loaded from content"))
  TArray<UStaticWeather*> AvailableWeatherAssets;

  virtual void Initialize(FSubsystemCollectionBase& Collection) override;
  virtual void Deinitialize() override;

  // Set current weather directly (immediate change)
  UFUNCTION(BlueprintCallable, Category = "Weather")
  void SetWeatherFromAsset(UStaticWeather* Asset);

  // Target state: Current will gradually move towards Target every tick
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
  FWeatherState Target;

  // Duration of weather transition in seconds (how long it takes to transition from current to target)
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0.1"))
  float TransitionDurationSeconds = 120.0f;

  // Epsilon for deciding if change is worth broadcasting
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0.0"))
  float BroadcastTolerance = 0.0005f;

  UFUNCTION(BlueprintCallable, Category = "Weather")
  void SetTargetWeatherFromAsset(UStaticWeather* Asset);

  UFUNCTION(BlueprintCallable, Category = "Weather")
  void SetTargetWeatherValues(float Cloudiness01, float Precipitation01, float Fog01, float Storminess01, float WindSpeed);

  UFUNCTION(BlueprintCallable, Category = "Weather")
  void SetTransitionDurationSeconds(float DurationSeconds);

  UFUNCTION(BlueprintCallable, Category = "Weather")
  const FWeatherState& GetTarget() const { return Target; }

  // Immediately align Current with Target and broadcast
  UFUNCTION(BlueprintCallable, Category = "Weather")
  void SnapCurrentToTarget();

  // Get a random weather asset from available weathers
  UFUNCTION(BlueprintCallable, Category = "Weather")
  UStaticWeather* GetRandomWeatherAsset() const;

private:
  bool TickWeather(float DeltaTime);
  FTSTicker::FDelegateHandle TickDelegateHandle;
  void Broadcast();
  
  // Random weather system
  UFUNCTION()
  void OnContentLoaded();
  void LoadAllWeatherAssets();
  void SelectNextRandomWeather();
  
  bool bWasTransitioning = false;
  float TimeSinceLastWeatherChange = 0.0f;
  float CurrentWeatherDuration = 0.0f; // Current duration for the weather (includes min duration + random duration)
  float TimeInCurrentWeather = 0.0f; // Time spent in current weather state
  float TransitionStartTime = 0.0f; // Time when current transition started
  
  // Minimum time between weather changes (even if keeping current weather)
  static constexpr float MinTimeBetweenWeatherChanges = 10.0f; // seconds
};


