// #pragma once
//
// #include "CoreMinimal.h"
// #include "ChunkObject.h"
// #include "GameFramework/Actor.h"
// #include "Components/HierarchicalInstancedStaticMeshComponent.h"
// #include "RealtimeMeshComponent.h"
// #include "TileKey.h"
// #include "BlockRegionActor.generated.h"
//
// FORCEINLINE uint32 GetTypeHash(const FTileKey& K)
// {
// 	uint32 H = HashCombineFast(GetTypeHash(K.TileX), GetTypeHash(K.TileY));
// 	H = HashCombineFast(H, GetTypeHash(K.Mesh));
// 	return HashCombineFast(H, GetTypeHash(K.Mat));
// }
//
// class UChunkObject;
//
// UCLASS()
// class EVOSPACE_API ABlockRegionActor : public AActor
// {
// 	GENERATED_BODY()
//
// public:
// 	ABlockRegionActor();
//
// 	/* --------------------------- region identity -------------------------- */
//
// 	FRegionId RegionId = FRegionId::ZeroValue;      ///< (rx, ry) 256 м сетка
//
// 	void InitRegion(FRegionId InId);                ///< вызывает спавнер
//
// 	/* ------------------------ streaming entry points ---------------------- */
//
// 	/** Активировать чанк: добавить блоки в рендер. */
// 	void EnableChunk(const FChunkId& Cx);
//
// 	/** Деактивировать чанк: убрать блоки из рендера. */
// 	void DisableChunk(const FChunkId& Cx);
//
// 	/* ----------------------------- tick build ----------------------------- */
//
// 	virtual void Tick(float DeltaSeconds) override;
//
// protected:
// 	virtual void BeginPlay() override;
//
// public:
// 	/* --------------------------- helper methods -------------------------- */
//
// 	/** Вернуть (или создать) HISMC по ключу. */
// 	UHierarchicalInstancedStaticMeshComponent*
// 	GetOrCreateHISMC(const FTileKey& Key);
//
// 	/** Спавнит компонент RealtimeMesh или берёт из пула. */
// 	URealtimeMeshComponent* AcquireRealtimeMesh();
//
// 	/** Возвращает компонент в пул. */
// 	void ReleaseRealtimeMesh(URealtimeMeshComponent* Cmp);
//
// public:
// 	/* ---------------------------- data blocks ---------------------------- */
//
// 	/** Чанки: ключ — `FChunkId` (cx,cy,cz). */
// 	UPROPERTY()
// 	TMap<FVector3i, UChunkObject *> ChunkObjects;
//
// 	/** HISMC: ключ — `FTileKey` (плитка 64 м × mesh × mat). */
// 	UPROPERTY(Transient)
// 	TMap<FTileKey, TObjectPtr<UHierarchicalInstancedStaticMeshComponent>> HISMMap;
//
// 	/** Пул свободных URealtimeMeshComponent'ов. */
// 	UPROPERTY(Transient)
// 	TArray<TObjectPtr<URealtimeMeshComponent>> RMCPool;
// };