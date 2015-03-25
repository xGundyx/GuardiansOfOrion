#include "OrionTypes.generated.h"
#pragma once

#define ORION_SURFACE_Default		SurfaceType_Default
#define ORION_SURFACE_Concrete		SurfaceType1
#define ORION_SURFACE_Dirt			SurfaceType2
#define ORION_SURFACE_Water			SurfaceType3
#define ORION_SURFACE_Metal			SurfaceType4
#define ORION_SURFACE_Wood			SurfaceType5
#define ORION_SURFACE_Grass			SurfaceType6
#define ORION_SURFACE_Glass			SurfaceType7
#define ORION_SURFACE_Flesh			SurfaceType8

USTRUCT()
struct FDecalData
{
	GENERATED_USTRUCT_BODY()

	/** material */
	UPROPERTY(EditDefaultsOnly, Category = Decal)
		UMaterial* DecalMaterial;

	/** quad size (width & height) */
	UPROPERTY(EditDefaultsOnly, Category = Decal)
		float DecalSize;

	/** lifespan */
	UPROPERTY(EditDefaultsOnly, Category = Decal)
		float LifeSpan;

	/** defaults */
	FDecalData()
		: DecalSize(256.f)
		, LifeSpan(10.f)
	{
	}
};

UENUM()
enum EFoodType
{
	FOOD_GRASS,
	FOOD_FRUIT,
	FOOD_VEGETABLE,
	FOOD_DEADMEATSMALL,
	FOOD_DEADMEATLARGE
};