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

	//playfab item id
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
		FString ItemID;

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

	//hat mesh, need to remove helmet mesh for this
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
		UStaticMesh* HatMesh;

	//hat socket
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
		FName HatSocket;

	//custom shader material
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
		UMaterialInstance *CustomMat;

	/*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
		float HatScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
		FVector HatOffset;*/

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

	//useable item helper
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
		FUseableItem UseableItem;

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
