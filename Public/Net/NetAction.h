// Copyright (c) 2017 - 2026, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

class ADimension;
class UBlockLogic;
class UNetSessionSubsystem;

/**
 * One remote-mirrored player action of the command pipeline (message type 0xB1).
 *
 * An action is defined in exactly one place: a struct with the payload fields, a
 * bidirectional Serialize (FArchive reads and writes through the same code, so the wire
 * format cannot drift between sender and receiver) and an Apply that re-runs the intent
 * on the receiving peer. Domain code registers its actions with EVO_REGISTER_NET_ACTION
 * in its own cpp; the session subsystem only routes bytes and knows no concrete action.
 *
 * Star-topology relay: the originator applies locally and submits; the host applies and
 * relays to every other guest. Payloads are intent deltas, not absolute state, so the
 * relay excludes the originator (re-applying would double the action).
 */
struct FNetAction {
  virtual ~FNetAction() = default;

  /** Wire id (second byte of the 0xB1 message). Stable: it IS the protocol. */
  virtual uint8 GetId() const = 0;

  /** Reads or writes the payload after the two header bytes. Loading code must leave the
   * archive in an error state on malformed input (see SerializeBoundedCount). */
  virtual void Serialize(FArchive &Ar) = 0;

  /** Re-runs the action on the receiving peer. Runs under the re-entrancy guard, so
   * Submit hooks inside the shared apply paths don't echo the action back. */
  virtual void Apply(const struct FNetActionContext &Ctx) = 0;

  /** Actions emitted as synchronous follow-ups of applying a remote action (drop
   * announce / instant pickup) must pass the re-entrancy guard; for everything else a
   * submit during a remote apply is an echo and is dropped. */
  virtual bool AllowDuringRemoteApply() const { return false; }
};

/** What Apply gets to work with, resolved once per received action. */
struct FNetActionContext {
  UNetSessionSubsystem &Net;
  UWorld *World = nullptr;
  /** Active dimension; null while no world is up (actions must tolerate it). */
  ADimension *Dim = nullptr;
};

/** Hard cap for any array count travelling in an action payload. Payloads come from
 * remote peers; a count is validated against this before any allocation or loop. */
inline constexpr int32 MaxNetActionArrayLen = 4096;

/**
 * Bounded array count for Serialize implementations. Saving writes Count as-is; loading
 * validates it and poisons the archive (and returns false) on malformed input, so the
 * receive path drops the whole action.
 */
inline bool SerializeBoundedCount(FArchive &Ar, int32 &Count) {
  Ar << Count;
  if (Ar.IsLoading() && (Ar.IsError() || Count < 0 || Count > MaxNetActionArrayLen)) {
    Ar.SetError();
    return false;
  }
  return true;
}

/** Wire id -> factory. Filled at static init by EVO_REGISTER_NET_ACTION. */
class FNetActionRegistry {
  public:
  using FFactory = TUniquePtr<FNetAction> (*)();

  static FNetActionRegistry &Get();

  void Register(uint8 Id, FFactory Factory);
  /** Null for unknown ids. */
  TUniquePtr<FNetAction> Create(uint8 Id) const;
  /** Every registered wire id (unordered). For diagnostics / smoke tests. */
  TArray<uint8> GetRegisteredIds() const;

  private:
  TMap<uint8, FFactory> Factories;
};

struct FNetActionRegistrar {
  FNetActionRegistrar(uint8 Id, FNetActionRegistry::FFactory Factory);
};

/** Registers Type (which must expose `static constexpr uint8 ActionId`) for the receive
 * path. Place next to the action's Serialize/Apply definitions in the domain cpp. */
#define EVO_REGISTER_NET_ACTION(Type)                         \
  static const FNetActionRegistrar GNetActionRegistrar##Type( \
    Type::ActionId, []() -> TUniquePtr<FNetAction> { return TUniquePtr<FNetAction>(new Type()); });

/** Resolve the session and submit in one call. Safe with a null context, no game
 * instance or no session (no-ops), so call sites don't need their own checks. */
void SubmitNetAction(const UObject *WorldContext, FNetAction &Action);

/** Root block logic at a world-block position (parts resolve to their root), or null. */
UBlockLogic *NetActionResolveRootBlock(ADimension *Dim, const FIntVector &Pos);
