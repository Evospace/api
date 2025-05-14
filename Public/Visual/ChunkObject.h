// #pragma once
//
// #include "CoreMinimal.h"
// #include "UObject/Object.h"
// #include "RealtimeMeshComponent.h"
// #include "TileKey.h"
// #include "Core/RealtimeMeshBuilder.h"
// #include "Evospace/World/BlockCell.h"
// #include "Evospace/World/Column.h"
// #include "Evospace/World/Tesselator.h"
// #include "ChunkObject.generated.h"
//
// class ADimension;
// class UBlockLogic;
// class USectorPropComponent;
// class UEmitterBatchComponent;
// class UChunkObject;
//
// /*----------- вспомогательные типы -------------------------------------*/
//
// using FChunkId = Vec3i;                      // (cx,cy,cz)  (16×16×16)
// using FRegionId = FIntPoint;
// using IndexType = int32;
// constexpr Vec3i kChunkSize = {16,16,16};
//
//
// struct FAdjacentChunks
// {
//   /* фиксированный массив 3×3×3 = 27 указателей */
//   UChunkObject* Data[27] = {nullptr};
//   int32         Count    = 0;                 // сколько непустых
//
//   /* индекс в массиве (-1…1) ³ → 0…26 */
//   static FORCEINLINE int32 Idx(const FIntVector& Off)
//   {   return (Off.Z+1)*9 + (Off.Y+1)*3 + (Off.X+1); }
//
//   /* получить/поставить элемент */
//   FORCEINLINE UChunkObject*& Element(const FIntVector& Off)
//   {   return Data[Idx(Off)]; }
//
//   /* обновить со счётом */
//   void Set(const FIntVector& Off, UChunkObject* Ptr)
//   {
//     Count -= (Element(Off) != nullptr);
//     Element(Off) = Ptr;
//     Count += (Ptr != nullptr);
//   }
// };
//
// UCLASS()
// class UChunkObject : public UObject
// {
//     GENERATED_BODY()
//
// public:
//     /* ---------- создание / уничтожение -------------------------------- */
//     void Init(class ABlockRegionActor* InOwner, const FChunkId& InId);
//     void Shutdown();
//
//     /* ---------- доступ к данным блока --------------------------------- */
//     FORCEINLINE const FChunkId& GetId() const     { return Id; }
//     FORCEINLINE FSectorData&       GetData()      { return Data; }
//     FORCEINLINE const FSectorData& GetData() const{ return Data; }
//
//     /* ---------- логику из старого ASector ----------------------------- */
//     ASector*    /*старое API*/ FindBlockCell(const FIntVector& Pos, IndexType& Out);
//     void        SetStaticBlock(IndexType Idx, const UStaticBlock* Block);
//     const UStaticBlock* GetStaticBlock(IndexType Idx) const;
//
//
//     FORCEINLINE void MarkDirty()       { bDirty.store(true); }
//     FORCEINLINE bool IsDirty()  const  { return bDirty.load(); }
//
//   bool mCompiled = false;
//   UTesselator::Data data;
//   TFunction<void()> Callback;
//
//     using BuilderType = RealtimeMesh::TRealtimeMeshBuilderLocal<uint16, FPackedNormal, FVector2DHalf, 1>;
//
//   static int32 GenerateSection(BuilderType &StaticProvider, const UTesselator::Data &data, int32 section_index, int32 lod_index, int32 precount);
//
//
//     /* ---------- рендер ------------------------------------------------- */
//     void AddToRender();          // кладёт инстансы в HISMC
//     void RemoveFromRender();     // RemoveInstance()
//
//     void UpdateRealtimeMesh();
//     /* ---------- сериализация ------------------------------------------ */
//     void SerializeVoxelData(FArchive& Ar);
//
//   void LoadFromColumn(const UColumn & column);
//   void UnloadSector();
//     /* по желанию: custom Save/Load */
//
// private:
//     /* ---------- helpers ------------------------------------------------ */
//     void SpawnRealtimeMesh();    // берёт из пула region-актора
//     bool ApplyDataFromCompiler(UTesselator::Data&& InData, int32 Lod, TFunction<void()> Done);
//
// private:
//     /* ---------- state -------------------------------------------------- */
//     friend class ABlockRegionActor;
//
//     /* идентификатор чанка */
//     FChunkId Id;
//
//     /* ссылка на хозяина-регион */
//     TObjectPtr<class ABlockRegionActor> Owner = nullptr;
//
//     /* данные сектора (стат. блоки + пропы) */
//     FSectorData Data;
//
//     UPROPERTY()
//     TObjectPtr<URealtimeMeshComponent> RMComponent = nullptr; // компонент геометрии
//
//     /* кеш инстансов HISMC, чтобы удалить при выгрузке */
//     struct FInstRef { FTileKey Key; int32 Index; };
//     TArray<FInstRef> Instances;
//
//     /* флаги */
//     std::atomic<bool> bDirty   { true };    // пересобрать меш
//     bool              bEnabled { false };   // в данный момент в WorldPartition
//
//
//   FAdjacentChunks    Adjacent;
//
// public:
//   FORCEINLINE FAdjacentChunks&       GetAdjacent()       { return Adjacent; }
//   FORCEINLINE const FAdjacentChunks& GetAdjacent() const { return Adjacent; }
// };