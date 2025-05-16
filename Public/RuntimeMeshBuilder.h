#pragma once
#include "RealtimeMeshComponent.h"
#include "RealtimeMeshSimple.h"
#include "Evospace/World/Tesselator.h"

struct RuntimeMeshBuilder {
  using MeshType = RealtimeMesh::TRealtimeMeshBuilderLocal<uint16, FPackedNormal, FVector2DHalf, 1>;

private:
  
  static int32 BuildSection(MeshType &StaticProvider, const UTesselator::Data &data, int32 section_index, int32 lod_index, int32 precount) {
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
  
public:
  
  static void BuildRealtimeMesh(URealtimeMeshSimple * rm, UTesselator::Data && data, bool & secrionGroup) {
    auto groupKey = FRealtimeMeshSectionGroupKey::Create(0, FName("Surface"));

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
          section_count += BuildSection(Builder, data, i, 0, section_count);
          rm->SetupMaterialSlot(i, data[i].material->GetFName(), data[i].material);
        } else {
          section_count += BuildSection(Builder, data, i, 0, section_count);
          rm->SetupMaterialSlot(i, "UnloadedMaterial", nullptr);
        }
      }
    }

    if (!secrionGroup) {
      rm->CreateSectionGroup(groupKey, StreamSet);
      secrionGroup = true;
    } else {
      rm->UpdateSectionGroup(groupKey, StreamSet);
    }

    for (size_t i = 0; i < data.Num(); ++i) {
      if (data[i].Triangles.Num() == 0) {
        LOG(ERROR_LL) << rm->GetName() << " contains empty section #" << i;
        continue;
      }
      const FRealtimeMeshSectionKey PolyGroupKey = FRealtimeMeshSectionKey::CreateForPolyGroup(groupKey, i);
      rm->UpdateSectionConfig(PolyGroupKey, FRealtimeMeshSectionConfig(i), true);
    }
  
    data = {};
  }
};
