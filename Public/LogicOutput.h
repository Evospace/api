#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Qr/SerializableJson.h"
#include "LogicOutput.generated.h"

UENUM(BlueprintType)
enum class ELogicOutputMode : uint8 {
  Constant,
  CopyA
};

class UStaticItem;

UCLASS(BlueprintType)
class ULogicOutput : public UObject, public ISerializableJson {
  GENERATED_BODY()

  public:

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UStaticItem *OutputSignal = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  ELogicOutputMode OutputMode = ELogicOutputMode::Constant;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int64 OutputValueTrue = 1;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;
};