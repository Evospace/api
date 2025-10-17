// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MusicPlaylist.generated.h"

class USoundBase;

UCLASS(BlueprintType)
class UMusicPlaylist : public UObject {
  GENERATED_BODY()

  public:
  UFUNCTION(BlueprintCallable)
  USoundBase *GetRandomSound();

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<USoundBase *> Tracks;

  private:
  // Transient shuffle bag to avoid immediate repeats; refilled from Tracks when empty
  UPROPERTY(Transient)
  TArray<USoundBase *> ShuffleBag;
};
