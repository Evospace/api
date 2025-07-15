// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once
#include "Qr/Prototype.h"
#include "EngineData.generated.h"

UCLASS()
class EVOSPACE_API UEngineData : public UInstance {
  GENERATED_BODY()
  EVO_CODEGEN_INSTANCE(EngineData)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UEngineData, UInstance>("EngineData") //@class EngineData : Instance
      .addProperty("props_mul", &UEngineData::PropsMul) //@field number
      .addProperty("dpi", &UEngineData::Dpi) //@field number
      .addProperty("mouse_sensitivity_x", &UEngineData::MouseSensitivityX) //@field number
      .addProperty("mouse_sensitivity_y", &UEngineData::MouseSensitivityY) //@field number
      .addProperty("props_quality", &UEngineData::PropsQuality) //@field number
      .addProperty("reflection_preset", &UEngineData::ReflectionPreset) //@field integer
      .addProperty("transparency_preset", &UEngineData::TransparencyPreset) //@field integer
      .addProperty("gi_preset", &UEngineData::GiPreset) //@field integer
      .addProperty("fps", &UEngineData::Fps) //@field integer
      .addProperty("res_x", &UEngineData::ResolutionX) //@field integer
      .addProperty("res_y", &UEngineData::ResolutionY) //@field integer
      .addProperty("loading_range", &UEngineData::LoadingRange) //@field integer
      .addProperty("performance", &UEngineData::Performance) //@field boolean
      .addProperty("performance_graph", &UEngineData::PerformanceGraph) //@field boolean
      .addProperty("ctrl_hotbar", &UEngineData::CtrlHotbar) //@field boolean
      .addProperty("alt_hotbar", &UEngineData::AltHotbar) //@field boolean
      .addProperty("shift_hotbar", &UEngineData::ShiftHotbar) //@field boolean
      .addProperty("fov", &UEngineData::Fov) //@field number
      .addProperty("fog", &UEngineData::Fog) //@field number
      .addProperty("window_mode", &UEngineData::Windowed) //@field integer
      //direct:
      //function EngineData:apply() end
      .addFunction("apply", &UEngineData::ApplyData)
      .endClass();
  }

  public:
  UPROPERTY(EditAnywhere)
  float PropsMul = 1.0;

  UPROPERTY(EditAnywhere)
  float PropsQuality = 1.0;

  UPROPERTY(EditAnywhere)
  int32 ReflectionPreset;

  UPROPERTY(EditAnywhere)
  int32 TransparencyPreset;

  UPROPERTY(EditAnywhere)
  int32 GiPreset;

  UPROPERTY(EditAnywhere)
  bool Performance;

  UPROPERTY(EditAnywhere)
  bool PerformanceGraph;

  UPROPERTY(EditAnywhere)
  bool CtrlHotbar;

  UPROPERTY(EditAnywhere)
  bool AltHotbar;

  UPROPERTY(EditAnywhere)
  bool ShiftHotbar;

  UPROPERTY(EditAnywhere)
  int32 Fps = 60;

  UPROPERTY(EditAnywhere)
  int32 LoadingRange = 9;

  UPROPERTY(EditAnywhere)
  float Fov = 80;

  UPROPERTY(EditAnywhere)
  float Fog = 1;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  float MouseSensitivityX = 1;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  float MouseSensitivityY = 1;

  UPROPERTY(EditAnywhere)
  int32 ResolutionX = 1920;

  UPROPERTY(EditAnywhere)
  int32 ResolutionY = 1080;

  UPROPERTY(EditAnywhere)
  int32 Windowed;

  UPROPERTY(EditAnywhere)
  float Dpi = 1.0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  class UMapWidgetData *MapWidgetData;

  void ApplyData() const;

  void ApplyControllerData() const;
};