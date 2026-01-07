// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EngineDataStorage.h"
#include "ThirdParty/luabridge/LuaBridge.h"
#include "Qr/CommonConverter.h"
#include "EngineDataSubsystem.generated.h"

UCLASS()
class UEngineDataSubsystem : public UGameInstanceSubsystem {
  GENERATED_BODY()
  using Self = UEngineDataSubsystem;

  public:
  virtual void Initialize(FSubsystemCollectionBase &Collection) override;
  virtual void Deinitialize() override;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FEngineDataStorage Storage;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  class UMapWidgetData *MapWidgetData;

  void FixResolution();
  void ShowConfirmationDialog();

  void ConfirmSettings();
  void CancelSettings();

  void ApplyData() const;

  void ApplyControllerData() const;

  void lua_reg(lua_State *L) const {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UEngineDataSubsystem, UObject>("Engine") //@class EngineData : Object
      .addProperty("props_mul", QR_GET_SET(Storage.PropsMul)) //@field number
      .addProperty("dpi", QR_GET_SET(Storage.Dpi)) //@field number
      .addProperty("mouse_sensitivity_x", QR_GET_SET(Storage.MouseSensitivityX)) //@field number
      .addProperty("mouse_sensitivity_y", QR_GET_SET(Storage.MouseSensitivityY)) //@field number
      .addProperty("mouse_inversion_x", QR_GET_SET(Storage.MouseInversionX)) //@field boolean
      .addProperty("mouse_inversion_y", QR_GET_SET(Storage.MouseInversionY)) //@field boolean
      .addProperty("props_quality", QR_GET_SET(Storage.PropsQuality)) //@field number
      .addProperty("reflection_preset", QR_GET_SET(Storage.ReflectionPreset)) //@field integer
      .addProperty("transparency_preset", QR_GET_SET(Storage.TransparencyPreset)) //@field integer
      .addProperty("gi_preset", QR_GET_SET(Storage.GiPreset)) //@field integer
      .addProperty("detail_shadows", QR_GET_SET(Storage.DetailShadows)) //@field boolean
      .addProperty("fps", QR_GET_SET(Storage.Fps)) //@field integer
      .addProperty("res_x", QR_GET_SET(Storage.ResolutionX)) //@field integer
      .addProperty("res_y", QR_GET_SET(Storage.ResolutionY)) //@field integer
      .addProperty("loading_range", QR_GET_SET(Storage.LoadingRange)) //@field integer
      .addProperty("performance", QR_GET_SET(Storage.Performance)) //@field boolean
      .addProperty("performance_graph", QR_GET_SET(Storage.PerformanceGraph)) //@field boolean
      .addProperty("ctrl_hotbar", QR_GET_SET(Storage.CtrlHotbar)) //@field boolean
      .addProperty("alt_hotbar", QR_GET_SET(Storage.AltHotbar)) //@field boolean
      .addProperty("shift_hotbar", QR_GET_SET(Storage.ShiftHotbar)) //@field boolean
      .addProperty("fov", QR_GET_SET(Storage.Fov)) //@field number
      .addProperty("fog", QR_GET_SET(Storage.Fog)) //@field number
      .addProperty("window_mode", QR_GET_SET(Storage.Windowed)) //@field integer
      .addProperty("autosave_period", QR_GET_SET(Storage.AutosavePeriod)) //@field integer
      .addProperty("memory_stats", QR_GET_SET(Storage.MemoryStats)) //@field boolean
      .addProperty("enable_rain", QR_GET_SET(Storage.EnableRain)) //@field boolean
      .addProperty("cloud_preset", QR_GET_SET(Storage.CloudPreset)) //@field integer
      .addProperty("sector_lod_count", QR_GET_SET(Storage.SectorLodCount)) //@field integer
      // direct:
      // function EngineData:apply() end
      .addFunction("apply", &Self::ApplyData)
      // direct:
      // function EngineData:show_confirmation() end
      .addFunction("show_confirmation", &Self::ShowConfirmationDialog)
      .endClass();
  }

  private:
  mutable bool initial = true;
};
