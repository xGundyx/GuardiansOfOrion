

#pragma once

#include "GameFramework/Actor.h"
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

	UPROPERTY(EditAnywhere, Category = Inventory)
		USkeletalMesh *Mesh;

	UPROPERTY(EditAnywhere, Category = Inventory)
		USkeletalMesh *Mesh1P;
};
