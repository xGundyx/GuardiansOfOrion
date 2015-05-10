// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionInventoryList.h"

TSubclassOf<UOrionInventoryItem> UOrionInventoryList::PickItemType(int32 Level)
{
	TArray<int32> PossibleIndices;

	for (int32 i = 0; i < InventoryList.Num(); i++)
	{
		//if (InventoryList[i].MinLevel <= Level && InventoryList[i].MaxLevel >= Level)
			PossibleIndices.Add(i);
	}

	if (PossibleIndices.Num() > 0)
		return InventoryList[FMath::RandRange(0, PossibleIndices.Num() - 1)];

	return nullptr;
}

