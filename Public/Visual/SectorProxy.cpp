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
bool ISectorProxy::SetDataForCompiler(SectorCompilerData &data){
  SetDirty(false);

  for (int i = 0; i < Vec3i(3, 3, 3).Capacity(); ++i) {
    auto pos = cs::IndexToArea(i, Vec3i(-1, -1, -1), Vec3i(1, 1, 1));
    data.SetSector(pos, GetAdjacentSectors().Element(pos));
  }

  return true;
}

USectorProxyHolder::USectorProxyHolder() {
  StaticBlocks.Init({}, gSectorSize.Capacity());
}

void USectorProxyHolder::SpawnColumnCallback(const FSectorData &data) {
  Dirty = true;
  SectorColdData = data;
}

void USectorProxyHolder::GetSectorDataHot(FSectorData &data) {
  SaveChanged = false;
  data = SectorColdData;
}

void USectorProxyHolder::GetSectorDataCold(FSectorData &data) {
  SaveChanged = false;
  data = SectorColdData;
}

void USectorProxyHolder::PromoteToSector() {
  // if (ensure(sector)) {
  //   sector->mMyColumn = column;
  //   sector->mPivotPos = spos * gSectorSize;
  //   sector->SetOwner(this);
  //   column->sectors.Add(sector);
  //   column->compiling.Add(false);
  //   sector->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
  // }
}

void USectorProxyHolder::Destroy(){
    if(IsValid(sector)) {
        sector->Destroy();
        sector = nullptr;
    }
  }
void USectorProxyHolder::SetStaticBlock(IndexType index, const UStaticBlock *value) {
  StaticBlocks[index].block = value;

  if (!value || value->Tesselator) {
    SetDirty(index);
  }
}
const UStaticBlock *USectorProxyHolder::GetStaticBlock(IndexType index) {
  return StaticBlocks[index].block;
}

void USectorProxyHolder::SetBlockDecity(IndexType index, BlockDensity density) {
  SetDirty(index);
  StaticBlocks[index].dencity = density;
}

const BlockDensity &USectorProxyHolder::GetBlockDesity(IndexType index) const {
  return StaticBlocks[index].dencity;
}

void USectorProxyHolder::LoadSector(const AColumn &c){ 
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
            logic->SetRenderable(this);
          } else if (ABlockActor *actor = logic->GetStaticBlock()->SpawnActorAndLuaDeferred(dim, logic, transform)) {
            logic->SetActor(actor);
            actor->FinishSpawning(transform);
            logic->DeferredPaintApply();
            actor->AttachToActor(owner, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
          }
          RenderBlocks.Add(cs::CellToIndex(logic->GetBlockPos() - GetPivotPos(), gSectorSize), logic);

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
      SetBlockDecity(i, 0);
    }
  }

  for (const auto &it : SectorColdData.mAttaches)
    for (const auto &tr : it.Value) {
      auto bpos = cs::WtoWB(tr.GetLocation()) - GetPivotPos();
      it.Key->Create(this, static_cast<FTransform>(tr), bpos);
      it.Key->OnSpawn(cs::WtoWB(tr.GetLocation()));
    }

  SetDirty(true);
}

void USectorProxyHolder::UnloadSector(){
  SectorColdData = {};
  SectorColdData.mStaticBlocks = StaticBlocks;

  for (auto i = RenderBlocks.begin(); i != RenderBlocks.end(); ++i) {
    i->Value->RemoveActorOrRenderable();
  }
  RenderBlocks.Empty();
}

bool USectorProxyHolder::ApplyDataFromCompiler(ADimension * dim, UTesselator::Data &&data, int32 lod, TFunction<void()> callback){
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
    rmc->AttachToComponent(owner->GetRootComponent(), {EAttachmentRule::KeepWorld, false});
    rmc->SetMobility(EComponentMobility::Type::Static);
    FTransform tr;
    tr.SetLocation(cs::WBtoWd(GetPivotPos()));
    rmc->SetWorldTransform(tr);
    rmc->RegisterComponent();
    rmc->InitializeRealtimeMesh<URealtimeMeshSimple>();
  }

  RuntimeMeshBuilder::BuildRealtimeMesh(rmc->GetRealtimeMeshAs<URealtimeMeshSimple>(), MoveTemp(data), IsSectionGroupCreated);
  
  //sector->ApplyDataFromCompiler(dim, MoveTemp(data), lod, callback);
  callback();
  return true;
}

USectorPropComponent * USectorProxyHolder::GetInstancingComponent() const{
  return owner->SectorPropComponent;
}

namespace {
  void Drop(const UStaticObject *static_object, UInventory *inv) {
    auto &minable = static_object->mMinable;
    if (minable.Minable) {
      UInventoryLibrary::Add(inv, { minable.Result, minable.Count });
    }
  }
}

void USectorProxyHolder::ClearBlockProps(IndexType index, bool doDrop /*= true*/) {
  const auto pos = cs::IndexToCell(index, gSectorSize);

  const auto out_inventory = NewObject<UAutosizeInventory>();

  for (int32 i = 0; i < Vec3i(3, 3, 3).Capacity(); ++i) {
    auto offset = cs::IndexToArea(i, Vec3i(-1), Vec3i(1));
    auto bpos = pos + offset;

    IndexType s_index = -1;
    if (auto sector = owner->Dim->FindBlockCell(bpos, s_index)) {
      if (doDrop) {
        auto props = owner->SectorPropComponent->Get(s_index);
        for (const auto &it : props) {
          if (auto prop = Cast<UStaticProp>(it.Key)) {
            for (const auto &tr : it.Value) {
              Drop(it.Key, out_inventory);
            }
          }
        }
      }

      sector->GetInstancingComponent()->DestroySmallInBlock(cs::WBtoWd(bpos), s_index);
    }
  }
  
  //TODO: Bad place for that
  if (!out_inventory->IsEmpty()) {
    auto mpc = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

    const auto mpcTransform = mpc->GetCharacter()->GetTransform();

    const auto dropped = GetWorld()->SpawnActorDeferred<ADroppedInventory>(ADroppedInventory::StaticClass(), mpcTransform, owner);
    if (ensure(dropped)) {
      UInventoryLibrary::TryMoveFromAny(dropped->GetInventory(), out_inventory);
      dropped->FinishSpawning(mpcTransform);
    }
  }
}