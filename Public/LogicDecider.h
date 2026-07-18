// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Qr/Prototype.h"
#include "Qr/SerializableJson.h"
#include <Dom/JsonObject.h>
#include <Templates/SharedPointer.h>
#include "LogicDecider.generated.h"

class ULogicContext;
class UCondition;
class ULogicOutput;

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class ULogicDecider : public UInstance {
  GENERATED_BODY()
  using Self = ULogicDecider;
  EVO_CODEGEN_INSTANCE(LogicDecider);
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<ULogicDecider, UInstance>("LogicDecider") //@class LogicDecider : Instance
      .endClass();
  }

  public:
  ULogicDecider();

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UCondition *Condition = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<ULogicOutput *> Output;

  UFUNCTION(BlueprintCallable)
  void Execute(ULogicContext *Ctx) const;

  UFUNCTION(BlueprintCallable)
  void RemoveOutput(ULogicOutput *to_remove);

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) const override;
};
