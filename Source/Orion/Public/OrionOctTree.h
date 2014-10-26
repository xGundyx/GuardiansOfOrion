

#pragma once

#include "Object.h"
#include "OrionGeneratedMeshComponent.h"
#include "OrionOctTree.generated.h"

struct TreeData
{
	TreeData *Children[8];

	//-100 means we're empty/null
	float Density;

	//our voxel location
	FVector pos;
};

/**
* class for storing voxel data
*/
UCLASS()
class UOrionOctTree : public UObject
{
	GENERATED_UCLASS_BODY()

	//add a density to the tree
	void InsertPoint(FVector pos, float den, FVector center, float width);

	//remove a density
	void RemovePoint(FVector pos);

	//get the oct we're in based on position
	int32 GetChild(FVector pos, FVector center);

	//init stuff
	void Init();

	//fill in our air and ground values
	void FillIn(FVector pos);

	//grab our stored data at a point
	float GetData(FVector pos, FVector center, int32 LOD);

	UOrionOctTree *Children[8];

	//-100 means we're empty/null
	float Density;
	float DensityLOD1;
	float DensityLOD2;

	//our voxel location
	////FVector Position;

	//our width (used for telling how deep we are in the tree)
	float Width;

	static FVector TreeOffsets[8];

	//testing
	UOrionGeneratedMeshComponent *mesh;
};
