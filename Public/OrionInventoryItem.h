// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "OrionTypes.h"
#include "OrionInventoryItem.generated.h"

/**
 * Every item in the game needs to be represented by one of these
 */
UCLASS(Blueprintable)
class ORION_API UOrionInventoryItem : public UObject
{
	GENERATED_BODY()
	
public:
	//item name as it will appear in game
	UPROPERTY(EditDefaultsOnly, Category = Item)
		FString ItemName;

	//text describing what this item is/does
	UPROPERTY(EditDefaultsOnly, Category = Item)
		FString ItemDesc;

	//name of item if it's legendary
	UPROPERTY(EditDefaultsOnly, Category = Item)
		FString LegendaryName;

	//icon to use
	//item name as it will appear in game
	UPROPERTY(EditDefaultsOnly, Category = Item)
		UTexture2D *ItemIcon;

	//type of class associated with us (mainly for weapons)
	UPROPERTY(EditDefaultsOnly, Category = Item)
		UClass *ItemClass;

	//skeletal mesh associated with us (let weapons handle this themselves)
	UPROPERTY(EditDefaultsOnly, Category = Item)
		USkeletalMesh* ItemMesh;

	//skeletal mesh associated with us in first person (if any, only valid on armors)
	UPROPERTY(EditDefaultsOnly, Category = Item)
		USkeletalMesh* ItemMesh1P;

	//type of item
	UPROPERTY(EditDefaultsOnly, Category = Item)
		TEnumAsByte<EItemType> ItemType;

	//can this item be consumed on use
	UPROPERTY(EditDefaultsOnly, Category = Item)
		bool bConsumeable;
	
	//is this item stackable
	UPROPERTY(EditDefaultsOnly, Category = Item)
		bool bStackable;

	//how many items can be stacked
	UPROPERTY(EditDefaultsOnly, Category = Item)
		int32 StackSize;

	//Primary Stats (scaled by level)
	UPROPERTY(EditDefaultsOnly, Category = Item)
		TArray<FPrimaryItemStats> PrimaryStats;

	//Secondary Stats
	UPROPERTY(EditDefaultsOnly, Category = Item)
		TArray<FSecondaryItemStats> SecondaryStats;

	//Rare Stats
	UPROPERTY(EditDefaultsOnly, Category = Item)
		TArray<FRareItemStats> RareStats;

	void CalcStats(FDecodeItemInfo &Info);
};