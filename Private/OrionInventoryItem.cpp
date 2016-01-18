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
	TArray<FRareStatsInfo> AvailableRares;
	int32 i = 0;

	if (Info.Slot == ITEM_ABILITY)
		index = 2;

	if (Info.Slot == ITEM_SHADER ||
		Info.Slot == ITEM_USEABLE ||
		Info.Slot == ITEM_ANY ||
		Info.Slot == ITEM_GENERICCRAFTING ||
		Info.Slot == ITEM_DISPLAYARMOR ||
		Info.Slot == ITEM_BREAKDOWNABLE ||
		Info.Slot == ITEM_DISPLAYARMOR)
		index = 0;
	else
	{
		switch (Info.Rarity)
		{
		case RARITY_LEGENDARY:
			index = FMath::RandRange(4, 6);

			for (; i < RareStats.StatsInfo.Num(); i++)
			{
				if (RareStats.StatsInfo[i].Slot == Info.Slot || RareStats.StatsInfo[i].Slot == ITEM_ANY)
					AvailableRares.Add(RareStats.StatsInfo[i]);
			}

			if (AvailableRares.Num() > 0)
				Info.RareStats.Add(AvailableRares[FMath::RandRange(0, AvailableRares.Num() - 1)]);
			break;

		case RARITY_SUPERENHANCED:
			index = FMath::RandRange(2, 4);
			break;

		case RARITY_ENHANCED:
			index = FMath::RandRange(1, 2);
			break;
		default:
			index = 1;
			break;
		}
	}

	TArray<int32> Primary;

	for (int32 j = 0; j < PRIMARYSTAT_NUM; j++)
		Primary.AddUnique(j);

	i = 0;
	//if (PrimaryStats.Num() > 0)
	//{
		for (; i < index / 2 && Primary.Num() > 0; i++)
		{
			int32 RandomIndex = FMath::RandRange(0, Primary.Num() - 1);

			FPrimaryItemStats StatToAdd;

			StatToAdd.StatType = EPrimaryStats(Primary[RandomIndex]);
			StatToAdd.MaxValue = int32(Info.GetMaxStatValue() * Info.GetRarityMultiplier());
			StatToAdd.MinValue = FMath::Max(1, int32(StatToAdd.MaxValue * 0.7f));
			StatToAdd.Value = FMath::RandRange(StatToAdd.MinValue, StatToAdd.MaxValue);

			Info.PrimaryStats.Add(StatToAdd);

			Primary.RemoveAt(RandomIndex);
		}
	//}

	TArray<int32> Secondary;

	for (int32 j = 0; j < SECONDARYSTAT_NUM; j++)
	{
		if (Info.GetMaxSecondaryStatValue(ESecondaryStats(j)) > 0)
			Secondary.AddUnique(j);
	}

	//if (SecondaryStats.Num() > 0)
	//{
		for (; i < index && Secondary.Num() > 0; i++)
		{
			int32 RandomIndex = FMath::RandRange(0, Secondary.Num() - 1);

			FSecondaryItemStats StatToAdd;

			StatToAdd.StatType = ESecondaryStats(Secondary[RandomIndex]);
			StatToAdd.MaxValue = Info.GetMaxSecondaryStatValue(ESecondaryStats(StatToAdd.StatType));
			StatToAdd.MinValue = FMath::Max(1, int32(StatToAdd.MaxValue * 0.7f));
			StatToAdd.Value = FMath::RandRange(StatToAdd.MinValue, StatToAdd.MaxValue);

			Info.SecondaryStats.Add(StatToAdd);

			Secondary.RemoveAt(RandomIndex);
		}
	//}

	Info.MainStat = int32(Info.GetRarityMultiplier() * FMath::RandRange(int32(Info.GetMaxStatValue() * 0.7f), Info.GetMaxStatValue()));
}

