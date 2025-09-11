// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

//#include "UObject/Object.h"
#include "CoreMinimal.h"
#include "Qr/Prototype.h"
#include "Qr/SerializableJson.h"
#include <Dom/JsonObject.h>
#include "Public/LogicOutput.h"
#include <Templates/SharedPointer.h>
#include "LogicProgram.generated.h"

class UBlockLogic;
class ULogicContext;
class UStaticItem;
class UCondition;
class ULogicNodeWidget;
class ULogicProgramWidget;

// Base node for logic graph
UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class ULogicNode : public UInstance {
  GENERATED_BODY()
  using Self = ULogicNode;
  EVO_CODEGEN_INSTANCE(LogicNode);
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<ULogicNode, UInstance>("LogicNode") //@class LogicNode : Instance
      .endClass();
  }

  public:
  // Executes this node against the provided context
  UFUNCTION(BlueprintCallable)
  virtual void Execute(TScriptInterface<ILogicInterface> Owner, ULogicContext *Ctx) {}

  UFUNCTION(BlueprintCallable, BlueprintPure)
  virtual TSubclassOf<ULogicNodeWidget> GetWidgetClass() const;

  // Default serialization does nothing; specialized nodes override when needed
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override { return true; }
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override { return true; }
};

// Constant node: writes fixed values to Output
UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class ULogicNode_Constant : public ULogicNode {
  GENERATED_BODY()
  using Self = ULogicNode;
  EVO_CODEGEN_INSTANCE(LogicNode);
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<ULogicNode, UInstance>("LogicNode") //@class LogicNode : LogicNode
      .endClass();
  }

  public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TMap<const UStaticItem *, int64> Values;

  virtual void Execute(TScriptInterface<ILogicInterface> Owner, ULogicContext *Ctx) override;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;
};

// Arithmetic node: applies simple arithmetic to Output
UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class ULogicNode_Arithmetic : public ULogicNode {
  GENERATED_BODY()
  using Self = ULogicNode_Arithmetic;
  EVO_CODEGEN_INSTANCE(LogicNode_Arithmetic);
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<ULogicNode_Arithmetic, UInstance>("LogicNode_Arithmetic") //@class LogicNode_Arithmetic : LogicNode
      .endClass();
  }

  public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FName Operation; // e.g., "Add", "Sub"

  // Source operand A: read value from Input by this signal
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UStaticItem *SourceSignal = nullptr;

  // Operand B: either const or signal
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool UseConstB = true;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int64 ConstB = 0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UStaticItem *SignalB = nullptr;

  // Destination: if not set, writes back into SourceSignal
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UStaticItem *OutputSignal = nullptr;

  virtual void Execute(TScriptInterface<ILogicInterface> Owner, ULogicContext *Ctx) override;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;
};

// Decider node: placeholder; relies on condition system elsewhere
UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class ULogicNode_Decider : public ULogicNode {
  GENERATED_BODY()
  using Self = ULogicNode_Decider;
  EVO_CODEGEN_INSTANCE(LogicNode_Decider);
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<ULogicNode_Decider, ULogicNode>("LogicNode_Decider") //@class LogicNode_Decider : LogicNode
      .endClass();
  }

  ULogicNode_Decider();

  public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UCondition *Condition = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<ULogicOutput *> Output;

  virtual void Execute(TScriptInterface<ILogicInterface> Owner, ULogicContext *Ctx) override;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;

  UFUNCTION(BlueprintCallable)
  void RemoveOutput(ULogicOutput *to_remove);
};

// Latch node: stores boolean state, exposes as signal
UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class ULogicNode_Latch : public ULogicNode {
  GENERATED_BODY()
  using Self = ULogicNode_Latch;
  EVO_CODEGEN_INSTANCE(LogicNode_Latch);
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<ULogicNode_Latch, ULogicNode>("LogicNode_Latch") //@class LogicNode_Latch : LogicNode
      .endClass();
  }

  public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool State = false;

  virtual void Execute(TScriptInterface<ILogicInterface> Owner, ULogicContext *Ctx) override;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;
};

// IO nodes: placeholders to integrate with network/machines
UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class ULogicNode_ReadNetwork : public ULogicNode {
  GENERATED_BODY()
  using Self = ULogicNode_ReadNetwork;
  EVO_CODEGEN_INSTANCE(LogicNode_ReadNetwork);
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<ULogicNode_ReadNetwork, ULogicNode>("LogicNode_ReadNetwork") //@class LogicNode_ReadNetwork : LogicNode
      .endClass();
  }

  public:
  virtual void Execute(TScriptInterface<ILogicInterface> Owner, ULogicContext *Ctx) override;
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class ULogicNode_WriteNetwork : public ULogicNode {
  GENERATED_BODY()
  using Self = ULogicNode_WriteNetwork;
  EVO_CODEGEN_INSTANCE(LogicNode_WriteNetwork);
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<ULogicNode_WriteNetwork, ULogicNode>("LogicNode_WriteNetwork") //@class LogicNode_WriteNetwork : LogicNode
      .endClass();
  }

  public:
  virtual void Execute(TScriptInterface<ILogicInterface> Owner, ULogicContext *Ctx) override;
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class ULogicNode_ReadMachine : public ULogicNode {
  GENERATED_BODY()
  using Self = ULogicNode_ReadMachine;
  EVO_CODEGEN_INSTANCE(LogicNode_ReadMachine);
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<ULogicNode_ReadMachine, ULogicNode>("LogicNode_ReadMachine") //@class LogicNode_ReadMachine : LogicNode
      .endClass();
  }

  public:
  virtual void Execute(TScriptInterface<ILogicInterface> Owner, ULogicContext *Ctx) override;
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class ULogicNode_ControlMachine : public ULogicNode {
  GENERATED_BODY()
  using Self = ULogicNode_ControlMachine;
  EVO_CODEGEN_INSTANCE(LogicNode_ControlMachine);
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<ULogicNode_ControlMachine, ULogicNode>("LogicNode_ControlMachine") //@class LogicNode_ControlMachine : LogicNode
      .endClass();
  }

  public:
  virtual void Execute(TScriptInterface<ILogicInterface> Owner, ULogicContext *Ctx) override;
};

// Program: collection of nodes executed in order
UCLASS(BlueprintType)
class ULogicProgram : public UInstance {
  GENERATED_BODY()
  using Self = ULogicProgram;
  EVO_CODEGEN_INSTANCE(LogicProgram);
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<ULogicProgram, UInstance>("LogicProgram") //@class LogicProgram : LogicNode
      .endClass();
  }

  public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
  TArray<TObjectPtr<ULogicNode>> Nodes;

  UFUNCTION(BlueprintCallable)
  void Execute(TScriptInterface<ILogicInterface> Owner, ULogicContext *Ctx);

  UFUNCTION(BlueprintCallable, BlueprintPure)
  virtual TSubclassOf<ULogicProgramWidget> GetWidgetClass() const;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;
};