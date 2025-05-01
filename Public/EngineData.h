// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once
#include "Prototype.h"
#include "Evospace/SerializableJson.h"
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
      .addProperty("props_quality", &UEngineData::PropsQuality) //@field number
      .addProperty("reflection_preset", &UEngineData::ReflectionPreset) //@field integer
      .addProperty("transparency_preset", &UEngineData::TransparencyPreset) //@field integer
      .addProperty("gi_preset", &UEngineData::GiPreset) //@field integer
      .addProperty("fps", &UEngineData::Fps) //@field integer
      .addProperty("res_x", &UEngineData::ResolutionX) //@field integer
      .addProperty("res_y", &UEngineData::ResolutionY) //@field integer
      .addProperty("fov", &UEngineData::Fov) //@field number
      .addProperty("fog", &UEngineData::Fog) //@field number
      .addProperty("window_mode", &UEngineData::Windowed) //@field integer
      //direct:
      //function EngineData:apply() end
      .addFunction("apply", &UEngineData::ApplyData)
      .endClass();
  }

  public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float PropsMul = 1.0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float PropsQuality = 1.0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 ReflectionPreset;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 TransparencyPreset;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 GiPreset;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 Fps = 60;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float Fov = 80;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float Fog = 1;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 ResolutionX = 1920;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 ResolutionY = 1080;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 Windowed;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float Dpi = 1.0;

  void ApplyData() const;
};