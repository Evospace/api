// Copyright (c) 2017 - 2026, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Public/SaveSourceTypes.h"

class FSaveSourceResolver {
  public:
  static bool ResolveAndStage(const FSaveSourceDescriptor &Source, FPreparedSaveContext &OutContext);
};
