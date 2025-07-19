#include "SectorProxy.h"

#include "EngineUtils.h"
#include "RealtimeMeshSimple.h"
#include "Evospace/Blocks/BlockActor.h"
#include "Evospace/Item/DroppedInventory.h"
#include "Evospace/Item/InventoryLibrary.h"
#include "Evospace/Player/MainPlayerController.h"
#include "Evospace/Props/SectorPropComponent.h"
#include "Evospace/World/Sector.h"
#include "Evospace/World/SectorCompiler.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Public/AutosizeInventory.h"
#include "Public/Dimension.h"
#include "Public/RuntimeMeshBuilder.h"

#include "Public/StaticBlock.h"
#include "Public/StaticProp.h"
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

  for (IndexType i = 0; i < Vec3i(2, 2, 2).Capacity(); ++i) {
    if (auto sector = ajacentSectors.Element(out * cs::IndexToCell(i, Vec3i(2, 2, 2))))
      sector->SetDirty(true);
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
    if (block == nullptr && logic != nullptr) {
      LOG(ERROR_LL) << "Sector desync at " << bpos << ". Logic " << logic->GetName() << ". Trying to fix";
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
          } else if (ABlockActor *actor = logic->GetStaticBlock()->SpawnActorAndLuaDeferred(dim, logic, transform)) {
            logic->DeferredPaintApply();
            actor->AttachToActor(owner, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
            owner->SetRenderable(logic->GetBlockPos(), actor, logic);
          }

          if (restored) [[unlikely]] {
            for (auto &pos : block->Positions) {
              auto subPos = bpos + RotateVector(logic->GetBlockQuat(), pos);
              auto type = dim->GetBlockLogic(subPos);
              int32 index;
              if (auto sector = dim->FindBlockCell(subPos, index)) [[likely]] {
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
      LOG(ERROR_LL) << "Sector desync at " << bpos << ". Block class " << block->GetName() << " clearing this cell";
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

bool USectorProxy::ApplyDataFromCompiler(ADimension *dim, UTesselator::Data &&data, int32 lod, TFunction<void()> callback) {
  if (data.IsEmpty() && rmc) {
    auto mesh = rmc->GetRealtimeMeshAs<URealtimeMeshSimple>();
    if (mesh) {
      mesh->Reset();
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
  }

  if (data.Num() != LastSectionCount) {
    IsSectionGroupCreated = false;
    LastSectionCount = data.Num();
    if (auto mesh = rmc->GetRealtimeMeshAs<URealtimeMeshSimple>()) {
      mesh->Reset();
    }
  }

  RuntimeMeshBuilder::BuildRealtimeMesh(rmc->GetRealtimeMeshAs<URealtimeMeshSimple>(), MoveTemp(data), IsSectionGroupCreated);

  callback();
  return true;
}

USectorPropComponent *USectorProxy::GetInstancingComponent() const {
  return owner->SectorPropComponent;
}

namespace {
void Drop(const UStaticProp *prop, UInventory *inv) {
  auto &minable = prop->mMinable;
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

  //TODO: Bad place for that
  if (!out_inventory->IsEmpty()) {
    auto mpc = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

    if (!ensure(mpc))
      return;

    const auto mpcTransform = mpc->GetCharacter()->GetTransform();

    const auto dropped = GetWorld()->SpawnActorDeferred<ADroppedInventory>(ADroppedInventory::StaticClass(), mpcTransform, owner);
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