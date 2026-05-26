#pragma once

#include "CoreMinimal.h"
#include "BlockLogicStore.h"
#include "Qr/CoordinameMinimal.h"
#include "Qr/Prototype.h"
#include "Qr/Vector.h"

#include "Public/SurfaceDefinition.h"

#include "DimensionRuntime.generated.h"

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
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSimulationLogicInputDelivered, UBlockLogic *, ULogicContext *);

UCLASS()
class EVOSPACE_API UDimensionRuntime : public UInstance {
  GENERATED_BODY()

  public:
  void InitializeForSurface(const FString &InSurfaceFolderName);
  void BindDimension(class ADimension *InDimension, UInstancedStaticMeshComponent *InDroneMeshComponent);
  void UnbindDimension(const ADimension *InDimension);
  void Shutdown();

  /** Clears all block logic and manager state. Recreates managers when Presentation is set (after bind). */
  void ResetSimulationState(class ADimension *Presentation, UInstancedStaticMeshComponent *InDroneMeshComponent);

  void ApplyLogicInput(UBlockLogic *Target, const ULogicContext *Context);
  /** After simulation consumes logic input: broadcast OnSimulationLogicInputDelivered (ADimension routes to blueprint while bound). */
  void NotifySimulationLogicInputDelivered(UBlockLogic *Target, ULogicContext *Context);

  UBlockLogic *SetBlockLogic(FQrVector3i Pos, UBlockLogic *Logic);
  UBlockLogic *GetBlockLogic(FQrVector3i BlockPos) const;

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

  /** Subscribed by ADimension::NotifyLogicInputDelivered while this surface is bound. */
  FOnSimulationLogicInputDelivered OnSimulationLogicInputDelivered;

  private:
  void TickConveyorNetworks();

  UPROPERTY(VisibleAnywhere)
  FString SurfaceFolderName;

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
