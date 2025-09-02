// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Qr/JsonHelperCommon.h"
#include "Qr/Prototype.h"
#include "Qr/Loc.h"
#include "LogicExportOption.generated.h"

class UStaticItem;

// Prototype describing a single exportable signal option for GUI and logic filtering
UCLASS(BlueprintType)
class ULogicExportOption : public UPrototype {
  GENERATED_BODY()
  PROTOTYPE_CODEGEN(LogicExportOption, LogicExportOption)
  virtual UClass *GetSuperProto() const override { return StaticClass(); }
  virtual void lua_reg(lua_State *L) const override {
    using Self = ULogicExportOption;
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UPrototype>("LogicExportOption") //@class LogicExportOption : Prototype
      .addProperty("signal", &Self::Signal) //@field StaticItem Signal
      .addProperty("enabled", &Self::bEnabled) //@field bool Enabled
      .addProperty("label", &Self::Label) //@field FLoc Label
      .addProperty("tooltip", &Self::Tooltip) //@field FLoc Tooltip
      .endClass();
  }

  public:
  // Which signal to export
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UStaticItem *Signal = nullptr;

  // Default checkbox state in GUI (can be used as initial value)
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool bEnabled = false;

  // Localized name and tooltip for GUI
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FLoc Label;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FLoc Tooltip;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
};
