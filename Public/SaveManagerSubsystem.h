// Copyright (c) 2017 - 2026, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Public/Net/ObserverTransport.h"
#include "Public/SaveSourceTypes.h"

#include "SaveManagerSubsystem.generated.h"

UCLASS()
class USaveManagerSubsystem : public UGameInstanceSubsystem {
  GENERATED_BODY()

  public:
  virtual void Initialize(FSubsystemCollectionBase &Collection) override;
  virtual void Deinitialize() override;

  UFUNCTION(BlueprintCallable, Category = "Evospace|Save")
  bool StartGameFromSave(const FString &SaveName, bool bListenServer);

  /** Loads save data into staging without opening the gameplay map. */
  UFUNCTION(BlueprintCallable, Category = "Evospace|Save")
  bool LoadGameFromSave(const FString &SaveName);

  UFUNCTION(BlueprintCallable, Category = "Evospace|Save")
  bool ResolveAndStage(const FSaveSourceDescriptor &Source, FPreparedSaveContext &OutContext);

  /** Loads save data from the source into staging without opening the gameplay map. */
  UFUNCTION(BlueprintCallable, Category = "Evospace|Save")
  bool LoadGameFromSource(const FSaveSourceDescriptor &Source);

  UFUNCTION(BlueprintCallable, Category = "Evospace|Save")
  bool StartGameFromSource(const FSaveSourceDescriptor &Source, bool bListenServer);

  UFUNCTION(BlueprintCallable, Category = "Evospace|Save")
  bool StartGameFromShippingBuiltIn(const FString &BuiltInPackageId, bool bListenServer);

  UFUNCTION(BlueprintCallable, Category = "Evospace|Save")
  bool StartGameFromWorkshopPath(const FString &PackagePath, bool bListenServer);

  UFUNCTION(BlueprintCallable, Category = "Evospace|Save")
  bool StartGameFromWorkshopId(int64 PublishedFileId, bool bListenServer);

  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Evospace|Save")
  const FPreparedSaveContext &GetLastPreparedSaveContext() const { return LastPreparedSaveContext; }

  UFUNCTION(BlueprintCallable, Category = "Evospace|Observer")
  bool ObserverHost(EObserverTransportType Transport, const FString &SaveName, int32 Port = 27050);

  UFUNCTION(BlueprintCallable, Category = "Evospace|Observer")
  bool ObserverConnect(EObserverTransportType Transport, const FString &Target, int32 Port = 27050);

  private:
  void HandlePostLoadMapForObserverHost(UWorld *LoadedWorld);

  UPROPERTY()
  FPreparedSaveContext LastPreparedSaveContext;

  FDelegateHandle PostLoadMapHandle;
  int32 PendingObserverHostPort = -1;
  EObserverTransportType PendingObserverHostTransport = EObserverTransportType::Lan;
};
