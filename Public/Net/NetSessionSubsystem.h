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
  Listening, // host: socket open, accepting guests
  PeerConnecting, // host: a guest's transport connected, handshake in progress
  PeerJoined, // a peer finished joining (roster grew)
  PeerLeft, // a peer disconnected (roster shrank)
  Joined, // guest: we finished joining the host
  Failed, // error; Message carries detail
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
  FOnNetSessionStatus, ENetSessionStatus, Status, int32, PeerId, const FString &, Message);

// Snapshot transfer progress for a peer: Pct in [0,1]. Host fires it while sending,
// guest fires it while receiving.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNetSnapshotProgress, int32, PeerId, float, Pct);

class ADimension;
struct FMapChangeSet;

/**
 * Multiplayer Layer 2 — session / membership (ai/todo/lan_multiplayer_entry_plan.md).
 *
 * Owns one INetTransport and runs the handshake + membership model on the game thread.
 * Star topology: a host accepts N guests; each guest talks only to the host. Lifecycle
 * and routing only — blob transfer (L3) and avatar replication (L4) live in their own
 * objects this will later compose.
 *
 * Global PeerId == the host's transport handle for a guest; the host itself is PeerId 0.
 *
 * Handshake:  Guest -Hello-> Host -Welcome-> Guest [-> snapshot transfer (M3) ->]
 *             Guest -Ready-> Host, then Host broadcasts PeerJoined and backfills the roster.
 * M2 scope is the framing + membership + status events; the snapshot step is deferred,
 * so the guest currently sends Ready immediately after Welcome.
 */
UCLASS()
class UNetSessionSubsystem : public UGameInstanceSubsystem, public FTickableGameObject {
  GENERATED_BODY()

  public:
  virtual void Initialize(FSubsystemCollectionBase &Collection) override;
  virtual void Deinitialize() override;

  // FTickableGameObject
  virtual void Tick(float DeltaTime) override;
  virtual TStatId GetStatId() const override;
  virtual bool IsTickable() const override {
    return bActive || !PendingSteamJoinHostId.IsEmpty() || !PendingHostSnapshotPeers.IsEmpty();
  }
  virtual bool IsTickableInEditor() const override { return false; }
  virtual bool IsTickableWhenPaused() const override { return true; }
  virtual UWorld *GetTickableGameObjectWorld() const override { return GetWorld(); }

  /** Host a LAN session. Requires a save already loaded (provides seed + SaveName). */
  UFUNCTION(BlueprintCallable, Category = "Evospace|Net")
  bool HostSession(int32 Port = 27050);

  /** Join a LAN host at Ip:Port. */
  UFUNCTION(BlueprintCallable, Category = "Evospace|Net")
  bool JoinSession(const FString &Ip, int32 Port = 27050);

  /** Host over Steam (P2P). Production path; requires the Steam client. Requires a save loaded. */
  UFUNCTION(BlueprintCallable, Category = "Evospace|Net")
  bool HostSessionSteam();

  /** Join a Steam host by its SteamID64 (P2P). Production path; requires the Steam client. */
  UFUNCTION(BlueprintCallable, Category = "Evospace|Net")
  bool JoinSessionSteam(const FString &HostSteamId);

  /**
   * Open the Steam friends overlay. Friends who called HostSessionSteam show "Join Game"
   * in the overlay context menu; Steam delivers GameRichPresenceJoinRequested to connect.
   */
  UFUNCTION(BlueprintCallable, Category = "Evospace|Net")
  void OpenSteamFriendsOverlay();

  /** Stop hosting/guesting and drop all peers. */
  UFUNCTION(BlueprintCallable, Category = "Evospace|Net")
  void StopSession();

  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Evospace|Net")
  bool IsHost() const { return Role == ESessionRole::Host; }

  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Evospace|Net")
  bool IsActive() const { return bActive; }

  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Evospace|Net")
  int32 GetLocalPeerId() const { return (int32)LocalPeerId; }

  /** Name advertised to others. Optional; defaults to the machine name. */
  UFUNCTION(BlueprintCallable, Category = "Evospace|Net")
  void SetDisplayName(const FString &Name) { LocalDisplayName = Name; }

  /**
   * Route a local player world-edit through the command pipeline. Called from the universal
   * edit choke point (AMainPlayerController::PushMapUndo). No-op in singleplayer (the edit is
   * already applied locally); host broadcasts it, guest sends it to the host. See
   * ai/todo plan: unified player-action command pipeline.
   */
  void SubmitLocalMapEdit(ADimension *Dim, const FMapChangeSet &Set);

  UPROPERTY(BlueprintAssignable, Category = "Evospace|Net")
  FOnNetSessionStatus OnStatus;

  UPROPERTY(BlueprintAssignable, Category = "Evospace|Net")
  FOnNetSnapshotProgress OnSnapshotProgress;

  /** Clear remote avatar ghosts after the local player switches presentation surface. */
  void OnLocalSurfaceChanged();

  private:
  enum class ESessionRole : uint8 { None,
                                    Host,
                                    Guest };

  // Which Layer-1 backend to use. LAN is the dev path; Steam is production internet play.
  enum class ETransportKind : uint8 { Lan,
                                      Steam };

  bool StartHost(int32 Port, ETransportKind Kind);
  bool StartGuest(const FString &Address, int32 Port, ETransportKind Kind);

  enum class EPeerHandshake : uint8 {
    Connecting, // host: transport up, awaiting Hello
    AwaitingReady, // host: Welcome sent, awaiting Ready (snapshot transfer slots in here)
    Joined,
  };

  struct FPeerSession {
    FNetPeerId PeerId = InvalidNetPeerId;
    FString DisplayName;
    EPeerHandshake State = EPeerHandshake::Connecting;
  };

  // Transport event handling.
  void HandleConnected(FNetPeerId Peer);
  void HandleDisconnected(FNetPeerId Peer);
  void HandleData(FNetPeerId Peer, const TArray<uint8> &Bytes);

  // Host side.
  void HostOnHello(FNetPeerId Peer, uint32 ProtocolVer, uint32 Nonce, const FString &Name);
  void HostOnReady(FNetPeerId Peer);
  void BroadcastToJoined(const TArray<uint8> &Bytes, FNetPeerId Except);

  // Guest side.
  void GuestOnWelcome(uint32 NonceEcho, uint32 AssignedPeerId, int64 Seed, const FString &SaveName);
  void GuestOnReject(uint8 Reason);
  void GuestOnPeerJoined(FNetPeerId PeerId, const FString &Name);
  void GuestOnPeerLeft(FNetPeerId PeerId);

  // Snapshot transfer (L3). Host sends the save blob after Welcome; guest sends Ready
  // once it arrives.
  void OnBlobReceived(FNetPeerId Peer, uint32 BlobId, const TArray<uint8> &Data);

  // Avatar replication (L4) — presentation only: float, interpolated, non-authoritative.
  // Each peer broadcasts its pawn transform/head/anim at ~20 Hz; remote peers drive a
  // collision/movement/camera-disabled ghost of the same pawn class. The host relays guest
  // packets so guests see each other (star topology).
  void SampleAndSendAvatar();
  void HandleAvatar(FNetPeerId FromPeer, const TArray<uint8> &Bytes);
  void ApplyGhostState(FNetPeerId PeerId, const FVector &Loc, const FRotator &ControlRot,
                       const FRotator &HeadRot, const FVector &Velocity, uint8 Flags);
  void UpdateGhosts(float DeltaTime);
  void DestroyGhost(FNetPeerId PeerId);
  void DestroyAllGhosts();
  class AMainCharacter *GetLocalMainCharacter() const;

  // Player-action command pipeline. A command is a serialized FMapChangeSet; the apply is
  // ADimension::ApplyMapChangeSet. Host orders edits and rebroadcasts; guests apply.
  void HandleMapEdit(FNetPeerId FromPeer, const TArray<uint8> &Bytes);
  ADimension *ResolveActiveDimension() const;

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
  FString LocalDisplayName;
  FNetPeerId LocalPeerId = InvalidNetPeerId;

  // Host membership.
  TMap<FNetPeerId, FPeerSession> Peers;

  // Guest state.
  FNetPeerId HostTransportPeer = InvalidNetPeerId;
  uint32 GuestNonce = 0;
  int64 HostSeed = 0;
  FString HostSaveName;
  TMap<FNetPeerId, FString> Members; // global id -> name (peers other than us, for UI)

  // True once the guest has unpacked the host snapshot and entered the world (Ready sent).
  // Gates avatar sends so we don't broadcast before we have a pawn.
  bool bSnapshotReceived = false;

  // Guest handshake watchdog: seconds since StartGuest. If the host never answers (no Welcome,
  // so LocalPeerId stays Invalid) within GuestHandshakeTimeoutSec the join is failed instead of
  // hanging forever. Welcome arrives before the (possibly long) snapshot transfer, so this only
  // covers the connect+Hello+Welcome control phase, never the blob stream.
  float GuestConnectElapsed = 0.f;
  static constexpr float GuestHandshakeTimeoutSec = 15.f;

  // Avatar replication state.
  struct FGhostState {
    TWeakObjectPtr<class AMainCharacter> Actor;
    FVector TargetLoc = FVector::ZeroVector;
    FRotator TargetControlRot = FRotator::ZeroRotator;
    FRotator TargetHeadRot = FRotator::ZeroRotator;
    FVector Velocity = FVector::ZeroVector;
    uint8 Flags = 0;
  };
  TMap<FNetPeerId, FGhostState> Ghosts; // keyed by global PeerId (host == 0)
  float AvatarSendAccum = 0.f;

  // Host-monotonic order stamp for confirmed map edits (own + relayed guest edits).
  int32 HostEditSeq = 0;
};
