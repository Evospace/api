// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#include "Public/RailNetwork.h"
#include "Public/BlockLogic.h"
#include "Public/Dimension.h"
#include "Public/RailNodeBlockLogic.h"
#include "Public/RailwayManager.h"
#include "Qr/StaticLogger.h"
#include "Math/NumericLimits.h"
#include "DrawDebugHelpers.h"
#include <algorithm>

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
  MarkDirty();
}

void URailNetwork::UnregisterNode(URailNodeBlockLogic *Node) {
  if (!Node) {
    return;
  }
  Node->UnlinkAllRail();
  const FQrVector3i k = RootKey(Node);
  Nodes.Remove(k);
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
  TMap<URailNodeBlockLogic *, float> dist;
  TMap<URailNodeBlockLogic *, URailNodeBlockLogic *> parent;
  TSet<URailNodeBlockLogic *> done;
  constexpr float kInf = TNumericLimits<float>::Max() * 0.5f;
  for (URailNodeBlockLogic *K : all) {
    dist.Add(K, TNumericLimits<float>::Max());
  }
  dist[*pStart] = 0.f;
  while (done.Num() < all.Num()) {
    URailNodeBlockLogic *u = nullptr;
    float bestD = TNumericLimits<float>::Max();
    bool foundU = false;
    for (URailNodeBlockLogic *K : all) {
      if (done.Contains(K)) {
        continue;
      }
      if (const float *D = dist.Find(K)) {
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
        dist.Add(v, TNumericLimits<float>::Max());
      }
      const float w = FVector::Dist(u->GetLocation(), v->GetLocation());
      const float alt = bestD + w;
      if (const float *OldD = dist.Find(v)) {
        if (alt < *OldD) {
          dist[v] = alt;
          parent.FindOrAdd(v) = u;
        }
      }
    }
  }
  if (const float *EndD = dist.Find(*pEnd)) {
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

float URailNetwork::GetEdgeLength(const FQrVector3i &From, const FQrVector3i &To) const {
  URailNodeBlockLogic *const *Na = Nodes.Find(From);
  URailNodeBlockLogic *const *Nb = Nodes.Find(To);
  if (!Na || !*Na || !Nb || !*Nb) {
    return 0.f;
  }
  return FVector::Dist((*Na)->GetLocation(), (*Nb)->GetLocation());
}

void URailNetwork::SampleEdgeWorld(const FQrVector3i &From, const FQrVector3i &To, float DistanceAlong, FVector &OutPos, FVector &OutTangent) const {
  URailNodeBlockLogic *const *Na = Nodes.Find(From);
  URailNodeBlockLogic *const *Nb = Nodes.Find(To);
  if (!Na || !*Na || !Nb || !*Nb) {
    OutPos = FVector::ZeroVector;
    OutTangent = FVector::ForwardVector;
    return;
  }
  const FVector Pa = (*Na)->GetLocation();
  const FVector Pb = (*Nb)->GetLocation();
  const float len = FVector::Dist(Pa, Pb);
  if (len < KINDA_SMALL_NUMBER) {
    OutPos = Pa;
    OutTangent = FVector::ForwardVector;
    return;
  }
  const float t = FMath::Clamp(DistanceAlong / len, 0.f, 1.f);
  OutPos = FMath::Lerp(Pa, Pb, t);
  OutTangent = (Pb - Pa).GetSafeNormal();
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
