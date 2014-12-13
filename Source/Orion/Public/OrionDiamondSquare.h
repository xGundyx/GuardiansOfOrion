

#pragma once

#include "Object.h"
#include "OrionDiamondSquare.generated.h"

/**
 * 
 */

#define DIAMOND_RES (CHUNK_SIZE_XY * VOXEL_X)


UCLASS()
class ORION_API UOrionDiamondSquare : public UObject
{
	GENERATED_BODY()
public:
	UOrionDiamondSquare(const FObjectInitializer& ObejctInitializer);

	static void GenerateHeightValues(int32 x, int32 y, int32 Resolution);
	static void Init();
	static void SeedValues(int32 x, int32 y, int32 Resolution);

	//store our heigh values in a 2D array, needs to be a power of 2 plus 1
	static float HeightValues[int(DIAMOND_RES) + 1][int(DIAMOND_RES) + 1];
	static float MaxHeight;
};
