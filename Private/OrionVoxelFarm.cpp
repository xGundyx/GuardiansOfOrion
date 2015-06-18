#include "Orion.h"
#include "OrionVoxelFarm.h"

CMaterialLibrary materialLibrary;
CArchitectureManager architectureManager;
CSimplexWorld* simplexWorld;
CSimplexWorldTerrainPage simplexPages(simplexWorld);
CInstanceSimplexWorld instanceManagerSimplex(simplexWorld, &architectureManager);
CGenerator worldGenerator;
CBlockData blockData;
CHeightmapWaterLayer waterLayer(&simplexPages, 63, 16000.0, 47000.0); 
CClipmapView clipmapView(&materialLibrary, &worldGenerator, &blockData);
ContourThreadContext* threadContext;// = VF_NEW VoxelFarm::ContourThreadContext();
CCellData::ThreadContext* threadContextCellData;// = VF_NEW VoxelFarm::CCellData::ThreadContext();
IO::CVoxelDB voxelDB;
//BlockIOHandler blockIO;

#define VOXELSCALE 20.0f
#define VOXELLOD 2

AOrionVoxelFarm::AOrionVoxelFarm(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AOrionVoxelFarm::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//update the view to match the player's view
	if (VoxFarm)//GEngine && GEngine->GetFirstLocalPlayerController(GetWorld()))
	{
		FVector pos = GetActorLocation();
		FVector dir;

		//if (GEngine)
		//	pos = GEngine->GetFirstLocalPlayerController(GetWorld())->PlayerCameraManager->GetCameraLocation();

		if (GEditor)
		{
			FEditorViewportClient* client = (FEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();
			pos = client->GetViewLocation();
			dir = client->GetViewRotation().Vector();
		}

		////UE_LOG(LogTemp, Warning, TEXT("ClipMapPos %f %f %f"), pos.X, pos.Y, pos.Z);

		VoxFarm->SetClipmapPos(FVector(pos.X, pos.Z, -pos.Y) / VOXELSCALE);

		dir = pos + dir*500;

		pos /= VOXELSCALE;
		dir /= VOXELSCALE;

		VoxFarm->SetLOS(pos.X, pos.Z, -pos.Y, dir.X, dir.Z, -dir.Y);

		//pos = GetActorLocation();

		bool bRet = false;

		if (ProcessCells())
			bRet = true;

		if (ProcessSeams())
			bRet = true;

		CleanupCells();

		if (bRet)
			return;

		bool NewScene = false;
		VoxFarm->IsSceneNew(&NewScene);

		if (NewScene)
		{
			int32 cellCount = 0;
			__int64 *sceneCellsPtr = NULL;

			VoxFarm->SceneCells(&cellCount, &sceneCellsPtr);

			if (cellCount > 0)
			{
				//__int64 *sceneCells = new __int64(cellCount);
				TArray<int32> VisibleItems;

				for (int32 c = 0; c < cellCount; ++c)
				{
					FProceduralMeshItem tItem;
					tItem.CellData = sceneCellsPtr[c];
					int32 index = MeshArray.Find(tItem);
					if (index != INDEX_NONE)
					{
						MeshArray[index].Comp->SetHiddenInGame(false);
						VisibleItems.Add(index);
						//showCell(sceneCells[c]);
						//tmp_visibleCells.Add(sceneCells[c]);
					}
					// a cell is in the scene but not in unity. we came across an empty cell.
				}

				for (int32 i = 0; i < MeshArray.Num(); i++)
				{
					if (VisibleItems.Find(i) == INDEX_NONE)
						MeshArray[i].Comp->SetHiddenInGame(true);
				}

				VisibleItems.Empty();

				//foreach(KeyValuePair<Int64, GameObject> entry in cells_)
				//	if (!tmp_visibleCells.Contains(entry.Key))
				//		hideCell(entry.Key);

				//tmp_visibleCells.Clear();

				//Resources.UnloadUnusedAssets();
			}
		}
		VoxFarm->Idle();
		//CleanupSeams();
	}
}

void AOrionVoxelFarm::CleanupCells()
{
	__int64 CellID = 0;
	__int64 CellData = 0;

	if (VoxFarm)
	{
		VoxFarm->GetNextCellToDestroy(&CellID, &CellData);

		while (CellData != 0)
		{
			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;
			float scale = 1.0f;

			VoxFarm->CellTransform(&x, &y, &z, &scale, CellData);

			if (RemoveCell(CellData))
			{
				VoxFarm->NotifyCellDestroyed(CellID, CellData);
				CellID = 0;
				CellData = 0;
				VoxFarm->GetNextCellToDestroy(&CellID, &CellData);
			}
			else
				CellData = 0;
		}
	}
}

bool AOrionVoxelFarm::RemoveCell(__int64 CellData)
{
	bool bRet = false;

	for (auto iter(MeshArray.CreateIterator()); iter; iter++)
	//for (int32 i = 0; i < MeshArray.Num(); i++)
	{
		FProceduralMeshItem Mesh = *iter;

		if (Mesh.CellData == CellData)
		{
			MeshArray.RemoveSingle(*iter);

			Mesh.Comp->ClearAllMeshSections();
			//Mesh.Comp->DestroyComponent();
			Mesh.Comp = NULL;
			Mesh.CellData = 0;

			bRet = true;
		}
	}

	return bRet;
}

void AOrionVoxelFarm::AddCell(TArray<FVector> Vertices, TArray<FVector> Normals, TArray<int32> Indices, TArray<FColor> Colors, TArray<FVector2D> UVs, __int64 CellData)
{
	FProceduralMeshItem NewCell;
	FProceduralHelper NewComp;

	NewCell.CellData = CellData;

	//FString fStr = FString("ProcMesh %lld", (long long)CellData);
	//NewComp = ConstructObject<UProceduralMeshComponent>(UProceduralMeshComponent::StaticClass(), this, FName(*fStr));

	//if (NewComp)
	{
		//TArray<FVector2D> UV0;
		//TArray<FColor> VertexColors;
		//TArray<FProcMeshTangent> Tangents;
		//NewComp->CreateMeshSection(0, Vertices, Indices, Normals, UV0, VertexColors, Tangents, false);
		EventAddMesh(Vertices, Normals, Indices, Colors, UVs, NewComp);
		NewCell.Comp = NewComp.Comp;
		MeshArray.Add(NewCell);
	}
}

void AOrionVoxelFarm::CleanupSeams()
{
}

bool AOrionVoxelFarm::ProcessCells()
{
	__int64 CellID = 0;
	__int64 CellData = 0;
	bool bImmediate;
	bool bRet = false;

	//get cell
	VoxFarm->GetNextCellToBake(&CellID, &CellData, &bImmediate);

	while (CellData != 0)
	{
		//do stuff to it
		int VertexCount;
		float *Vertices = NULL;
		float *Normals = NULL;
		float *FaceNormals = NULL;
		unsigned int *MaterialsA = NULL;
		unsigned int *MaterialsB = NULL;

		VoxFarm->GetCellGeometry(&VertexCount, &Vertices, &Normals, &FaceNormals, &MaterialsA, &MaterialsB, CellData);

		//unpack the id so we can use it properly
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;

		//now that we have some data, create some meshes with it!
		TArray<FVector> VertexBuffer;
		TArray<FVector> NormalBuffer;
		TArray<int32> IndexBuffer;
		TArray<FColor> ColorBuffer;
		TArray<FVector2D> UVBuffer;
		
		float scale = 1.0f;
		
		VoxFarm->CellTransform(&x, &y, &z, &scale, CellData);

		//int32 MeshIndex = int32(x / scale) 
		//	+ int32(y / scale) * 1024 
		//	+ int32(z / scale) * 1024 * 1024;

		for (int32 i = 0; i < VertexCount * 3; i += 3)
		{
			FVector vert(Vertices[i], -Vertices[i + 2], Vertices[i + 1]);

			vert *= scale;
			vert += FVector(x, -z, y);
			vert *= VOXELSCALE;

			VertexBuffer.Add(vert);

			FVector norm(Normals[i], -Normals[i + 2], Normals[i + 1]);
			NormalBuffer.Add(norm);

			IndexBuffer.Add(i);
			IndexBuffer.Add(i + 2);
			IndexBuffer.Add(i + 1);

			//materials
			byte *bPtrA = new byte[VertexCount * 4];
			memcpy(bPtrA, MaterialsA, sizeof(byte) * VertexCount * 4);

			byte *bPtrB = new byte[VertexCount * 4];
			memcpy(bPtrB, MaterialsB, sizeof(byte) * VertexCount * 4);

			int16 matId1 = bPtrA[i / 3 * 4];
			int16 matId2 = bPtrA[i / 3 * 4 + 2];
			int16 matId3 = bPtrB[i / 3 * 4];

			float div = 1;// materialCount;
			FColor col(matId1, matId2, matId3, 255);
			ColorBuffer.Add(col);

			UVBuffer.Add(FVector2D((float)bPtrB[i / 3 * 4 + 2] / 255, (float)bPtrB[i / 3 * 4 + 3] / 255));

			delete bPtrA;
			delete bPtrB;
		}

		AddCell(VertexBuffer, NormalBuffer, IndexBuffer, ColorBuffer, UVBuffer, CellData);

		//release cell
		VoxFarm->NotifyCellBaked(CellID, CellData);

		CellID = 0;
		CellData = 0;

		//try for more!
		VoxFarm->GetNextCellToBake(&CellID, &CellData, &bImmediate);

		bRet = true;
	}

	return bRet;
}

bool AOrionVoxelFarm::ProcessSeams()
{
	__int64 CellID = 0;
	__int64 CellData = 0;
	bool bRet = false;

	//get cell
	VoxFarm->GetNextSeam(&CellID, &CellData);

	while (CellData != 0)
	{
		//do stuff to it
		int VertexCount;
		float *Vertices = NULL;
		float *Normals = NULL;
		float *FaceNormals = NULL;
		unsigned int *MaterialsA = NULL;
		unsigned int *MaterialsB = NULL;

		VoxFarm->GetSeamGeometry(&VertexCount, &Vertices, &Normals, &FaceNormals, &MaterialsA, &MaterialsB, CellData);

		//unpack the id so we can use it properly
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;

		//now that we have some data, create some meshes with it!
		TArray<FVector> VertexBuffer;
		TArray<FVector> NormalBuffer;
		TArray<int32> IndexBuffer;
		TArray<FColor> ColorBuffer;
		TArray<FVector2D> UVBuffer;

		float scale = 1.0f;

		VoxFarm->CellTransform(&x, &y, &z, &scale, CellData);

		//int32 MeshIndex = int32(x / scale) 
		//	+ int32(y / scale) * 1024 
		//	+ int32(z / scale) * 1024 * 1024;

		for (int32 i = 0; i < VertexCount * 3; i += 3)
		{
			FVector vert(Vertices[i], -Vertices[i + 2], Vertices[i + 1]);

			vert *= scale;
			vert += FVector(x, -z, y);
			vert *= VOXELSCALE;

			VertexBuffer.Add(vert);

			FVector norm(Normals[i], -Normals[i + 2], Normals[i + 1]);
			NormalBuffer.Add(norm);

			IndexBuffer.Add(i);
			IndexBuffer.Add(i + 2);
			IndexBuffer.Add(i + 1);

			//materials
			byte *bPtrA = new byte[VertexCount * 4];
			memcpy(bPtrA, MaterialsA, sizeof(byte) * VertexCount * 4);

			byte *bPtrB = new byte[VertexCount * 4];
			memcpy(bPtrB, MaterialsB, sizeof(byte) * VertexCount * 4);

			int16 matId1 = bPtrA[i / 3 * 4];
			int16 matId2 = bPtrA[i / 3 * 4 + 2];
			int16 matId3 = bPtrB[i / 3 * 4];

			float div = 1;// materialCount;
			FColor col(matId1, matId2, matId3, 255);
			ColorBuffer.Add(col);

			UVBuffer.Add(FVector2D((float)bPtrB[i / 3 * 4 + 2] / 255, (float)bPtrB[i / 3 * 4 + 3] / 255));

			delete bPtrA;
			delete bPtrB;
		}

		AddCell(VertexBuffer, NormalBuffer, IndexBuffer, ColorBuffer, UVBuffer, CellData);

		//release cell
		VoxFarm->NotifyBakedSeam(CellID, CellData);

		CellID = 0;
		CellData = 0;

		VoxFarm->GetNextSeam(&CellID, &CellData);

		bRet = true;
	}

	return bRet;
}

void AOrionVoxelFarm::BeginPlay()
{
	Super::BeginPlay();

	InitializeTerrain();
}

void AOrionVoxelFarm::InitializeTerrain()
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.bNoCollisionFail = true;

	VoxFarm = GetWorld()->SpawnActor<AOrionVoxelMain>(AOrionVoxelMain::StaticClass(), SpawnInfo);

	if (VoxFarm)
	{
		VoxFarm->InitProject(TEXT("G:\\Users\\Gundy\\Documents\\Unreal Projects\\Orion\\Source\\Voxel\\sdk_2.0.3.99\\Examples\\TerrainExample2\\"));
		////VoxFarm->InitPerlin();
	}
}