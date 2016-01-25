// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "OrionInventoryGrid.generated.h"
/**
 * 
 */

/*USTRUCT()
struct FInventoryRow
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY()
		TArray<AOrionInventory*> Row;

	FORCEINLINE	bool	operator==(const FInventoryRow other) const
	{
		return false;
	}
};*/

UCLASS(Blueprintable)
class ORION_API AOrionInventoryGrid : public AActor
{
	GENERATED_BODY()
public:
	AOrionInventoryGrid(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
		void OnRep_Inventory();

	UPROPERTY(ReplicatedUsing = OnRep_Inventory, BlueprintReadOnly, Category = Inventory)
		TArray<FInventoryItem> Inventory;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Inventory)
		TEnumAsByte<EItemType> InventoryType;

	//UPROPERTY(BlueprintReadWrite, Category = Inventory)
	//	TArray<UTexture2D*> Icons;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Inventory)
		int32 Height;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = Inventory)
		int32 Width;
	//UPROPERTY(Replicated, BlueprintReadOnly, Category = Inventory)
	//	int32 ItemSize;
	//UPROPERTY(BlueprintReadOnly, Category = Inventory)
	//	int32 Padding;

	FString GetGridName();

	void CreateInventory(int32 height, int32 width, int32 size, int32 pad, EItemType type)
	{
		InventoryType = type;
		Height = height;
		Width = width;
		//ItemSize = size;
		//Padding = pad;

		for (int32 i = 0; i < height; i++)
		{
			for (int32 j = 0; j < width; j++)
			{
				FInventoryItem NewItem;

				Inventory.Add(NewItem);
			}
		}
	}
};
