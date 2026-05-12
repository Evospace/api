#pragma once

#include "CoreMinimal.h"
#include "BlockLogicStore.h"
#include "Qr/Prototype.h"
#include "Qr/Vector.h"
#include "DimensionRuntime.generated.h"

class ADimension;
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
class FJsonObject;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnRuntimeLogicAdded, const FQrVector3i &, UBlockLogic *);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnRuntimeLogicRemoved, const FQrVector3i &, UBlockLogic *);

UCLASS()
class EVOSPACE_API UDimensionRuntime : public UInstance {
  GENERATED_BODY()

  public:
  void InitializeForSurface(const FString &InSurfaceFolderName);
  void BindDimension(ADimension *InDimension, UInstancedStaticMeshComponent *InDroneMeshComponent);
  void UnbindDimension(const ADimension *InDimension);
  void Shutdown();

  UBlockLogic *PlaceBlock(const FQrVector3i &BlockPos, const UStaticBlock *StaticBlock, const FQuat &Rotation);
  UBlockLogic *RemoveBlock(const FQrVector3i &BlockPos);
  void ApplyLogicInput(UBlockLogic *Target, const ULogicContext *Context);

  UBlockLogic *SetBlockLogic(FQrVector3i Pos, UBlockLogic *Logic);
  UBlockLogic *GetBlockLogic(FQrVector3i BlockPos) const;
  const FString &GetSurfaceFolderName() const { return SurfaceFolderName; }

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

  int32 TickRuntime(float DeltaTime, int32 TickRate, bool TickBlocksEnabled, bool TickNetworksEnabled);
  void TickVisual(float DeltaTime);

  FOnRuntimeLogicAdded OnRuntimeLogicAdded;
  FOnRuntimeLogicRemoved OnRuntimeLogicRemoved;

  private:
  void TickConveyorNetworks();

  UPROPERTY(VisibleAnywhere)
  FString SurfaceFolderName;

  TWeakObjectPtr<ADimension> Dimension;

  UPROPERTY()
  UInstancedStaticMeshComponent *DroneMeshComponent = nullptr;

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
};
