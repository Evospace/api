// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#include "MusicPlaylist.h"
#include <Sound/SoundBase.h>

USoundBase *UMusicPlaylist::GetRandomSound() {
  // Refill shuffle bag when empty
  if (ShuffleBag.Num() == 0) {
    ShuffleBag = Tracks;
  }
  if (ShuffleBag.Num() == 0)
    return nullptr;
  const int32 Index = FMath::RandRange(0, ShuffleBag.Num() - 1);
  USoundBase* Chosen = ShuffleBag[Index];
  ShuffleBag.RemoveAtSwap(Index, 1, /*bAllowShrinking*/ false);
  return Chosen;
}


