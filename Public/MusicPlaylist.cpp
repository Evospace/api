// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#include "MusicPlaylist.h"
#include <Sound/SoundBase.h>

USoundBase *UMusicPlaylist::GetRandomSound() const {
  if (Tracks.Num() == 0)
    return nullptr;
  const int32 Index = FMath::RandRange(0, Tracks.Num() - 1);
  return Tracks[Index];
}


