#include "Orion.h"
#include "OrionVoxelFarm.h"

#define VOXELSCALE 20.0f
#define VOXELLOD 2
#define MAX_CELLS_PROCESSED 2
#define MAX_LOD_RENDERED 15

AOrionVoxelFarm::AOrionVoxelFarm(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	RandomSeed = -1;
}
#if 0 //WITH_EDITOR
//make us tick during level editing
bool AOrionVoxelFarm::ShouldTickIfViewportsOnly() const
{
	return GetWorld() != nullptr;
}

void AOrionVoxelFarm::Tick(float DeltaSeconds)
{
	if (VoxFarm == NULL && RandomSeed > -1)
		InitializeTerrain();

	Super::Tick(DeltaSeconds);

	//update the view to match the player's view
	if (VoxFarm)//GEngine && GEngine->GetFirstLocalPlayerController(GetWorld()))
	{
		FVector pos = GetActorLocation();
		FVector dir;

		UpdateCells(DeltaSeconds);

		//if (GEngine)
		//	pos = GEngine->GetFirstLocalPlayerController(GetWorld())->PlayerCameraManager->GetCameraLocation();
#if WITH_EDITOR
		if (GEditor)
		{
			if (GEngine && GEngine->GetFirstLocalPlayerController(GetWorld()) && Cast<AOrionCharacter>(GEngine->GetFirstLocalPlayerController(GetWorld())->GetPawn()))
			{
				FRotator rot;
				GEngine->GetFirstLocalPlayerController(GetWorld())->GetPlayerViewPoint(pos, rot);
				dir = rot.Vector();
			}
			else
			{
				FEditorViewportClient* client = (FEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();
				pos = client->GetViewLocation();
				dir = client->GetViewRotation().Vector();
			}
		}
		else if (GEngine)
		{
			FRotator rot;
			GEngine->GetFirstLocalPlayerController(GetWorld())->GetPlayerViewPoint(pos, rot);
			dir = rot.Vector();
		}
#else
		if (GEngine)
		{
			FRotator rot;
			GEngine->GetFirstLocalPlayerController(GetWorld())->GetPlayerViewPoint(pos, rot);
			dir = rot.Vector();
		}
#endif

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

		//if (bRet)
		//	return;

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
						if (MeshArray[index].SolidPercent < 1.0f)
							ActivateArray.AddUnique(MeshArray[index].CellData);

						DeactivateArray.Remove(MeshArray[index].CellData);

						VisibleItems.Add(index);
					}
					// a cell is in the scene but not in unreal. we came across an empty cell.
				}

				for (int32 i = 0; i < MeshArray.Num(); i++)
				{
					if (VisibleItems.Find(i) == INDEX_NONE)
					{
						if (MeshArray[i].SolidPercent > 0.0f)
							DeactivateArray.AddUnique(MeshArray[i].CellData);

						ActivateArray.Remove(MeshArray[i].CellData);
					}
				}

				VisibleItems.Empty();
			}
		}
		VoxFarm->Idle();
		//CleanupSeams();
	}
}

//update material parameters for fading in/out
void AOrionVoxelFarm::UpdateCells(float DeltaTime)
{
	float FadeSpeed = 2.5f;

	for (auto iter(ActivateArray.CreateIterator()); iter; iter++)
	//for (int32 i = 0; i < ActivateArray.Num(); i++)
	{
		float Target = 1.0f;

		FProceduralMeshItem tItem;
		tItem.CellData = *iter;

		int32 index = MeshArray.Find(tItem);

		if (index != INDEX_NONE)
		{
			Target = FMath::Min(1.0f, MeshArray[index].SolidPercent + DeltaTime * FadeSpeed);

			MeshArray[index].Mat->SetScalarParameterValue(FName(TEXT("SolidPercent")), Target);
			MeshArray[index].SolidPercent = Target;

			//update the seams
			for (int32 j = 0; j < MeshArray[index].Seams.Num(); j++)
			{
				MeshArray[index].Seams[j].Mat->SetScalarParameterValue(FName(TEXT("SolidPercent")), Target);
			}

			if (Target >= 1.0f)
			{
				ActivateArray.Remove(MeshArray[index].CellData);
			}
		}
		else
		{
			__int64 CellToRemove = *iter;
			ActivateArray.Remove(CellToRemove);
		}
	}

	//make sure everything is active before removing old meshes
	if (ActivateArray.Num() > 0)
		return;

	for (auto iter2(DeactivateArray.CreateIterator()); iter2; iter2++)
	//for (int32 i = 0; i < DeactivateArray.Num(); i++)
	{
		float Target = 1.0f;

		FProceduralMeshItem tItem;
		tItem.CellData = *iter2;

		int32 index = MeshArray.Find(tItem);

		if (index != INDEX_NONE)
		{
			Target = FMath::Max(0.0f, MeshArray[index].SolidPercent - DeltaTime * FadeSpeed);

			MeshArray[index].Mat->SetScalarParameterValue(FName(TEXT("SolidPercent")), Target);
			MeshArray[index].SolidPercent = Target;

			//update the seams
			for (int32 j = 0; j < MeshArray[index].Seams.Num(); j++)
			{
				MeshArray[index].Seams[j].Mat->SetScalarParameterValue(FName(TEXT("SolidPercent")), Target);
			}

			if (Target <= 0.0f)
			{
				DeactivateArray.Remove(MeshArray[index].CellData);
			}
		}
		else
			DeactivateArray.Remove(*iter2);
	}

	for (auto iter3(MeshesToDelete.CreateIterator()); iter3; iter3++)
	{
		float Target = 1.0f;

		//FProceduralMeshItem Item = *iter3;

		Target = FMath::Max(0.0f, iter3->SolidPercent - DeltaTime * FadeSpeed);

		iter3->Mat->SetScalarParameterValue(FName(TEXT("SolidPercent")), Target);
		iter3->SolidPercent = Target;

		//update the seams
		for (int32 j = 0; j < iter3->Seams.Num(); j++)
		{
			iter3->Seams[j].Mat->SetScalarParameterValue(FName(TEXT("SolidPercent")), Target);
		}

		if (Target <= 0.0f)
		{
			//remove it!
			FProceduralMeshItem Mesh;
			Mesh.CellData = iter3->CellData;

			for (int32 j = 0; j < iter3->Seams.Num(); j++)
			{
				iter3->Seams[j].Comp->ClearAllMeshSections();
				iter3->Seams[j].Comp = NULL;
			}

			iter3->Seams.Empty();

			iter3->Comp->ClearAllMeshSections();
			iter3->Comp = NULL;

			MeshesToDelete.RemoveSingle(Mesh);
		}
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

	FProceduralMeshItem tItem;
	tItem.CellData = CellData;

	int32 index = MeshArray.Find(tItem);

	if (index != INDEX_NONE)
	{
		DeactivateArray.Remove(CellData);
		ActivateArray.Remove(CellData);

		MeshesToDelete.AddUnique(MeshArray[index]);
		MeshArray.Remove(tItem);

		bRet = true;
	}

	return bRet;
}

void AOrionVoxelFarm::AddCell(TArray<FVector> Vertices, TArray<FVector> Normals, TArray<int32> Indices, TArray<FColor> Colors, TArray<FVector2D> UVs, __int64 CellData, bool bSeam)
{
	FProceduralMeshItem NewCell;
	FProceduralHelper NewComp;

	NewCell.CellData = CellData;
	EventAddMesh(Vertices, Normals, Indices, Colors, UVs, NewComp);

	if (NewComp.Comp == NULL)
		return;

	UMaterialInstanceDynamic* MI = UMaterialInstanceDynamic::Create(NewComp.Comp->GetMaterial(0), this);

	NewComp.Comp->SetMaterial(0, MI);

	if (bSeam)
	{
		int32 index = MeshArray.Find(NewCell);
		if (index != INDEX_NONE)
		{
			FProceduralSeamItem NewSeam;
			NewSeam.Comp = NewComp.Comp;

			NewSeam.Mat = MI;
			MI->SetScalarParameterValue(FName(TEXT("SolidPercent")), MeshArray[index].SolidPercent);
			MeshArray[index].Seams.Add(NewSeam);
		}
		else
		{
			//error!
		}
	}
	else
	{
		NewCell.Comp = NewComp.Comp;
		NewCell.Mat = MI;
		NewCell.bRemoving = false;
		NewCell.SolidPercent = 0.0f;

		ActivateArray.AddUnique(CellData);
		DeactivateArray.Remove(CellData);

		MI->SetScalarParameterValue(FName(TEXT("SolidPercent")), 0.0f);

		MeshArray.AddUnique(NewCell);
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

	int32 CellsProcessed = 0;
	int level = 0;

	//get cell
	VoxFarm->GetNextCellToBake(&CellID, &CellData, &bImmediate, &level);

	while (CellData != 0)
	{
		//ignore things above a certain lod
		if (level < MAX_LOD_RENDERED)
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

				if (i % 9 == 0)
				{
					IndexBuffer.Add(i / 3);
					IndexBuffer.Add(i / 3 + 2);
					IndexBuffer.Add(i / 3 + 1);
				}

				//materials
				uint8 *bPtrA = new uint8[VertexCount * 4];
				memcpy(bPtrA, MaterialsA, sizeof(uint8) * VertexCount * 4);

				uint8 *bPtrB = new uint8[VertexCount * 4];
				memcpy(bPtrB, MaterialsB, sizeof(uint8) * VertexCount * 4);

				int16 matId1 = bPtrA[i / 3 * 4];
				int16 matId2 = bPtrA[i / 3 * 4 + 2];
				int16 matId3 = bPtrB[i / 3 * 4];

				float div = 1;// materialCount;
				FColor col(matId1, matId2, matId3, 255);
				ColorBuffer.Add(col);

				UVBuffer.Add(FVector2D((float)bPtrB[i / 3 * 4 + 2] / 255.0f, (float)bPtrB[i / 3 * 4 + 3] / 255.0f));

				delete bPtrA;
				delete bPtrB;
			}

			AddCell(VertexBuffer, NormalBuffer, IndexBuffer, ColorBuffer, UVBuffer, CellData, false);

			CellsProcessed++;
		}

		//release cell
		VoxFarm->NotifyCellBaked(CellID, CellData);

		CellID = 0;
		CellData = 0;

		bRet = true;

		if (CellsProcessed >= MAX_CELLS_PROCESSED)
			break;

		//try for more!
		VoxFarm->GetNextCellToBake(&CellID, &CellData, &bImmediate, &level);
	}

	return bRet;
}

bool AOrionVoxelFarm::ProcessSeams()
{
	__int64 CellID = 0;
	__int64 CellData = 0;
	bool bRet = false;

	int32 CellsProcessed = 0;
	int level = 0;

	//get cell
	VoxFarm->GetNextSeam(&CellID, &CellData, &level);

	while (CellData != 0)
	{
		if (level < MAX_LOD_RENDERED)
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

				if (i % 9 == 0)
				{
					IndexBuffer.Add(i / 3);
					IndexBuffer.Add(i / 3 + 2);
					IndexBuffer.Add(i / 3 + 1);
				}

				//materials
				uint8 *bPtrA = new uint8[VertexCount * 4];
				memcpy(bPtrA, MaterialsA, sizeof(uint8) * VertexCount * 4);

				uint8 *bPtrB = new uint8[VertexCount * 4];
				memcpy(bPtrB, MaterialsB, sizeof(uint8) * VertexCount * 4);

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

			AddCell(VertexBuffer, NormalBuffer, IndexBuffer, ColorBuffer, UVBuffer, CellData, true);

			CellsProcessed++;
		}

		//release cell
		VoxFarm->NotifyBakedSeam(CellID, CellData);

		CellID = 0;
		CellData = 0;

		bRet = true;

		if (CellsProcessed >= MAX_CELLS_PROCESSED)
			break;

		VoxFarm->GetNextSeam(&CellID, &CellData, &level);
	}

	return bRet;
}

void AOrionVoxelFarm::BeginPlay()
{
	Super::BeginPlay();

	if (RandomSeed < 0)
		RandomSeed = 180;
		//InitializeTerrain();
}

void AOrionVoxelFarm::InitializeTerrain()
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	VoxFarm = GetWorld()->SpawnActor<AOrionVoxelMain>(AOrionVoxelMain::StaticClass(), SpawnInfo);

	if (VoxFarm)
	{
		//VoxFarm->InitProject(TEXT("G:\\Users\\Gundy\\Documents\\Unreal Projects\\Orion\\Source\\Voxel\\sdk_2.0.3.99\\Examples\\TerrainExample2\\"));
		////VoxFarm->InitPerlin();
		FString ProjDir = FPaths::GameDir();
		ProjDir += "Voxel/";
		VoxFarm->InitCustom(ProjDir);
	}
}

void AOrionVoxelFarm::BeginDestroy()
{
	if (VoxFarm)
	{
		VoxFarm->ShutdownVoxel();
		VoxFarm->Destroy();
	}

	VoxFarm = nullptr;

	Super::BeginDestroy();
}
#endif