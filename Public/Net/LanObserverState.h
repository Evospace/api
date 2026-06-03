// Copyright (c) 2017 - 2026, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Public/Net/ObserverTransport.h"

enum class ELanObsMode : uint8 {
  None = 0,
  Host = 1,
  Guest = 2,
};

struct FLanObsState {
  ELanObsMode Mode = ELanObsMode::None;
  EObserverTransportType TransportType = EObserverTransportType::Lan;
  TUniquePtr<IObserverTransport> Transport;

  bool bHandshakeComplete = false;
  bool bHandshakeFailed = false;
  bool bSaveMissingReported = false;
  bool bAwaitingConfig = false;
  bool bAwaitingSave = false;
  FString HostSaveName;
  uint32 LocalNonce = 0;
  int32 ConfigTotalBytes = 0;
  int32 ConfigChunkSize = 0;
  int32 ConfigChunksExpected = 0;
  int32 ConfigChunksReceived = 0;
  TArray<uint8> ConfigArchive;
  TArray<uint8> ConfigChunkMask;
  int32 SaveTotalBytes = 0;
  int32 SaveChunkSize = 0;
  int32 SaveChunksExpected = 0;
  int32 SaveChunksReceived = 0;
  TArray<uint8> SaveArchive;
  TArray<uint8> SaveChunkMask;
  bool bStartGamePending = false;
  FString PendingObserverSaveName;
  uint32 ConfigArchiveCrc = 0;
  uint32 SaveArchiveCrc = 0;

  double NowSeconds = 0.0;
  double NextHelloSendAt = 0.0;
  double NextHeartbeatSendAt = 0.0;
  double NextTransformSendAt = 0.0;
  double LastTransformRecvAt = 0.0;
  uint32 HeartbeatSeq = 0;

  TWeakObjectPtr<ACharacter> LocalPawn;
  TWeakObjectPtr<ACharacter> RemoteGhost;
  FVector RemoteTargetLocation = FVector::ZeroVector;
  FRotator RemoteTargetRotation = FRotator::ZeroRotator;
  FRotator RemoteTargetHeadRotation = FRotator::ZeroRotator;
  float RemoteTargetMoveSpeed = 0.0f;
  float RemoteTargetMoveAcceleration = 0.0f;
  FVector RemoteTargetMoveVelocity = FVector::ZeroVector;
  float RemoteCurrentMoveAcceleration = 0.0f;
  FVector RemoteCurrentMoveVelocity = FVector::ZeroVector;
  bool bRemoteTargetCrouched = false;
  bool bRemoteTargetFlying = false;
  bool bRemoteTargetFalling = false;
  double LastAnimRecvAt = 0.0;
  float LastAnimSpeed = 0.0f;
};
