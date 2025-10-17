// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include <Containers/Ticker.h>
#include "BlockSimSubsystem.generated.h"

class USurfaceDefinition;
class USurfaceSimContext;

UCLASS()
class UBlockSimSubsystem : public UGameInstanceSubsystem {
  GENERATED_BODY()
  public:
  // Get or create simulation context for a surface
  USurfaceSimContext *GetOrCreate(const FGuid &SurfaceId, USurfaceDefinition *SurfaceDef);

  // Subsystem lifecycle
  virtual void Initialize(FSubsystemCollectionBase &Collection) override;
  virtual void Deinitialize() override;

  private:
  TMap<FGuid, USurfaceSimContext *> Contexts;
  FTSTicker::FDelegateHandle TickHandle;
  bool TickAll(float DeltaSeconds);
};
