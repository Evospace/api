// Copyright (c) 2017 - 2026, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "Public/Net/ObserverTransport.h"

#include "LanObserverSubsystem.generated.h"

/**
 * Minimal custom LAN "observer" transport (protocol v0).
 *
 * Goals for v0:
 * - Host listens on UDP port and replies to Hello with Welcome(seed)
 * - Guest connects by IP:Port, sends Hello until Welcome received
 * - Both sides exchange Heartbeat
 *
 * This does NOT use UE replication and is intentionally limited to a single guest.
 */
UCLASS()
class ULanObserverSubsystem : public UGameInstanceSubsystem, public FTickableGameObject {
  GENERATED_BODY()

  public:
  virtual void Initialize(FSubsystemCollectionBase &Collection) override;
  virtual void Deinitialize() override;

  // FTickableGameObject interface
  virtual void Tick(float DeltaTime) override;
  virtual TStatId GetStatId() const override;
  virtual bool IsTickable() const override { return true; }
  virtual bool IsTickableInEditor() const override { return false; }
  virtual bool IsTickableWhenPaused() const override { return false; }
  virtual UWorld *GetTickableGameObjectWorld() const override { return GetWorld(); }

  /** Start observer host (LAN or Steam). */
  UFUNCTION(BlueprintCallable, Category = "Evospace|Observer")
  void ObserverHost(EObserverTransportType Transport, int32 Port = 27050);

  /** Connect guest to host (Target = IP for LAN, SteamId for Steam). */
  UFUNCTION(BlueprintCallable, Category = "Evospace|Observer")
  void ObserverConnect(EObserverTransportType Transport, const FString &Target, int32 Port = 27050);

  /** Stop host/guest and close transport. */
  UFUNCTION(BlueprintCallable, Category = "Evospace|Observer")
  void ObserverStop();

  /** Start host on UDP port (default 27050). */
  UFUNCTION(Exec)
  void LanObsHost(int32 Port = 27050);

  /** Connect guest to host by IPv4 + UDP port (default 27050). */
  UFUNCTION(Exec)
  void LanObsConnect(const FString &HostIp, int32 Port = 27050);

  /** Stop host/guest and close socket. */
  UFUNCTION(Exec)
  void LanObsStop();
};

