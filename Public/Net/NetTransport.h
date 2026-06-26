// Copyright (c) 2017 - 2026, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

/**
 * Net transport abstraction — Layer 1 of the multiplayer stack.
 * See ai/todo/lan_multiplayer_entry_plan.md.
 *
 * Connection-oriented and MULTI-peer. This is the single boundary the Steam
 * Sockets backend plugs into; nothing above this layer references UDP, sockets,
 * IP or SteamID. Star topology: a host Listen()s and gets N peers; a guest
 * Connect()s and gets exactly one peer (the host).
 *
 * Steam seam (for reference — not implemented yet):
 *   Listen   -> CreateListenSocketP2P
 *   Connect  -> ConnectP2P
 *   peer     -> HSteamNetConnection
 *   PollEvent-> ReceiveMessagesOnPoller + connection-status callbacks
 *   reliability -> k_nSteamNetworkingSend_Reliable / _Unreliable
 */

/** Host-local handle for a connection. 0 == invalid. */
using FNetPeerId = uint32;
inline constexpr FNetPeerId InvalidNetPeerId = 0;

/** Opaque address: "ip"/port for LAN, SteamID for Steam. Parsed inside the impl. */
struct FNetEndpoint {
  /** Transport-specific: IP/hostname for LAN, SteamID string for Steam. */
  FString Address;
  /** Ignored by transports that don't use ports (e.g. Steam P2P). */
  int32 Port = 0;

  FNetEndpoint() = default;
  explicit FNetEndpoint(FString InAddress, int32 InPort = 0)
    : Address(MoveTemp(InAddress)), Port(InPort) {}
};

enum class ENetReliability : uint8 {
  /** Raw datagram: may drop / reorder. Avatar stream. */
  Unreliable,
  /** Delivered, in order. Handshake + blob transfer. */
  Reliable,
};

/** Drained on the game thread via INetTransport::PollEvent. */
struct FNetEvent {
  enum class EType : uint8 {
    /** Peer is now usable (host: guest accepted; guest: host reached). */
    Connected,
    /** Peer is gone (timeout / close / error). */
    Disconnected,
    /** Payload received from Peer. */
    Data,
  };

  EType Type = EType::Data;
  FNetPeerId Peer = InvalidNetPeerId;
  /** Valid for Data events. */
  TArray<uint8> Data;
};

/**
 * Transport backend interface. Implementations: FLanUdpTransport (now),
 * FSteamSocketsTransport (next). Owned by the session layer (UNetSessionSubsystem),
 * which is the only thing that talks to it.
 */
class INetTransport {
  public:
  virtual ~INetTransport() = default;

  // -- Lifecycle ------------------------------------------------------------
  /** Host: start accepting peers on Bind. */
  virtual bool Listen(const FNetEndpoint &Bind) = 0;
  /** Guest: connect to a host. Yields exactly one peer (the host) on success. */
  virtual bool Connect(const FNetEndpoint &Target) = 0;
  /** Tear everything down: drop all peers, release the socket. */
  virtual void Close() = 0;

  // -- Messaging ------------------------------------------------------------
  /** Host targets a specific peer; a guest's only valid Peer is the host. */
  virtual bool Send(FNetPeerId Peer, ENetReliability Reliability, const TArray<uint8> &Payload) = 0;
  /** Drop a single peer (host-side kick / cleanup). */
  virtual void Disconnect(FNetPeerId Peer) = 0;

  // -- Pump -----------------------------------------------------------------
  /** Drain one queued event per call on the game thread; false when empty. */
  virtual bool PollEvent(FNetEvent &OutEvent) = 0;
  virtual bool IsListening() const = 0;
};
