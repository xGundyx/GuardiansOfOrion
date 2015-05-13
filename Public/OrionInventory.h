

#pragma once

#include "GameFramework/Actor.h"
#include "OrionTypes.h"
#include "OrionInventory.generated.h"

/**
 * 
 */

UCLASS()
class ORION_API AOrionInventory : public AActor
{
	GENERATED_BODY()
public:
	AOrionInventory(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
		FString ItemName;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
		FString ItemDescription;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
		bool bStackable;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
		FString EncodedValue;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
		TEnumAsByte<EItemRarity> Rarity;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
		int32 StackAmount;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
		UTexture2D *Image;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
		int32 RequiredLevel;

	//this just links us to the inventory widget for us
	//UPROPERTY(BlueprintReadWrite, Category = Inventory)
	//	UObject *MyWidget;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
		TEnumAsByte<EItemType> InventoryType;

	UFUNCTION(BlueprintCallable, Category = Inventory)
		FString GetSlotName();
};
