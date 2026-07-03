#pragma once

#include "CoreMinimal.h"
#include "BlockLogicStore.h"
#include "Qr/CoordinameMinimal.h"
#include "Qr/Prototype.h"
#include "Qr/Vector.h"

#include "Public/SurfaceDefinition.h"

#include "DimensionRuntime.generated.h"

class ADimension;
class FJsonObject;
class UBlockLogic;
class UBlockNetwork;
class UConveyorNetwork;
class UDroneManager;
class UInstancedStaticMeshComponent;
class ULogicContext;
class URailNetwork;
class URailwayManager;
class UResourceNetworkManager;
class UStaticBlock;
class UGameInstance;
class UWorld;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnRuntimeLogicAdded, const FQrVector3i &, UBlockLogic *);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnRuntimeLogicRemoved, const FQrVector3i &, UBlockLogic *);

UCLASS()
class EVOSPACE_API UDimensionRuntime : public UInstance {
  GENERATED_BODY()

  public:
  void InitializeForSurface(const FString &InSurfaceFolderName);
  void BindDimension(class ADimension *InDimension, UInstancedStaticMeshComponent *InDroneMeshComponent);
  void UnbindDimension(const ADimension *InDimension);
  void Shutdown();

  /** Clears all block logic and manager state. Recreates managers when InPresentation is set (after bind). */
  void ResetSimulationState(class ADimension *InPresentation, UInstancedStaticMeshComponent *InDroneMeshComponent);

  void ApplyLogicInput(UBlockLogic *Target, const ULogicContext *Context);

  UBlockLogic *SetBlockLogic(FQrVector3i Pos, UBlockLogic *Logic);
  UBlockLogic *GetBlockLogic(FQrVector3i BlockPos) const;

  /** Spawn a block with all its part cells: logic, sector cells and presentation renderables.
   * Requires a bound presentation while sector cells live in streamed columns. */
  UBlockLogic *SpawnBlockFull(const FQrVector3i &BlockPos, const UStaticBlock *StaticBlock, const FQuat &Rotation);
  UBlockLogic *SpawnBlockFullIdentity(const FQrVector3i &BlockPos, const UStaticBlock *StaticBlock);

  /** Remove a block with all its part cells (or clear a static-only cell). Returns the removed root logic. */
  UBlockLogic *DestroyBlockFull(const FQrVector3i &BlockPos);

  /** Block cells involved in a break at BlockPos (multi-cell-aware). */
  TArray<FQrVector3i> GatherBreakFootprint(const FQrVector3i &BlockPos) const;

  const FString &GetSurfaceFolderName() const { return SurfaceFolderName; }
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

  int32 GetManagedResourceNetworkCount() const;
  int32 GetActiveConveyorNetworkCount() const;

  FOnRuntimeLogicAdded OnRuntimeLogicAdded;
  FOnRuntimeLogicRemoved OnRuntimeLogicRemoved;

  private:
  void TickConveyorNetworks();

  UBlockLogic *SpawnBlockCell(const FQrVector3i &BlockPos, UBlockLogic *Parent, const UStaticBlock *StaticBlock,
                              const FQuat &Rotation);

  UPROPERTY(VisibleAnywhere)
  FString SurfaceFolderName;

  /** Bound presentation actor; sector cell writes and renderable/actor spawn go through it. */
  UPROPERTY()
  ADimension *Presentation = nullptr;

  UPROPERTY()
  USurfaceDefinition *SurfaceDefinition = nullptr;

  UPROPERTY()
  UInstancedStaticMeshComponent *DroneMeshComponent = nullptr;

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
