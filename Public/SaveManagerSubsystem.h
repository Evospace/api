// Copyright (c) 2017 - 2026, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Public/Net/ObserverTransport.h"

#include "SaveManagerSubsystem.generated.h"

UCLASS()
class USaveManagerSubsystem : public UGameInstanceSubsystem {
  GENERATED_BODY()

  public:
  UFUNCTION(BlueprintCallable, Category = "Evospace|Save")
  bool StartGameFromSave(const FString &SaveName, bool bListenServer);

  UFUNCTION(BlueprintCallable, Category = "Evospace|Observer")
  bool ObserverHost(EObserverTransportType Transport, const FString &SaveName, int32 Port = 27050);

  UFUNCTION(BlueprintCallable, Category = "Evospace|Observer")
  bool ObserverConnect(EObserverTransportType Transport, const FString &Target, int32 Port = 27050);
};
