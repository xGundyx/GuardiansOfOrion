// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "DHArmors.generated.h"

USTRUCT(BlueprintType)
struct FArmorColor
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Mat)
		UMaterialInstance* Mat;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Mat)
		FName ArmorName;
};

USTRUCT(BlueprintType)
struct FArmorInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Mesh)
		USkeletalMesh* HelmetMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Mesh)
		USkeletalMesh* BodyMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Mesh)
		USkeletalMesh* ArmsMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Mesh)
		USkeletalMesh* LegsMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Mesh)
		USkeletalMesh* Flight1Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Mesh)
		USkeletalMesh* Flight2Mesh;
};

/**
*
*/
UCLASS()
class ADHArmors : public AActor
{
	GENERATED_BODY()
public:
	ADHArmors(const FObjectInitializer& ObjectInitializer);
};
