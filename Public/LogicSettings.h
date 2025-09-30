// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Qr/CommonConverter.h"
#include "Qr/Loc.h"
#include "Condition.h"
#include "LogicExportOption.h"
#include "LogicSettings.generated.h"

UCLASS(BlueprintType)
class ULogicSettings : public UInstance {
  GENERATED_BODY()
  using Self = ULogicSettings;
  EVO_CODEGEN_INSTANCE(LogicSettings);
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UInstance>("LogicSettings") //@class LogicSettings : Instance
      .addProperty("export", QR_ARRAY_GET_SET(ExportSignals)) //@field LogicExportOption[]
      .endClass();
  }

  public:
  // Signals available in UI for export/indication (metadata only)
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<ULogicExportOption *> ExportSignals;

  // Per-instance export enabled flags; length matches ExportSignals
  // Do not mutate ULogicExportOption::bEnabled at runtime – use these flags instead
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<bool> ExportEnabled;

  // Initialize ExportEnabled from default options if sizes mismatch or empty
  UFUNCTION(BlueprintCallable)
  void EnsureExportFlagsInitialized();

  UFUNCTION(BlueprintCallable, BlueprintPure)
  bool IsExportEnabled(int32 Index = 0) const;

  UFUNCTION(BlueprintCallable)
  void SetExportEnabled(int32 Index, bool bEnabled);

  // Serialization for saving per-block state
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) const override;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
};
