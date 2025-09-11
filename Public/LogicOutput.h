#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Qr/SerializableJson.h"
#include "LogicOutput.generated.h"

UENUM(BlueprintType)
enum class ELogicOutputMode : uint8 {
  Constant,
  CopyA,
  Expression
};

UENUM(BlueprintType)
enum class ELogicExprOp : uint8 {
  Add UMETA(DisplayName = "+"),
  Subtract UMETA(DisplayName = "-"),
  Multiply UMETA(DisplayName = "*"),
  Divide UMETA(DisplayName = "/"),
  Modulo UMETA(DisplayName = "%"),
  Power UMETA(DisplayName = "^")
};

class UStaticItem;

UCLASS(BlueprintType, EditInlineNew)
class ULogicOutput : public UObject, public ISerializableJson {
  GENERATED_BODY()

  public:
  // Target or input signal depending on mode. Also supports special
  // pseudo-signals named "Anything" and "Everything".
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UStaticItem *SignalA = nullptr;

  // How to produce output for the given Signal
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  ELogicOutputMode Mode = ELogicOutputMode::Constant;

  // For Constant mode: value to write
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int64 ConstA = 1;

  // For Expression mode: compute (A op B) and write into Signal
  // A is taken from input(Signal)
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  ELogicExprOp ExprOp = ELogicExprOp::Add;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool UseConstB = true;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int64 ConstB = 0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UStaticItem *SignalB = nullptr;

  // Execute this output definition against the context, writing to Ctx->Output
  UFUNCTION(BlueprintCallable)
  void Compute(class ULogicContext *Ctx) const;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;
};