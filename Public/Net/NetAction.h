// Copyright (c) 2017 - 2026, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

class ADimension;
class UBlockLogic;
class UNetSessionSubsystem;

struct FNetAction {
  virtual ~FNetAction() = default;

  virtual uint8 GetId() const = 0;

  virtual void Serialize(FArchive &Ar) = 0;

  virtual void Apply(const struct FNetActionContext &Ctx) = 0;

  virtual bool AllowDuringRemoteApply() const { return false; }
};

struct FNetActionContext {
  UNetSessionSubsystem &Net;
  UWorld *World = nullptr;
  ADimension *Dim = nullptr;
};

inline constexpr int32 MaxNetActionArrayLen = 4096;

inline bool SerializeBoundedCount(FArchive &Ar, int32 &Count) {
  Ar << Count;
  if (Ar.IsLoading() && (Ar.IsError() || Count < 0 || Count > MaxNetActionArrayLen)) {
    Ar.SetError();
    return false;
  }
  return true;
}

class FNetActionRegistry {
  public:
  using FFactory = TUniquePtr<FNetAction> (*)();

  static FNetActionRegistry &Get();

  void Register(uint8 Id, FFactory Factory);
  TUniquePtr<FNetAction> Create(uint8 Id) const;
  TArray<uint8> GetRegisteredIds() const;

  private:
  TMap<uint8, FFactory> Factories;
};

struct FNetActionRegistrar {
  FNetActionRegistrar(uint8 Id, FNetActionRegistry::FFactory Factory);
};

#define EVO_REGISTER_NET_ACTION(Type)                         \
  static const FNetActionRegistrar GNetActionRegistrar##Type( \
    Type::ActionId, []() -> TUniquePtr<FNetAction> { return TUniquePtr<FNetAction>(new Type()); });

void SubmitNetAction(const UObject *WorldContext, FNetAction &Action);

UBlockLogic *NetActionResolveRootBlock(ADimension *Dim, const FIntVector &Pos);
