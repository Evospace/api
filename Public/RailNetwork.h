// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "NetworkBase.h"
#include "Qr/CoordinameMinimal.h"
#include "RailTypes.h"
#include "RailNetwork.generated.h"

class URailNodeBlockLogic;
class ADimension;

UCLASS()
class URailNetwork : public UNetworkBase {
  GENERATED_BODY()

  public:
  void Initialize(ADimension *InOwner);

  void RegisterNode(URailNodeBlockLogic *Node);
  void UnregisterNode(URailNodeBlockLogic *Node);

  bool TryAddSegment(URailNodeBlockLogic *A, URailNodeBlockLogic *B);

  /** Count of undirected links (for diagnostics). */
  int32 GetUndirectedLinkCount() const;

  bool FindPathBlockToBlock(const FQrVector3i &Start, const FQrVector3i &End, TArray<FQrVector3i> &OutKeyPath) const;
  void BuildPathStepsFromKeyPath(const TArray<FQrVector3i> &KeyPath, TArray<FRailPathStep> &Out) const;

  void SampleEdgeWorld(const FQrVector3i &From, const FQrVector3i &To, float DistanceAlong, FVector &OutPos, FVector &OutTangent) const;
  float GetEdgeLength(const FQrVector3i &From, const FQrVector3i &To) const;

  void DebugDrawGraphEdges(class UWorld *World, const FColor &Color, float LineLife) const;

  private:
  FQrVector3i RootKey(URailNodeBlockLogic *Node) const;

  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail", meta = (AllowPrivateAccess = "true"))
  TWeakObjectPtr<ADimension> OwnerWorld;

  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail", meta = (AllowPrivateAccess = "true"))
  TMap<FQrVector3i, URailNodeBlockLogic *> Nodes;
};
