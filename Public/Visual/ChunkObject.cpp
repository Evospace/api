// #include "ChunkObject.h"
// #include "BlockRegionActor.h"
// #include "RealtimeMeshSimple.h"
// #include "Async/Async.h"
// #include "Evospace/World/Tesselator.h"
// #include "Public/StaticBlock.h"
// #include "Public/StaticProp.h"
//
// /*------------------------ life-cycle ----------------------------------*/
//
// void UChunkObject::Init(ABlockRegionActor* InOwner, const FChunkId& InId)
// {
//     Owner = InOwner;
//     Id    = InId;
//     SpawnRealtimeMesh();                 // создаём/берём из пула
//     AddToRender();                       // HISMC
//     bEnabled = true;
// }
//
// void UChunkObject::Shutdown()
// {
//     RemoveFromRender();
//     if (RMComponent)                      // вернём компонент в пул
//     {
//         Owner->ReleaseRealtimeMesh(RMComponent);
//         RMComponent = nullptr;
//     }
//     bEnabled = false;
// }
//
// /*--------------------- Static-блоки -----------------------------------*/
//
// void UChunkObject::SetStaticBlock(IndexType Idx, const UStaticBlock* Block)
// {
//     if (!Data.mStaticBlocks.IsValidIndex(Idx))
//         Data.mStaticBlocks.SetNumZeroed(kChunkSize.Size());
//     Data.mStaticBlocks[Idx].block = Block;
//     bDirty = true;
// }
//
// const UStaticBlock* UChunkObject::GetStaticBlock(IndexType Idx) const
// {
//     return Data.mStaticBlocks.IsValidIndex(Idx) ?
//            Data.mStaticBlocks[Idx].block : nullptr;
// }
//
// /*--------------------------- Render -----------------------------------*/
//
// void UChunkObject::SpawnRealtimeMesh()
// {
//     RMComponent = Owner->AcquireRealtimeMesh();
//     const FVector RelPos = Id.vec() * 1600.f;     // 16 м * 100 см
//     RMComponent->SetRelativeLocation(RelPos);
// }
//
// void UChunkObject::AddToRender()
// {
//     const FVector RegionBase = Id.vec() * 1600.f;
//
//     auto AddInst = [&](const FTileKey& Key, const FTransform& Xf)
//     {
//         auto* H = Owner->GetOrCreateHISMC(Key);
//         int32 Idx = H->AddInstance(Xf);
//         Instances.Add({Key, Idx});
//     };
//
//     /* 1. статические блоки */
//     for (int32 i = 0; i < Data.mStaticBlocks.Num(); ++i)
//     {
//         const UStaticBlock* SB = Data.mStaticBlocks[i].block;
//         if (!SB) continue;
//
//         const FIntVector Cell = cs::IndexToCell(i, kChunkSize);
//         FTileKey Key;
//         Key.TileX = (Id.X & 3);  Key.TileY = (Id.Y & 3);
//         //Key.Mesh  = SB->Mesh;
//         //Key.Mat   = SB->Material;
//
//         const FTransform Xf(FRotator::ZeroRotator,
//                             RegionBase + FVector(Cell) * 100.f + FVector(50.f),
//                             FVector::OneVector);
//         AddInst(Key, Xf);
//     }
//
//     /* 2. проп-аттачи */
//     for (auto& P : Data.mAttaches)
//     {
//         FTileKey Key;
//         Key.TileX = (Id.X & 3);  Key.TileY = (Id.Y & 3);
//         Key.Mesh  = P.Key->Mesh;
//         Key.Mat   = P.Key->Materials[0];
//
//         for (const FTransform3f& T : P.Value) {
//           auto t = FTransform(T);
//           t.AddToTranslation(RegionBase);
//           AddInst(Key,  t );
//         }
//     }
// }
//
// void UChunkObject::RemoveFromRender()
// {
//     for (auto& Ref : Instances)
//         if (auto H = Owner->HISMMap.FindRef(Ref.Key))
//             H->RemoveInstance(Ref.Index);
//     Instances.Reset();
// }
//
// /*-------------------- Mesh rebuild (RMC) ------------------------------*/
//
// bool UChunkObject::ApplyDataFromCompiler(UTesselator::Data&& InData, int32 Lod, TFunction<void()> Done)
// {
//   if (InData.IsEmpty())
//   {
//     if (const auto* Mesh = RMComponent ? RMComponent->GetRealtimeMeshAs<URealtimeMeshSimple>() : nullptr)
//     {
//       const_cast<URealtimeMeshSimple*>(Mesh)->Reset();
//     }
//     mCompiled     = true;
//     data   = {};          // сбрасываем
//     Callback      = {};
//     return true;
//   }
//
//   /* 1. Сохраняем входные данные -----------------------------------------*/
//   mCompiled   = false;
//   data = MoveTemp(InData);
//   Callback    = MoveTemp(Done);
//
//   /* 2. Если RMC ещё не создан — флажок на BuildMesh в TickBuildMesh ------*/
//   if (!RMComponent || RMComponent->GetRealtimeMeshAs<URealtimeMeshSimple>() == nullptr)
//   {
//     //bGeneratedMeshRebuildPending = true;   // опросит TickBuildMesh
//     bDirty = true;                         // параллельно пересчёт коллизии
//     return true;
//   }
//
//   UpdateRealtimeMesh();
//   return true;
// }
//
// int32 UChunkObject::GenerateSection(BuilderType &StaticProvider, const UTesselator::Data &data, int32 section_index, int32 lod_index, int32 precount) {
//   auto &index_data = data[section_index];
//
//   for (int i = 0; i < data[section_index].Vertices.Num(); ++i) {
//     StaticProvider.AddVertex(index_data.Vertices[i])
//       .SetNormalAndTangent(index_data.Normals[i], index_data.Tangents[i])
//       .SetTexCoord(FVector2DHalf(index_data.UV0[i].X, index_data.UV0[i].Y))
//       .SetColor(index_data.VertexColors[i]);
//   }
//
//   for (int i = 0; i < index_data.Triangles.Num(); i += 3) {
//     StaticProvider.AddTriangle(index_data.Triangles[i] + precount, index_data.Triangles[i + 1] + precount, index_data.Triangles[i + 2] + precount, section_index);
//   }
//
//   return data[section_index].Vertices.Num();
// }
//
// void UChunkObject::UpdateRealtimeMesh() {
//   auto groupKey = FRealtimeMeshSectionGroupKey::Create(0, FName("Surface"));
//   auto realtimeMesh = RMComponent->GetRealtimeMeshAs<URealtimeMeshSimple>();
//
//   RealtimeMesh::FRealtimeMeshStreamSet StreamSet;
//   BuilderType Builder(StreamSet);
//
//   Builder.EnableTangents();
//   Builder.EnableTexCoords();
//   Builder.EnableColors();
//   Builder.EnablePolyGroups();
//
//   int32 section_count = 0;
//   for (size_t i = 0; i < data.Num(); ++i) {
//     if (data[i].Vertices.Num() > 0) {
//       if (ensure(data[i].material)) {
//         section_count += GenerateSection(Builder, data, i, 0, section_count);
//         realtimeMesh->SetupMaterialSlot(i, data[i].material->GetFName(), data[i].material);
//       } else {
//         section_count += GenerateSection(Builder, data, i, 0, section_count);
//         realtimeMesh->SetupMaterialSlot(i, "UnloadedMaterial", nullptr);
//       }
//     }
//   }
//
//   if (!mCompiled) {
//     realtimeMesh->CreateSectionGroup(groupKey, StreamSet);
//   } else {
//     realtimeMesh->UpdateSectionGroup(groupKey, StreamSet);
//   }
//
//   for (size_t i = 0; i < data.Num(); ++i) {
//     const FRealtimeMeshSectionKey PolyGroupKey = FRealtimeMeshSectionKey::CreateForPolyGroup(groupKey, i);
//     realtimeMesh->UpdateSectionConfig(PolyGroupKey, FRealtimeMeshSectionConfig(i), true);
//   }
//
//   mCompiled = true;
//   data = {};
// }
//
// /*---------------------- Serialize voxel data --------------------------*/
//
// void UChunkObject::SerializeVoxelData(FArchive& Ar)
// {
//     Ar << Data.mStaticBlocks;
//     Ar << Data.mAttaches;
// }
//
// void UChunkObject::LoadFromColumn(const UColumn &column){
//   // StaticBlocks = mSectorDataLogic.mStaticBlocks;
//   //
//   // for (int i = 0; i < StaticBlocks.Num(); ++i) {
//   //   bool restored = false;
//   //   const auto bpos = cs::IndexToCell(i, gSectorSize) + mPivotPos;
//   //   auto logic = GetDimension()->GetBlockLogic(bpos);
//   //   auto &block = StaticBlocks[i].block;
//   //   if (block == nullptr && logic != nullptr) {
//   //     LOG(ERROR_LL) << "Sector desync at " << bpos << ". Logic " << logic->GetName() << ". Trying to fix";
//   //     block = logic->GetStaticBlock();
//   //     restored = true;
//   //   }
//   //   if (block != nullptr && block->mActorClass == nullptr && !block->NoActorRenderable) {
//   //     continue;
//   //   }
//   //   if (block == nullptr) {
//   //     continue;
//   //   }
//   //   if (logic) [[likely]] {
//   //     if (!logic->IsA(UPartBlockLogic::StaticClass())) {
//   //       auto staticBlock = logic->GetStaticBlock();
//   //       if (staticBlock->mActorClass || staticBlock->NoActorRenderable) {
//   //         FTransform transform(cs::WBtoWd(logic->GetBlockPos()) + gCubeSize / 2.0);
//   //         transform.SetRotation(logic->GetBlockQuat());
//   //         if (staticBlock->NoActorRenderable) {
//   //           logic->SetRenderable(this);
//   //         } else if (ABlockActor *actor = logic->GetStaticBlock()->SpawnActorAndLuaDeferred(GetDimension(), logic, transform)) {
//   //           Actors.Add(cs::CellToIndex(logic->GetBlockPos() - mPivotPos, gSectorSize), ActorsPair(logic, actor));
//   //           logic->SetActor(actor);
//   //           actor->FinishSpawning(transform);
//   //           logic->DeferredPaintApply();
//   //           actor->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
//   //         }
//   //
//   //         if (restored) [[unlikely]] {
//   //           for (auto &pos : block->Positions) {
//   //             auto subPos = bpos + RotateVector(logic->GetBlockQuat(), pos);
//   //             auto type = GetDimension()->GetBlockLogic(subPos);
//   //             int32 index;
//   //             if (auto sector = GetDimension()->FindBlockCell(subPos, index)) [[likely]] {
//   //               sector->SetStaticBlock(index, block);
//   //             } else [[unlikely]] {
//   //               LOG(ERROR_LL) << "fixing part of block is failed at " << pos;
//   //             }
//   //             if (type) {
//   //               LOG(WARN_LL) << type->GetName() << " at " << pos;
//   //             } else {
//   //               LOG(ERROR_LL) << "nullptr at " << pos;
//   //             }
//   //           }
//   //         }
//   //       }
//   //     }
//   //   } else [[unlikely]] {
//   //     LOG(ERROR_LL) << "Sector desync at " << bpos << ". Block class " << block->GetName() << " clearing this cell";
//   //     GetDimension()->SetBlockLogic(bpos, nullptr);
//   //     SetStaticBlock(i, nullptr);
//   //     SetBlockDesity(i, 0);
//   //   }
//   // }
//   //
//   // for (const auto &it : mSectorDataLogic.mAttaches)
//   //   for (const auto &tr : it.Value) {
//   //     auto bpos = cs::WtoWB(tr.GetLocation()) - mPivotPos;
//   //     it.Key->Create(this, static_cast<FTransform>(tr), bpos, true);
//   //     it.Key->OnSpawn(cs::WtoWB(tr.GetLocation()));
//   //   }
//   //
//   // mDirty = true;
//   // mEnabled = true;
// }