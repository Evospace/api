#pragma once

#include "CoreMinimal.h"
#include "EngineDataStorage.generated.h"

USTRUCT(BlueprintType)
struct FEngineDataStorage {
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere)
    float PropsMul = 1.0;
  
    UPROPERTY(EditAnywhere)
    float PropsQuality = 1.0;
  
    UPROPERTY(EditAnywhere)
    int32 ReflectionPreset = 0;
  
    UPROPERTY(EditAnywhere)
    int32 TransparencyPreset = 0;
  
    UPROPERTY(EditAnywhere)
    int32 GiPreset = 0;
  
    UPROPERTY(EditAnywhere)
    bool DetailShadows = true;
  
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool Performance = false;
  
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool PerformanceGraph = false;
  
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool MemoryStats = false;
  
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool CtrlHotbar = false;
  
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool AltHotbar = false;
  
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool ShiftHotbar = false;
  
    UPROPERTY(EditAnywhere)
    int32 Fps = 60;
  
    UPROPERTY(EditAnywhere)
    int32 LoadingRange = 9;
  
    UPROPERTY(EditAnywhere)
    float Fov = 80;
  
    UPROPERTY(EditAnywhere)
    float Fog = 1;
  
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 AutosavePeriod = 10;
  
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float MouseSensitivityX = 1;
  
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float MouseSensitivityY = 1;
  
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool MouseInversionX = false;
  
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool MouseInversionY = false;
  
    UPROPERTY(EditAnywhere)
    int32 ResolutionX = 1920;
  
    UPROPERTY(EditAnywhere)
    int32 ResolutionY = 1080;
  
    UPROPERTY(EditAnywhere)
    int32 Windowed = 0;
  
    // Для ручного отката настроек
    int32 OldResolutionX = 1920;
    int32 OldResolutionY = 1080;
    int32 OldWindowed = 0;
  
    UPROPERTY(EditAnywhere)
    float Dpi = 1.0;

    UPROPERTY(EditAnywhere)
    bool EnableRain = true;
};