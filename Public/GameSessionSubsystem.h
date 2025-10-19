// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Public/GameSessionData.h"
#include "GameSessionSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDataUpdated, UGameSessionData *, gameSessionData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveRequested, const FString &, SaveName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveLoading, const FString &, SaveName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMenuMuffling, bool, bMuffled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSurfaceChange, FString, SurfaceName);

class UStaticItem;

UCLASS()
class UGameSessionSubsystem : public UGameInstanceSubsystem {
  GENERATED_BODY()

  public:
  UFUNCTION(BlueprintCallable)
  const UGameSessionData *GetData() const { return Data; }

  UFUNCTION(BlueprintCallable)
  void SetData(UGameSessionData *dat);

  UFUNCTION(BlueprintCallable)
  void SetDataExt(UGameSessionData *dat, const FString &saveName, const FVersionStruct &version, const TArray<FString> &mods);

  UFUNCTION(BlueprintCallable)
  void SetSeed(const FString &seed);

  UFUNCTION(BlueprintCallable)
  int64 GetSeed() const;

  UFUNCTION(BlueprintCallable)
  bool SetCreativeMode(bool val);

  UFUNCTION(BlueprintCallable)
  void SetSaveName(const FString &saveName);

  UFUNCTION(BlueprintCallable)
  void SetInfiniteOre(bool val);

  UFUNCTION(BlueprintCallable)
  bool GetInfiniteOre() const;

  UFUNCTION(BlueprintCallable)
  int32 GetTickRate() const;

  UFUNCTION(BlueprintCallable, BlueprintPure)
  float GetTickDelta() const;

  UFUNCTION(BlueprintCallable)
  void SetCreativeAllowed(bool val);

  UFUNCTION(BlueprintCallable)
  void SetGenerator(const FName &generator);

  UFUNCTION(BlueprintCallable)
  int64 IncrementTicks();

  UFUNCTION(BlueprintCallable)
  void IncrementTime(double delta);

  // World time controls stored in GameSessionData
  UFUNCTION(BlueprintCallable)
  double GetWorldTimeOfDayHours() const;

  UFUNCTION(BlueprintCallable)
  void SetWorldTimeOfDayHours(double Hours);

  UFUNCTION(BlueprintCallable)
  bool GetWorldTimeAutoAdvance() const;

  UFUNCTION(BlueprintCallable)
  void SetWorldTimeAutoAdvance(bool bEnable);

  UFUNCTION(BlueprintCallable)
  bool IsCreative() const;

  UFUNCTION(BlueprintCallable)
  void SetAllResearchCompleted(bool val);

  UFUNCTION(BlueprintCallable)
  void RequestSave(const FString &saveName);

  UFUNCTION(BlueprintCallable)
  void NotifySaveLoaded(const FString &saveName);

  virtual void Initialize(FSubsystemCollectionBase &Collection) override;
  virtual void Deinitialize() override {}

  UFUNCTION(BlueprintCallable)
  void SetMenuMuffling(bool bMuffled);

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  UStaticItem *ItemTypeHighlight = nullptr;

  UPROPERTY(BlueprintAssignable)
  FDataUpdated OnDataUpdated;

  UPROPERTY(BlueprintAssignable)
  FOnSaveRequested OnSaveRequested;

  UPROPERTY(BlueprintAssignable)
  FOnSaveLoading OnSaveLoading;

  UPROPERTY(BlueprintAssignable)
  FOnMenuMuffling OnMenuMuffling;

  UPROPERTY(BlueprintAssignable)
  FOnSurfaceChange OnSurfaceChange;

  private:
  UPROPERTY(EditAnywhere)
  UGameSessionData *Data = nullptr;
};