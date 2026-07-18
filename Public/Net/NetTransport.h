// Copyright (c) 2017 - 2026, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

using FNetPeerId = uint32;
inline constexpr FNetPeerId InvalidNetPeerId = 0;

struct FNetEndpoint {
  FString Address;
  int32 Port = 0;

  FNetEndpoint() = default;
  explicit FNetEndpoint(FString InAddress, int32 InPort = 0)
    : Address(MoveTemp(InAddress)), Port(InPort) {}
};

enum class ENetReliability : uint8 {
  Unreliable,
  Reliable,
};

struct FNetEvent {
  enum class EType : uint8 {
    Connected,
    Disconnected,
    Data,
  };

  EType Type = EType::Data;
  FNetPeerId Peer = InvalidNetPeerId;
  TArray<uint8> Data;
};

class INetTransport {
  public:
  virtual ~INetTransport() = default;

  virtual bool Listen(const FNetEndpoint &Bind) = 0;
  virtual bool Connect(const FNetEndpoint &Target) = 0;
  virtual void Close() = 0;

  virtual bool Send(FNetPeerId Peer, ENetReliability Reliability, const TArray<uint8> &Payload) = 0;
  virtual void Disconnect(FNetPeerId Peer) = 0;

  virtual bool PollEvent(FNetEvent &OutEvent) = 0;
  virtual bool IsListening() const = 0;
};
