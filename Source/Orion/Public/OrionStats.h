// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include <iostream>
#include <string>
#include <map>
//#include "playfab/PlayFabBaseModel.h"
#include "PlayFabRequestProxy.h"
#include "OrionStats.generated.h"

class AOrionPlayerController;
/**
 * 
 */

UENUM()
enum FStatType
{
	STATTYPE_INT,
	STATTYPE_FLOAT
};

UENUM()
enum FStatID
{
	STAT_NONE,
	STAT_RAPTORKILL,
	STAT_TREXKILL
};

USTRUCT()
struct FPlayerStats
{
	GENERATED_USTRUCT_BODY()

	FStatID StatID;			//unique id for the stat entry
	int32 StatValueInt;		//int value
	float StatValueFloat;	//float value
	FStatType StatType;		//whether we are float or int
	FString StatName;		//human readable name

	FPlayerStats()
	{
		StatID = STAT_NONE;
		StatValueInt = 0;
		StatValueFloat = 0.0f;
		StatType = STATTYPE_INT;
		StatName = TEXT("");
	}

	FPlayerStats(FStatID ID, FStatType type, FString sName)
	{
		StatID = ID;
		StatValueInt = 0;
		StatValueFloat = 0.0f;
		StatType = type;
		StatName = sName;
	}
};

UCLASS()
class ORION_API UOrionStats : public UObject
{
	GENERATED_BODY()
public:
	UOrionStats(const FObjectInitializer& ObjectInitializer);

	//read in a player's stats (can be done from client or server)
	void ReadPlayerStats();



	void FlushPlayerStats(AOrionPlayerController* PC);
	
	TArray<FPlayerStats> aStats;

	//std::map<std::string, PlayFab::Int32> GetStatsMap();

	bool bInitialized;

private:
};
