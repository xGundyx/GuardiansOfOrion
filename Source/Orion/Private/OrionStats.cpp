// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionStats.h"

AOrionStats::AOrionStats(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//initialize our stats arrray
	aStats.Empty();

	aStats.Add(FPlayerStats(STAT_RAPTORKILL, TEXT("RaptorKills"), TEXT("RAPTORS KILLED")));
	aStats.Add(FPlayerStats(STAT_TREXKILL, TEXT("TRexKills"), TEXT("T-REX KILLED")));
	aStats.Add(FPlayerStats(STAT_COMPYKILL, TEXT("CompyKills"), TEXT("COMPIES KILLED")));
	aStats.Add(FPlayerStats(STAT_DILOKILL, TEXT("DiloKills"), TEXT("DILOS KILLED")));
	aStats.Add(FPlayerStats(STAT_RHAMKILL, TEXT("RhamKills"), TEXT("RHAMS KILLED")));
	aStats.Add(FPlayerStats(STAT_PTERAKILL, TEXT("PteraKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_STEGKILL, TEXT("StegKills"), TEXT("STEGOSAURUS KILLED")));
	aStats.Add(FPlayerStats(STAT_TRIKEKILL, TEXT("TrikeKills"), TEXT("TRICERATOPS KILLED")));
	aStats.Add(FPlayerStats(STAT_ANKYKILL, TEXT("AnkyKills"), TEXT("ANKIES KILLED")));
	aStats.Add(FPlayerStats(STAT_PARAKILL, TEXT("ParaKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ALLOKILL, TEXT("AlloKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_KRUGERKILL, TEXT("KrugerKills"), TEXT("KRUGERS KILLED")));
	aStats.Add(FPlayerStats(STAT_JECKYLKILL, TEXT("JeckylKills"), TEXT("JECKYLS KILLED")));
	aStats.Add(FPlayerStats(STAT_NAMORKILL, TEXT("NamorKills"), TEXT("NAMORS KILLED")));
	aStats.Add(FPlayerStats(STAT_VIPERKILL, TEXT("ViperKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_BONESKILL, TEXT("BonesKills"), TEXT("BONES KILLED")));
	aStats.Add(FPlayerStats(STAT_ORBKILL, TEXT("OrbKills"), TEXT("ORBS KILLED")));
	aStats.Add(FPlayerStats(STAT_GRUMPSKILL, TEXT("GrumpsKills"), TEXT("GRUMPS KILLED")));
	aStats.Add(FPlayerStats(STAT_SPIDERKILL, TEXT("SpiderKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_SUPPORTKILL, TEXT("SupportKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_RECONKILL, TEXT("ReconKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ASSAULTKILL, TEXT("AssaultKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_TECHKILL, TEXT("TechKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_PYROKILL, TEXT("PyroKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_MARKSMANKILL, TEXT("MarksmanKills"), TEXT("")));

	aStats.Add(FPlayerStats(STAT_RAPTORDEATH, TEXT("RaptorDeaths"), TEXT("RAPTOR DEATHS")));
	aStats.Add(FPlayerStats(STAT_TREXDEATH, TEXT("TRexDeaths"), TEXT("TREX DEATHS")));
	aStats.Add(FPlayerStats(STAT_COMPYDEATH, TEXT("CompyDeaths"), TEXT("COMPY DEATHS")));
	aStats.Add(FPlayerStats(STAT_DILODEATH, TEXT("DiloDeaths"), TEXT("DILO DEATHS")));
	aStats.Add(FPlayerStats(STAT_RHAMDEATH, TEXT("RhamDeaths"), TEXT("RHAM DEATHS")));
	aStats.Add(FPlayerStats(STAT_PTERADEATH, TEXT("PteraDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_STEGDEATH, TEXT("StegDeaths"), TEXT("STEGOSAURUS DEATHS")));
	aStats.Add(FPlayerStats(STAT_TRIKEDEATH, TEXT("TrikeDeaths"), TEXT("TRICERATOPS DEATHS")));
	aStats.Add(FPlayerStats(STAT_ANKYDEATH, TEXT("AnkyDeaths"), TEXT("ANKY DEATHS")));
	aStats.Add(FPlayerStats(STAT_PARADEATH, TEXT("ParaDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ALLODEATH, TEXT("AlloDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_KRUGERDEATH, TEXT("KrugerDeaths"), TEXT("KRUGER DEATHS")));
	aStats.Add(FPlayerStats(STAT_JECKYLDEATH, TEXT("JeckylDeaths"), TEXT("JECKYL DEATHS")));
	aStats.Add(FPlayerStats(STAT_NAMORDEATH, TEXT("NamorDeaths"), TEXT("NAMOR DEATHS")));
	aStats.Add(FPlayerStats(STAT_VIPERDEATH, TEXT("ViperDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_BONESDEATH, TEXT("BonesDeaths"), TEXT("BONES DEATHS")));
	aStats.Add(FPlayerStats(STAT_ORBDEATH, TEXT("OrbDeaths"), TEXT("ORB DEATHS")));
	aStats.Add(FPlayerStats(STAT_GRUMPSDEATH, TEXT("GrumpsDeaths"), TEXT("GRUMPS DEATHS")));
	aStats.Add(FPlayerStats(STAT_SPIDERDEATH, TEXT("SpiderDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_SUPPORTDEATH, TEXT("SupportDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_RECONDEATH, TEXT("ReconDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ASSAULTDEATH, TEXT("AssaultDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_TECHDEATH, TEXT("TechDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_PYRODEATH, TEXT("PyroDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_MARKSMANDEATH, TEXT("MarksmanDeaths"), TEXT("")));

	aStats.Add(FPlayerStats(STAT_MAGNUMKILLS, TEXT("MagnumKills"), TEXT("KILLS WITH MAGNUM")));
	aStats.Add(FPlayerStats(STAT_MAGNUMBULLETSFIRED, TEXT("MagnumBulletsFired"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_MAGNUMBULLETSHIT, TEXT("MagnumBulletsHit"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_PISTOLKILLS, TEXT("PistolKills"), TEXT("KILLS WITH PISTOL")));
	aStats.Add(FPlayerStats(STAT_PISTOLBULLETSFIRED, TEXT("PistolBulletsFired"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_PISTOLBULLETSHIT, TEXT("PistolBulletsHit"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_AUTOPISTOLKILLS, TEXT("AutoPistolKills"), TEXT("AUTO PISTOL KILLS")));
	aStats.Add(FPlayerStats(STAT_AUTOPISTOLBULLETSFIRED, TEXT("AutoPistolBulletsFired"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_AUTOPISTOLBULLETSHIT, TEXT("AutoPistolBulletsHit"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_COMBATSHOTGUNKILLS, TEXT("CombatShotgunKills"), TEXT("COMBAT SHOTGUN KILL")));
	aStats.Add(FPlayerStats(STAT_COMBATSHOTGUNBULLETSFIRED, TEXT("CombatShotgunBulletsFired"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_COMBATSHOTGUNBULLETSHIT, TEXT("CombatShotgunBulletsHit"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_AUTORIFLEKILLS, TEXT("AutoRifleKills"), TEXT("AUTO RIFLE KILLS")));
	aStats.Add(FPlayerStats(STAT_AUTORIFLEBULLETSFIRED, TEXT("AutoRifleBulletsFired"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_AUTORIFLEBULLETSHIT, TEXT("AutoRifleBulletsHit"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_SMGKILLS, TEXT("SMGKills"), TEXT("SMG KILLS")));
	aStats.Add(FPlayerStats(STAT_SMGBULLETSFIRED, TEXT("SMGBulletsFired"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_SMGBULLETSHIT, TEXT("SMGBulletsHit"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_SILENCEDSMGKILLS, TEXT("SilencedSMGKills"), TEXT("SILENCED SMG KILLS")));
	aStats.Add(FPlayerStats(STAT_SILENCEDSMGBULLETSFIRED, TEXT("SilencedSMGBulletsFired"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_SILENCEDSMGBULLETSHIT, TEXT("SilencedSMGBulletsHit"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_AUTOSHOTGUNKILLS, TEXT("AutoShotgunKills"), TEXT("AUTO SHOTGUN KILLS")));
	aStats.Add(FPlayerStats(STAT_AUTOSHOTGUNBULLETSFIRED, TEXT("AutoShotgunBulletsFired"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_AUTOSHOTGUNBULLETSHIT, TEXT("AutoShotgunBulletsHit"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_SNIPERKILLS, TEXT("SniperKills"), TEXT("SNIPER RIFLE KILLS")));
	aStats.Add(FPlayerStats(STAT_SNIPERBULLETSFIRED, TEXT("SniperBulletsFired"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_SNIPERBULLETSHIT, TEXT("SniperBulletsHit"), TEXT("")));

	aStats.Add(FPlayerStats(STAT_CRAFTITEMS, TEXT("Crafting"), TEXT("ITEMS CRAFTED")));

	aStats.Add(FPlayerStats(STAT_RELICLOSSES, TEXT("RelicLosses"), TEXT("RELIC LOSSES")));
	aStats.Add(FPlayerStats(STAT_RELICWINS, TEXT("RelicWins"), TEXT("RELIC WINS")));
	aStats.Add(FPlayerStats(STAT_RELICLOSSES, TEXT("OutbackLosses"), TEXT("OUTBACK LOSSES")));
	aStats.Add(FPlayerStats(STAT_RELICWINS, TEXT("OutbackWins"), TEXT("OUTBACK WINS")));
	aStats.Add(FPlayerStats(STAT_ARIDLOSSES, TEXT("AridLosses"), TEXT("ARID LOSSES")));
	aStats.Add(FPlayerStats(STAT_ARIDWINS, TEXT("AridWins"), TEXT("ARID WINS")));
	aStats.Add(FPlayerStats(STAT_SUMMITLOSSES, TEXT("SummitLosses"), TEXT("SUMMIT LOSSES")));
	aStats.Add(FPlayerStats(STAT_SUMMITWINS, TEXT("SummitWins"), TEXT("SUMMIT WINS")));
	aStats.Add(FPlayerStats(STAT_WHITEOUTLOSSES, TEXT("WhiteOutLosses"), TEXT("WHITEOUT LOSSES")));
	aStats.Add(FPlayerStats(STAT_WHITEOUTWINS, TEXT("WhiteOutWins"), TEXT("WHITEOUT WINS")));

	aStats.Add(FPlayerStats(STAT_SUPPORTEXP, TEXT("SupportExp"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ASSAULTEXP, TEXT("AssaultExp"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_RECONEXP, TEXT("ReconExp"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_TECHEXP, TEXT("TechExp"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_PYROEXP, TEXT("PyroExp"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_MARKSMANEXP, TEXT("MarksmanExp"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_EXPEARNED, TEXT("ExpEarned"), TEXT("OVERALL XP EARNED")));

	aStats.Add(FPlayerStats(STAT_ELITERAPTORKILL, TEXT("EliteRaptorKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITETREXKILL, TEXT("EliteTRexKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITECOMPYKILL, TEXT("EliteCompyKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITEDILOKILL, TEXT("EliteDiloKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITERHAMKILL, TEXT("EliteRhamKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITEPTERAKILL, TEXT("ElitePteraKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITESTEGKILL, TEXT("EliteStegKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITETRIKEKILL, TEXT("EliteTrikeKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITEANKYKILL, TEXT("EliteAnkyKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITEPARAKILL, TEXT("EliteParaKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITEALLOKILL, TEXT("EliteAlloKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITEKRUGERKILL, TEXT("EliteKrugerKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITEJECKYLKILL, TEXT("EliteJeckylKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITENAMORKILL, TEXT("EliteNamorKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITEVIPERKILL, TEXT("EliteViperKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITEBONESKILL, TEXT("EliteBonesKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITEORBKILL, TEXT("EliteOrbKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITEGRUMPSKILL, TEXT("EliteGrumpsKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITESPIDERKILL, TEXT("EliteSpiderKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITESUPPORTKILL, TEXT("EliteSupportKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITERECONKILL, TEXT("EliteReconKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITEASSAULTKILL, TEXT("EliteAssaultKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITETECHKILL, TEXT("EliteTechKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITEPYROKILL, TEXT("ElitePyroKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITEMARKSMANKILL, TEXT("EliteMarksmanKills"), TEXT("")));

	aStats.Add(FPlayerStats(STAT_ELITERAPTORDEATH, TEXT("EliteRaptorDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITETREXDEATH, TEXT("EliteTRexDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITECOMPYDEATH, TEXT("EliteCompyDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITEDILODEATH, TEXT("EliteDiloDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITERHAMDEATH, TEXT("EliteRhamDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITEPTERADEATH, TEXT("ElitePteraDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITESTEGDEATH, TEXT("EliteStegDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITETRIKEDEATH, TEXT("EliteTrikeDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITEANKYDEATH, TEXT("EliteAnkyDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITEPARADEATH, TEXT("EliteParaDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITEALLODEATH, TEXT("EliteAlloDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITEKRUGERDEATH, TEXT("EliteKrugerDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITEJECKYLDEATH, TEXT("EliteJeckylDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITENAMORDEATH, TEXT("EliteNamorDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITEVIPERDEATH, TEXT("EliteViperDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITEBONESDEATH, TEXT("EliteBonesDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITEORBDEATH, TEXT("EliteOrbDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITEGRUMPSDEATH, TEXT("EliteGrumpsDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITESPIDERDEATH, TEXT("EliteSpiderDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITESUPPORTDEATH, TEXT("EliteSupportDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITERECONDEATH, TEXT("EliteReconDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITEASSAULTDEATH, TEXT("EliteAssaultDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITETECHDEATH, TEXT("EliteTechDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITEPYRODEATH, TEXT("ElitePyroDeaths"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_ELITEMARKSMANDEATH, TEXT("EliteMarksmanDeaths"), TEXT("")));

	aStats.Add(FPlayerStats(STAT_PVPWINS, TEXT("PVPWins"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_PVPLOSSES, TEXT("PVPLosses"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_PVPKILLS, TEXT("PVPKills"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_PVPDEATHS, TEXT("PVPDeaths"), TEXT("")));

	aStats.Add(FPlayerStats(STAT_SMOKEGRENADESTHROWN, TEXT("SmokeGrenadesThrown"), TEXT("SMOKE GRENADES THROWN")));
	aStats.Add(FPlayerStats(STAT_FRAGGRENADESTHROWN, TEXT("FragGrenadesThrown"), TEXT("FRAG GRENADES THROWN")));
	aStats.Add(FPlayerStats(STAT_EMPGRENADESTHROWN, TEXT("EMPGrenadesThrown"), TEXT("EMP GRENADES THROWN")));
	aStats.Add(FPlayerStats(STAT_STUNGRENADESTHROWN, TEXT("StunGrenadesThrown"), TEXT("STUN GRENADES THROWN")));
	aStats.Add(FPlayerStats(STAT_TIMESROLLED, TEXT("TimesRolled"), TEXT("TIMES ROLLED")));
	aStats.Add(FPlayerStats(STAT_TIMESBLINKED, TEXT("TimesBlinked"), TEXT("TIMES BLINKED")));

	aStats.Add(FPlayerStats(STAT_HEALINGDONE, TEXT("HealingDone"), TEXT("HEALING DONE")));
	aStats.Add(FPlayerStats(STAT_HEALINGRECEIVED, TEXT("HealingReceived"), TEXT("HEALING RECEIVED")));
	aStats.Add(FPlayerStats(STAT_TEAMMATESREVIVED, TEXT("TeammatesRevived"), TEXT("TEAMMATES REVIVED")));
	aStats.Add(FPlayerStats(STAT_TIMESREVIVED, TEXT("TimesRevived"), TEXT("TIMES REVIVED")));

	aStats.Add(FPlayerStats(STAT_HEALTHORBS, TEXT("HealthOrbs"), TEXT("HEALTH ORBS PICKED UP")));
	aStats.Add(FPlayerStats(STAT_STOPPINGORBS, TEXT("StoppingOrbs"), TEXT("DAMAGE ORBS PICKED UP")));
	aStats.Add(FPlayerStats(STAT_ROFORBS, TEXT("ROFOrbs"), TEXT("FIRING RATE ORBS PICKED UP")));
	aStats.Add(FPlayerStats(STAT_SPEEDORBS, TEXT("SpeedOrbs"), TEXT("RUN SPEED ORBS PICKED UP")));
	aStats.Add(FPlayerStats(STAT_REDUCTIONORBS, TEXT("ReductionOrbs"), TEXT("DAMAGE REDUCTION ORBS PICKED UP")));
	aStats.Add(FPlayerStats(STAT_EXPORBS, TEXT("ExpOrbs"), TEXT("XP ORBS PICKED UP")));

	aStats.Add(FPlayerStats(STAT_LIMBSBLOWNOFF, TEXT("LimbsBlownOff"), TEXT("LIMBS BLOWN OFF")));
	aStats.Add(FPlayerStats(STAT_HEADSBLOWNOFF, TEXT("HeadsBlownOff"), TEXT("HEADS BLOWN OFF")));
	
	aStats.Add(FPlayerStats(STAT_KILLSASASSAULT, TEXT("KillsAsAssault"), TEXT("KILLS AS ASSAULT")));
	aStats.Add(FPlayerStats(STAT_KILLSASSUPPORT, TEXT("KillsAsSupport"), TEXT("KILLS AS SUPPORT")));
	aStats.Add(FPlayerStats(STAT_KILLSASRECON, TEXT("KillsAsRecon"), TEXT("KILLS AS RECON")));
	aStats.Add(FPlayerStats(STAT_KILLSASTECH, TEXT("KillsAsTech"), TEXT("KILLS AS TECH")));
	aStats.Add(FPlayerStats(STAT_KILLSASPYRO, TEXT("KillsAsPyro"), TEXT("KILLS AS PYRO")));
	aStats.Add(FPlayerStats(STAT_KILLSASMARKSMAN, TEXT("KillsAsMarksman"), TEXT("KILLS AS MARKSMAN")));
	aStats.Add(FPlayerStats(STAT_KILLSASASSAULT, TEXT("AssistsAsAssault"), TEXT("ASSISTS AS ASSAULT")));
	aStats.Add(FPlayerStats(STAT_KILLSASSUPPORT, TEXT("AssistsAsSupport"), TEXT("ASSISTS AS SUPPORT")));
	aStats.Add(FPlayerStats(STAT_KILLSASRECON, TEXT("AssistsAsRecon"), TEXT("ASSISTS AS RECON")));
	aStats.Add(FPlayerStats(STAT_KILLSASTECH, TEXT("AssistsAsTech"), TEXT("ASSISTS AS TECH")));
	aStats.Add(FPlayerStats(STAT_KILLSASPYRO, TEXT("AssistsAsPyro"), TEXT("ASSISTS AS PYRO")));
	aStats.Add(FPlayerStats(STAT_KILLSASMARKSMAN, TEXT("AssistsAsMarksman"), TEXT("ASSISTS AS MARKSMAN")));
	aStats.Add(FPlayerStats(STAT_DEATHSASASSAULT, TEXT("DeathsAsAssault"), TEXT("DEATHS AS ASSAULT")));
	aStats.Add(FPlayerStats(STAT_DEATHSASSUPPORT, TEXT("DeathsAsSupport"), TEXT("DEATHS AS SUPPORT")));
	aStats.Add(FPlayerStats(STAT_DEATHSASRECON, TEXT("DeathsAsRecon"), TEXT("DEATHS AS RECON")));
	aStats.Add(FPlayerStats(STAT_DEATHSASTECH, TEXT("DeathsAsTech"), TEXT("DEATHS AS TECH")));
	aStats.Add(FPlayerStats(STAT_DEATHSASPYRO, TEXT("DeathsAsPyro"), TEXT("DEATHS AS PYRO")));
	aStats.Add(FPlayerStats(STAT_DEATHSASMARKSMAN, TEXT("DeathsAsMarksman"), TEXT("DEATHS AS MARKSMAN")));

	aStats.Add(FPlayerStats(STAT_EASYWINS, TEXT("EasyWins"), TEXT("WINS ON EASY")));
	aStats.Add(FPlayerStats(STAT_EASYLOSSES, TEXT("EasyLosses"), TEXT("LOSSES ON EASY")));
	aStats.Add(FPlayerStats(STAT_NORMALWINS, TEXT("NormalWins"), TEXT("WINS ON NORMAL")));
	aStats.Add(FPlayerStats(STAT_NORMALLOSSES, TEXT("NormalLosses"), TEXT("LOSSES ON NORMAL")));
	aStats.Add(FPlayerStats(STAT_HARDWINS, TEXT("HardWins"), TEXT("WINS ON HARD")));
	aStats.Add(FPlayerStats(STAT_HARDLOSSES, TEXT("HardLosses"), TEXT("LOSSES ON HARD")));
	aStats.Add(FPlayerStats(STAT_INSANEWINS, TEXT("InsaneWins"), TEXT("WINS ON INSANE")));
	aStats.Add(FPlayerStats(STAT_INSANELOSSES, TEXT("InsaneLosses"), TEXT("LOSSES ON INSANE")));
	aStats.Add(FPlayerStats(STAT_REDIKULOUSWINS, TEXT("RedikulousWins"), TEXT("WINS ON REDIKULOUS")));
	aStats.Add(FPlayerStats(STAT_REDIKULOUSLOSSES, TEXT("RedikulousLosses"), TEXT("LOSSES ON REDIKULOUS")));

	aStats.Add(FPlayerStats(STAT_GENERATORHEALINGDONE, TEXT("GeneratorHealingDone"), TEXT("")));
	aStats.Add(FPlayerStats(STAT_GENERATORSREPAIRED, TEXT("GeneratorsRepaired"), TEXT("")));

	aStats.Add(FPlayerStats(STAT_ASSISTS, TEXT("Assists"), TEXT("TOTAL ASSISTS")));

	aStats.Add(FPlayerStats(STAT_TOTALKILLS, TEXT("totalkills"), TEXT("TOTAL KILLS")));
	aStats.Add(FPlayerStats(STAT_ACHIEVEMENTPOINTS, TEXT("achpoints"), TEXT("TOTAL ACHIEVEMENT POINTS EARNED")));

	aStats.Add(FPlayerStats(STAT_MAXSLAUGHTERWAVE, TEXT("maxslaughterwave"), TEXT("HIGHEST SLAUGHTER WAVE")));
	aStats.Add(FPlayerStats(STAT_MAXSLAUGHTERKILLS, TEXT("maxslaughterkills"), TEXT("MOST SLAUGHTER KILLS")));

	aStats.Add(FPlayerStats(STAT_COMPYSLICE, TEXT("compyslice"), TEXT("COMPY HEADS CHOPPED OFF")));

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

void AOrionStats::AddCraftStat()
{
	AddStatValue(STAT_CRAFTITEMS, 1);
}

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
			if (ID == STAT_RECONEXP || ID == STAT_ASSAULTEXP || ID == STAT_SUPPORTEXP || ID == STAT_TECHEXP || ID == STAT_PYROEXP || ID == STAT_MARKSMANEXP)
				aStats[ID].StatValue = FMath::Min(aStats[ID].StatValue + Value, CalculateExpToLevel(50));
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
#else
	//local save file
	if (PC && PC->SavedGame)
	{
		PC->SaveGameToFile(PC->SavedGame->FileName);
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

	FillInLevels();
}

void AOrionStats::FillInLevels()
{
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
				PRI->MarksmanXP = aStats[STAT_MARKSMANEXP].StatValue;
			}
		}
	}
}

TArray<FEnhancedPlayerStats> AOrionStats::GetEnhancedStats()
{
	TArray<FEnhancedPlayerStats> StatsRet;

	FEnhancedPlayerStats SingleStat;

	//total kills
	SingleStat.FriendlyName = "TOTAL ENEMIES KILLED";
	SingleStat.StatValue = FString::Printf(TEXT("%i"), aStats[STAT_TOTALKILLS].StatValue);
	StatsRet.Add(SingleStat);

	//total deaths
	SingleStat.FriendlyName = "TOTAL TIMES DOWNED";
	int32 Deaths = 0;
	for (int32 i = STAT_RAPTORDEATH; i < STAT_ASSAULTDEATH; i++)
		Deaths += aStats[i].StatValue;
	SingleStat.StatValue = FString::Printf(TEXT("%i"), aStats[STAT_TOTALKILLS].StatValue);
	StatsRet.Add(SingleStat);

	//total assists
	SingleStat.FriendlyName = "TOTAL ASSISTS";
	SingleStat.StatValue = FString::Printf(TEXT("%i"), aStats[STAT_ASSISTS].StatValue);
	StatsRet.Add(SingleStat);

	//kills per death
	SingleStat.FriendlyName = "KILLS PER DEATH";
	SingleStat.StatValue = FString::Printf(TEXT("%.2f"), float(aStats[STAT_TOTALKILLS].StatValue) / FMath::Max(1.0f, float(Deaths)));
	StatsRet.Add(SingleStat);

	//assists per death
	SingleStat.FriendlyName = "ASSISTS PER DEATH";
	SingleStat.StatValue = FString::Printf(TEXT("%.2f"), float(aStats[STAT_ASSISTS].StatValue) / FMath::Max(1.0f, float(Deaths)));
	StatsRet.Add(SingleStat);

	//assists per kill
	SingleStat.FriendlyName = "ASSISTS PER KILL";
	SingleStat.StatValue = FString::Printf(TEXT("%.2f"), float(aStats[STAT_ASSISTS].StatValue) / FMath::Max(1.0f, float(aStats[STAT_TOTALKILLS].StatValue)));
	StatsRet.Add(SingleStat);

	//most killed dino
	SingleStat.FriendlyName = "MOST KILLED ENEMY";
	int32 Max = 0;
	FString Dino = TEXT("RAPTOR");
	for (int32 i = STAT_RAPTORKILL; i < STAT_ASSAULTKILL; i++)
	{
		if (aStats[i].StatValue > Max)
		{
			Max = aStats[i].StatValue;
			Dino = GetDinoNameFromStat(EStatID(i));
		}
	}
	SingleStat.StatValue = FString::Printf(TEXT("%s - %i"), *Dino, Max);
	StatsRet.Add(SingleStat);

	//died most dino
	SingleStat.FriendlyName = "MOST DEATHS TO ENEMY";
	Max = 0;
	Dino = TEXT("RAPTOR");
	for (int32 i = STAT_RAPTORDEATH; i < STAT_ASSAULTDEATH; i++)
	{
		if (aStats[i].StatValue > Max)
		{
			Max = aStats[i].StatValue;
			Dino = GetDinoNameFromStat(EStatID(i));
		}
	}
	SingleStat.StatValue = FString::Printf(TEXT("%s - %i"), *Dino, Max);
	StatsRet.Add(SingleStat);

	//favorite difficulty
	int32 EasyMatches = aStats[STAT_EASYWINS].StatValue + aStats[STAT_EASYLOSSES].StatValue;
	int32 NormalMatches = aStats[STAT_NORMALWINS].StatValue + aStats[STAT_NORMALLOSSES].StatValue;
	int32 HardMatches = aStats[STAT_HARDWINS].StatValue + aStats[STAT_HARDLOSSES].StatValue;
	int32 InsaneMatches = aStats[STAT_INSANEWINS].StatValue + aStats[STAT_INSANELOSSES].StatValue;
	int32 RedikulousMatches = aStats[STAT_REDIKULOUSWINS].StatValue + aStats[STAT_REDIKULOUSLOSSES].StatValue;

	int32 MaxMatches = EasyMatches;

	FString Diff = "EASY";
	if (NormalMatches > MaxMatches) { Diff = "NORMAL"; MaxMatches = NormalMatches; }
	if (HardMatches > MaxMatches) { Diff = "HARD"; MaxMatches = HardMatches; }
	if (InsaneMatches > MaxMatches) { Diff = "INSANE"; MaxMatches = InsaneMatches; }
	if (RedikulousMatches > MaxMatches) { Diff = "REDIKULOUS"; MaxMatches = RedikulousMatches; }

	SingleStat.FriendlyName = "MOST PLAYED DIFFICULTY";
	SingleStat.StatValue = FString::Printf(TEXT("%s - %i"), *Diff, MaxMatches);
	StatsRet.Add(SingleStat);

	//win percents on difficulties and maps

	return StatsRet;
}

FString AOrionStats::GetDinoNameFromStat(EStatID ID)
{
	switch (ID)
	{
	case STAT_RAPTORKILL:
	case STAT_RAPTORDEATH:
		return "RAPTOR";
	case STAT_TREXKILL:
	case STAT_TREXDEATH:
		return "TREX";
	case STAT_COMPYKILL:
	case STAT_COMPYDEATH:
		return "COMPY";
	case STAT_DILOKILL:
	case STAT_DILODEATH:
		return "DILO";
	case STAT_RHAMKILL:
	case STAT_RHAMDEATH:
		return "RHAM";
	case STAT_PTERAKILL:
	case STAT_PTERADEATH:
		return "PTERA";
	case STAT_STEGKILL:
	case STAT_STEGDEATH:
		return "STEG";
	case STAT_TRIKEKILL:
	case STAT_TRIKEDEATH:
		return "TRIKE";
	case STAT_ANKYKILL:
	case STAT_ANKYDEATH:
		return "ANKY";
	case STAT_PARAKILL:
	case STAT_PARADEATH:
		return "PARA";
	case STAT_ALLOKILL:
	case STAT_ALLODEATH:
		return "ALLO";
	case STAT_KRUGERKILL:
	case STAT_KRUGERDEATH:
		return "KRUGER";
	case STAT_JECKYLKILL:
	case STAT_JECKYLDEATH:
		return "JECKYL";
	case STAT_NAMORKILL:
	case STAT_NAMORDEATH:
		return "NAMOR";
	case STAT_VIPERKILL:
	case STAT_VIPERDEATH:
		return "VIPER";
	case STAT_BONESKILL:
	case STAT_BONESDEATH:
		return "BONES BOT";
	case STAT_ORBKILL:
	case STAT_ORBDEATH:
		return "ORB BOT";
	case STAT_GRUMPSKILL:
	case STAT_GRUMPSDEATH:
		return "GRUMPS BOT";
	case STAT_SPIDERKILL:
	case STAT_SPIDERDEATH:
		return "SPIDER BOT";
	}

	return "RAPTOR";
}