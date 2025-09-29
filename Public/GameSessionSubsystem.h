// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Public/GameSessionData.h"
#include "GameSessionSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDataUpdated, UGameSessionData *, gameSessionData);

UCLASS()
class UGameSessionSubsystem : public UGameInstanceSubsystem {
  GENERATED_BODY()

  public: 

  UFUNCTION(BlueprintCallable)
  const UGameSessionData *GetData() const { return Data; }

  UFUNCTION(BlueprintCallable)
  void SetData(UGameSessionData *dat);

  UFUNCTION(BlueprintCallable)
  void SetDataExt(UGameSessionData *dat, const FString &saveName, const FVersionStruct &version, const TArray<FString> & mods);

  void SetSeed(const FString &seed);

  UFUNCTION(BlueprintCallable)
  int64 GetSeed() const;

  UFUNCTION(BlueprintCallable)
  bool SetCreativeMode(bool val);

  void SetGenerator(const FName &generator);

  UFUNCTION(BlueprintCallable)
  int64 IncrementTicks();

  UFUNCTION(BlueprintCallable)
  void IncrementTime(double delta);

  UFUNCTION(BlueprintCallable)
  bool IsCreative() const;

  UPROPERTY(BlueprintAssignable)
  FDataUpdated OnDataUpdated;

  virtual void Initialize(FSubsystemCollectionBase &Collection) override;
  virtual void Deinitialize() override {}

  private:
  UPROPERTY(EditAnywhere)
  UGameSessionData *Data = nullptr;
};


