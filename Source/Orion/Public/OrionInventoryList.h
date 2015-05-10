// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "OrionInventory.h"
#include "OrionInventoryItem.h"
#include "OrionInventoryList.generated.h"

/**
 * 
 */

UCLASS(Blueprintable)
class ORION_API UOrionInventoryList : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = List)
		TArray< TSubclassOf<UOrionInventoryItem> > InventoryList;

	//find an index to an item we want
	TSubclassOf<UOrionInventoryItem> PickItemType(int32 Level);
};
