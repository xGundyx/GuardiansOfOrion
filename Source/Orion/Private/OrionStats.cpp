// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionStats.h"

UOrionStats::UOrionStats(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//initialize our stats arrray
	aStats.Empty();

	aStats.Add(FPlayerStats(STAT_RAPTORKILL, STATTYPE_INT, TEXT("RAPTOR KILLS")));
	aStats.Add(FPlayerStats(STAT_TREXKILL, STATTYPE_INT, TEXT("T-REX KILLS")));

	bInitialized = false;

#if IS_SERVER
#else
	//attempt to read in our stats directly from playfab
	ReadPlayerStats();
#endif
}

void UOrionStats::ReadPlayerStats()
{
#if IS_SERVER
#else
	UOrionTCPLink::GetPlayerStats(this);
#endif
}

void UOrionStats::FlushPlayerStats(AOrionPlayerController* PC)
{
#if IS_SERVER
	//call directly to playfab from dedicated servers only!
	UOrionTCPLink::SavePlayerStatistics(PC->PlayFabID, this);
#endif
}

std::map<std::string, PlayFab::Int32> UOrionStats::GetStatsMap()
{
	std::map<std::string, PlayFab::Int32> Map;

	for (int32 i = 0; i < aStats.Num(); i++)
	{
		std::string index = std::to_string(int(aStats[i].StatID));
		Map[index] = (aStats[i].StatType == STATTYPE_INT ? PlayFab::Int32(aStats[i].StatValueInt) : PlayFab::Int32(aStats[i].StatValueFloat));
	}

	return Map;
}