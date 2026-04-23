// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#include "Public/RailSplineRenderManagerActor.h"
#include "Components/SceneComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Engine/StaticMesh.h"

namespace {
struct FHermiteArcSample {
  float T = 0.0f;
  float Distance = 0.0f;
};

FVector EvaluateHermitePosition(float T, const FVector &P0, const FVector &T0, const FVector &P1, const FVector &T1) {
  const float T2 = T * T;
  const float T3 = T2 * T;
  const float H00 = (2.0f * T3) - (3.0f * T2) + 1.0f;
  const float H10 = T3 - (2.0f * T2) + T;
  const float H01 = (-2.0f * T3) + (3.0f * T2);
  const float H11 = T3 - T2;
  return (H00 * P0) + (H10 * T0) + (H01 * P1) + (H11 * T1);
}

FVector EvaluateHermiteDerivative(float T, const FVector &P0, const FVector &T0, const FVector &P1, const FVector &T1) {
  const float T2 = T * T;
  const float DH00 = (6.0f * T2) - (6.0f * T);
  const float DH10 = (3.0f * T2) - (4.0f * T) + 1.0f;
  const float DH01 = (-6.0f * T2) + (6.0f * T);
  const float DH11 = (3.0f * T2) - (2.0f * T);
  return (DH00 * P0) + (DH10 * T0) + (DH01 * P1) + (DH11 * T1);
}

void BuildHermiteArcSamples(
  const FVector &P0,
  const FVector &T0,
  const FVector &P1,
  const FVector &T1,
  int32 SampleCount,
  TArray<FHermiteArcSample> &OutSamples,
  float &OutArcLength) {
  OutSamples.Reset();
  OutArcLength = 0.0f;

  const int32 ClampedSampleCount = FMath::Max(4, SampleCount);
  OutSamples.Reserve(ClampedSampleCount + 1);
  OutSamples.Add({ 0.0f, 0.0f });

  FVector PrevPosition = P0;
  float AccumulatedDistance = 0.0f;
  for (int32 Index = 1; Index <= ClampedSampleCount; ++Index) {
    const float T = static_cast<float>(Index) / static_cast<float>(ClampedSampleCount);
    const FVector Position = EvaluateHermitePosition(T, P0, T0, P1, T1);
    AccumulatedDistance += FVector::Distance(PrevPosition, Position);
    OutSamples.Add({ T, AccumulatedDistance });
    PrevPosition = Position;
  }

  OutArcLength = AccumulatedDistance;
}

float FindHermiteTForDistance(const TArray<FHermiteArcSample> &Samples, float Distance) {
  if (Samples.Num() == 0) {
    return 0.0f;
  }

  if (Distance <= 0.0f) {
    return 0.0f;
  }

  const float TotalDistance = Samples.Last().Distance;
  if (Distance >= TotalDistance || Samples.Num() == 1) {
    return 1.0f;
  }

  int32 UpperIndex = 1;
  while (UpperIndex < Samples.Num() && Samples[UpperIndex].Distance < Distance) {
    ++UpperIndex;
  }
  const int32 LowerIndex = FMath::Max(0, UpperIndex - 1);
  const FHermiteArcSample &LowerSample = Samples[LowerIndex];
  const FHermiteArcSample &UpperSample = Samples[UpperIndex];
  const float SegmentDistance = UpperSample.Distance - LowerSample.Distance;
  if (SegmentDistance <= KINDA_SMALL_NUMBER) {
    return UpperSample.T;
  }

  const float Alpha = (Distance - LowerSample.Distance) / SegmentDistance;
  return FMath::Lerp(LowerSample.T, UpperSample.T, Alpha);
}
} // namespace

ARailSplineRenderManagerActor::ARailSplineRenderManagerActor() {
  PrimaryActorTick.bCanEverTick = false;
  bReplicates = false;

  RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RailSplineRoot"));
  RootComponent->SetMobility(EComponentMobility::Static);
}

void ARailSplineRenderManagerActor::RebuildRailSegments(const TArray<FRailRenderSegmentData> &Segments) {
  ClearRailSegments();

  if (!RootComponent) {
    return;
  }

  if (!RailSplineStaticMesh) {
    RailSplineStaticMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/CoreContent/Rail2.Rail2"));
  }
  if (!RailSplineStaticMesh) {
    return;
  }

  const FTransform RootTransform = RootComponent->GetComponentTransform();
  constexpr float HeightOffset = -50.0f;
  const float MeshLength = RailSplineStaticMesh->GetBounds().BoxExtent.X * 2.0f;
  const float SegmentStepLength = FMath::Max(1.0f, MeshLength);

  for (const FRailRenderSegmentData &Segment : Segments) {
    const float SegmentLength = FVector::Dist(Segment.StartWorld, Segment.EndWorld);
    if (SegmentLength < KINDA_SMALL_NUMBER) {
      continue;
    }

    const FVector StartWorld = Segment.StartWorld + FVector(0.0f, 0.0f, HeightOffset);
    const FVector EndWorld = Segment.EndWorld + FVector(0.0f, 0.0f, HeightOffset);
    const FVector SegmentDirection = (EndWorld - StartWorld).GetSafeNormal();

    FVector StartDirection = Segment.StartTangentWorld.GetSafeNormal();
    if (StartDirection.IsNearlyZero()) {
      StartDirection = SegmentDirection;
    }
    if (FVector::DotProduct(StartDirection, SegmentDirection) < 0.0f) {
      StartDirection *= -1.0f;
    }

    FVector EndDirection = Segment.EndTangentWorld.GetSafeNormal();
    if (EndDirection.IsNearlyZero()) {
      EndDirection = SegmentDirection;
    }
    if (FVector::DotProduct(EndDirection, SegmentDirection) < 0.0f) {
      EndDirection *= -1.0f;
    }

    const float TangentScale = SegmentLength * 0.5f;
    const FVector StartHermiteTangent = StartDirection * TangentScale;
    const FVector EndHermiteTangent = EndDirection * TangentScale;

    TArray<FHermiteArcSample> ArcSamples;
    const int32 ArcSampleCount = FMath::Clamp(FMath::RoundToInt(SegmentLength / 20.0f), 12, 96);
    float ArcLength = 0.0f;
    BuildHermiteArcSamples(StartWorld, StartHermiteTangent, EndWorld, EndHermiteTangent, ArcSampleCount, ArcSamples, ArcLength);
    if (ArcLength < KINDA_SMALL_NUMBER) {
      continue;
    }

    const int32 SegmentMeshCount = FMath::Max(1, FMath::CeilToInt(ArcLength / SegmentStepLength));
    TArray<FVector> ResampledPoints;
    TArray<float> ResampledDistances;
    ResampledPoints.Reserve(SegmentMeshCount + 1);
    ResampledDistances.Reserve(SegmentMeshCount + 1);
    for (int32 PointIndex = 0; PointIndex <= SegmentMeshCount; ++PointIndex) {
      const float Distance = (PointIndex == SegmentMeshCount)
                               ? ArcLength
                               : FMath::Min(static_cast<float>(PointIndex) * SegmentStepLength, ArcLength);
      const float T = FindHermiteTForDistance(ArcSamples, Distance);
      ResampledPoints.Add(EvaluateHermitePosition(T, StartWorld, StartHermiteTangent, EndWorld, EndHermiteTangent));
      ResampledDistances.Add(Distance);
    }

    TArray<FVector> PointTangentDirections;
    PointTangentDirections.SetNum(ResampledPoints.Num());
    for (int32 PointIndex = 0; PointIndex < ResampledPoints.Num(); ++PointIndex) {
      const int32 PrevIndex = FMath::Max(0, PointIndex - 1);
      const int32 NextIndex = FMath::Min(ResampledPoints.Num() - 1, PointIndex + 1);
      FVector TangentDirection = (ResampledPoints[NextIndex] - ResampledPoints[PrevIndex]).GetSafeNormal();
      if (TangentDirection.IsNearlyZero()) {
        TangentDirection = SegmentDirection;
      }
      PointTangentDirections[PointIndex] = TangentDirection;
    }

    for (int32 MeshIndex = 0; MeshIndex < SegmentMeshCount; ++MeshIndex) {
      const FVector WorldP0 = ResampledPoints[MeshIndex];
      const FVector WorldP1 = ResampledPoints[MeshIndex + 1];
      const float SegmentPieceLength = FMath::Max(
        KINDA_SMALL_NUMBER,
        ResampledDistances[MeshIndex + 1] - ResampledDistances[MeshIndex]);
      const float TangentLength = SegmentPieceLength * 0.5f;
      const FVector WorldT0 = PointTangentDirections[MeshIndex] * TangentLength;
      const FVector WorldT1 = PointTangentDirections[MeshIndex + 1] * TangentLength;

      USplineMeshComponent *SplineMesh = NewObject<USplineMeshComponent>(this);
      if (!SplineMesh) {
        continue;
      }

      SplineMesh->SetMobility(RootComponent->Mobility);
      SplineMesh->SetStaticMesh(RailSplineStaticMesh);
      SplineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
      SplineMesh->SetCastShadow(false);
      SplineMesh->SetGenerateOverlapEvents(false);
      SplineMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

      const FVector StartLocal = RootTransform.InverseTransformPosition(WorldP0);
      const FVector EndLocal = RootTransform.InverseTransformPosition(WorldP1);
      const FVector StartTangentLocal = RootTransform.InverseTransformVectorNoScale(WorldT0);
      const FVector EndTangentLocal = RootTransform.InverseTransformVectorNoScale(WorldT1);
      SplineMesh->SetStartAndEnd(StartLocal, StartTangentLocal, EndLocal, EndTangentLocal);

      AddInstanceComponent(SplineMesh);
      SplineMesh->RegisterComponent();
      RailSplineMeshes.Add(SplineMesh);
    }
  }
}

void ARailSplineRenderManagerActor::EndPlay(const EEndPlayReason::Type EndPlayReason) {
  ClearRailSegments();
  Super::EndPlay(EndPlayReason);
}

void ARailSplineRenderManagerActor::ClearRailSegments() {
  for (USplineMeshComponent *SplineMesh : RailSplineMeshes) {
    if (SplineMesh) {
      SplineMesh->DestroyComponent();
    }
  }
  RailSplineMeshes.Empty();
}
