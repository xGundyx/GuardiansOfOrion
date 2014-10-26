

#pragma once

#include "GameFramework/Actor.h"
#include "OrionArmor.generated.h"

/**
 * 
 */

UENUM()
enum ArmorSlots
{
	SLOT_HELMET,
	SLOT_CHEST,
	SLOT_LEGS,
	SLOT_HANDS
};

UCLASS()
class ORION_API AOrionArmor : public AOrionInventory
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category = Inventory)
		USkeletalMesh *Mesh;

	UPROPERTY(EditAnywhere, Category = Inventory)
		int32 RequiredLevel;

	UPROPERTY(EditAnywhere, Category = Inventory)
		int32 Strength;

	UPROPERTY(EditAnywhere, Category = Inventory)
		int32 Defense;

	UPROPERTY(EditAnywhere, Category = Inventory)
		TEnumAsByte<ArmorSlots> Slot;
};
