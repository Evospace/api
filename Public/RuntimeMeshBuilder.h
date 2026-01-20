#pragma once
#include "RealtimeMeshComponent.h"
#include "RealtimeMeshSimple.h"
#include "Data/RealtimeMeshUpdateBuilder.h"
#include "Evospace/World/Tesselator.h"

struct RuntimeMeshBuilder {
  using MeshType = RealtimeMesh::TRealtimeMeshBuilderLocal<uint32, FPackedNormal, FVector2DHalf, 1>;

  private:
  static int32 BuildSection(MeshType &StaticProvider, const UTesselator::MeshData &SectionData, int32 SectionIndex,
                            int32 VertexBase) {
    if (SectionData.Normals.Num() != SectionData.Vertices.Num()) {
      LOG(ERROR_LL) << "RuntimeMeshBuilder: Normals mismatch: Vertices=" << SectionData.Vertices.Num()
                    << " Normals=" << SectionData.Normals.Num()
                    << " Section=" << SectionIndex;
      return 0;
    }
    if (SectionData.Tangents.Num() != SectionData.Vertices.Num()) {
      LOG(ERROR_LL) << "RuntimeMeshBuilder: Tangents mismatch: Vertices=" << SectionData.Vertices.Num()
                    << " Tangents=" << SectionData.Tangents.Num()
                    << " Section=" << SectionIndex;
      return 0;
    }
    if (SectionData.UV0.Num() != SectionData.Vertices.Num()) {
      LOG(ERROR_LL) << "RuntimeMeshBuilder: UV0 mismatch: Vertices=" << SectionData.Vertices.Num()
                    << " UV0=" << SectionData.UV0.Num()
                    << " Section=" << SectionIndex;
      return 0;
    }
    if (SectionData.VertexColors.Num() != SectionData.Vertices.Num()) {
      LOG(ERROR_LL) << "RuntimeMeshBuilder: VertexColors mismatch: Vertices=" << SectionData.Vertices.Num()
                    << " Colors=" << SectionData.VertexColors.Num()
                    << " Section=" << SectionIndex;
      return 0;
    }
    if (SectionData.Triangles.Num() % 3 != 0) {
      LOG(ERROR_LL) << "RuntimeMeshBuilder: Triangles not multiple of 3: Triangles=" << SectionData.Triangles.Num()
                    << " Section=" << SectionIndex;
      return 0;
    }

    for (int32 i = 0; i < SectionData.Vertices.Num(); ++i) {
      StaticProvider.AddVertex(SectionData.Vertices[i])
        .SetNormalAndTangent(SectionData.Normals[i], SectionData.Tangents[i])
        .SetTexCoord(FVector2DHalf(SectionData.UV0[i].X, SectionData.UV0[i].Y))
        .SetColor(SectionData.VertexColors[i]);
    }

    for (int32 i = 0; i < SectionData.Triangles.Num(); i += 3) {
      StaticProvider.AddTriangle(
        SectionData.Triangles[i] + VertexBase,
        SectionData.Triangles[i + 1] + VertexBase,
        SectionData.Triangles[i + 2] + VertexBase,
        SectionIndex);
    }

    return SectionData.Vertices.Num();
  }

  static FRealtimeMeshSectionGroupKey MakeSurfaceGroupKey(int32 LodIndex) {
    static const FName SurfaceName(TEXT("Surface"));
    return FRealtimeMeshSectionGroupKey::Create(FRealtimeMeshLODKey(LodIndex), SurfaceName);
  }

  static bool HasSectionGroup(const URealtimeMeshSimple *Mesh, const FRealtimeMeshSectionGroupKey &GroupKey) {
    if (!Mesh) {
      return false;
    }

    const TArray<FRealtimeMeshSectionGroupKey> SectionGroups = Mesh->GetSectionGroups(GroupKey.LOD());
    return SectionGroups.Contains(GroupKey);
  }

  static void EnsureLodExists(URealtimeMeshSimple *Mesh, int32 LodIndex) {
    if (!Mesh || LodIndex <= 0) {
      return;
    }

    bool bHasLod = false;
    for (const FRealtimeMeshLODKey &Key : Mesh->GetLODs()) {
      if (Key.Index() == LodIndex) {
        bHasLod = true;
        break;
      }
    }

    if (!bHasLod) {
      float ScreenSize = FMath::Pow(0.5f, LodIndex);
      if (LodIndex == 1) {
        ScreenSize = 0.7f;
      } else if (LodIndex == 2) {
        ScreenSize = 0.3f;
      }
      Mesh->AddLOD(FRealtimeMeshLODConfig(ScreenSize));
    }
  }

  public:
  static void BuildRealtimeMesh(URealtimeMeshSimple *rm, UTesselator::Data &&data, int32 LodIndex) {
    if (!rm) {
      data = {};
      return;
    }

    EnsureLodExists(rm, LodIndex);
    const FRealtimeMeshSectionGroupKey groupKey = MakeSurfaceGroupKey(LodIndex);
    RealtimeMesh::FRealtimeMeshStreamSet StreamSet;
    MeshType Builder(StreamSet);

    Builder.EnableTangents();
    Builder.EnableTexCoords();
    Builder.EnableColors();
    Builder.EnablePolyGroups();

    const bool bUpdateMaterialSlots = (LodIndex == 0);

    TSet<int32> ActivePolyGroups;
    TMap<int32, int32> PolyGroupToMaterialSlot;
    int32 VertexBase = 0;
    for (int32 SectionIndex = 0; SectionIndex < data.Num(); ++SectionIndex) {
      const auto &SectionData = data[SectionIndex];
      if (SectionData.Vertices.Num() == 0) {
        continue;
      }

      if (bUpdateMaterialSlots) {
        if (SectionData.material) {
          // Use full object path as slot name to avoid FName collisions between different packages.
          rm->SetupMaterialSlot(SectionIndex, FName(*SectionData.material->GetPathName()), SectionData.material);
        } else {
          rm->SetupMaterialSlot(SectionIndex, "UnloadedMaterial", nullptr);
        }
      }

      PolyGroupToMaterialSlot.Add(SectionIndex, SectionIndex);
      const int32 AddedVertexCount = BuildSection(Builder, SectionData, SectionIndex, VertexBase);
      if (AddedVertexCount > 0 && SectionData.Triangles.Num() > 0) {
        ActivePolyGroups.Add(SectionIndex);
      }
      VertexBase += AddedVertexCount;
    }

    RealtimeMesh::FRealtimeMeshUpdateBuilder UpdateBuilder;

    const bool bHasSectionGroup = HasSectionGroup(rm, groupKey);
    if (!bHasSectionGroup) {
      UpdateBuilder.AddLODTask<RealtimeMesh::FRealtimeMeshLODSimple>(groupKey,
                                                                     [groupKey](RealtimeMesh::FRealtimeMeshUpdateContext &UpdateContext, RealtimeMesh::FRealtimeMeshLODSimple &LOD) {
                                                                       LOD.CreateOrUpdateSectionGroup(UpdateContext, groupKey, FRealtimeMeshSectionGroupConfig());
                                                                     });
    }

    UpdateBuilder.AddSectionGroupTask<RealtimeMesh::FRealtimeMeshSectionGroupSimple>(groupKey,
                                                                                     [PolyGroupToMaterialSlot, StreamSet = MoveTemp(StreamSet)](RealtimeMesh::FRealtimeMeshUpdateContext &UpdateContext,
                                                                                                                                                RealtimeMesh::FRealtimeMeshSectionGroupSimple &SectionGroup) mutable {
                                                                                       SectionGroup.SetShouldAutoCreateSectionsForPolyGroups(UpdateContext, true);
                                                                                       SectionGroup.SetPolyGroupSectionHandler(
                                                                                         UpdateContext,
                                                                                         RealtimeMesh::FRealtimeMeshPolyGroupConfigHandler::CreateLambda(
                                                                                           [PolyGroupToMaterialSlot](int32 PolyGroupIndex) {
                                                                                             if (const int32 *SlotIndex = PolyGroupToMaterialSlot.Find(PolyGroupIndex)) {
                                                                                               return FRealtimeMeshSectionConfig(*SlotIndex);
                                                                                             }
                                                                                             return FRealtimeMeshSectionConfig(PolyGroupIndex);
                                                                                           }));
                                                                                       SectionGroup.SetAllStreams(UpdateContext, MoveTemp(StreamSet));
                                                                                     });

    UpdateBuilder.AddSectionGroupTask<RealtimeMesh::FRealtimeMeshSectionGroupSimple>(groupKey,
                                                                                     [ActivePolyGroups = MoveTemp(ActivePolyGroups), PolyGroupToMaterialSlot, MeshName = rm->GetName()](RealtimeMesh::FRealtimeMeshUpdateContext &UpdateContext,
                                                                                                                                                                                        RealtimeMesh::FRealtimeMeshSectionGroupSimple &SectionGroup) mutable {
                                                                                       for (const int32 PolyGroupIndex : ActivePolyGroups) {
                                                                                         const FRealtimeMeshSectionKey SectionKey = FRealtimeMeshSectionKey::CreateForPolyGroup(SectionGroup.GetKey(UpdateContext), PolyGroupIndex);
                                                                                         if (const auto Section = SectionGroup.GetSectionAs<RealtimeMesh::FRealtimeMeshSectionSimple>(UpdateContext, SectionKey)) {
                                                                                           const int32 MaterialSlot =
                                                                                             PolyGroupToMaterialSlot.Contains(PolyGroupIndex)
                                                                                               ? PolyGroupToMaterialSlot[PolyGroupIndex]
                                                                                               : PolyGroupIndex;
                                                                                           const FRealtimeMeshSectionConfig SectionConfig(MaterialSlot);
                                                                                           Section->UpdateConfig(UpdateContext, SectionConfig);
                                                                                           Section->SetShouldCreateCollision(UpdateContext, true);
                                                                                         } else {
                                                                                           // This should not happen if poly-group streams are valid, but avoid hard ensures in the plugin.
                                                                                           LOG(ERROR_LL) << "RuntimeMeshBuilder: missing poly-group section " << SectionKey.ToString()
                                                                                                         << " Mesh=" << MeshName;
                                                                                         }
                                                                                       }
                                                                                     });

    UpdateBuilder.Commit(rm->GetMeshData());

    data = {};
  }
};
