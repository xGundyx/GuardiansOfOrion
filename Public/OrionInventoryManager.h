

#pragma once

#include "GameFramework/Actor.h"
#include "OrionInventory.h"
#include "OrionInventoryGrid.h"
#include "OrionInventoryManager.generated.h"

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
};

USTRUCT()
struct FInventoryGrid
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		TArray<FInventoryRow> Column;

	UPROPERTY(BlueprintReadWrite, Category = Inventory)
		TArray<UObject*> Icons;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
		int32 MaxHeight;
	UPROPERTY(BlueprintReadOnly, Category = Inventory)
		int32 MaxWidth;
	UPROPERTY(BlueprintReadOnly, Category = Inventory)
		int32 ItemSize;
	UPROPERTY(BlueprintReadOnly, Category = Inventory)
		int32 Padding;

	void CreateInventory(int32 height, int32 width, int32 size, int32 pad)
	{
		MaxHeight = height;
		MaxWidth = width;
		ItemSize = size;
		Padding = pad;

		while(Column.Num() < height)
		{
			FInventoryRow newColumn;
			while(newColumn.Row.Num() < width)
			{
				newColumn.Row.Add(NULL);
			}
			Column.Add(newColumn);
		}
	}
};*/

UCLASS()
class ORION_API AOrionInventoryManager : public AActor
{
	GENERATED_UCLASS_BODY()

	//2 dimensional inventory grid representation
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
		UOrionInventoryGrid *Grid;

	//equipped slots
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
		UOrionInventoryGrid *HelmetSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
		UOrionInventoryGrid *BodySlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
		UOrionInventoryGrid *LegsSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
		UOrionInventoryGrid *HandsSlot;

	UFUNCTION(BlueprintCallable, Category = Inventory)
		int32 AddItemToInventory(UOrionInventoryGrid *theGrid, AOrionInventory* newItem);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		void RemoveItemFromInventory(UOrionInventoryGrid *theGrid, int32 x, int32 y);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		void Init();

	UFUNCTION(BlueprintCallable, Category = Inventory)
		void DestroyInventory();

	UFUNCTION(BlueprintCallable, Category = Inventory)
		void EquipItems(AOrionCharacter *aPawn);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		AOrionInventory *GetItemAt(UOrionInventoryGrid *theGrid, int32 x, int32 y);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		void SwapItems(UOrionInventoryGrid *theGrid1, int32 x1, int32 y1, UOrionInventoryGrid *theGrid2, int32 x2, int32 y2);

	APlayerController *OwnerController;
};
