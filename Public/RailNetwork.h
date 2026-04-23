// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "NetworkBase.h"
#include "Qr/CoordinameMinimal.h"
#include "RailTypes.h"
#include "RailNetwork.generated.h"

class URailNodeBlockLogic;
class ADimension;

struct FRailDirectedEdgeKey {
  FQrVector3i From = FQrVector3i::Zero();
  FQrVector3i To = FQrVector3i::Zero();

  bool operator==(const FRailDirectedEdgeKey &Other) const {
    return From == Other.From && To == Other.To;
  }
};

FORCEINLINE uint32 GetTypeHash(const FRailDirectedEdgeKey &Key) {
  return HashCombine(GetTypeHash(Key.From), GetTypeHash(Key.To));
}

struct FRailEdgeArcSampleCache {
  int64 LengthFixed = 0;
  FVector StartPoint = FVector::ZeroVector;
  FVector EndPoint = FVector::ZeroVector;
  FVector StartHermiteTangent = FVector::ForwardVector;
  FVector EndHermiteTangent = FVector::ForwardVector;
  float TotalArcLength = 0.0f;
  TArray<float> ArcPrefixLengths;
};

struct FRailRenderSegmentData {
  FQrVector3i From = FQrVector3i::Zero();
  FQrVector3i To = FQrVector3i::Zero();
  FVector StartWorld = FVector::ZeroVector;
  FVector EndWorld = FVector::ZeroVector;
  FVector StartTangentWorld = FVector::ForwardVector;
  FVector EndTangentWorld = FVector::ForwardVector;
};

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

  void SampleEdgeWorld(const FQrVector3i &From, const FQrVector3i &To, int64 DistanceAlongFixed, FVector &OutPos, FVector &OutTangent) const;
  int64 GetEdgeLength(const FQrVector3i &From, const FQrVector3i &To) const;
  void CollectRenderSegments(TArray<FRailRenderSegmentData> &OutSegments) const;

  void DebugDrawGraphEdges(class UWorld *World, const FColor &Color, float LineLife) const;

  bool SerializeJson(TSharedPtr<FJsonObject> Json) const;
  bool DeserializeJson(TSharedPtr<FJsonObject> Json);

  private:
  FQrVector3i RootKey(URailNodeBlockLogic *Node) const;
  FVector ComputeNodeRenderTangent(URailNodeBlockLogic *Node, const FVector &ReferenceDirection) const;
  bool BuildEdgePolyline(
    const FQrVector3i &From,
    const FQrVector3i &To,
    TArray<FVector> &OutPoints,
    TArray<float> *OutPrefixDistances,
    float *OutTotalLength) const;

  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail", meta = (AllowPrivateAccess = "true"))
  TWeakObjectPtr<ADimension> OwnerWorld;

  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail", meta = (AllowPrivateAccess = "true"))
  TMap<FQrVector3i, URailNodeBlockLogic *> Nodes;
};
