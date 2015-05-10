// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionInventoryItem.h"

//legendary = 1 rare + 3-6
//super enh = 2-4
//enh = 1-2
//common = 0
void UOrionInventoryItem::CalcStats(FDecodeItemInfo &Info)
{
	int32 index = 0;

	switch (Info.Rarity)
	{
	case RARITY_LEGENDARY:
		index = FMath::RandRange(3, 6);
		if (RareStats.Num() > 0)
			Info.RareStats.Add(RareStats[FMath::RandRange(0, RareStats.Num() - 1)]);
		break;

	case RARITY_SUPERENHANCED:
		index = FMath::RandRange(2, 4);
		break;

	case RARITY_ENHANCED:
		index = FMath::RandRange(1, 2);
		break;
	default:
		break;
	}

	int32 i = 0;
	if (PrimaryStats.Num() > 0)
	{
		for (; i < index / 2; i++)
		{
			Info.PrimaryStats.Add(PrimaryStats[FMath::RandRange(0, PrimaryStats.Num() - 1)]);
		}
	}
	if (SecondaryStats.Num() > 0)
	{
		for (; i < index; i++)
		{
			Info.SecondaryStats.Add(SecondaryStats[FMath::RandRange(0, SecondaryStats.Num() - 1)]);
		}
	}
}

