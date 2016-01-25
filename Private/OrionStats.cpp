// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionStats.h"

AOrionStats::AOrionStats(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//initialize our stats arrray
	aStats.Empty();

	aStats.Add(FPlayerStats(STAT_RAPTORKILL, TEXT("RaptorKills")));
	aStats.Add(FPlayerStats(STAT_TREXKILL, TEXT("TRexKills")));
	aStats.Add(FPlayerStats(STAT_COMPYKILL, TEXT("CompyKills")));
	aStats.Add(FPlayerStats(STAT_DILOKILL, TEXT("DiloKills")));
	aStats.Add(FPlayerStats(STAT_RHAMKILL, TEXT("RhamKills")));
	aStats.Add(FPlayerStats(STAT_PTERAKILL, TEXT("PteraKills")));
	aStats.Add(FPlayerStats(STAT_STEGKILL, TEXT("StegKills")));
	aStats.Add(FPlayerStats(STAT_TRIKEKILL, TEXT("TrikeKills")));
	aStats.Add(FPlayerStats(STAT_ANKYKILL, TEXT("AnkyKills")));
	aStats.Add(FPlayerStats(STAT_PARAKILL, TEXT("ParaKills")));
	aStats.Add(FPlayerStats(STAT_ALLOKILL, TEXT("AlloKills")));
	aStats.Add(FPlayerStats(STAT_KRUGERKILL, TEXT("KrugerKills")));
	aStats.Add(FPlayerStats(STAT_JECKYLKILL, TEXT("JeckylKills")));
	aStats.Add(FPlayerStats(STAT_NAMORKILL, TEXT("NamorKills")));
	aStats.Add(FPlayerStats(STAT_VIPERKILL, TEXT("ViperKills")));
	aStats.Add(FPlayerStats(STAT_BONESKILL, TEXT("BonesKills")));
	aStats.Add(FPlayerStats(STAT_ORBKILL, TEXT("OrbKills")));
	aStats.Add(FPlayerStats(STAT_GRUMPSKILL, TEXT("GrumpsKills")));
	aStats.Add(FPlayerStats(STAT_SPIDERKILL, TEXT("SpiderKills")));
	aStats.Add(FPlayerStats(STAT_SUPPORTKILL, TEXT("SupportKills")));
	aStats.Add(FPlayerStats(STAT_RECONKILL, TEXT("ReconKills")));
	aStats.Add(FPlayerStats(STAT_ASSAULTKILL, TEXT("AssaultKills")));
	aStats.Add(FPlayerStats(STAT_TECHKILL, TEXT("TechKills")));
	aStats.Add(FPlayerStats(STAT_PYROKILL, TEXT("PyroKills")));

	aStats.Add(FPlayerStats(STAT_RAPTORDEATH, TEXT("RaptorDeaths")));
	aStats.Add(FPlayerStats(STAT_TREXDEATH, TEXT("TRexDeaths")));
	aStats.Add(FPlayerStats(STAT_COMPYDEATH, TEXT("CompyDeaths")));
	aStats.Add(FPlayerStats(STAT_DILODEATH, TEXT("DiloDeaths")));
	aStats.Add(FPlayerStats(STAT_RHAMDEATH, TEXT("RhamDeaths")));
	aStats.Add(FPlayerStats(STAT_PTERADEATH, TEXT("PteraDeaths")));
	aStats.Add(FPlayerStats(STAT_STEGDEATH, TEXT("StegDeaths")));
	aStats.Add(FPlayerStats(STAT_TRIKEDEATH, TEXT("TrikeDeaths")));
	aStats.Add(FPlayerStats(STAT_ANKYDEATH, TEXT("AnkyDeaths")));
	aStats.Add(FPlayerStats(STAT_PARADEATH, TEXT("ParaDeaths")));
	aStats.Add(FPlayerStats(STAT_ALLODEATH, TEXT("AlloDeaths")));
	aStats.Add(FPlayerStats(STAT_KRUGERDEATH, TEXT("KrugerDeaths")));
	aStats.Add(FPlayerStats(STAT_JECKYLDEATH, TEXT("JeckylDeaths")));
	aStats.Add(FPlayerStats(STAT_NAMORDEATH, TEXT("NamorDeaths")));
	aStats.Add(FPlayerStats(STAT_VIPERDEATH, TEXT("ViperDeaths")));
	aStats.Add(FPlayerStats(STAT_BONESDEATH, TEXT("BonesDeaths")));
	aStats.Add(FPlayerStats(STAT_ORBDEATH, TEXT("OrbDeaths")));
	aStats.Add(FPlayerStats(STAT_GRUMPSDEATH, TEXT("GrumpsDeaths")));
	aStats.Add(FPlayerStats(STAT_SPIDERDEATH, TEXT("SpiderDeaths")));
	aStats.Add(FPlayerStats(STAT_SUPPORTDEATH, TEXT("SupportDeaths")));
	aStats.Add(FPlayerStats(STAT_RECONDEATH, TEXT("ReconDeaths")));
	aStats.Add(FPlayerStats(STAT_ASSAULTDEATH, TEXT("AssaultDeaths")));
	aStats.Add(FPlayerStats(STAT_TECHDEATH, TEXT("TechDeaths")));
	aStats.Add(FPlayerStats(STAT_PYRODEATH, TEXT("PyroDeaths")));

	aStats.Add(FPlayerStats(STAT_MAGNUMKILLS, TEXT("MagnumKills")));
	aStats.Add(FPlayerStats(STAT_MAGNUMBULLETSFIRED, TEXT("MagnumBulletsFired")));
	aStats.Add(FPlayerStats(STAT_MAGNUMBULLETSHIT, TEXT("MagnumBulletsHit")));
	aStats.Add(FPlayerStats(STAT_PISTOLKILLS, TEXT("PistolKills")));
	aStats.Add(FPlayerStats(STAT_PISTOLBULLETSFIRED, TEXT("PistolBulletsFired")));
	aStats.Add(FPlayerStats(STAT_PISTOLBULLETSHIT, TEXT("PistolBulletsHit")));
	aStats.Add(FPlayerStats(STAT_AUTOPISTOLKILLS, TEXT("AutoPistolKills")));
	aStats.Add(FPlayerStats(STAT_AUTOPISTOLBULLETSFIRED, TEXT("AutoPistolBulletsFired")));
	aStats.Add(FPlayerStats(STAT_AUTOPISTOLBULLETSHIT, TEXT("AutoPistolBulletsHit")));
	aStats.Add(FPlayerStats(STAT_COMBATSHOTGUNKILLS, TEXT("CombatShotgunKills")));
	aStats.Add(FPlayerStats(STAT_COMBATSHOTGUNBULLETSFIRED, TEXT("CombatShotgunBulletsFired")));
	aStats.Add(FPlayerStats(STAT_COMBATSHOTGUNBULLETSHIT, TEXT("CombatShotgunBulletsHit")));
	aStats.Add(FPlayerStats(STAT_AUTORIFLEKILLS, TEXT("AutoRifleKills")));
	aStats.Add(FPlayerStats(STAT_AUTORIFLEBULLETSFIRED, TEXT("AutoRifleBulletsFired")));
	aStats.Add(FPlayerStats(STAT_AUTORIFLEBULLETSHIT, TEXT("AutoRifleBulletsHit")));
	aStats.Add(FPlayerStats(STAT_SMGKILLS, TEXT("SMGKills")));
	aStats.Add(FPlayerStats(STAT_SMGBULLETSFIRED, TEXT("SMGBulletsFired")));
	aStats.Add(FPlayerStats(STAT_SMGBULLETSHIT, TEXT("SMGBulletsHit")));
	aStats.Add(FPlayerStats(STAT_SILENCEDSMGKILLS, TEXT("SilencedSMGKills")));
	aStats.Add(FPlayerStats(STAT_SILENCEDSMGBULLETSFIRED, TEXT("SilencedSMGBulletsFired")));
	aStats.Add(FPlayerStats(STAT_SILENCEDSMGBULLETSHIT, TEXT("SilencedSMGBulletsHit")));

	aStats.Add(FPlayerStats(STAT_RELICLOSSES, TEXT("RelicLosses")));
	aStats.Add(FPlayerStats(STAT_RELICWINS, TEXT("RelicWins")));
	aStats.Add(FPlayerStats(STAT_RELICLOSSES, TEXT("OutbackLosses")));
	aStats.Add(FPlayerStats(STAT_RELICWINS, TEXT("OutbackWins")));
	aStats.Add(FPlayerStats(STAT_ARIDLOSSES, TEXT("AridLosses")));
	aStats.Add(FPlayerStats(STAT_ARIDWINS, TEXT("AridWins")));
	aStats.Add(FPlayerStats(STAT_SUMMITLOSSES, TEXT("SummitLosses")));
	aStats.Add(FPlayerStats(STAT_SUMMITWINS, TEXT("SummitWins")));

	aStats.Add(FPlayerStats(STAT_SUPPORTEXP, TEXT("SupportExp")));
	aStats.Add(FPlayerStats(STAT_ASSAULTEXP, TEXT("AssaultExp")));
	aStats.Add(FPlayerStats(STAT_RECONEXP, TEXT("ReconExp")));
	aStats.Add(FPlayerStats(STAT_TECHEXP, TEXT("TechExp")));
	aStats.Add(FPlayerStats(STAT_PYROEXP, TEXT("PyroExp")));
	aStats.Add(FPlayerStats(STAT_EXPEARNED, TEXT("ExpEarned")));

	aStats.Add(FPlayerStats(STAT_ELITERAPTORKILL, TEXT("EliteRaptorKills")));
	aStats.Add(FPlayerStats(STAT_ELITETREXKILL, TEXT("EliteTRexKills")));
	aStats.Add(FPlayerStats(STAT_ELITECOMPYKILL, TEXT("EliteCompyKills")));
	aStats.Add(FPlayerStats(STAT_ELITEDILOKILL, TEXT("EliteDiloKills")));
	aStats.Add(FPlayerStats(STAT_ELITERHAMKILL, TEXT("EliteRhamKills")));
	aStats.Add(FPlayerStats(STAT_ELITEPTERAKILL, TEXT("ElitePteraKills")));
	aStats.Add(FPlayerStats(STAT_ELITESTEGKILL, TEXT("EliteStegKills")));
	aStats.Add(FPlayerStats(STAT_ELITETRIKEKILL, TEXT("EliteTrikeKills")));
	aStats.Add(FPlayerStats(STAT_ELITEANKYKILL, TEXT("EliteAnkyKills")));
	aStats.Add(FPlayerStats(STAT_ELITEPARAKILL, TEXT("EliteParaKills")));
	aStats.Add(FPlayerStats(STAT_ELITEALLOKILL, TEXT("EliteAlloKills")));
	aStats.Add(FPlayerStats(STAT_ELITEKRUGERKILL, TEXT("EliteKrugerKills")));
	aStats.Add(FPlayerStats(STAT_ELITEJECKYLKILL, TEXT("EliteJeckylKills")));
	aStats.Add(FPlayerStats(STAT_ELITENAMORKILL, TEXT("EliteNamorKills")));
	aStats.Add(FPlayerStats(STAT_ELITEVIPERKILL, TEXT("EliteViperKills")));
	aStats.Add(FPlayerStats(STAT_ELITEBONESKILL, TEXT("EliteBonesKills")));
	aStats.Add(FPlayerStats(STAT_ELITEORBKILL, TEXT("EliteOrbKills")));
	aStats.Add(FPlayerStats(STAT_ELITEGRUMPSKILL, TEXT("EliteGrumpsKills")));
	aStats.Add(FPlayerStats(STAT_ELITESPIDERKILL, TEXT("EliteSpiderKills")));
	aStats.Add(FPlayerStats(STAT_ELITESUPPORTKILL, TEXT("EliteSupportKills")));
	aStats.Add(FPlayerStats(STAT_ELITERECONKILL, TEXT("EliteReconKills")));
	aStats.Add(FPlayerStats(STAT_ELITEASSAULTKILL, TEXT("EliteAssaultKills")));
	aStats.Add(FPlayerStats(STAT_ELITETECHKILL, TEXT("EliteTechKills")));
	aStats.Add(FPlayerStats(STAT_ELITEPYROKILL, TEXT("ElitePyroKills")));

	aStats.Add(FPlayerStats(STAT_ELITERAPTORDEATH, TEXT("EliteRaptorDeaths")));
	aStats.Add(FPlayerStats(STAT_ELITETREXDEATH, TEXT("EliteTRexDeaths")));
	aStats.Add(FPlayerStats(STAT_ELITECOMPYDEATH, TEXT("EliteCompyDeaths")));
	aStats.Add(FPlayerStats(STAT_ELITEDILODEATH, TEXT("EliteDiloDeaths")));
	aStats.Add(FPlayerStats(STAT_ELITERHAMDEATH, TEXT("EliteRhamDeaths")));
	aStats.Add(FPlayerStats(STAT_ELITEPTERADEATH, TEXT("ElitePteraDeaths")));
	aStats.Add(FPlayerStats(STAT_ELITESTEGDEATH, TEXT("EliteStegDeaths")));
	aStats.Add(FPlayerStats(STAT_ELITETRIKEDEATH, TEXT("EliteTrikeDeaths")));
	aStats.Add(FPlayerStats(STAT_ELITEANKYDEATH, TEXT("EliteAnkyDeaths")));
	aStats.Add(FPlayerStats(STAT_ELITEPARADEATH, TEXT("EliteParaDeaths")));
	aStats.Add(FPlayerStats(STAT_ELITEALLODEATH, TEXT("EliteAlloDeaths")));
	aStats.Add(FPlayerStats(STAT_ELITEKRUGERDEATH, TEXT("EliteKrugerDeaths")));
	aStats.Add(FPlayerStats(STAT_ELITEJECKYLDEATH, TEXT("EliteJeckylDeaths")));
	aStats.Add(FPlayerStats(STAT_ELITENAMORDEATH, TEXT("EliteNamorDeaths")));
	aStats.Add(FPlayerStats(STAT_ELITEVIPERDEATH, TEXT("EliteViperDeaths")));
	aStats.Add(FPlayerStats(STAT_ELITEBONESDEATH, TEXT("EliteBonesDeaths")));
	aStats.Add(FPlayerStats(STAT_ELITEORBDEATH, TEXT("EliteOrbDeaths")));
	aStats.Add(FPlayerStats(STAT_ELITEGRUMPSDEATH, TEXT("EliteGrumpsDeaths")));
	aStats.Add(FPlayerStats(STAT_ELITESPIDERDEATH, TEXT("EliteSpiderDeaths")));
	aStats.Add(FPlayerStats(STAT_ELITESUPPORTDEATH, TEXT("EliteSupportDeaths")));
	aStats.Add(FPlayerStats(STAT_ELITERECONDEATH, TEXT("EliteReconDeaths")));
	aStats.Add(FPlayerStats(STAT_ELITEASSAULTDEATH, TEXT("EliteAssaultDeaths")));
	aStats.Add(FPlayerStats(STAT_ELITETECHDEATH, TEXT("EliteTechDeaths")));
	aStats.Add(FPlayerStats(STAT_ELITEPYRODEATH, TEXT("ElitePyroDeaths")));

	aStats.Add(FPlayerStats(STAT_PVPWINS, TEXT("PVPWins")));
	aStats.Add(FPlayerStats(STAT_PVPLOSSES, TEXT("PVPLosses")));
	aStats.Add(FPlayerStats(STAT_PVPKILLS, TEXT("PVPKills")));
	aStats.Add(FPlayerStats(STAT_PVPDEATHS, TEXT("PVPDeaths")));

	aStats.Add(FPlayerStats(STAT_SMOKEGRENADESTHROWN, TEXT("SmokeGrenadesThrown")));
	aStats.Add(FPlayerStats(STAT_FRAGGRENADESTHROWN, TEXT("FragGrenadesThrown")));
	aStats.Add(FPlayerStats(STAT_EMPGRENADESTHROWN, TEXT("EMPGrenadesThrown")));
	aStats.Add(FPlayerStats(STAT_STUNGRENADESTHROWN, TEXT("StunGrenadesThrown")));
	aStats.Add(FPlayerStats(STAT_TIMESROLLED, TEXT("TimesRolled")));
	aStats.Add(FPlayerStats(STAT_TIMESBLINKED, TEXT("TimesBlinked")));

	aStats.Add(FPlayerStats(STAT_HEALINGDONE, TEXT("HealingDone")));
	aStats.Add(FPlayerStats(STAT_HEALINGRECEIVED, TEXT("HealingReceived")));
	aStats.Add(FPlayerStats(STAT_TEAMMATESREVIVED, TEXT("TeammatesRevived")));
	aStats.Add(FPlayerStats(STAT_TIMESREVIVED, TEXT("TimesRevived")));

	aStats.Add(FPlayerStats(STAT_HEALTHORBS, TEXT("HealthOrbs")));
	aStats.Add(FPlayerStats(STAT_STOPPINGORBS, TEXT("StoppingOrbs")));
	aStats.Add(FPlayerStats(STAT_ROFORBS, TEXT("ROFOrbs")));
	aStats.Add(FPlayerStats(STAT_SPEEDORBS, TEXT("SpeedOrbs")));
	aStats.Add(FPlayerStats(STAT_REDUCTIONORBS, TEXT("ReductionOrbs")));
	aStats.Add(FPlayerStats(STAT_EXPORBS, TEXT("ExpOrbs")));

	aStats.Add(FPlayerStats(STAT_LIMBSBLOWNOFF, TEXT("LimbsBlownOff")));
	aStats.Add(FPlayerStats(STAT_HEADSBLOWNOFF, TEXT("HeadsBlownOff")));
	
	aStats.Add(FPlayerStats(STAT_KILLSASASSAULT, TEXT("KillsAsAssault")));
	aStats.Add(FPlayerStats(STAT_KILLSASSUPPORT, TEXT("KillsAsSupport")));
	aStats.Add(FPlayerStats(STAT_KILLSASRECON, TEXT("KillsAsRecon")));
	aStats.Add(FPlayerStats(STAT_KILLSASTECH, TEXT("KillsAsTech")));
	aStats.Add(FPlayerStats(STAT_KILLSASPYRO, TEXT("KillsAsPyro")));
	aStats.Add(FPlayerStats(STAT_KILLSASASSAULT, TEXT("AssistsAsAssault")));
	aStats.Add(FPlayerStats(STAT_KILLSASSUPPORT, TEXT("AssistsAsSupport")));
	aStats.Add(FPlayerStats(STAT_KILLSASRECON, TEXT("AssistsAsRecon")));
	aStats.Add(FPlayerStats(STAT_KILLSASTECH, TEXT("AssistsAsTech")));
	aStats.Add(FPlayerStats(STAT_KILLSASPYRO, TEXT("AssistsAsPyro")));
	aStats.Add(FPlayerStats(STAT_DEATHSASASSAULT, TEXT("DeathsAsAssault")));
	aStats.Add(FPlayerStats(STAT_DEATHSASSUPPORT, TEXT("DeathsAsSupport")));
	aStats.Add(FPlayerStats(STAT_DEATHSASRECON, TEXT("DeathsAsRecon")));
	aStats.Add(FPlayerStats(STAT_DEATHSASTECH, TEXT("DeathsAsTech")));
	aStats.Add(FPlayerStats(STAT_DEATHSASPYRO, TEXT("DeathsAsPyro")));

	aStats.Add(FPlayerStats(STAT_EASYWINS, TEXT("EasyWins")));
	aStats.Add(FPlayerStats(STAT_EASYLOSSES, TEXT("EasyLosses")));
	aStats.Add(FPlayerStats(STAT_NORMALWINS, TEXT("NormalWins")));
	aStats.Add(FPlayerStats(STAT_NORMALLOSSES, TEXT("NormalLosses")));
	aStats.Add(FPlayerStats(STAT_HARDWINS, TEXT("HardWins")));
	aStats.Add(FPlayerStats(STAT_HARDLOSSES, TEXT("HardLosses")));
	aStats.Add(FPlayerStats(STAT_INSANEWINS, TEXT("InsaneWins")));
	aStats.Add(FPlayerStats(STAT_INSANELOSSES, TEXT("InsaneLosses")));
	aStats.Add(FPlayerStats(STAT_REDIKULOUSWINS, TEXT("RedikulousWins")));
	aStats.Add(FPlayerStats(STAT_REDIKULOUSLOSSES, TEXT("RedikulousLosses")));

	aStats.Add(FPlayerStats(STAT_GENERATORHEALINGDONE, TEXT("GeneratorHealingDone")));
	aStats.Add(FPlayerStats(STAT_GENERATORSREPAIRED, TEXT("GeneratorsRepaired")));

	aStats.Add(FPlayerStats(STAT_ASSISTS, TEXT("Assists")));

	aStats.Add(FPlayerStats(STAT_TOTALKILLS, TEXT("totalkills")));
	aStats.Add(FPlayerStats(STAT_ACHIEVEMENTPOINTS, TEXT("achpoints")));

	aStats.Add(FPlayerStats(STAT_MAXSLAUGHTERWAVE, TEXT("maxslaughterwave")));
	aStats.Add(FPlayerStats(STAT_MAXSLAUGHTERKILLS, TEXT("maxslaughterkills")));

	aStats.Add(FPlayerStats(STAT_COMPYSLICE, TEXT("compyslice")));

	bInitialized = false;

	bReplicates = true;

	bSaveInProgress = false;
}

/*void AOrionStats::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// everyone
	DOREPLIFETIME_CONDITION(AOrionStats, aStats, COND_OwnerOnly);
}*/

void AOrionStats::ReadPlayerStats(AOrionPlayerController* PC)
{
	EventReadPlayerStats(PC);
}

void AOrionStats::AddStatValue(EStatID ID, int32 Value)
{
	if(bInitialized)
	{
		if (aStats.Num() > ID)
		{
			aStats[ID].bDirty = true;

			//don't want people going over the level cap of 30
			if (ID == STAT_RECONEXP || ID == STAT_ASSAULTEXP || ID == STAT_SUPPORTEXP || ID == STAT_TECHEXP || ID == STAT_PYROEXP)
				aStats[ID].StatValue = FMath::Min(aStats[ID].StatValue + Value, CalculateExpToLevel(30));
			else
				aStats[ID].StatValue += Value;

			if (PCOwner && PCOwner->GetAchievements())
			{
				PCOwner->GetAchievements()->CheckForStatUnlocks();
			}
		}
	}
}

void AOrionStats::SetStatValue(EStatID ID, int32 Value)
{
	if (bInitialized)
	{
		if (aStats.Num() > ID)
		{
			aStats[ID].bDirty = true;
			aStats[ID].StatValue = FMath::Max(aStats[ID].StatValue, Value);

			if (PCOwner && PCOwner->GetAchievements())
			{
				PCOwner->GetAchievements()->CheckForStatUnlocks();
			}
		}
	}
}

void AOrionStats::SaveAllStats()
{
	FlushPlayerStats(SaveStatsPC);
}

void AOrionStats::FlushPlayerStats(AOrionPlayerController* PC)
{
#if IS_SERVER
	if(bInitialized && PC)
	{
		//if we have a save already in progress, just delay us
		if(bSaveInProgress)
		{
			SaveStatsPC = PC;
			GetWorldTimerManager().SetTimer(SaveTimer, this, &AOrionStats::SaveAllStats, 0.1f, false);
			return;
		}

		SaveStatsPC = nullptr;

		EventSavePlayerStats(PC);

		//reset all stat values to non dirty
		for (int32 i = 0; i < aStats.Num(); i++)
			aStats[i].bDirty = false;
	}
#endif
}

/*std::map<std::string, PlayFab::Int32> AOrionStats::GetStatsMap()
{
	std::map<std::string, PlayFab::Int32> Map;

	for (int32 i = 0; i < aStats.Num(); i++)
	{
		std::string index = std::to_string(int(aStats[i].StatID));
		Map[index] = (aStats[i].StatType == STATTYPE_INT ? PlayFab::Int32(aStats[i].StatValueInt) : PlayFab::Int32(aStats[i].StatValueFloat));
	}

	return Map;
}*/

void AOrionStats::SetStatValues(TArray<FPlayerStats> StatsRead)
{
	for (int32 i = 0; i < StatsRead.Num(); i++)
	{
		int32 index = aStats.Find(StatsRead[i]);

		if (index != INDEX_NONE)
		{
			aStats[index].StatValue = StatsRead[i].StatValue;
		}
	}

	TArray<AActor*> Controllers;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrionPlayerController::StaticClass(), Controllers);

	for (int32 i = 0; i < Controllers.Num(); i++)
	{
		AOrionPlayerController *C = Cast<AOrionPlayerController>(Controllers[i]);
		if (C && C->GetStats() == this)
		{
			AOrionPRI *PRI = Cast<AOrionPRI>(C->PlayerState);
			//need to force the xp values here for initial load
			if (PRI)
			{
				PRI->AssaultXP = aStats[STAT_ASSAULTEXP].StatValue;
				PRI->SupportXP = aStats[STAT_SUPPORTEXP].StatValue;
				PRI->ReconXP = aStats[STAT_RECONEXP].StatValue;
				PRI->TechXP = aStats[STAT_TECHEXP].StatValue;
				PRI->PyroXP = aStats[STAT_PYROEXP].StatValue;
			}
		}
	}
}