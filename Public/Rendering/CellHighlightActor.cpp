#include "CellHighlightActor.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Evospace/CoordinateSystem.h"
#include "UObject/ConstructorHelpers.h"

namespace {
constexpr int32 ColorDataSize = 3;
}

ACellHighlightActor::ACellHighlightActor() {
  PrimaryActorTick.bCanEverTick = true;

  CellInstances = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("CellPreview"));
  CellInstances->NumCustomDataFloats = ColorDataSize;
  if (const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Models/cell_selector.cell_selector")); MeshFinder.Succeeded()) {
    CellInstances->SetStaticMesh(MeshFinder.Object);
  }
  SetRootComponent(CellInstances);

  SetActorEnableCollision(false);
  RelativeCells.Add(FVector3i::Zero());
}

void ACellHighlightActor::Tick(float DeltaSeconds) {
  Super::Tick(DeltaSeconds);
  RebuildCells();
}

void ACellHighlightActor::SetRelativeCells(const TArray<FVector3i> &Cells) {
  RelativeCells = Cells;
  UseAbsoluteCoordinatesFlag = false;
  ForceRebuild();
}

void ACellHighlightActor::SetAbsoluteCells(const TArray<FVector3i> &Cells) {
  AbsoluteCells = Cells;
  UseAbsoluteCoordinatesFlag = true;
  ForceRebuild();
}

void ACellHighlightActor::UseAbsoluteCoordinates(bool bEnable) {
  UseAbsoluteCoordinatesFlag = bEnable;
  ForceRebuild();
}

void ACellHighlightActor::SetHighlightColor(const FLinearColor &Color) {
  HighlightColor = Color;
  ForceRebuild();
}

void ACellHighlightActor::SetInstanceColor(int32 Index, const FLinearColor &Color) {
  if (Index < 0) {
    return;
  }

  if (!InstanceColorOverrides.IsValidIndex(Index)) {
    const int32 previousNum = InstanceColorOverrides.Num();
    InstanceColorOverrides.SetNum(Index + 1);
    InstanceColorOverrideMask.SetNum(Index + 1, false);
    for (int32 i = previousNum; i < InstanceColorOverrides.Num(); ++i) {
      InstanceColorOverrides[i] = HighlightColor;
    }
  } else if (InstanceColorOverrideMask.Num() != InstanceColorOverrides.Num()) {
    InstanceColorOverrideMask.SetNum(InstanceColorOverrides.Num(), false);
  }

  InstanceColorOverrides[Index] = Color;
  InstanceColorOverrideMask[Index] = true;
  ForceRebuild();
}

void ACellHighlightActor::SetInstanceColors(const TArray<FLinearColor> &Colors) {
  if (Colors.IsEmpty()) {
    InstanceColorOverrides.Reset();
    InstanceColorOverrideMask.Reset();
  } else {
    InstanceColorOverrides = Colors;
    InstanceColorOverrideMask.Reset();
    InstanceColorOverrideMask.Init(true, Colors.Num());
  }

  ForceRebuild();
}

TArray<FLinearColor> ACellHighlightActor::GetInstanceColors() const {
  return CachedInstanceColors;
}

void ACellHighlightActor::ForceRebuild() {
  NeedsFullRebuild = true;
}

void ACellHighlightActor::RebuildCells() {
  if (!CellInstances) {
    CachedInstanceColors.Reset();
    return;
  }

  const TArray<FVector3i> &SourceCells = UseAbsoluteCoordinatesFlag ? AbsoluteCells : RelativeCells;
  if (SourceCells.IsEmpty()) {
    if (NeedsFullRebuild && CellInstances->GetInstanceCount() > 0) {
      CellInstances->ClearInstances();
      CellInstances->MarkRenderStateDirty();
    }
    CachedInstanceColors.Reset();
    NeedsFullRebuild = false;
    return;
  }

  if (CellInstances->NumCustomDataFloats < ColorDataSize) {
    CellInstances->NumCustomDataFloats = ColorDataSize;
  }

  const AActor *ReferenceActor = nullptr;
  if (!UseAbsoluteCoordinatesFlag) {
    ReferenceActor = GetOwner() ? GetOwner() : this;
  }

  const FQuat ReferenceRotation = ReferenceActor ? ReferenceActor->GetActorQuat() : FQuat::Identity;
  const FVector ReferenceLocation = ReferenceActor ? ReferenceActor->GetActorLocation() : FVector::ZeroVector;
  const FVector3i PivotWB = UseAbsoluteCoordinatesFlag ? FVector3i::Zero() : cs::WtoWB(ReferenceLocation);

  TArray<FTransform> InstanceTransforms;
  InstanceTransforms.Reserve(SourceCells.Num());
  for (const FVector3i &Cell : SourceCells) {
    FVector3i WorldCell = Cell;
    if (!UseAbsoluteCoordinatesFlag) {
      const FVector Rotated = ReferenceRotation.RotateVector(Cell.vec());
      WorldCell = PivotWB + RoundToInt(Rotated);
    }

    const FVector CellCenter = cs::WBtoWd(WorldCell) + FVector(gCubeSize / 2.0f);
    InstanceTransforms.Add(FTransform(CellCenter));
  }

  if (NeedsFullRebuild || CellInstances->GetInstanceCount() != InstanceTransforms.Num()) {
    CellInstances->ClearInstances();
    CellInstances->AddInstances(InstanceTransforms, false, true, false);
  } else {
    for (int32 Index = 0; Index < InstanceTransforms.Num(); ++Index) {
      CellInstances->UpdateInstanceTransform(Index, InstanceTransforms[Index], true, false, true);
    }
  }

  CachedInstanceColors.SetNum(InstanceTransforms.Num());
  for (int32 Index = 0; Index < InstanceTransforms.Num(); ++Index) {
    const FLinearColor Color = ResolveInstanceColor(Index);
    CachedInstanceColors[Index] = Color;
    const float ColorArray[ColorDataSize] = { Color.R, Color.G, Color.B };
    CellInstances->SetCustomData(Index, MakeArrayView(ColorArray, ColorDataSize), false);
  }

  CellInstances->MarkRenderStateDirty();
  NeedsFullRebuild = false;
}

FLinearColor ACellHighlightActor::ResolveInstanceColor(int32 Index) const {
  if (InstanceColorOverrideMask.IsValidIndex(Index) && InstanceColorOverrideMask[Index] && InstanceColorOverrides.IsValidIndex(Index)) {
    return InstanceColorOverrides[Index];
  }
  return HighlightColor;
}