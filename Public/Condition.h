#pragma once
#include "ItemMap.h"
#include "Qr/SerializableJson.h"
#include "Condition.generated.h"

class UInventoryAccess;
class UConditionWidget;

UCLASS(BlueprintType)
class ULogicContext : public UObject {
  GENERATED_BODY()
  public:
  ULogicContext();

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UItemMap *Input;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UItemMap *Output;
};

UENUM(BlueprintType)
enum class ECompareOp : uint8 {
  Less,
  Greater,
  Equal,
  NotEqual,
  LessEqual,
  GreaterEqual,
  _Count
};

UENUM(BlueprintType)
enum class EConditionMode : uint8 {
  Expr,

  And,
  Or,
  Xor,
  Not,

  Always,

  _Count
};

UCLASS(BlueprintType)
class UCondition : public UObject, public ISerializableJson {
  GENERATED_BODY()
  public:
  UFUNCTION(BlueprintCallable)
  virtual int64 Evaluate(const ULogicContext *ctx) const;

  UFUNCTION(BlueprintCallable, BlueprintCosmetic)
  float EvaluateGui(const ULogicContext *ctx) const;

  UFUNCTION(BlueprintCallable)
  void RemoveOperand(class UCondition *op);

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  TArray<UCondition *> Operands;

  UFUNCTION(BlueprintCallable, BlueprintPure)
  virtual TSubclassOf<UConditionWidget> GetWidgetClass() const;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  EConditionMode Mode = EConditionMode::Expr;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  const UStaticItem *VarA = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  const UStaticItem *VarB = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool ConstB = true;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  ECompareOp Operator = ECompareOp::Less;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int64 ConstValue = 0;

  UFUNCTION(BlueprintCallable, BlueprintPure)
  virtual FString GetLabel() const;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;
};