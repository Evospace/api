#include "SectorProxy.h"

#include "EngineUtils.h"
#include "RealtimeMeshSimple.h"
#include "Public/BlockActor.h"
#include "Evospace/Item/DroppedInventory.h"
#include "Evospace/Item/InventoryLibrary.h"
#include "Evospace/Player/MainPlayerController.h"
#include "Evospace/Props/SectorPropComponent.h"
#include "Evospace/World/Column.h"
#include "Evospace/World/Sector.h"
#include "Evospace/World/SectorCompiler.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Public/AutosizeInventory.h"
#include "Public/Dimension.h"
#include "Public/RuntimeMeshBuilder.h"
#include <GameFramework/Actor.h>

#include "Public/StaticBlock.h"
#include "Public/StaticProp.h"
#include "Evospace/WorldEntities/WetnessMapSubsystem.h"

namespace {

const FName SurfaceSectionGroupName(TEXT("Surface"));

void RemoveLodSectionGroup(URealtimeMeshSimple *Mesh, int32 LodIndex) {
  if (!Mesh) {
    return;
  }

  const FRealtimeMeshLODKey LodKey(LodIndex);
  bool bLodExists = false;
  for (const FRealtimeMeshLODKey &Key : Mesh->GetLODs()) {
    if (Key == LodKey) {
      bLodExists = true;
      break;
    }
  }

  if (!bLodExists) {
    return;
  }

  const FRealtimeMeshSectionGroupKey SectionGroupKey =
    FRealtimeMeshSectionGroupKey::Create(LodKey, SurfaceSectionGroupName);

  // Use safer check for section group existence
  const TArray<FRealtimeMeshSectionGroupKey> Groups = Mesh->GetSectionGroups(LodKey);
  if (Groups.Contains(SectionGroupKey)) {
    Mesh->RemoveSectionGroup(SectionGroupKey);
  }
}

} // namespace

bool USectorProxy::SetDataForCompiler(SectorCompilerData &data) {
  SetDirty(false);

  for (int i = 0; i < Vec3i(3, 3, 3).Capacity(); ++i) {
    auto pos = cs::IndexToArea(i, Vec3i(-1, -1, -1), Vec3i(1, 1, 1));
    data.SetSector(pos, GetAdjacentSectors().Element(pos));
  }

  return true;
}

USectorProxy::USectorProxy() {
  StaticBlocks.Init({}, gSectorSize.Capacity());
}

void USectorProxy::SpawnColumn(const FSectorData &data) {
  Dirty = true;
  SectorColdData = data;
}

void USectorProxy::GetSectorDataHot(FSectorData &data) {
  data.mStaticBlocks = StaticBlocks;

  // Костыль, нужно переехать на хранение в колонне
  if (PivotPos.Z == 0 && ensure(owner && owner->SectorPropComponent)) {
    owner->SectorPropComponent->GetAll(data.mAttaches);
  }
}

void USectorProxy::GetSectorDataCold(FSectorData &data) {
  data = SectorColdData;
}

void USectorProxy::SetDirty(IndexType index) {
  auto pos = cs::IndexToCell(index, gSectorSize);

  Vec3i out(0, 0, 0);
  out.X += (pos.X == 0 ? -1 : 0);
  out.X += (pos.X == gSectorSize.X - 1 ? 1 : 0);
  out.Y += (pos.Y == 0 ? -1 : 0);
  out.Y += (pos.Y == gSectorSize.Y - 1 ? 1 : 0);
  out.Z += (pos.Z == 0 ? -1 : 0);
  out.Z += (pos.Z == gSectorSize.Z - 1 ? 1 : 0);

  if (out == Vec3i(0, 0, 0)) {
    if (auto sector = ajacentSectors.Element(out)) {
      sector->SetDirty(true);
    }
  } else {
    TArray<Vec3i> uniqueOffsets;
    uniqueOffsets.Reserve(Vec3i(2, 2, 2).Capacity());
    for (IndexType i = 0; i < Vec3i(2, 2, 2).Capacity(); ++i) {
      const Vec3i offset = out * cs::IndexToCell(i, Vec3i(2, 2, 2));
      bool isUnique = true;
      for (const Vec3i &existing : uniqueOffsets) {
        if (existing == offset) {
          isUnique = false;
          break;
        }
      }
      if (isUnique) {
        uniqueOffsets.Add(offset);
      }
    }
    for (const Vec3i &offset : uniqueOffsets) {
      if (auto sector = ajacentSectors.Element(offset)) {
        sector->SetDirty(true);
      }
    }
  }

  if (ensure(owner)) {
    owner->SaveDirty = true;
  }
}

void USectorProxy::SetStaticBlock(IndexType index, const UStaticBlock *value) {
  StaticBlocks[index].block = value;

  if (value == nullptr) {
    StaticBlocks[index].density = {};
  }

  if (!value || value->Tesselator) {
    SetDirty(index);
  }
}
const UStaticBlock *USectorProxy::GetStaticBlock(IndexType index) {
  return StaticBlocks[index].block;
}

void USectorProxy::SetBlockDensity(IndexType index, BlockDensity density) {
  SetDirty(index);
  StaticBlocks[index].density = density;
}

const BlockDensity &USectorProxy::GetBlockDensity(IndexType index) const {
  return StaticBlocks[index].density;
}

void USectorProxy::LoadSector(const AColumn &c) {
  StaticBlocks = SectorColdData.mStaticBlocks;

  auto dim = c.Dim;

  for (int i = 0; i < StaticBlocks.Num(); ++i) {
    bool restored = false;
    const auto bpos = cs::IndexToCell(i, gSectorSize) + GetPivotPos();
    auto logic = dim->GetBlockLogic(bpos);
    auto &block = StaticBlocks[i].block;
    if (logic != nullptr && (block == nullptr || block != logic->GetStaticBlock())) {
      const UStaticBlock *logicStaticBlock = logic->GetStaticBlock();
      if (block) {
        LOG(ERROR_LL) << "Sector desync at " << bpos << " (pivot " << GetPivotPos() << ", index " << i
                      << "). Logic " << logic->GetName()
                      << " static block " << (logicStaticBlock ? logicStaticBlock->GetName() : TEXT("nullptr"))
                      << ", but sector has block " << block->GetName() << ". Trying to fix";
      } else {
        LOG(ERROR_LL) << "Sector desync at " << bpos << " (pivot " << GetPivotPos() << ", index " << i
                      << "). Logic " << logic->GetName()
                      << " static block " << (logicStaticBlock ? logicStaticBlock->GetName() : TEXT("nullptr"))
                      << ", but sector has no block. Trying to fix";
      }
      block = logic->GetStaticBlock();
      restored = true;
    }
    if (block != nullptr && block->mActorClass == nullptr && !block->NoActorRenderable) {
      continue;
    }
    if (block == nullptr) {
      continue;
    }
    if (logic) [[likely]] {
      if (!logic->IsA(UPartBlockLogic::StaticClass())) {
        auto staticBlock = logic->GetStaticBlock();
        if (staticBlock->mActorClass || staticBlock->NoActorRenderable) {
          FTransform transform(cs::WBtoWd(logic->GetBlockPos()) + gCubeSize / 2.0);
          transform.SetRotation(logic->GetBlockQuat());
          if (staticBlock->NoActorRenderable) {
            owner->SetRenderable(logic->GetBlockPos(), true, logic);
          } else if (ABlockActor *actor =
                       logic->GetStaticBlock()->SpawnActorAndLuaDeferred(dim, logic, transform)) {
            logic->DeferredPaintApply();
            actor->AttachToActor(owner, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
            owner->SetRenderable(logic->GetBlockPos(), actor, logic);
          }

          if (restored) [[unlikely]] {
            for (auto &pos : block->Positions) {
              auto subPos = bpos + RotateVector(logic->GetBlockQuat(), pos);
              auto type = dim->GetBlockLogic(subPos);
              int32 index;
              if (auto sector = dim->FindBlockCell(subPos, index)) [[likely]]
              {
                sector->SetStaticBlock(index, block);
              } else [[unlikely]] {
                LOG(ERROR_LL) << "fixing part of block is failed at " << pos;
              }
              if (type) {
                LOG(WARN_LL) << type->GetName() << " at " << pos;
              } else {
                LOG(ERROR_LL) << "nullptr at " << pos;
              }
            }
          }
        }
      }
    } else [[unlikely]] {
      LOG(ERROR_LL) << "Sector desync at " << bpos << " (pivot " << GetPivotPos() << ", index " << i
                    << "). Block class " << block->GetName()
                    << " has static data but no logic, clearing this cell";
      dim->SetBlockLogic(bpos, nullptr);
      SetStaticBlock(i, nullptr);
      SetBlockDensity(i, 0);
    }
  }

  for (const auto &it : SectorColdData.mAttaches)
    for (const auto &tr : it.Value) {
      auto bpos = cs::WtoWB(tr.GetLocation());
      it.Key->Create(this, static_cast<FTransform>(tr), bpos);
      it.Key->OnSpawn(cs::WtoWB(tr.GetLocation()));
    }
  // Spawn lightweight actor decorations (not saved, not per-cell stored)
  if (owner) {
    UWorld *world = owner->GetWorld();
    if (world) {
      for (const auto &it : SectorColdData.mActorAttaches) {
        for (const auto &tr : it.Value) {
          UClass *cls = it.Key->ActorClass ? it.Key->ActorClass : AActor::StaticClass();
          AActor *actor = world->SpawnActor<AActor>(cls, static_cast<FTransform>(tr));
          if (actor) {
            actor->AttachToActor(owner, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
            owner->ActorDecorations.Add(actor);
          }
        }
      }
    }
  }

  SetDirty(true);
}

void USectorProxy::UnloadSector() {
  SectorColdData = {};
  SectorColdData.mStaticBlocks = StaticBlocks;

  for (auto &i : owner->RenderBlocks) {
    i.Value->RemoveActorOrRenderable();
  }
  owner->RenderBlocks.Empty();
}

bool USectorProxy::ApplyDataFromCompiler(ADimension *dim, UTesselator::Data &&data, int32 lod,
                                         TFunction<void()> callback) {
  const bool bHasGeometry = !data.IsEmpty();
  auto Mesh = rmc ? rmc->GetRealtimeMeshAs<URealtimeMeshSimple>() : nullptr;

  if (!bHasGeometry) {
    if (Mesh) {
      RemoveLodSectionGroup(Mesh, lod);
    }
    if (lod == 0) {
      IsSectionGroupCreated = false;
      if (rmc) {
        rmc->DestroyComponent();
        rmc = nullptr;
      }
    }
    callback();
    return true;
  }

  if (!rmc) {
    rmc = NewObject<URealtimeMeshComponent>(owner, URealtimeMeshComponent::StaticClass());
    rmc->AttachToComponent(owner->GetRootComponent(), { EAttachmentRule::KeepWorld, false });
    rmc->SetMobility(EComponentMobility::Type::Static);
    FTransform tr;
    tr.SetLocation(cs::WBtoWd(GetPivotPos()));
    rmc->SetWorldTransform(tr);
    rmc->RegisterComponent();
    rmc->InitializeRealtimeMesh<URealtimeMeshSimple>();
    Mesh = rmc->GetRealtimeMeshAs<URealtimeMeshSimple>();
  } else if (!Mesh) {
    rmc->InitializeRealtimeMesh<URealtimeMeshSimple>();
    Mesh = rmc->GetRealtimeMeshAs<URealtimeMeshSimple>();
  }

  if (!Mesh) {
    callback();
    return false;
  }

  RuntimeMeshBuilder::BuildRealtimeMesh(Mesh, MoveTemp(data), lod);
  callback();
  if (lod == 0 && owner) {
    IsSectionGroupCreated = true;
    owner->UpdateWetnessMap();
  }
  return true;
}

USectorPropComponent *USectorProxy::GetInstancingComponent() const {
  return owner->SectorPropComponent;
}

// No-op accessor removed to avoid hard dependency

namespace {
void Drop(const UStaticProp *prop, UInventory *inv) {
  auto &minable = prop->Minable;
  if (minable.Minable) {
    UInventoryLibrary::Add(inv, { minable.Result, minable.Count });
  }
}
} // namespace

void USectorProxy::ClearBlockPropsDrop(const FVector3i &_bpos, bool only_small) {
  const auto out_inventory = NewObject<UAutosizeInventory>();

  for (int32 i = 0; i < Vec3i(3, 3, 3).Capacity(); ++i) {
    auto offset = cs::IndexToArea(i, Vec3i(-1), Vec3i(1));
    auto bpos = _bpos + offset;

    IndexType s_index = -1;
    if (auto sector = owner->Dim->FindBlockCell(bpos, s_index)) {
      auto props = owner->SectorPropComponent->Get(bpos);
      for (const auto it : props) {
        Drop(it, out_inventory);
      }

      sector->GetInstancingComponent()->DestroyInBlock(bpos, only_small);
    }
  }

  // TODO: Bad place for that
  if (!out_inventory->IsEmpty()) {
    auto mpc = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

    if (!ensure(mpc))
      return;

    const auto mpcTransform = mpc->GetCharacter()->GetTransform();

    const auto dropped =
      GetWorld()->SpawnActorDeferred<ADroppedInventory>(ADroppedInventory::StaticClass(), mpcTransform, owner);
    if (ensure(dropped)) {
      UInventoryLibrary::TryMoveFromAny(dropped->GetInventory(), out_inventory);
      dropped->FinishSpawning(mpcTransform);
    }
  }
}

void USectorProxy::ClearBlockProps(const FVector3i &_bpos, bool only_small) {
  for (int32 i = 0; i < Vec3i(3, 3, 3).Capacity(); ++i) {
    auto offset = cs::IndexToArea(i, Vec3i(-1), Vec3i(1));
    auto bpos = _bpos + offset;

    IndexType s_index = -1;
    if (auto sector = owner->Dim->FindBlockCell(bpos, s_index)) {
      sector->GetInstancingComponent()->DestroyInBlock(bpos, only_small);
    }
  }
}

void USectorProxy::ClearNearActors(const FVector3i &_bpos, float radius) {
  if (!owner)
    return;
  const FVector worldPos = cs::WBtoWd(_bpos) + FVector(gCubeSize * 0.5f);
  const float r2 = radius * radius;
  int32 i = 0;
  while (i < owner->ActorDecorations.Num()) {
    AActor *a = owner->ActorDecorations[i];
    if (!a) {
      owner->ActorDecorations.RemoveAtSwap(i);
      continue;
    }
    if (FVector::DistSquared(a->GetActorLocation(), worldPos) <= r2) {
      a->Destroy();
      owner->ActorDecorations.RemoveAtSwap(i);
      continue;
    }
    ++i;
  }
}