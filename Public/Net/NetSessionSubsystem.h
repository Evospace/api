// Copyright (c) 2017 - 2026, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"

#include "Public/Net/NetBlobChannel.h"
#include "Public/Net/NetTransport.h"

#include "NetSessionSubsystem.generated.h"

UENUM(BlueprintType)
enum class ENetSessionStatus : uint8 {
  None = 0,
  Listening,
  PeerConnecting,
  PeerJoined,
  PeerLeft,
  PeerRenamed,
  Connecting,
  ReceivingWorld,
  Joined,
  Failed,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
  FOnNetSessionStatus, ENetSessionStatus, Status, int32, PeerId, const FString &, Message);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNetSnapshotProgress, int32, PeerId, float, Pct);

class ADimension;
class ADroppedInventory;
class UBlockLogic;
class UInventoryAccess;
struct FMapChangeSet;
struct FNetAction;

UCLASS()
class UNetSessionSubsystem : public UGameInstanceSubsystem, public FTickableGameObject {
  GENERATED_BODY()

  public:
  virtual void Initialize(FSubsystemCollectionBase &Collection) override;
  virtual void Deinitialize() override;

  virtual void Tick(float DeltaTime) override;
  virtual TStatId GetStatId() const override;
  virtual bool IsTickable() const override {
    return bActive || !bSteamRelayWarmedUp || !PendingSteamJoinHostId.IsEmpty() || !PendingHostSnapshotPeers.IsEmpty();
  }
  virtual bool IsTickableInEditor() const override { return false; }
  virtual bool IsTickableWhenPaused() const override { return true; }
  virtual UWorld *GetTickableGameObjectWorld() const override { return GetWorld(); }

  UFUNCTION(BlueprintCallable, Category = "Evospace|Net")
  bool HostSession(int32 Port = 27050);

  UFUNCTION(BlueprintCallable, Category = "Evospace|Net")
  bool JoinSession(const FString &Ip, int32 Port = 27050);

  UFUNCTION(BlueprintCallable, Category = "Evospace|Net")
  bool HostSessionSteam();

  UFUNCTION(BlueprintCallable, Category = "Evospace|Net")
  bool JoinSessionSteam(const FString &HostSteamId);

  UFUNCTION(BlueprintCallable, Category = "Evospace|Net")
  void OpenSteamFriendsOverlay();

  UFUNCTION(BlueprintCallable, Category = "Evospace|Net")
  void StopSession();

  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Evospace|Net")
  bool IsHost() const { return Role == ESessionRole::Host; }

  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Evospace|Net")
  bool IsActive() const { return bActive; }

  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Evospace|Net")
  int32 GetLocalPeerId() const { return (int32)LocalPeerId; }

  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Evospace|Net")
  FString GetPlayerName() const;

  UFUNCTION(BlueprintCallable, Category = "Evospace|Net")
  void SetPlayerName(const FString &Name);

  void SubmitLocalMapEdit(ADimension *Dim, const FMapChangeSet &Set);

  static UNetSessionSubsystem *Get(const UObject *WorldContext);

  static FString GetLocalProfileId(const UObject *WorldContext);

  bool IsApplyingRemoteAction() const { return bApplyingRemoteAction; }

  void SubmitAction(FNetAction &Action);

  void BeginBlockInvCapture(UBlockLogic *Block);
  void EndBlockInvCapture();

  void RegisterDropActor(ADroppedInventory *Drop);
  bool HasDropActor(const FGuid &Id) const;
  ADroppedInventory *ClaimDropActor(const FGuid &Id);

  UPROPERTY(BlueprintAssignable, Category = "Evospace|Net")
  FOnNetSessionStatus OnStatus;

  UPROPERTY(BlueprintAssignable, Category = "Evospace|Net")
  FOnNetSnapshotProgress OnSnapshotProgress;

  void OnLocalSurfaceChanged();

  struct FNetGhostView {
    int32 PeerId = 0;
    FString Name;
    FVector Location = FVector::ZeroVector;
    float Yaw = 0.0f;
  };

  void GetGhostViews(TArray<FNetGhostView> &Out) const;

  private:
  enum class ESessionRole : uint8 { None,
                                    Host,
                                    Guest };

  enum class ETransportKind : uint8 { Lan,
                                      Steam };

  bool StartHost(int32 Port, ETransportKind Kind);
  bool StartGuest(const FString &Address, int32 Port, ETransportKind Kind);

  enum class EPeerHandshake : uint8 {
    Connecting,
    AwaitingReady,
    Joined,
  };

  struct FPeerSession {
    FNetPeerId PeerId = InvalidNetPeerId;
    FString DisplayName;
    EPeerHandshake State = EPeerHandshake::Connecting;
  };

  void HandleConnected(FNetPeerId Peer);
  void HandleDisconnected(FNetPeerId Peer);
  void HandleData(FNetPeerId Peer, const TArray<uint8> &Bytes);

  void HostOnHello(FNetPeerId Peer, uint32 ProtocolVer, uint32 Nonce, const FString &Name);
  void HostOnReady(FNetPeerId Peer);
  void BroadcastToJoined(const TArray<uint8> &Bytes, FNetPeerId Except);

  void GuestOnWelcome(uint32 NonceEcho, uint32 AssignedPeerId, int64 Seed, const FString &SaveName);
  void GuestOnReject(uint8 Reason);
  void GuestOnPeerJoined(FNetPeerId PeerId, const FString &Name);
  void GuestOnPeerLeft(FNetPeerId PeerId);

  void HandlePeerRenamed(FNetPeerId FromPeer, FNetPeerId RenamedPeer, const FString &Name);

  bool SendLocalProfile();
  void HandleProfileUpload(FNetPeerId FromPeer, const TArray<uint8> &Bytes);

  void OnBlobReceived(FNetPeerId Peer, uint32 BlobId, const TArray<uint8> &Data);

  void SampleAndSendAvatar();
  void HandleAvatar(FNetPeerId FromPeer, const TArray<uint8> &Bytes);
  void ApplyGhostState(FNetPeerId PeerId, const FVector &Loc, const FRotator &ControlRot,
                       const FRotator &HeadRot, const FVector &Velocity, uint8 Flags);
  void UpdateGhosts(float DeltaTime);
  void DestroyGhost(FNetPeerId PeerId);
  void DestroyAllGhosts();
  class AMainCharacter *GetLocalMainCharacter() const;

  void HandleMapEdit(FNetPeerId FromPeer, const TArray<uint8> &Bytes);
  ADimension *ResolveActiveDimension() const;

  void SendPlayerAction(const TArray<uint8> &Bytes);
  void HandlePlayerAction(FNetPeerId FromPeer, const TArray<uint8> &Bytes);
  void SubmitBlockInvDeltas();

  void EmitStatus(ENetSessionStatus Status, FNetPeerId PeerId, const FString &Message);
  void EnsureDisplayName();

  UFUNCTION()
  void OnSteamRichPresenceJoinRequested(int64 FriendSteamId, const FString &ConnectString);

  void ProcessPendingSteamJoin();
  void ProcessPendingHostSnapshots();

  FString PendingSteamJoinHostId;
  TArray<FNetPeerId> PendingHostSnapshotPeers;

  TUniquePtr<INetTransport> Transport;
  FNetBlobChannel BlobChannel;
  ESessionRole Role = ESessionRole::None;
  bool bActive = false;
  bool bSteamRelayWarmedUp = false;
  FString LocalDisplayName;
  FNetPeerId LocalPeerId = InvalidNetPeerId;

  TMap<FNetPeerId, FPeerSession> Peers;

  FNetPeerId HostTransportPeer = InvalidNetPeerId;
  uint32 GuestNonce = 0;
  int64 HostSeed = 0;
  FString HostSaveName;
  TMap<FNetPeerId, FString> Members;

  bool bSnapshotReceived = false;

  float GuestConnectElapsed = 0.f;
  static constexpr float GuestHandshakeTimeoutSec = 15.f;

  float GuestSnapshotStallElapsed = 0.f;
  static constexpr float GuestSnapshotStallTimeoutSec = 45.f;

  struct FGhostState {
    TWeakObjectPtr<class AMainCharacter> Actor;
    FVector TargetLoc = FVector::ZeroVector;
    FRotator TargetControlRot = FRotator::ZeroRotator;
    FRotator TargetHeadRot = FRotator::ZeroRotator;
    FVector Velocity = FVector::ZeroVector;
    uint8 Flags = 0;
  };
  TMap<FNetPeerId, FGhostState> Ghosts;
  float AvatarSendAccum = 0.f;

  float ProfileSendAccum = 0.f;
  static constexpr float ProfileSendIntervalSec = 30.f;

  int32 HostEditSeq = 0;

  bool bApplyingRemoteAction = false;

  struct FBlockInvCapture {
    FIntVector BlockPos = FIntVector::ZeroValue;
    TArray<TWeakObjectPtr<UInventoryAccess>> Inventories;
    TArray<TMap<const class UStaticItem *, int64>> Sums;
    bool bArmed = false;
  };
  FBlockInvCapture InvCapture;
  int32 InvCaptureDepth = 0;

  void UpdateBlockConfigWatch(float DeltaTime);
  void FlushBlockConfigWatch();
  void RebaseBlockConfigWatch();
  TWeakObjectPtr<UBlockLogic> ConfigWatchBlock;
  FIntVector ConfigWatchPos = FIntVector::ZeroValue;
  FString ConfigWatchJson;
  float ConfigWatchAccum = 0.f;

  TMap<FGuid, TWeakObjectPtr<ADroppedInventory>> DropActors;
};

struct FScopedBlockInvSync {
  FScopedBlockInvSync(const UObject *WorldContext, UBlockLogic *Block) {
    Net = UNetSessionSubsystem::Get(WorldContext);
    if (Net) {
      Net->BeginBlockInvCapture(Block);
    }
  }
  ~FScopedBlockInvSync() {
    if (Net) {
      Net->EndBlockInvCapture();
    }
  }
  FScopedBlockInvSync(const FScopedBlockInvSync &) = delete;
  FScopedBlockInvSync &operator=(const FScopedBlockInvSync &) = delete;

  private:
  UNetSessionSubsystem *Net = nullptr;
};
