#pragma once

#include "CoreMinimal.h"
#include "BlockLogicStore.h"
#include "Qr/CoordinameMinimal.h"
#include "Qr/Prototype.h"
#include "Qr/Vector.h"

#include "Public/MapChangeSet.h"
#include "Public/SurfaceDefinition.h"

#include "DimensionRuntime.generated.h"

class ADimension;
class FJsonObject;
class UColumnStreamingManager;
class UBlockLogic;
class UBlockNetwork;
class UConveyorNetwork;
class UDroneManager;
class ULogicContext;
class URailNetwork;
class URailwayManager;
class UResourceNetworkManager;
class UStaticBlock;
class UGameInstance;
class UGameSessionSubsystem;
class UWorld;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnRuntimeLogicAdded, const FQrVector3i &, UBlockLogic *);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnRuntimeLogicRemoved, const FQrVector3i &, UBlockLogic *);

enum class ECellWritePolicy : uint8 {
  RequireLoaded,
  QueueIfUnloaded,
};

UCLASS()
class EVOSPACE_API UDimensionRuntime : public UInstance {
  GENERATED_BODY()

  public:
  void InitializeForSurface(const FString &InSurfaceFolderName);
  void BindDimension(class ADimension *InDimension);
  void UnbindDimension(const ADimension *InDimension);
  void Shutdown();

  void ResetSimulationState(class ADimension *InPresentation);

  void ApplyLogicInput(UBlockLogic *Target, const ULogicContext *Context);

  UBlockLogic *SetBlockLogic(FQrVector3i Pos, UBlockLogic *Logic);
  UBlockLogic *GetBlockLogic(FQrVector3i BlockPos) const;

  UBlockLogic *SpawnBlockFull(const FQrVector3i &BlockPos, const UStaticBlock *StaticBlock, const FQuat &Rotation,
                              ECellWritePolicy Policy = ECellWritePolicy::RequireLoaded,
                              const TFunction<void(UBlockLogic *)> &ConfigureRoot = nullptr);
  UBlockLogic *SpawnBlockFullIdentity(const FQrVector3i &BlockPos, const UStaticBlock *StaticBlock);

  UBlockLogic *DestroyBlockFull(const FQrVector3i &BlockPos, ECellWritePolicy Policy = ECellWritePolicy::RequireLoaded);

  TArray<FQrVector3i> GatherBreakFootprint(const FQrVector3i &BlockPos) const;

  FMapCellState CaptureCellState(const FQrVector3i &Pos) const;

  void ApplyCellSnapshots(const TArray<FMapCellState> &Targets);

  void ApplyChangeSet(const FMapChangeSet &Change, EMapChangeDirection Direction);

  FMapChangeSet MakeEditChangeSet(const TArray<FQrVector3i> &Positions, TFunctionRef<void()> ApplyBody);

  const FString &GetSurfaceFolderName() const { return SurfaceFolderName; }

  uint64 GetBlockRngSeedBase() const;
  /** World/surface authoring data synced from ADimension during BindDimension; gameplay must not depend on presentation. */
  USurfaceDefinition *GetSurfaceDefinition() const { return SurfaceDefinition; }

  const TMap<FQrVector3i, UBlockLogic *> &GetLogics() const { return DimensionLogics; }

  void RemoveActorOrRenderableForAllTicked();
  void ResetTickStore();

  UBlockNetwork *GetNewNetwork();
  UDroneManager *GetDroneManager();
  URailNetwork *GetRailNetwork();
  URailwayManager *GetRailwayManager();
  UConveyorNetwork *GetNewConveyorNetwork();
  void KillConveyorNetworkDeferred(UConveyorNetwork *Network);
  void KillNetworkDeferred(UBlockNetwork *Network);

  void CaptureNetworkStorageSnapshots();
  void ClearNetworkStorageSnapshots();
  void SerializeRuntimeManagers(TSharedPtr<FJsonObject> JsonLogic) const;
  void DeserializeRuntimeManagers(TSharedPtr<FJsonObject> JsonObject);

  void BeginDeferredResourceNetworkLoad();
  void FinishDeferredResourceNetworkLoad(TSharedPtr<FJsonObject> JsonLogic);
  void CancelDeferredResourceNetworkLoad();
  bool IsResourceNetworkRebuildDeferred() const { return bDeferResourceNetworkRebuild; }

  int32 TickRuntime(UWorld *World, UGameInstance *GameInstance, float DeltaTime, int32 TickRate, bool TickBlocksEnabled);
  void TickVisual(float DeltaTime);

  int64 RunBenchTicks(int32 Count, int32 TickRate, UGameInstance *GameInstance);

  static void SetBenchOwnsSim(bool bOwns);
  static bool IsBenchOwningSim();

  void ComputeSimStateHash(struct FSimStateHashRecord &Out) const;

  int32 GetManagedResourceNetworkCount() const;
  int32 GetActiveConveyorNetworkCount() const;

  FOnRuntimeLogicAdded OnRuntimeLogicAdded;
  FOnRuntimeLogicRemoved OnRuntimeLogicRemoved;

  private:
  void TickConveyorNetworks();

  void RunSimSubstep(int32 TickRate, UGameSessionSubsystem *GameSession, bool bTickBlocksEnabled);

  UBlockLogic *SpawnBlockCell(const FQrVector3i &BlockPos, UBlockLogic *Parent, const UStaticBlock *StaticBlock,
                              const FQuat &Rotation, ECellWritePolicy Policy,
                              const TFunction<void(UBlockLogic *)> &ConfigureRoot);

  void RestoreBlockFromSnapshot(const FMapCellState &S);

  bool WriteCell(const FQrVector3i &Pos, const UStaticBlock *Block, const BlockDensity &Density, ECellWritePolicy Policy);

  UPROPERTY(VisibleAnywhere)
  FString SurfaceFolderName;

  UPROPERTY()
  ADimension *Presentation = nullptr;

  UPROPERTY()
  UColumnStreamingManager *ColumnManager = nullptr;

  UPROPERTY()
  USurfaceDefinition *SurfaceDefinition = nullptr;

  UPROPERTY()
  TMap<FQrVector3i, UBlockLogic *> DimensionLogics;

  FBlockLogicStore DimensionLogicsTick;

  UPROPERTY()
  UResourceNetworkManager *ResourceNetworkManager = nullptr;

  UPROPERTY()
  UDroneManager *DroneManager = nullptr;

  UPROPERTY()
  URailNetwork *RailNetwork = nullptr;

  UPROPERTY()
  URailwayManager *RailwayManager = nullptr;

  UPROPERTY()
  TArray<UConveyorNetwork *> ConveyorNetworks;

  UPROPERTY()
  TArray<UConveyorNetwork *> ConveyorNetworksToKill;

  int64 TickAccumulatorMicros = 0;

  bool bDeferResourceNetworkRebuild = false;
};
