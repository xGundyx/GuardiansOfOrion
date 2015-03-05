

#pragma once

#include "OrionInventory.h"
#include "OrionArmor.h"
#include "OrionInventoryArmor.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API AOrionInventoryArmor : public AOrionInventory
{
	GENERATED_BODY()
public:
	AOrionInventoryArmor(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, Category = Inventory)
		AOrionArmor *Armor;
};
