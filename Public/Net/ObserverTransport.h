// Copyright (c) 2017 - 2026, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "ObserverTransport.generated.h"

UENUM(BlueprintType)
enum class EObserverTransportType : uint8 {
  Lan = 0,
  Steam = 1,
};

struct FObserverInboundPacket {
  FString From;
  TArray<uint8> Data;
};

class IObserverTransport {
  public:
  virtual ~IObserverTransport() = default;

  virtual bool StartHost(int32 Port) = 0;
  virtual bool Connect(const FString &Target, int32 Port) = 0;
  virtual void Stop() = 0;

  virtual bool Send(const TArray<uint8> &Payload) = 0;
  virtual bool DequeuePacket(FObserverInboundPacket &OutPacket) = 0;

  virtual bool IsConnected() const = 0;
  virtual FString GetRemoteId() const = 0;
};

