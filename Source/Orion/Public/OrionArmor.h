

#pragma once

#include "GameFramework/Actor.h"
#include "OrionInventory.h"
#include "OrionArmor.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API AOrionArmor : public AOrionInventory
{
	GENERATED_BODY()
public:
	AOrionArmor(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Replicated, EditAnywhere, Category = Inventory)
		USkeletalMesh *Mesh;

	UPROPERTY(Replicated, EditAnywhere, Category = Inventory)
		USkeletalMesh *ExtraMesh;

	UPROPERTY(Replicated, EditAnywhere, Category = Inventory)
		USkeletalMesh *Mesh1P;
};
