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
  Connecting, // guest: join attempt started; connecting transport + handshaking (no progress yet)
  ReceivingWorld, // guest: Welcome accepted, world snapshot transfer starting (Pct via OnSnapshotProgress)
  Joined, // guest: we finished joining the host
  Failed, // error; Message carries detail
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
  FOnNetSessionStatus, ENetSessionStatus, Status, int32, PeerId, const FString &, Message);

// Snapshot transfer progress for a peer: Pct in [0,1]. Host fires it while sending,
// guest fires it while receiving.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNetSnapshotProgress, int32, PeerId, float, Pct);

class ADimension;
class ADroppedInventory;
class UBlockLogic;
class UInventoryAccess;
class URecipe;
class UStaticResearch;
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
    return bActive || !bSteamRelayWarmedUp || !PendingSteamJoinHostId.IsEmpty() || !PendingHostSnapshotPeers.IsEmpty();
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

  // -- Player-action command pipeline (non-map-edit actions) ------------------
  // Same star-topology relay as map edits, but the payload is an intent delta, not absolute
  // cell state, so the host relays to everyone EXCEPT the originator (the originator already
  // applied it locally; re-applying would double it).

  /** Convenience resolver (any world-context object). Null when no game instance. */
  static UNetSessionSubsystem *Get(const UObject *WorldContext);

  /** True while this peer is applying a remote player-action; Submit* hooks must no-op. */
  bool IsApplyingRemoteAction() const { return bApplyingRemoteAction; }

  /**
   * Block-inventory diff capture. Wrap any local player operation that may move items
   * between the player inventory and the open block's inventories (clicks, drags, drops).
   * Begin snapshots the block's per-item sums; End diffs them and broadcasts the deltas.
   * Re-entrant (depth-counted): only the outermost pair sends. No-op without a session.
   */
  void BeginBlockInvCapture(UBlockLogic *Block);
  void EndBlockInvCapture();

  /** Recipe picked in a crafter GUI. Recipe == nullptr syncs a reset. */
  void SubmitRecipeSelect(UBlockLogic *Block, const URecipe *Recipe);

  /** Settings JSON pasted onto a block (clipboard paste in the block GUI). */
  void SubmitBlockSettings(UBlockLogic *Block, const FString &SettingsJson);

  /** Research queue edits from the research screen. */
  void SubmitResearchEnqueue(const UStaticResearch *Research);
  void SubmitResearchDequeue();

  /** Track a spawned drop actor so remote pickup commands can resolve it. */
  void RegisterDropActor(ADroppedInventory *Drop);
  /** Broadcast a locally spawned drop (called once from the actor when physics is live). */
  void AnnounceLocalDrop(ADroppedInventory *Drop);
  /** Broadcast that the local player consumed a drop; remote copies get destroyed. */
  void SubmitDropPickup(const FGuid &DropId);

  /** Screwdriver rotation: peers re-run it on their own block copy at the absolute orientation. */
  void SubmitBlockRotate(const FIntVector &Pos, const FQuat &Rotation);

  /** Mop clean: peers hide their own footprint decals inside the world sphere (Center, Radius). */
  void SubmitFootprintClean(const FVector &Center, float Radius);

  /** Train placed at a station (train placer item); peers re-run PlaceTrainAtStation on their sim. */
  void SubmitTrainPlace(UBlockLogic *StationRoot, int32 NumCars);

  /** Train removed by index; indices are lockstep-deterministic across peers. */
  void SubmitTrainRemove(int32 TrainIndex);

  /** Station renamed (rail or drone); peers re-run RenameStation on the block at Pos. */
  void SubmitStationRename(UBlockLogic *StationRoot, const FString &NewName);

  // Train schedule edits (train GUI); peers re-run the same URailwayManager mutation
  // by train index (indices are lockstep-deterministic across peers).
  void SubmitTrainScheduleAddStop(int32 TrainIndex, int32 InsertIndex, const FString &StationName);
  void SubmitTrainScheduleRemoveStop(int32 TrainIndex, int32 StopIndex);
  void SubmitTrainScheduleMoveStop(int32 TrainIndex, int32 StopIndex, int32 NewStopIndex);
  void SubmitTrainScheduleSetStop(int32 TrainIndex, int32 StopIndex, const FString &StationName);
  void SubmitTrainScheduleSetLoop(int32 TrainIndex, bool bLoop);
  /** Full serialized DepartureCondition of one stop (the GUI edits it in place, then commits). */
  void SubmitTrainScheduleSetCondition(int32 TrainIndex, int32 StopIndex, const FString &ConditionJson);
  /** "Depart now": the train skips its departure condition at the next dispatch opportunity. */
  void SubmitTrainForceDepart(int32 TrainIndex);

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

  // Non-map-edit player actions (recipe/settings/research/inventory/drops).
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
  // Steam SDR warm-up done (or n/a). False until SteamAPI comes up; Tick retries until then.
  bool bSteamRelayWarmedUp = false;
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

  // Guest snapshot-transfer stall watchdog. Starts at Welcome and resets on every received
  // chunk (OnRecvProgress). Fails the join only if the stream goes fully silent for longer than
  // the timeout — a big world may legitimately take a long time to transfer, so this is stall-
  // based, not a total-time cap. The budget is generous enough to also cover the host packing
  // the snapshot (the gap between Welcome and the first chunk, during which no progress fires).
  float GuestSnapshotStallElapsed = 0.f;
  static constexpr float GuestSnapshotStallTimeoutSec = 45.f;

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

  // -- Player-action state ----------------------------------------------------

  // Guard: true while applying a remote action, so the local Submit* hooks inside the
  // shared apply code (SelectRecipe, LoadSettings, Enqueue...) don't echo it back.
  bool bApplyingRemoteAction = false;

  // Block-inventory diff capture (see BeginBlockInvCapture).
  struct FBlockInvCapture {
    FIntVector BlockPos = FIntVector::ZeroValue;
    TArray<TWeakObjectPtr<UInventoryAccess>> Inventories;
    TArray<TMap<const class UStaticItem *, int64>> Sums;
    bool bArmed = false;
  };
  FBlockInvCapture InvCapture;
  int32 InvCaptureDepth = 0;

  // Live drop actors by net id (local + remote spawns), for remote pickup resolution.
  TMap<FGuid, TWeakObjectPtr<ADroppedInventory>> DropActors;
};

/**
 * RAII wrapper for the block-inventory diff capture around a local player inventory
 * operation. Safe with a null context / no session (no-ops).
 */
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
