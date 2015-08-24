#pragma once
#include "GameFramework/Actor.h"
#if WITH_EDITOR
#include "OrionVoxelMain.h"
#endif
#include "ProceduralMeshComponent.h"
#include "OrionVoxelFarm.generated.h"

USTRUCT()
struct FProceduralSeamItem
{
	GENERATED_BODY()

	UPROPERTY()
		UProceduralMeshComponent *Comp;

	UPROPERTY()
		UMaterialInstanceDynamic *Mat;

	bool operator==(const FProceduralSeamItem Other) const { return Comp == Other.Comp; }
};

USTRUCT()
struct FProceduralMeshItem
{
	GENERATED_BODY()

	__int64 CellData;
	UPROPERTY()
		UProceduralMeshComponent *Comp;

	UPROPERTY()
		TArray<FProceduralSeamItem> Seams;

	float SolidPercent;
	bool bRemoving;

	UPROPERTY()
		UMaterialInstanceDynamic *Mat;

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

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "AddMesh"))
		void EventAddMesh(const TArray<FVector> &Vertices, const TArray<FVector> &Normals, const TArray<int32> &Indices, const TArray<FColor> &Colors, const TArray<FVector2D> &UVs, FProceduralHelper &Comp);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Voxel)
		int32 RandomSeed;

#if WITH_EDITOR
	void Tick(float DeltaSeconds) override;
	void BeginPlay() override;
	void BeginDestroy() override;
	bool ShouldTickIfViewportsOnly() const override;

	void InitializeTerrain();

	bool ProcessCells();
	bool ProcessSeams();

	void CleanupCells();
	void CleanupSeams();

	void UpdateCells(float DeltaTime);

	//UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "DeleteMesh"))
	//	void EventDeleteMesh(int32 MeshIndex);

	//UPROPERTY(BlueprintReadWrite, Category = Procedural)
		TArray<FProceduralMeshItem> MeshArray;
		TArray<FProceduralMeshItem> MeshesToDelete;

		TArray<__int64> ActivateArray;
		TArray<__int64> DeactivateArray;

	bool RemoveCell(__int64 CellData);
	void AddCell(TArray<FVector> Vertices, TArray<FVector> Normals, TArray<int32> Indices, TArray<FColor> Colors, TArray<FVector2D> UVs, __int64 CellData, bool bSeam = false);
private:

	AOrionVoxelMain *VoxFarm;
#endif
};