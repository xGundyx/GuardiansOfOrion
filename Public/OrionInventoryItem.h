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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
		FString ItemName;

	//text describing what this item is/does
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
		FString ItemDesc;

	//name of item if it's legendary
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
		FString LegendaryName;

	//icon to use
	//item name as it will appear in game
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
		UTexture2D *ItemIcon;

	//type of class associated with us (mainly for weapons)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
		UClass *ItemClass;

	//skeletal mesh associated with us (let weapons handle this themselves)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
		USkeletalMesh* ItemMesh;

	//skeletal mesh associated with us in first person (if any, only valid on armors)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
		USkeletalMesh* ItemMesh1P;

	//type of item
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
		TEnumAsByte<EItemType> ItemType;

	//can this item be consumed on use
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
		bool bConsumeable;
	
	//is this item stackable
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
		bool bStackable;

	//does this item have various quality levels
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
		bool bHasQuality;

	//what this item can be broken down into, if anything
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
		TArray< TSubclassOf<class UOrionInventoryItem> > BreakdownClass;

	//how many items can be stacked
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
		int32 StackSize;

	//Primary Stats (scaled by level)
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
	//	TArray<FPrimaryItemStats> PrimaryStats;

	//Secondary Stats
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
	//	TArray<FSecondaryItemStats> SecondaryStats;

	//Rare Stats
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
	//	TArray<FRareItemStats> RareStats;

	void CalcStats(FDecodeItemInfo &Info);
};
