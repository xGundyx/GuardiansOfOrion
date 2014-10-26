

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
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category = Inventory)
		AOrionArmor *Armor;
};
