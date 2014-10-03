// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "DHArmors.generated.h"

USTRUCT(BlueprintType)
struct FArmorColor
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Mat)
		UMaterialInstance* Mat;

	UPROPERTY(EditDefaultsOnly, Category = Mat)
		FName ArmorName;
};

USTRUCT(BlueprintType)
struct FArmorInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		USkeletalMesh* HelmetMesh;

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		USkeletalMesh* BodyMesh;

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		USkeletalMesh* ArmsMesh;

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		USkeletalMesh* LegsMesh;

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		USkeletalMesh* Flight1Mesh;

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		USkeletalMesh* Flight2Mesh;
};

/**
*
*/
UCLASS()
class ADHArmors : public AActor
{
	GENERATED_UCLASS_BODY()

};
