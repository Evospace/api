// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#include "Public/RailNetwork.h"
#include "Public/BlockLogic.h"
#include "Public/Dimension.h"
#include "Public/RailNodeBlockLogic.h"
#include "Public/RailwayManager.h"
#include "Evospace/JsonHelper.h"
#include "Qr/StaticLogger.h"
#include "Math/NumericLimits.h"
#include "DrawDebugHelpers.h"
#include <algorithm>

namespace {
constexpr int32 RailArcSampleCount = 16;

FVector EvaluateRailEdgeHermitePosition(const float T, const FVector &P0, const FVector &T0, const FVector &P1, const FVector &T1) {
  const float T2 = T * T;
  const float T3 = T2 * T;
  const float H00 = (2.0f * T3) - (3.0f * T2) + 1.0f;
  const float H10 = T3 - (2.0f * T2) + T;
  const float H01 = (-2.0f * T3) + (3.0f * T2);
  const float H11 = T3 - T2;
  return (H00 * P0) + (H10 * T0) + (H01 * P1) + (H11 * T1);
}

FVector EvaluateRailEdgeHermiteDerivative(const float T, const FVector &P0, const FVector &T0, const FVector &P1, const FVector &T1) {
  const float T2 = T * T;
  const float DH00 = (6.0f * T2) - (6.0f * T);
  const float DH10 = (3.0f * T2) - (4.0f * T) + 1.0f;
  const float DH01 = (-6.0f * T2) + (6.0f * T);
  const float DH11 = (3.0f * T2) - (2.0f * T);
  return (DH00 * P0) + (DH10 * T0) + (DH01 * P1) + (DH11 * T1);
}

} // namespace

void URailNetwork::Initialize(ADimension *InOwner) {
  OwnerWorld = InOwner;
}

FQrVector3i URailNetwork::RootKey(URailNodeBlockLogic *Node) const {
  if (!Node) {
    return FQrVector3i::Zero();
  }
  UBlockLogic *Base = static_cast<UBlockLogic *>(Node);
  if (UBlockLogic *P = Base->GetPartRootBlock()) {
    return P->GetBlockPos();
  }
  return Base->GetBlockPos();
}

void URailNetwork::RegisterNode(URailNodeBlockLogic *Node) {
  if (!Node) {
    return;
  }
  const FQrVector3i k = RootKey(Node);
  Nodes.Add(k, Node);
  InvalidateRenderCache();
  MarkDirty();
}

void URailNetwork::UnregisterNode(URailNodeBlockLogic *Node) {
  if (!Node) {
    return;
  }
  Node->UnlinkAllRail();
  const FQrVector3i k = RootKey(Node);
  Nodes.Remove(k);
  InvalidateRenderCache();
  MarkDirty();
  if (ADimension *D = OwnerWorld.Get()) {
    if (URailwayManager *Rm = D->GetRailwayManager()) {
      Rm->OnRailGraphChanged();
    }
  }
}

int32 URailNetwork::GetUndirectedLinkCount() const {
  int32 sum = 0;
  for (const auto &P : Nodes) {
    if (P.Value) {
      sum += P.Value->GetLinkedRailNodes().Num();
    }
  }
  return sum / 2;
}

bool URailNetwork::TryAddSegment(URailNodeBlockLogic *A, URailNodeBlockLogic *B) {
  if (!A || !B) {
    return false;
  }
  const FQrVector3i ka = RootKey(A);
  const FQrVector3i kb = RootKey(B);
  if (ka == kb) {
    return false;
  }
  URailNodeBlockLogic *Ra = A;
  URailNodeBlockLogic *Rb = B;
  if (A->GetPartRootBlock()) {
    Ra = Cast<URailNodeBlockLogic>(A->GetPartRootBlock());
  }
  if (B->GetPartRootBlock()) {
    Rb = Cast<URailNodeBlockLogic>(B->GetPartRootBlock());
  }
  if (!Ra || !Rb) {
    return false;
  }
  if (Ra->GetLinkedRailNodes().Contains(Rb)) {
    return false;
  }
  Ra->RailLink(Rb);
  Rb->RailLink(Ra);
  InvalidateRenderCache();
  MarkDirty();
  if (ADimension *D = OwnerWorld.Get()) {
    if (URailwayManager *Rm = D->GetRailwayManager()) {
      Rm->OnRailGraphChanged();
    }
  }
  return true;
}

bool URailNetwork::FindPathBlockToBlock(const FQrVector3i &Start, const FQrVector3i &End, TArray<FQrVector3i> &OutKeyPath) const {
  OutKeyPath.Empty();
  if (Start == End) {
    return true;
  }
  URailNodeBlockLogic *const *pStart = Nodes.Find(Start);
  URailNodeBlockLogic *const *pEnd = Nodes.Find(End);
  if (!pStart || !*pStart || !pEnd || !*pEnd) {
    const int32 linkCount = GetUndirectedLinkCount();
    LOG(ERROR_LL) << "RailPath not_on_graph hasStart=" << (pStart && *pStart ? 1 : 0) << " hasEnd=" << (pEnd && *pEnd ? 1 : 0) << " start=" << Start.ToString() << " end=" << End.ToString() << " registeredNodes=" << Nodes.Num() << " links=" << linkCount;
    return false;
  }
  TSet<URailNodeBlockLogic *> all;
  for (const auto &P : Nodes) {
    if (P.Value) {
      all.Add(P.Value);
    }
  }
  TMap<URailNodeBlockLogic *, int64> dist;
  TMap<URailNodeBlockLogic *, URailNodeBlockLogic *> parent;
  TSet<URailNodeBlockLogic *> done;
  constexpr int64 kInf = TNumericLimits<int64>::Max() / 4;
  for (URailNodeBlockLogic *K : all) {
    dist.Add(K, TNumericLimits<int64>::Max());
  }
  dist[*pStart] = 0;
  while (done.Num() < all.Num()) {
    URailNodeBlockLogic *u = nullptr;
    int64 bestD = TNumericLimits<int64>::Max();
    bool foundU = false;
    for (URailNodeBlockLogic *K : all) {
      if (done.Contains(K)) {
        continue;
      }
      if (const int64 *D = dist.Find(K)) {
        if (*D < bestD) {
          bestD = *D;
          u = K;
          foundU = true;
        }
      }
    }
    if (!foundU || bestD >= kInf) {
      LOG(ERROR_LL) << "RailPath dijkstra_stuck foundU=" << (foundU ? 1 : 0) << " bestD=" << bestD << " start=" << Start.ToString() << " end=" << End.ToString() << " done=" << done.Num() << " all=" << all.Num();
      return false;
    }
    if (u == *pEnd) {
      break;
    }
    done.Add(u);
    for (URailNodeBlockLogic *v : u->GetLinkedRailNodes()) {
      if (done.Contains(v) || !v) {
        continue;
      }
      if (!dist.Contains(v)) {
        dist.Add(v, TNumericLimits<int64>::Max());
      }
      const int64 w = GetEdgeLength(RootKey(u), RootKey(v));
      const int64 alt = bestD + w;
      if (const int64 *OldD = dist.Find(v)) {
        if (alt < *OldD) {
          dist[v] = alt;
          parent.FindOrAdd(v) = u;
        }
      }
    }
  }
  if (const int64 *EndD = dist.Find(*pEnd)) {
    if (*EndD >= kInf) {
      LOG(ERROR_LL) << "RailPath end_inf end=" << End.ToString() << " d=" << *EndD;
      return false;
    }
  } else {
    LOG(ERROR_LL) << "RailPath end_not_in_dist end=" << End.ToString() << " distKeys=" << dist.Num();
    return false;
  }
  TArray<URailNodeBlockLogic *> back;
  for (URailNodeBlockLogic *c = *pEnd; c;) {
    back.Add(c);
    if (c == *pStart) {
      break;
    }
    if (URailNodeBlockLogic **pPar = parent.Find(c)) {
      c = *pPar;
    } else {
      LOG(ERROR_LL) << "RailPath recon (ptr) missing parent start=" << Start.ToString() << " end=" << End.ToString();
      return false;
    }
  }
  if (back.Num() == 0 || back.Last() != *pStart) {
    LOG(ERROR_LL) << "RailPath recon broken start=" << Start.ToString() << " end=" << End.ToString();
    return false;
  }
  if (back.Num() > 1) {
    std::reverse(back.GetData(), back.GetData() + back.Num());
  }
  OutKeyPath.Empty();
  for (URailNodeBlockLogic *n : back) {
    if (n) {
      OutKeyPath.Add(RootKey(n));
    }
  }
  return true;
}

void URailNetwork::BuildPathStepsFromKeyPath(const TArray<FQrVector3i> &KeyPath, TArray<FRailPathStep> &Out) const {
  Out.Empty();
  for (int i = 0; i + 1 < KeyPath.Num(); ++i) {
    FRailPathStep step;
    step.From = KeyPath[i];
    step.To = KeyPath[i + 1];
    Out.Add(step);
  }
}

int64 URailNetwork::GetEdgeLength(const FQrVector3i &From, const FQrVector3i &To) const {
  URailNodeBlockLogic *const *Na = Nodes.Find(From);
  URailNodeBlockLogic *const *Nb = Nodes.Find(To);
  if (!Na || !*Na || !Nb || !*Nb) {
    return 0;
  }
  const float worldLen = FVector::Dist((*Na)->GetLocation(), (*Nb)->GetLocation());
  return FMath::RoundToInt64(static_cast<double>(worldLen) * static_cast<double>(RailDistanceFixedScale));
}

void URailNetwork::CollectRenderSegments(TArray<FRailRenderSegmentData> &OutSegments) const {
  OutSegments.Empty();
  for (const TPair<FQrVector3i, URailNodeBlockLogic *> &Kvp : Nodes) {
    URailNodeBlockLogic *Node = Kvp.Value;
    if (!Node) {
      continue;
    }
    for (URailNodeBlockLogic *LinkedNode : Node->GetLinkedRailNodes()) {
      if (!LinkedNode || Node >= LinkedNode) {
        continue;
      }
      FRailRenderSegmentData Segment;
      Segment.From = RootKey(Node);
      Segment.To = RootKey(LinkedNode);
      Segment.StartWorld = Node->GetLocation();
      Segment.EndWorld = LinkedNode->GetLocation();
      const FVector SegmentDirection = (Segment.EndWorld - Segment.StartWorld).GetSafeNormal();
      Segment.StartTangentWorld = ComputeNodeRenderTangent(Node, SegmentDirection);
      Segment.EndTangentWorld = ComputeNodeRenderTangent(LinkedNode, SegmentDirection);
      OutSegments.Add(Segment);
    }
  }
}

FVector URailNetwork::ComputeNodeRenderTangent(URailNodeBlockLogic *Node, const FVector &ReferenceDirection) const {
  if (!Node) {
    return FVector::ForwardVector;
  }
  FVector RefDir = ReferenceDirection.GetSafeNormal();
  if (RefDir.IsNearlyZero()) {
    return FVector::ForwardVector;
  }

  const FVector NodePosition = Node->GetLocation();
  FVector AccumulatedDirection = RefDir;
  float TotalWeight = 1.0f;

  for (URailNodeBlockLogic *Neighbor : Node->GetLinkedRailNodes()) {
    if (!Neighbor) {
      continue;
    }
    FVector NeighborDirection = (Neighbor->GetLocation() - NodePosition).GetSafeNormal();
    if (NeighborDirection.IsNearlyZero()) {
      continue;
    }

    const float Alignment = FVector::DotProduct(NeighborDirection, RefDir);
    if (Alignment < 0.0f) {
      NeighborDirection *= -1.0f;
    }
    const float Weight = FMath::Abs(Alignment);
    AccumulatedDirection += NeighborDirection * Weight;
    TotalWeight += Weight;
  }

  if (TotalWeight <= KINDA_SMALL_NUMBER) {
    return RefDir;
  }

  const FVector AveragedDirection = AccumulatedDirection / TotalWeight;
  if (AveragedDirection.IsNearlyZero()) {
    return RefDir;
  }
  return AveragedDirection.GetSafeNormal();
}

void URailNetwork::SampleEdgeWorld(const FQrVector3i &From, const FQrVector3i &To, int64 DistanceAlongFixed, FVector &OutPos, FVector &OutTangent) const {
  const FRailEdgeArcSampleCache *Cache = GetOrBuildEdgeArcSampleCache(From, To);
  if (!Cache) {
    OutPos = FVector::ZeroVector;
    OutTangent = FVector::ForwardVector;
    return;
  }
  const double alphaRaw = static_cast<double>(DistanceAlongFixed) / static_cast<double>(Cache->LengthFixed);
  const float alpha = FMath::Clamp(static_cast<float>(alphaRaw), 0.0f, 1.0f);
  float t = alpha;
  if (Cache->TotalArcLength > KINDA_SMALL_NUMBER && Cache->ArcPrefixLengths.Num() == RailArcSampleCount + 1) {
    const float TargetArcLength = alpha * Cache->TotalArcLength;
    for (int32 SampleIdx = 1; SampleIdx <= RailArcSampleCount; ++SampleIdx) {
      if (Cache->ArcPrefixLengths[SampleIdx] < TargetArcLength) {
        continue;
      }
      const float SegmentStartArc = Cache->ArcPrefixLengths[SampleIdx - 1];
      const float SegmentArcLength = Cache->ArcPrefixLengths[SampleIdx] - SegmentStartArc;
      const float LocalAlpha = (SegmentArcLength > KINDA_SMALL_NUMBER)
                                 ? (TargetArcLength - SegmentStartArc) / SegmentArcLength
                                 : 0.0f;
      const float SegmentStartT = static_cast<float>(SampleIdx - 1) / static_cast<float>(RailArcSampleCount);
      const float SegmentEndT = static_cast<float>(SampleIdx) / static_cast<float>(RailArcSampleCount);
      t = FMath::Lerp(SegmentStartT, SegmentEndT, FMath::Clamp(LocalAlpha, 0.0f, 1.0f));
      break;
    }
  }
  OutPos = EvaluateRailEdgeHermitePosition(t, Cache->StartPoint, Cache->StartHermiteTangent, Cache->EndPoint, Cache->EndHermiteTangent);
  OutTangent = EvaluateRailEdgeHermiteDerivative(t, Cache->StartPoint, Cache->StartHermiteTangent, Cache->EndPoint, Cache->EndHermiteTangent).GetSafeNormal();
  if (OutTangent.IsNearlyZero()) {
    OutTangent = (Cache->EndPoint - Cache->StartPoint).GetSafeNormal();
    if (OutTangent.IsNearlyZero()) {
      OutTangent = FVector::ForwardVector;
    }
  }
}

void URailNetwork::InvalidateRenderCache() {
  EdgeArcSampleCache.Reset();
}

const FRailEdgeArcSampleCache *URailNetwork::GetOrBuildEdgeArcSampleCache(const FQrVector3i &From, const FQrVector3i &To) const {
  const FRailDirectedEdgeKey CacheKey{ From, To };
  if (const FRailEdgeArcSampleCache *Found = EdgeArcSampleCache.Find(CacheKey)) {
    return Found;
  }

  URailNodeBlockLogic *const *Na = Nodes.Find(From);
  URailNodeBlockLogic *const *Nb = Nodes.Find(To);
  if (!Na || !*Na || !Nb || !*Nb) {
    return nullptr;
  }

  const FVector Pa = (*Na)->GetLocation();
  const FVector Pb = (*Nb)->GetLocation();
  const int64 LenFixed = GetEdgeLength(From, To);
  const FVector SegmentDirection = (Pb - Pa).GetSafeNormal();
  if (LenFixed <= 0 || SegmentDirection.IsNearlyZero()) {
    return nullptr;
  }

  FVector StartDirection = ComputeNodeRenderTangent(*Na, SegmentDirection).GetSafeNormal();
  if (StartDirection.IsNearlyZero()) {
    StartDirection = SegmentDirection;
  }
  if (FVector::DotProduct(StartDirection, SegmentDirection) < 0.0f) {
    StartDirection *= -1.0f;
  }

  FVector EndDirection = ComputeNodeRenderTangent(*Nb, SegmentDirection).GetSafeNormal();
  if (EndDirection.IsNearlyZero()) {
    EndDirection = SegmentDirection;
  }
  if (FVector::DotProduct(EndDirection, SegmentDirection) < 0.0f) {
    EndDirection *= -1.0f;
  }

  FRailEdgeArcSampleCache CacheEntry;
  CacheEntry.LengthFixed = LenFixed;
  CacheEntry.StartPoint = Pa;
  CacheEntry.EndPoint = Pb;
  const float SegmentLength = FVector::Dist(Pa, Pb);
  const float TangentScale = SegmentLength * 0.5f;
  CacheEntry.StartHermiteTangent = StartDirection * TangentScale;
  CacheEntry.EndHermiteTangent = EndDirection * TangentScale;
  CacheEntry.ArcPrefixLengths.SetNumUninitialized(RailArcSampleCount + 1);
  CacheEntry.ArcPrefixLengths[0] = 0.0f;
  FVector PrevPoint = EvaluateRailEdgeHermitePosition(0.0f, Pa, CacheEntry.StartHermiteTangent, Pb, CacheEntry.EndHermiteTangent);
  for (int32 SampleIdx = 1; SampleIdx <= RailArcSampleCount; ++SampleIdx) {
    const float SampleT = static_cast<float>(SampleIdx) / static_cast<float>(RailArcSampleCount);
    const FVector Point = EvaluateRailEdgeHermitePosition(SampleT, Pa, CacheEntry.StartHermiteTangent, Pb, CacheEntry.EndHermiteTangent);
    CacheEntry.TotalArcLength += FVector::Dist(PrevPoint, Point);
    CacheEntry.ArcPrefixLengths[SampleIdx] = CacheEntry.TotalArcLength;
    PrevPoint = Point;
  }

  return &EdgeArcSampleCache.Add(CacheKey, MoveTemp(CacheEntry));
}

void URailNetwork::DebugDrawGraphEdges(UWorld *World, const FColor &Color, float LineLife) const {
  if (!World) {
    return;
  }
  constexpr uint8 ForegroundDP = 1;
  for (const auto &Kvp : Nodes) {
    URailNodeBlockLogic *n = Kvp.Value;
    if (!n) {
      continue;
    }
    for (URailNodeBlockLogic *o : n->GetLinkedRailNodes()) {
      if (o && n < o) {
        DrawDebugLine(World, n->GetLocation(), o->GetLocation(), Color, false, LineLife, ForegroundDP, 4.f);
      }
    }
  }
}

bool URailNetwork::SerializeJson(TSharedPtr<FJsonObject> Json) const {
  if (!Json.IsValid()) {
    return false;
  }

  TArray<TSharedPtr<FJsonValue>> Links;
  for (const auto &Kvp : Nodes) {
    URailNodeBlockLogic *A = Kvp.Value;
    if (!A) {
      continue;
    }
    const FQrVector3i AKey = RootKey(A);
    for (URailNodeBlockLogic *B : A->GetLinkedRailNodes()) {
      if (!B) {
        continue;
      }
      const FQrVector3i BKey = RootKey(B);
      if (BKey == AKey) {
        continue;
      }
      // Save each undirected edge once (lexicographic key order).
      const bool bBIsBeforeA = (BKey.X < AKey.X) ||
                               (BKey.X == AKey.X && BKey.Y < AKey.Y) ||
                               (BKey.X == AKey.X && BKey.Y == AKey.Y && BKey.Z < AKey.Z);
      if (bBIsBeforeA) {
        continue;
      }
      TSharedPtr<FJsonObject> LinkJson = MakeShareable(new FJsonObject());
      json_helper::TrySet(LinkJson, TEXT("A"), AKey);
      json_helper::TrySet(LinkJson, TEXT("B"), BKey);
      Links.Add(MakeShareable(new FJsonValueObject(LinkJson)));
    }
  }

  Json->SetArrayField(TEXT("Links"), Links);
  return true;
}

bool URailNetwork::DeserializeJson(TSharedPtr<FJsonObject> Json) {
  if (!Json.IsValid()) {
    return false;
  }

  const TArray<TSharedPtr<FJsonValue>> *Links = nullptr;
  if (!Json->TryGetArrayField(TEXT("Links"), Links) || !Links) {
    return true;
  }

  int32 RestoredLinks = 0;
  for (const TSharedPtr<FJsonValue> &LinkValue : *Links) {
    TSharedPtr<FJsonObject> LinkJson = LinkValue.IsValid() ? LinkValue->AsObject() : nullptr;
    if (!LinkJson.IsValid()) {
      LOG(ERROR_LL) << "RailLoad: invalid link entry json";
      return false;
    }

    FQrVector3i AKey = FQrVector3i::Zero();
    FQrVector3i BKey = FQrVector3i::Zero();
    const bool bHasA = json_helper::TryGet(LinkJson, TEXT("A"), AKey);
    const bool bHasB = json_helper::TryGet(LinkJson, TEXT("B"), BKey);
    if (!bHasA || !bHasB) {
      LOG(ERROR_LL) << "RailLoad: link entry missing A/B";
      return false;
    }

    URailNodeBlockLogic *const *A = Nodes.Find(AKey);
    URailNodeBlockLogic *const *B = Nodes.Find(BKey);
    if (!A || !*A || !B || !*B) {
      LOG(ERROR_LL) << "RailLoad: missing node for link A=" << AKey.ToString() << " B=" << BKey.ToString();
      return false;
    }
    if (TryAddSegment(*A, *B)) {
      ++RestoredLinks;
    }
  }

  LOG(INFO_LL) << "RailLoad: links_total=" << Links->Num()
               << " restored=" << RestoredLinks
               << " nodes_registered=" << Nodes.Num();

  return true;
}
