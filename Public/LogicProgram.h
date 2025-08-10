// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Qr/SerializableJson.h"

#include <Dom/JsonObject.h>
#include <Templates/SharedPointer.h>

#include "LogicProgram.generated.h"

class UBlockLogic;
class ULogicContext;
class UStaticItem;

// Base node for logic graph
UCLASS(BlueprintType)
class ULogicNode : public UObject, public ISerializableJson {
  GENERATED_BODY()

  public:
  // Executes this node against the provided context
  UFUNCTION(BlueprintCallable)
  virtual void Execute(UBlockLogic* Owner, ULogicContext* Ctx) {}

  // Default serialization does nothing; specialized nodes override when needed
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override { return true; }
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override { return true; }
};

// Constant node: writes fixed values to Output
UCLASS(BlueprintType)
class ULogicNode_Constant : public ULogicNode {
  GENERATED_BODY()

  public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TMap<const UStaticItem*, int64> Values;

  virtual void Execute(UBlockLogic* Owner, ULogicContext* Ctx) override;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;
};

// Arithmetic node: applies simple arithmetic to Output
UCLASS(BlueprintType)
class ULogicNode_Arithmetic : public ULogicNode {
  GENERATED_BODY()

  public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FName Operation; // e.g., "Add", "Sub"

  virtual void Execute(UBlockLogic* Owner, ULogicContext* Ctx) override;
};

// Decider node: placeholder; relies on condition system elsewhere
UCLASS(BlueprintType)
class ULogicNode_Decider : public ULogicNode {
  GENERATED_BODY()

  public:
  virtual void Execute(UBlockLogic* Owner, ULogicContext* Ctx) override;
};

// Latch node: stores boolean state, exposes as signal
UCLASS(BlueprintType)
class ULogicNode_Latch : public ULogicNode {
  GENERATED_BODY()

  public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool State = false;

  virtual void Execute(UBlockLogic* Owner, ULogicContext* Ctx) override;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;
};

// IO nodes: placeholders to integrate with network/machines
UCLASS(BlueprintType)
class ULogicNode_ReadNetwork : public ULogicNode {
  GENERATED_BODY()

  public:
  virtual void Execute(UBlockLogic* Owner, ULogicContext* Ctx) override;
};

UCLASS(BlueprintType)
class ULogicNode_WriteNetwork : public ULogicNode {
  GENERATED_BODY()

  public:
  virtual void Execute(UBlockLogic* Owner, ULogicContext* Ctx) override;
};

UCLASS(BlueprintType)
class ULogicNode_ReadMachine : public ULogicNode {
  GENERATED_BODY()

  public:
  virtual void Execute(UBlockLogic* Owner, ULogicContext* Ctx) override;
};

UCLASS(BlueprintType)
class ULogicNode_ControlMachine : public ULogicNode {
  GENERATED_BODY()

  public:
  virtual void Execute(UBlockLogic* Owner, ULogicContext* Ctx) override;
};

// Program: collection of nodes executed in order
UCLASS(BlueprintType)
class ULogicProgram : public UObject, public ISerializableJson {
  GENERATED_BODY()

  public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<ULogicNode*> Nodes;

  UFUNCTION(BlueprintCallable)
  void Execute(UBlockLogic* Owner, ULogicContext* Ctx);

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;
};


