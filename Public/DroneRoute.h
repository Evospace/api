#pragma once

#include "CoreMinimal.h"
#include "DroneRoute.generated.h"

UENUM(BlueprintType)
enum class EDroneSendCondition : uint8 {
  Auto,
  OnMinCountReached,
  ManualOnly
};

USTRUCT(BlueprintType)
struct FDroneRoute final {
  GENERATED_BODY()

  // Куда отправлять
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString TargetStationID;

  // Фильтрация предметов (по ID или тегам)
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<FName> AllowedItems;

  // Минимальное количество предметов для отправки
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 MinItemCount = 10;

  // Задержка между отправками
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float CooldownSeconds = 15.0f;

  // Как часто отправлять
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  EDroneSendCondition SendCondition = EDroneSendCondition::Auto;

  // Внутреннее поле: накопленное время
  float TimeSinceLastSend = 0.0f;

  // Проверка на готовность к отправке
  bool CanDispatch(float DeltaTime) {

    TimeSinceLastSend += DeltaTime;

    if (SendCondition == EDroneSendCondition::ManualOnly)
      return false;

    return TimeSinceLastSend >= CooldownSeconds;
  }

  void MarkDispatched() {
    TimeSinceLastSend = 0.0f;
  }

  bool SerializeJson(TSharedPtr<FJsonObject> json);
  bool DeserializeJson(TSharedPtr<FJsonObject> json);
};