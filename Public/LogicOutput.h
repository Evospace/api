#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Qr/SerializableJson.h"
#include "LogicOutput.generated.h"

UENUM(BlueprintType)
enum class ELogicExprOp : uint8 {
  Add UMETA(DisplayName = "+"),
  Subtract UMETA(DisplayName = "-"),
  Multiply UMETA(DisplayName = "*"),
  Divide UMETA(DisplayName = "/"),
  Modulo UMETA(DisplayName = "%"),
  Power UMETA(DisplayName = "^"),
};

UENUM(BlueprintType)
enum class ELogicFormulaMode : uint8 {
  Number,
  Signal,
  Current,
  Expression,
};

class UStaticItem;

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class ULogicSignalFormula : public UObject, public ISerializableJson {
  GENERATED_BODY()

  public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  ELogicFormulaMode Mode = ELogicFormulaMode::Number;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int64 Number = 0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UStaticItem *Signal = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
  ULogicSignalFormula *Left = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  ELogicExprOp Op = ELogicExprOp::Add;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
  ULogicSignalFormula *Right = nullptr;

  int64 Evaluate(const class ULogicContext *Ctx, const UStaticItem *CurrentSignal = nullptr) const;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) const override;
};

UCLASS(BlueprintType, EditInlineNew)
class ULogicOutput : public UObject, public ISerializableJson {
  GENERATED_BODY()

  public:
  // Target signal, or pseudo-signals "Anything" / "Everything" from vanilla data.
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UStaticItem *OutSignal = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
  ULogicSignalFormula *Formula = nullptr;

  // Execute this output definition against the context, writing to Ctx->Output
  UFUNCTION(BlueprintCallable)
  void Compute(class ULogicContext *Ctx) const;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) const override;
};