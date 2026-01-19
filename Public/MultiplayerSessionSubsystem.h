// Copyright (c) 2017 - 2026, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include <OnlineSessionSettings.h>

#include "MultiplayerSessionSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FMultiplayerSessionInfo {
  GENERATED_BODY()

  UPROPERTY(BlueprintReadOnly)
  FString SessionId;

  UPROPERTY(BlueprintReadOnly)
  FString HostName;

  UPROPERTY(BlueprintReadOnly)
  int32 CurrentPlayers = 0;

  UPROPERTY(BlueprintReadOnly)
  int32 MaxPlayers = 0;

  UPROPERTY(BlueprintReadOnly)
  int32 PingMs = 0;

  UPROPERTY(BlueprintReadOnly)
  bool IsLan = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionsFound, const TArray<FMultiplayerSessionInfo> &, Sessions);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionOpError, const FString &, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSessionHosted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSessionJoined);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSessionLeft);

UCLASS()
class UMultiplayerSessionSubsystem : public UGameInstanceSubsystem {
  GENERATED_BODY()

  public:
  /** Create an advertised session. After success, the server travels to MapPath with ?listen. */
  UFUNCTION(BlueprintCallable, Category = "Evospace|Multiplayer")
  void HostSession(bool useLan, int32 maxPlayers, const FString &MapPath);

  /** Find sessions. Results arrive via OnSessionsFound. */
  UFUNCTION(BlueprintCallable, Category = "Evospace|Multiplayer")
  void FindSessions(bool useLan, int32 maxResults);

  /** Join one of the sessions returned by the last FindSessions call. */
  UFUNCTION(BlueprintCallable, Category = "Evospace|Multiplayer")
  void JoinSessionByIndex(int32 index);

  /** Leave current session (if any). */
  UFUNCTION(BlueprintCallable, Category = "Evospace|Multiplayer")
  void LeaveSession();

  UPROPERTY(BlueprintAssignable)
  FOnSessionsFound OnSessionsFound;

  UPROPERTY(BlueprintAssignable)
  FOnSessionOpError OnSessionError;

  UPROPERTY(BlueprintAssignable)
  FOnSessionHosted OnSessionHosted;

  UPROPERTY(BlueprintAssignable)
  FOnSessionJoined OnSessionJoined;

  UPROPERTY(BlueprintAssignable)
  FOnSessionLeft OnSessionLeft;

  private:
  TSharedPtr<FOnlineSessionSearch> LastSessionSearch;
  FString PendingJoinSaveName;
};
