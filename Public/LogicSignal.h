// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Qr/CommonConverter.h"
#include "Qr/Loc.h"
#include "Condition.h"
#include "LogicExportOption.h"
#include "LogicSignal.generated.h"

UCLASS(BlueprintType)
class EVOSPACE_API ULogicSignal : public UInstance {
  GENERATED_BODY()
  using Self = ULogicSignal;
  EVO_CODEGEN_INSTANCE(LogicSignal);
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UInstance>("LogicSignal") //@class LogicSignal : Instance
      .addProperty("export", QR_ARRAY_GET_SET(ExportSignals)) //@field LogicExportOption[]
      .addProperty("import", QR_ARRAY_GET_SET(ImportSignals)) //@field LogicExportOption[]
      .endClass();
  }

  public:
  // Signals available in UI for export/indication (metadata only)
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<ULogicExportOption*> ExportSignals;

  // Signals available in UI for input/indication (metadata only)
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<ULogicExportOption*> ImportSignals;
};


