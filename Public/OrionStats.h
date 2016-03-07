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
	STAT_TECHKILL,
	STAT_PYROKILL,
	STAT_MARKSMANKILL,

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
	STAT_TECHDEATH,
	STAT_PYRODEATH,
	STAT_MARKSMANDEATH,

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
	STAT_SILENCEDSMGKILLS,
	STAT_SILENCEDSMGBULLETSFIRED,
	STAT_SILENCEDSMGBULLETSHIT,
	STAT_AUTOSHOTGUNKILLS,
	STAT_AUTOSHOTGUNBULLETSFIRED,
	STAT_AUTOSHOTGUNBULLETSHIT,
	STAT_SNIPERKILLS,
	STAT_SNIPERBULLETSFIRED,
	STAT_SNIPERBULLETSHIT,

	STAT_CRAFTITEMS,

	//levels
	STAT_RELICLOSSES,
	STAT_RELICWINS,
	STAT_OUTBACKLOSSES,
	STAT_OUTBACKWINS,
	STAT_ARIDLOSSES,
	STAT_ARIDWINS,
	STAT_SUMMITLOSSES,
	STAT_SUMMITWINS,
	STAT_WHITEOUTLOSSES,
	STAT_WHITEOUTWINS,

	//exp
	STAT_SUPPORTEXP,
	STAT_ASSAULTEXP,
	STAT_RECONEXP,
	STAT_TECHEXP,
	STAT_PYROEXP,
	STAT_MARKSMANEXP,
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
	STAT_ELITETECHKILL,
	STAT_ELITEPYROKILL,
	STAT_ELITEMARKSMANKILL,
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
	STAT_ELITETECHDEATH,
	STAT_ELITEPYRODEATH,
	STAT_ELITEMARKSMANDEATH,

	//pvp stats
	STAT_PVPWINS,
	STAT_PVPLOSSES,
	STAT_PVPKILLS,
	STAT_PVPDEATHS,

	//misc
	STAT_SMOKEGRENADESTHROWN,
	STAT_FRAGGRENADESTHROWN,
	STAT_EMPGRENADESTHROWN,
	STAT_STUNGRENADESTHROWN,
	STAT_NAPALMGRENADESTHROWN,
	STAT_TIMESROLLED,
	STAT_TIMESBLINKED,

	//healing
	STAT_HEALINGDONE,
	STAT_HEALINGRECEIVED,
	STAT_TEAMMATESREVIVED,
	STAT_TIMESREVIVED,

	//orbs
	STAT_HEALTHORBS,
	STAT_STOPPINGORBS,
	STAT_ROFORBS,
	STAT_SPEEDORBS,
	STAT_REDUCTIONORBS,
	STAT_EXPORBS,

	//gore
	STAT_LIMBSBLOWNOFF,
	STAT_HEADSBLOWNOFF,

	//survival only?
	STAT_KILLSASASSAULT,
	STAT_KILLSASSUPPORT,
	STAT_KILLSASRECON,
	STAT_KILLSASTECH,
	STAT_KILLSASPYRO,
	STAT_KILLSASMARKSMAN,
	STAT_ASSISTSSASASSAULT,
	STAT_ASSISTSASSUPPORT,
	STAT_ASSISTSASRECON,
	STAT_ASSISTSASTECH,
	STAT_ASSISTSASPYRO,
	STAT_ASSISTSASMARKSMAN,
	STAT_DEATHSASASSAULT,
	STAT_DEATHSASSUPPORT,
	STAT_DEATHSASRECON,
	STAT_DEATHSASTECH,
	STAT_DEATHSASPYRO,
	STAT_DEATHSASMARKSMAN,

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

	//overalls
	STAT_ASSISTS,

	//for leaderboards
	STAT_TOTALKILLS,
	STAT_ACHIEVEMENTPOINTS,

	//slaughter
	STAT_MAXSLAUGHTERWAVE,
	STAT_MAXSLAUGHTERKILLS,

	//fatalities
	STAT_COMPYSLICE,

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
	UPROPERTY(BlueprintReadWrite, Category = Stats)
		FString FriendlyName;	//name used in the menu

	FPlayerStats()
	{
		StatID = -1;
		StatValue = 0;
		StatName = TEXT("");
		FriendlyName = TEXT("");
		bDirty = false;
	}

	FPlayerStats(EStatID ID, FString sName, FString fName)
	{
		StatID = int32(ID);
		StatValue = 0;
		StatName = sName;
		FriendlyName = fName;
		bDirty = false;
	}

	bool operator==(const FPlayerStats Other) const { return StatName == Other.StatName; }
};

USTRUCT(BlueprintType)
struct FEnhancedPlayerStats
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Category = Stats)
		FString FriendlyName;
	UPROPERTY(BlueprintReadWrite, Category = Stats)
		FString StatValue;

	FEnhancedPlayerStats()
	{
		StatValue = TEXT("");
		FriendlyName = TEXT("");
	}

	bool operator==(const FEnhancedPlayerStats Other) const { return FriendlyName == Other.FriendlyName; }
};

UCLASS()
class ORION_API AOrionStats : public AActor
{
	GENERATED_BODY()
public:
	AOrionStats(const FObjectInitializer& ObjectInitializer);

	//UPROPERTY(Replicated);
		AOrionPlayerController *PCOwner;

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

	UPROPERTY(/*Replicated, */BlueprintReadWrite, Category = Stats)
		TArray<FPlayerStats> aStats;

	void FillInLevels();

	UFUNCTION(BlueprintCallable, Category = Stats)
		void AddCraftStat();

	void AddStatValue(EStatID ID, int32 Value);

	void SetStatValue(EStatID ID, int32 Value);

	UFUNCTION(BlueprintCallable, Category = Stats)
		void SetStatValues(TArray<FPlayerStats> StatsRead);

	void SaveAllStats();

	UPROPERTY(BlueprintReadWrite, Category = Playfab)
		bool bSaveInProgress;

	FTimerHandle SaveTimer;
	AOrionPlayerController *SaveStatsPC;

	UFUNCTION(BlueprintCallable, Category = Stats)
		TArray<FEnhancedPlayerStats> GetEnhancedStats();

	FString GetDinoNameFromStat(EStatID ID);
	
private:
	bool bInitialized;
};
