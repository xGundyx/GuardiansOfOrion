

#pragma once

#include "GameFramework/Actor.h"
#include "OrionInventory.generated.h"

/**
 * 
 */

UENUM()
enum ItemTypes
{
	ITEM_HELMET,
	ITEM_CHEST,
	ITEM_LEGS,
	ITEM_HANDS,
	ITEM_WEAPON,
	ITEM_CAPE,
	ITEM_USEABLE
};

UENUM()
enum ItemRarity
{
	RARITY_COMMON,
	RARITY_RARE
};

UCLASS()
class ORION_API AOrionInventory : public AActor
{
	GENERATED_BODY()
public:
	AOrionInventory(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Inventory)
		TEnumAsByte<ItemTypes> ItemType;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Inventory)
		FString ItemName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Inventory)
		FString ItemDescription;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Inventory)
		bool bStackable;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Inventory)
		TEnumAsByte<ItemRarity> Rarity;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Inventory)
		int32 StackAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
		UTexture2D *Image;

	//this just links us to the inventory widget for us
	UPROPERTY(BlueprintReadWrite, Category = Inventory)
		UObject *MyWidget;
};
