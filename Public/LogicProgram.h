// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

//#include "UObject/Object.h"
#include "CoreMinimal.h"
#include "Qr/SerializableJson.h"
#include <Dom/JsonObject.h>
#include <Templates/SharedPointer.h>
#include "LogicProgram.generated.h"

class UBlockLogic;
class ULogicContext;
class UStaticItem;
class UCondition;

UENUM(BlueprintType)
enum class EDeciderOutputMode : uint8 {
  Constant,
  CopyA
};

UENUM(BlueprintType)
enum class EDeciderFalseBehavior : uint8 {
  DoNothing,
  WriteZero,
  CopyA
};

// Base node for logic graph
UCLASS(BlueprintType)
class EVOSPACE_API ULogicNode : public UObject, public ISerializableJson {
  GENERATED_BODY()

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
UCLASS(BlueprintType)
class EVOSPACE_API ULogicNode_Constant : public ULogicNode {
  GENERATED_BODY()

  public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TMap<const UStaticItem *, int64> Values;

  virtual void Execute(TScriptInterface<ILogicInterface> Owner, ULogicContext *Ctx) override;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;
};

// Arithmetic node: applies simple arithmetic to Output
UCLASS(BlueprintType)
class EVOSPACE_API ULogicNode_Arithmetic : public ULogicNode {
  GENERATED_BODY()

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
UCLASS(BlueprintType)
class EVOSPACE_API ULogicNode_Decider : public ULogicNode {
  GENERATED_BODY()

  public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UCondition *Condition = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UStaticItem *OutputSignal = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  EDeciderOutputMode OutputMode = EDeciderOutputMode::Constant;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int64 OutputValueTrue = 1;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  EDeciderFalseBehavior FalseBehavior = EDeciderFalseBehavior::DoNothing;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int64 OutputValueFalse = 0;

  virtual void Execute(TScriptInterface<ILogicInterface> Owner, ULogicContext *Ctx) override;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;
};

// Latch node: stores boolean state, exposes as signal
UCLASS(BlueprintType)
class EVOSPACE_API ULogicNode_Latch : public ULogicNode {
  GENERATED_BODY()

  public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool State = false;

  virtual void Execute(TScriptInterface<ILogicInterface> Owner, ULogicContext *Ctx) override;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;
};

// IO nodes: placeholders to integrate with network/machines
UCLASS(BlueprintType)
class EVOSPACE_API ULogicNode_ReadNetwork : public ULogicNode {
  GENERATED_BODY()

  public:
  virtual void Execute(TScriptInterface<ILogicInterface> Owner, ULogicContext *Ctx) override;
};

UCLASS(BlueprintType)
class EVOSPACE_API ULogicNode_WriteNetwork : public ULogicNode {
  GENERATED_BODY()

  public:
  virtual void Execute(TScriptInterface<ILogicInterface> Owner, ULogicContext *Ctx) override;
};

UCLASS(BlueprintType)
class EVOSPACE_API ULogicNode_ReadMachine : public ULogicNode {
  GENERATED_BODY()

  public:
  virtual void Execute(TScriptInterface<ILogicInterface> Owner, ULogicContext *Ctx) override;
};

UCLASS(BlueprintType)
class EVOSPACE_API ULogicNode_ControlMachine : public ULogicNode {
  GENERATED_BODY()

  public:
  virtual void Execute(TScriptInterface<ILogicInterface> Owner, ULogicContext *Ctx) override;
};

// Program: collection of nodes executed in order
UCLASS(BlueprintType)
class EVOSPACE_API ULogicProgram : public UObject, public ISerializableJson {
  GENERATED_BODY()

  public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<ULogicNode *> Nodes;

  UFUNCTION(BlueprintCallable)
  void Execute(TScriptInterface<ILogicInterface> Owner, ULogicContext *Ctx);

  UFUNCTION(BlueprintCallable, BlueprintPure)
  virtual TSubclassOf<ULogicProgramWidget> GetWidgetClass() const;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;
};