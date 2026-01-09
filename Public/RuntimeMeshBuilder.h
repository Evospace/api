#pragma once
#include "RealtimeMeshComponent.h"
#include "RealtimeMeshSimple.h"
#include "Data/RealtimeMeshUpdateBuilder.h"
#include "Evospace/World/Tesselator.h"

struct RuntimeMeshBuilder {
  using MeshType = RealtimeMesh::TRealtimeMeshBuilderLocal<uint32, FPackedNormal, FVector2DHalf, 1>;

  private:
  static int32 BuildSection(MeshType &StaticProvider, const UTesselator::Data &data, int32 section_index,
                            int32 precount) {
    auto &index_data = data[section_index];

    for (int i = 0; i < data[section_index].Vertices.Num(); ++i) {
      StaticProvider.AddVertex(index_data.Vertices[i])
        .SetNormalAndTangent(index_data.Normals[i], index_data.Tangents[i])
        .SetTexCoord(FVector2DHalf(index_data.UV0[i].X, index_data.UV0[i].Y))
        .SetColor(index_data.VertexColors[i]);
    }

    for (int i = 0; i < index_data.Triangles.Num(); i += 3) {
      StaticProvider.AddTriangle(index_data.Triangles[i] + precount, index_data.Triangles[i + 1] + precount, index_data.Triangles[i + 2] + precount, section_index);
    }

    return data[section_index].Vertices.Num();
  }

  static FRealtimeMeshSectionGroupKey MakeSurfaceGroupKey(int32 LodIndex) {
    static const FName SurfaceName(TEXT("Surface"));
    return FRealtimeMeshSectionGroupKey::Create(FRealtimeMeshLODKey(LodIndex), SurfaceName);
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

    int32 section_count = 0;
    for (size_t i = 0; i < data.Num(); ++i) {
      if (data[i].Vertices.Num() > 0) {
        if (ensure(data[i].material)) {
          section_count += BuildSection(Builder, data, i, section_count);
          const FString SlotNameString =
            data[i].material->GetFName().ToString() + (data[i].SmoothNormals ? TEXT("_Smooth") : TEXT("_Flat"));
          rm->SetupMaterialSlot(i, FName(*SlotNameString), data[i].material);
        } else {
          section_count += BuildSection(Builder, data, i, section_count);
          const FString SlotNameString =
            FString(TEXT("UnloadedMaterial")) + (data[i].SmoothNormals ? TEXT("_Smooth") : TEXT("_Flat"));
          rm->SetupMaterialSlot(i, FName(*SlotNameString), nullptr);
        }
      }
    }

    // IMPORTANT:
    // - Creating/updating section-group streams and updating section configs must be done in a single commit.
    //   Otherwise, UpdateSectionConfig may run before the poly-group sections are created and triggers
    //   `ensure(Section.IsValid())` inside RealtimeMeshComponent.
    RealtimeMesh::FRealtimeMeshUpdateBuilder UpdateBuilder;

    UpdateBuilder.AddLODTask<RealtimeMesh::FRealtimeMeshLODSimple>(
      groupKey.LOD(),
      [groupKey](RealtimeMesh::FRealtimeMeshUpdateContext &UpdateContext, RealtimeMesh::FRealtimeMeshLODSimple &LOD) {
        LOD.CreateOrUpdateSectionGroup(UpdateContext, groupKey, FRealtimeMeshSectionGroupConfig());
      });

    // Copy once; SectionGroup::SetAllStreams expects an rvalue in this path.
    RealtimeMesh::FRealtimeMeshStreamSet StreamSetCopy(StreamSet);
    UpdateBuilder.AddSectionGroupTask<RealtimeMesh::FRealtimeMeshSectionGroupSimple>(
      groupKey,
      [StreamSetCopy = MoveTemp(StreamSetCopy)](RealtimeMesh::FRealtimeMeshUpdateContext &UpdateContext,
                                               RealtimeMesh::FRealtimeMeshSectionGroupSimple &SectionGroup) mutable {
        SectionGroup.SetShouldAutoCreateSectionsForPolyGroups(UpdateContext, true);
        SectionGroup.SetAllStreams(UpdateContext, MoveTemp(StreamSetCopy));
      });

    for (size_t i = 0; i < data.Num(); ++i) {
      if (data[i].Triangles.Num() == 0) {
        continue;
      }

      const FRealtimeMeshSectionKey PolyGroupKey = FRealtimeMeshSectionKey::CreateForPolyGroup(groupKey, i);
      const FRealtimeMeshSectionConfig Config(static_cast<int32>(i));

      UpdateBuilder.AddSectionTask<RealtimeMesh::FRealtimeMeshSectionSimple>(
        PolyGroupKey,
        [Config](RealtimeMesh::FRealtimeMeshUpdateContext &UpdateContext, RealtimeMesh::FRealtimeMeshSectionSimple &Section) {
          Section.UpdateConfig(UpdateContext, Config);
          Section.SetShouldCreateCollision(UpdateContext, true);
        });
    }

    UpdateBuilder.Commit(rm->GetMeshData());

    data = {};
  }
};
