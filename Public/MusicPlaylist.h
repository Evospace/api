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
  USoundBase *GetRandomSound() const;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<USoundBase *> Tracks;
};


