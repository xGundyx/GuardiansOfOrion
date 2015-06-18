#pragma once

#include "GameFramework/Actor.h"
//voxelfarm includes
#include "AllowWindowsPlatformTypes.h"
#pragma warning (disable: 4263)
#pragma warning (disable: 4264)

using namespace std;
#include "VoxelFarm.h"
#include "BundleDataByteParser.h"
#include "BundleProject.h"
#include "Worley.h"
#include "HydroTile.h"
#include "FileUtils.h"
#include "ClipmapView.h"
#include "VoxelDB.h"
#include "BlockData.h"

#pragma warning (default: 4263)
#pragma warning (default: 4264)
#include "HideWindowsPlatformTypes.h"
//end voxelfarm

#include "OrionVoxelMain.h"
#include "ProceduralMeshComponent.h"

#include "OrionVoxelFarm.generated.h"

using namespace VoxelFarm::Bundle;
using namespace VoxelFarm;
using namespace VoxelFarm::Architecture;

USTRUCT()
struct FProceduralMeshItem
{
	GENERATED_BODY()

	__int64 CellData;
	UProceduralMeshComponent *Comp;

	bool operator==(const FProceduralMeshItem Other) const { return CellData == Other.CellData; }
	bool operator==(const __int64 Other) const { return CellData == Other; }
};

USTRUCT(BlueprintType)
struct FProceduralHelper
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite)
		UProceduralMeshComponent *Comp;
};

UCLASS()
class ORION_API AOrionVoxelFarm : public AActor
{
	GENERATED_BODY()
public:
	AOrionVoxelFarm(const FObjectInitializer& ObjectInitializer);

	void Tick(float DeltaSeconds) override;
	void BeginPlay() override;

	void InitializeTerrain();

	bool ProcessCells();
	bool ProcessSeams();

	void CleanupCells();
	void CleanupSeams();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "AddMesh"))
		void EventAddMesh(const TArray<FVector> &Vertices, const TArray<FVector> &Normals, const TArray<int32> &Indices, const TArray<FColor> &Colors, const TArray<FVector2D> &UVs, FProceduralHelper &Comp);

	//UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "DeleteMesh"))
	//	void EventDeleteMesh(int32 MeshIndex);

	//UPROPERTY(BlueprintReadWrite, Category = Procedural)
		TArray<FProceduralMeshItem> MeshArray;

	bool RemoveCell(__int64 CellData);
	void AddCell(TArray<FVector> Vertices, TArray<FVector> Normals, TArray<int32> Indices, TArray<FColor> Colors, TArray<FVector2D> UVs, __int64 CellData);
private:

	AOrionVoxelMain *VoxFarm;
};