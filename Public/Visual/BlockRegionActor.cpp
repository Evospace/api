// #include "BlockRegionActor.h"
// #include "Async/Async.h"
// #include "Evospace/Ensure.h"
//
// /*-------------------------------- Constructor -----------------------------*/
//
// ABlockRegionActor::ABlockRegionActor()
// {
// 	PrimaryActorTick.bCanEverTick = true;
//
// 	// Корневой компонент – пустой сц. компонент (можно и DefaultSceneRoot).
// 	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("RegionRoot"));
// 	SetRootComponent(Root);
// }
//
// /*-------------------------------- BeginPlay ------------------------------*/
//
// void ABlockRegionActor::BeginPlay()
// {
// 	Super::BeginPlay();
// }
//
// /*-------------------------------- InitRegion -----------------------------*/
//
// void ABlockRegionActor::InitRegion(FRegionId InId)
// {
// 	RegionId = InId;
//
// 	const float Side = 256.f * 100.f;                     // 256 м → см
// 	SetActorLocation(FVector(InId.X * Side, InId.Y * Side, 0.f));
// }
//
// /*-------------------------------- EnableChunk ----------------------------*/
//
// void ABlockRegionActor::EnableChunk(const FChunkId& Cx)
// {
// 	if (ChunkObjects.Contains(Cx))
// 		return;                       // уже активен
//
// 	// создаём пустой чанк и RealtimeMeshComponent
// 	UChunkObject* Chunk = NewObject<UChunkObject>(this);
// 	Chunk->Id = Cx;
// 	SpawnChunkMesh(Chunk);
//
// 	// размещаем блоки -> HISMC
// 	Chunk->AddToRender(this);
// 	ChunkObjects.Add(Cx, Chunk);
// }
//
// /*-------------------------------- DisableChunk ---------------------------*/
//
// void ABlockRegionActor::DisableChunk(const FChunkId& Cx)
// {
// 	UChunkObject* Chunk = nullptr;
// 	if (!ChunkObjects.RemoveAndCopyValue(Cx, Chunk))
// 		return;
//
// 	// убираем блоки из HISMC
// 	Chunk->RemoveFromRender(this);
//
// 	// возвращаем RM component в пул
// 	if (Chunk->MeshCmp)
// 		ReleaseRealtimeMesh(Chunk->MeshCmp);
// }
//
// /*-------------------------------- Tick -----------------------------------*/
//
// void ABlockRegionActor::Tick(float Delta)
// {
// 	Super::Tick(Delta);
//
// 	// // Строим грязные чанки
// 	// for (auto& Elem : ChunkObjects)
// 	// 	Elem.Value->TickBuildMesh(this);
// }
//
// /*----------------------- GetOrCreateHISMC --------------------------------*/
//
// UHierarchicalInstancedStaticMeshComponent*
// ABlockRegionActor::GetOrCreateHISMC(const FTileKey& Key)
// {
// 	if (auto Found = HISMMap.FindRef(Key))
// 		return Found;
//
// 	// Создаём новый компонент
// 	const FString Name = FString::Printf(TEXT("HISMC_%d_%d_%s"),
// 	                                     Key.TileX, Key.TileY,
// 	                                     *Key.Mesh->GetName());
// 	auto* C = NewObject<UHierarchicalInstancedStaticMeshComponent>(this, *Name);
// 	C->SetStaticMesh(Key.Mesh);
// 	C->SetMaterial(0, Key.Mat);
// 	C->SetMobility(EComponentMobility::Static);
// 	C->SetupAttachment(GetRootComponent());
// 	C->RegisterComponent();
//
// 	HISMMap.Add(Key, C);
// 	return C;
// }
//
// /*----------------------- RM Component pool -------------------------------*/
//
// URealtimeMeshComponent* ABlockRegionActor::AcquireRealtimeMesh()
// {
// 	if (RMCPool.Num())
// 		return RMCPool.Pop(false);
//
// 	auto* Cmp = NewObject<URealtimeMeshComponent>(this);
// 	Cmp->SetMobility(EComponentMobility::Static);
// 	Cmp->SetupAttachment(GetRootComponent());
// 	Cmp->RegisterComponent();
// 	return Cmp;
// }
//
// void ABlockRegionActor::ReleaseRealtimeMesh(URealtimeMeshComponent* Cmp)
// {
// 	if (!Cmp) return;
// 	Cmp->UnregisterComponent();
// 	RMCPool.Add(Cmp);
// }
//
// /*---------------------------- SpawnChunkMesh -----------------------------*/
//
// void ABlockRegionActor::SpawnChunkMesh(UChunkObject* Chunk)
// {
// 	check(Chunk);
//
// 	URealtimeMeshComponent* Cmp = AcquireRealtimeMesh();
// 	const FVector RelPos(Chunk->Id.X * 1600.f,  // 16 м * 100
// 	                     Chunk->Id.Y * 1600.f,
// 	                     Chunk->Id.Z * 1600.f);
// 	Cmp->SetRelativeLocation(RelPos);
//
// 	Chunk->RMComponent = Cmp;
// }