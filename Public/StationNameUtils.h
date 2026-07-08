// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

namespace EvoStationName {

// Default station naming shared by drone and rail networks: "Station N" with the
// smallest positive N not already taken. Pure function of the existing name set,
// so lockstep peers generate identical defaults without any RNG.
inline FString SmallestFreeDefaultName(const TArray<FString> &ExistingNames) {
  static const TCHAR *Prefix = TEXT("Station ");
  TSet<int32> Used;
  for (const FString &Name : ExistingNames) {
    if (!Name.StartsWith(Prefix)) {
      continue;
    }
    const FString Tail = Name.Mid(FCString::Strlen(Prefix));
    if (!Tail.IsEmpty() && Tail.IsNumeric()) {
      Used.Add(FCString::Atoi(*Tail));
    }
  }
  int32 N = 1;
  while (Used.Contains(N)) {
    ++N;
  }
  return FString::Printf(TEXT("%s%d"), Prefix, N);
}

} // namespace EvoStationName
