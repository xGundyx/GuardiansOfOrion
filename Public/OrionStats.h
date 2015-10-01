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
//UENUM()
enum EStatID// : uint8
{
	//kills
	STAT_RAPTORKILL,
	STAT_TREXKILL,
	STAT_COMPYKILL,
	STAT_DILOKILL,
	STAT_RHAMKILL,
	STAT_PTERAKILL,
	STAT_STEGKILL,
	STAT_TRIKEKILL,
	STAT_ANKYKILL,
	STAT_PARAKILL,
	STAT_ALLOKILL,
	STAT_KRUGERKILL,
	STAT_JECKYLKILL,
	STAT_NAMORKILL,
	STAT_VIPERKILL,
	STAT_BONESKILL,
	STAT_ORBKILL,
	STAT_GRUMPSKILL,
	STAT_SPIDERKILL,
	STAT_SUPPORTKILL,
	STAT_RECONKILL,
	STAT_ASSAULTKILL,

	//deaths by
	STAT_RAPTORDEATH,
	STAT_TREXDEATH,
	STAT_COMPYDEATH,
	STAT_DILODEATH,
	STAT_RHAMDEATH,
	STAT_PTERADEATH,
	STAT_STEGDEATH,
	STAT_TRIKEDEATH,
	STAT_ANKYDEATH,
	STAT_PARADEATH,
	STAT_ALLODEATH,
	STAT_KRUGERDEATH,
	STAT_JECKYLDEATH,
	STAT_NAMORDEATH,
	STAT_VIPERDEATH,
	STAT_BONESDEATH,
	STAT_ORBDEATH,
	STAT_GRUMPSDEATH,
	STAT_SPIDERDEATH,
	STAT_SUPPORTDEATH,
	STAT_RECONDEATH,
	STAT_ASSAULTDEATH,

	//weapons
	STAT_MAGNUMKILLS,
	STAT_MAGNUMBULLETSFIRED,
	STAT_MAGNUMBULLETSHIT,
	STAT_PISTOLKILLS,
	STAT_PISTOLBULLETSFIRED,
	STAT_PISTOLBULLETSHIT,
	STAT_AUTOPISTOLKILLS,
	STAT_AUTOPISTOLBULLETSFIRED,
	STAT_AUTOPISTOLBULLETSHIT,
	STAT_COMBATSHOTGUNKILLS,
	STAT_COMBATSHOTGUNBULLETSFIRED,
	STAT_COMBATSHOTGUNBULLETSHIT,
	STAT_AUTORIFLEKILLS,
	STAT_AUTORIFLEBULLETSFIRED,
	STAT_AUTORIFLEBULLETSHIT,
	STAT_SMGKILLS,
	STAT_SMGBULLETSFIRED,
	STAT_SMGBULLETSHIT,

	//levels
	STAT_RELICLOSSES,
	STAT_RELICWINS,

	//exp
	STAT_SUPPORTEXP,
	STAT_ASSAULTEXP,
	STAT_RECONEXP,
	STAT_EXPEARNED,

	//heroic enemies
	STAT_ELITERAPTORKILL,
	STAT_ELITETREXKILL,
	STAT_ELITECOMPYKILL,
	STAT_ELITEDILOKILL,
	STAT_ELITERHAMKILL,
	STAT_ELITEPTERAKILL,
	STAT_ELITESTEGKILL,
	STAT_ELITETRIKEKILL,
	STAT_ELITEANKYKILL,
	STAT_ELITEPARAKILL,
	STAT_ELITEALLOKILL,
	STAT_ELITEKRUGERKILL,
	STAT_ELITEJECKYLKILL,
	STAT_ELITENAMORKILL,
	STAT_ELITEVIPERKILL,
	STAT_ELITEBONESKILL,
	STAT_ELITEORBKILL,
	STAT_ELITEGRUMPSKILL,
	STAT_ELITESPIDERKILL,
	STAT_ELITESUPPORTKILL,
	STAT_ELITERECONKILL,
	STAT_ELITEASSAULTKILL,
	STAT_ELITERAPTORDEATH,
	STAT_ELITETREXDEATH,
	STAT_ELITECOMPYDEATH,
	STAT_ELITEDILODEATH,
	STAT_ELITERHAMDEATH,
	STAT_ELITEPTERADEATH,
	STAT_ELITESTEGDEATH,
	STAT_ELITETRIKEDEATH,
	STAT_ELITEANKYDEATH,
	STAT_ELITEPARADEATH,
	STAT_ELITEALLODEATH,
	STAT_ELITEKRUGERDEATH,
	STAT_ELITEJECKYLDEATH,
	STAT_ELITENAMORDEATH,
	STAT_ELITEVIPERDEATH,
	STAT_ELITEBONESDEATH,
	STAT_ELITEORBDEATH,
	STAT_ELITEGRUMPSDEATH,
	STAT_ELITESPIDERDEATH,
	STAT_ELITESUPPORTDEATH,
	STAT_ELITERECONDEATH,
	STAT_ELITEASSAULTDEATH,

	//pvp stats
	STAT_PVPWINS,
	STAT_PVPLOSSES,
	STAT_PVPKILLS,
	STAT_PVPDEATHS,

	//misc
	STAT_SMOKEGRENADESTHROWN,
	STAT_FRAGGRENADESTHROWN,
	STAT_EMPGRENADESTHROWN,
	STAT_TIMESROLLED,
	STAT_TIMESBLINKED,

	//healing
	STAT_HEALINGDONE,
	STAT_HEALINGRECEIVED,
	STAT_TEAMMATESREVIVED,
	STAT_TIMESREVIVED,

	//orbs
	STAT_HEALTHORBS,
	STAT_SHIELDORBS,
	STAT_EXPORBS,

	//gore
	STAT_LIMBSBLOWNOFF,
	STAT_HEADSBLOWNOFF,

	//survival only?
	STAT_KILLSASASSAULT,
	STAT_KILLSASSUPPORT,
	STAT_KILLSASRECON,
	STAT_DEATHSASASSAULT,
	STAT_DEATHSASSUPPORT,
	STAT_DEATHSASRECON,

	//difficulty win/loss
	STAT_EASYWINS,
	STAT_EASYLOSSES,
	STAT_NORMALWINS,
	STAT_NORMALLOSSES,
	STAT_HARDWINS,
	STAT_HARDLOSSES,
	STAT_INSANEWINS,
	STAT_INSANELOSSES,
	STAT_REDIKULOUSWINS,
	STAT_REDIKULOUSLOSSES,

	//generator
	STAT_GENERATORHEALINGDONE,
	STAT_GENERATORSREPAIRED,

	STAT_MAX,
	STAT_ERROR
};

USTRUCT(BlueprintType)
struct FPlayerStats
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		int32 StatID;			//unique id for the stat entry
	UPROPERTY(BlueprintReadWrite, Category = Stats)
		int32 StatValue;		//int value
	UPROPERTY(BlueprintReadWrite, Category = Stats)
		FString StatName;		//human readable name, sent to playfab
	UPROPERTY(BlueprintReadWrite, Category = Stats)
		bool bDirty;			//only need to update stats that are dirty

	FPlayerStats()
	{
		StatID = -1;
		StatValue = 0;
		StatName = TEXT("");
		bDirty = false;
	}

	FPlayerStats(EStatID ID, FString sName)
	{
		StatID = int32(ID);
		StatValue = 0;
		StatName = sName;
		bDirty = false;
	}

	bool operator==(const FPlayerStats Other) const { return StatName == Other.StatName; }
};

UCLASS()
class ORION_API AOrionStats : public AActor
{
	GENERATED_BODY()
public:
	AOrionStats(const FObjectInitializer& ObjectInitializer);

	//read in a player's stats (can be done from client or server)
	void ReadPlayerStats(AOrionPlayerController* PC);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Read Player Stats From Playfab"))
		void EventReadPlayerStats(AOrionPlayerController* PC);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Save Player Stats to Playfab"))
		void EventSavePlayerStats(AOrionPlayerController* PC);

	//server only saving of stats
	void FlushPlayerStats(AOrionPlayerController* PC);

	UFUNCTION(BlueprintCallable, Category = Stats)
		void SetInitialized(bool bInit) { bInitialized = bInit; }

	UPROPERTY(Replicated, BlueprintReadWrite, Category = Stats)
		TArray<FPlayerStats> aStats;

	void AddStatValue(EStatID ID, int32 Value);

	UFUNCTION(BlueprintCallable, Category = Stats)
		void SetStatValues(TArray<FPlayerStats> StatsRead);
	
private:
	bool bInitialized;
};
