// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "OrionInventoryGrid.generated.h"
/**
 * 
 */

USTRUCT()
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

UCLASS()
class ORION_API UOrionInventoryGrid : public UObject
{
	GENERATED_BODY()
public:
	UOrionInventoryGrid(const FObjectInitializer& ObjectInitializer);

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

		while (Column.Num() < height)
		{
			FInventoryRow newColumn;
			while (newColumn.Row.Num() < width)
			{
				newColumn.Row.Add(NULL);
			}
			Column.Add(newColumn);
		}
	}
};
