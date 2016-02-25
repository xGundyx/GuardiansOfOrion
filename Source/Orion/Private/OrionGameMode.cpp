// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Orion.h"
#include "OrionGameMode.h"
#include "OrionGameMenu.h"
#include "OrionHUD.h"
#include "OrionDinoPawn.h"
#include "OrionCharacter.h"
#include "OrionPRI.h"
#include "OrionGRI.h"
#include "OrionStats.h"
#include "PlayFabRequestProxy.h"
#include "OrionAIController.h"
#include "OrionPickupOrb.h"
#include "OrionDamageType.h"
#include "OrionDinoPawn.h"

AOrionGameMode::AOrionGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnObject(TEXT("/Game/Character/Blueprints/BasePawn"));
	if (PlayerPawnObject.Object != NULL)
	{
		DefaultPawnClass = (UClass*)PlayerPawnObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> SpecObject(TEXT("/Game/Character/Blueprints/BaseSpectator"));
	if (SpecObject.Object != NULL)
	{
		SpectatorClass = (UClass*)SpecObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> PickupObject(TEXT("/Game/Pickups/Blueprints/BasePickup"));
	if (PickupObject.Object != NULL)
	{
		DefaultPickupClass = (UClass*)PickupObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> PickupOrbObject(TEXT("/Game/Pickups/Blueprints/BasePickupOrb"));
	if (PickupOrbObject.Object != NULL)
	{
		DefaultPickupOrbClass = (UClass*)PickupOrbObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> PickupCoinObject(TEXT("/Game/Pickups/Blueprints/BasePickupCoins"));
	if (PickupCoinObject.Object != NULL)
	{
		DefaultPickupCoinClass = (UClass*)PickupCoinObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint>MusicObject(TEXT("/Game/Music/GOOMusic/GOOMusicManager"));
	if (MusicObject.Object != NULL)
	{
		DefaultMusicClass = (UClass*)MusicObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerControllerObject(TEXT("/Game/Character/Blueprints/BasePlayerController"));
	if (PlayerControllerObject.Object != NULL)
	{
		PlayerControllerClass = (UClass*)PlayerControllerObject.Object->GeneratedClass;
	}
	//PlayerControllerClass = AOrionPlayerController::StaticClass();

	GameStateClass = AOrionGRI::StaticClass();

	PlayerStateClass = AOrionPRI::StaticClass();

	// use our custom HUD class
	HUDClass = AOrionHUD::StaticClass();

	//this will keep players connected and keep their data persistent across map changes
	bUseSeamlessTravel = true;

	bAlwaysShowCursor = false;

	bTopDown = false;

	bSinglePlayer = false;

	//no team means every player is on the same side
	bTeamGame = false;

	TimeLimit = 0;
	WarmupTime = 0;
	bWarmingUp = true;

	// do this so the match doesn't accidently start on us
	bDelayedStart = true;

	DinoKillZ = -100000.0f;

	InactivePlayerStateLifeSpan = 0.1f;
}

//no saving data on disconnects
void AOrionGameMode::AddInactivePlayer(APlayerState* PlayerState, APlayerController* PC)
{
	if (PlayerState)
		PlayerState->Destroy();
}

void AOrionGameMode::TickExitTimer()
{
	TArray<AActor*> Controllers;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrionPlayerController::StaticClass(), Controllers);
	for (int32 i = 0; i < Controllers.Num(); i++)
	{
		ExitCounter = 0;
		return;
	}

	ExitCounter++;

	//close the server if it has been empty for a minute straight
#if IS_SERVER
	if (ExitCounter >= 60)
		if (GEngine)
			GEngine->Exec(nullptr, TEXT("QUIT"), *GLog);
#endif
}

void AOrionGameMode::BeginPlay()
{
	Super::BeginPlay();

	bLobbyCreated = false;

#if IS_SERVER
	GetWorldTimerManager().SetTimer(ExitTimer, this, &AOrionGameMode::TickExitTimer, 1.0f, true);
#endif

	InitGRI();

	if (WarmupTime > 0)
	{
		bWarmingUp = true;
		GetWorldTimerManager().SetTimer(WarmupTimer, this, &AOrionGameMode::WarmupOver, WarmupTime, false);
	}
	else
	{
		bDelayedStart = false;
		bWarmingUp = false;
	}

	//out of bounds checker
	GetWorldTimerManager().SetTimer(OOBTimer, this, &AOrionGameMode::HandleOutOfBounds, 1.0f, true);
}

void AOrionGameMode::HandleOutOfBounds()
{
	if (!OOBVolume)
	{
		TArray<AActor*> OOB;

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrionOutOfBoundsVolume::StaticClass(), OOB);

		for (int32 i = 0; i < OOB.Num(); i++)
		{
			OOBVolume = Cast<AOrionOutOfBoundsVolume>(OOB[i]);

			FVector Origin;
			FVector Extents;
			OOBVolume->GetActorBounds(false, Origin, Extents);

			DinoKillZ = Origin.Z - Extents.Z;

			break;
		}
	}

	if (!OOBVolume)
		return;

	TArray<AActor*> Controllers;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrionPlayerController::StaticClass(), Controllers);

	for (int32 i = 0; i < Controllers.Num(); i++)
	{
		AOrionPlayerController *C = Cast<AOrionPlayerController>(Controllers[i]);
		if (C)
		{
			AOrionCharacter *P = Cast<AOrionCharacter>(C->GetPawn());
			if (P)
			{
				if (!P->IsOnShip() && !OOBVolume->EncompassesPoint(P->GetActorLocation()))
				{
					P->OutOfBoundsCounter++;

					if (P->OutOfBoundsCounter >= 10)
					{
						FDamageEvent dEvent = FDamageEvent::FDamageEvent();
						dEvent.DamageTypeClass = UOrionDamageType::StaticClass();

						P->Die(10000000.0f, dEvent, nullptr, P);
					}
				}
				else
				{
					P->OutOfBoundsCounter = -1;
				}
			}
		}
	}
}

void AOrionGameMode::WarmupOver()
{
	bDelayedStart = false;
	bWarmingUp = false;
}

bool AOrionGameMode::ReadyToStartMatch_Implementation()
{
	// If bDelayed Start is set, wait for a manual match start
	if (bDelayedStart)
	{
		return false;
	}

	// By default start when warmup is over and we have at leasst one player
	if (GetMatchState() == MatchState::WaitingToStart)
	{
		if (!bWarmingUp && NumPlayers > 0)
		{
			return true;
		}
	}
	return false;
}

void AOrionGameMode::StartMatch()
{
	if (HasMatchStarted())
	{
		// Already started
		return;
	}

	//Let the game session override the StartMatch function, in case it wants to wait for arbitration
	if (GameSession->HandleStartMatchRequest())
	{
		return;
	}

	SetMatchState(MatchState::InProgress);

	if (TimeLimit > 0)
		GetWorldTimerManager().SetTimer(MatchTimer, this, &AOrionGameMode::EndMatch, TimeLimit, false);
}

void AOrionGameMode::InitGRI()
{
	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);

	if (!GRI)
	{
		GetWorldTimerManager().SetTimer(GRITimer, this, &AOrionGameMode::InitGRI, 0.1, false);
		return;
	}

	GRI->bTeamGame = bTeamGame;
	GRI->bAlwaysShowCursor = bAlwaysShowCursor;
	GRI->bTopDown = bTopDown;
}

void AOrionGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

#if IS_SERVER
	//UOrionTCPLink::Update();
#endif
}

void AOrionGameMode::Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType)
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Killer);
	AOrionPlayerController *DeadPC = Cast<AOrionPlayerController>(KilledPlayer);

	AOrionCharacter *KillerPawn = nullptr;
	
	if(Killer && Killer->IsValidLowLevel())
		KillerPawn = Cast<AOrionCharacter>(Killer->GetPawn());

	//HandleStats(Killer, KilledPlayer, KilledPawn, DamageType);

	AOrionAIController *AIC = Cast<AOrionAIController>(Killer);

	if (AIC)
		AIC->bShouldRoar = true;

	//assists
	AOrionCharacter *DeadPawn = Cast<AOrionCharacter>(KilledPawn);
	AOrionDinoPawn *DeadDino = Cast<AOrionDinoPawn>(KilledPawn);
	AOrionDroidPawn *DeadDroid = Cast<AOrionDroidPawn>(KilledPawn);

	//when a downed player gets a kill, give them %40 life for small dinos, and full revive for large dinos
	if (PC && (DeadDino || DeadDroid))
	{
		AOrionCharacter *Pawn = Cast<AOrionCharacter>(PC->GetPawn());

		if (Pawn && DeadPawn && DeadPawn->bIsBigDino)
			Pawn->PlayVoice(VOICE_BOSSKILL);

		if (Pawn && DeadPawn && Pawn->bDowned)
		{
			Pawn->Health = FMath::Min(Pawn->HealthMax, Pawn->Health + (Pawn->HealthMax * ((DeadPawn->bIsBigDino || (!IsSlaughter() && Difficulty <= DIFF_MEDIUM)) ? 1.0f : 0.4f)));

			if (Pawn->Health >= Pawn->HealthMax)
				Pawn->Revived();
		}
	}

	if (DeadPC)
	{
		PlayRandomVoiceFromPlayer(VOICE_TEAMMATEDOWN);

		if (AIC)
		{
			AOrionDroidPawn *Droid = Cast<AOrionDroidPawn>(AIC->GetPawn());

			if (Droid)
				Droid->PlayVoice(VOICE_BOSSKILL);
		}
	}

	if (DeadPC && DeadPawn && !DeadPawn->bFatality)
		PlaySlowMotion(7.0f);

	if ((PC || (KillerPawn && KillerPawn->bIsHealableMachine)) && DeadDino && DeadDino->DinoName.ToString().ToUpper() == TEXT("TREX"))
	{
		PlaySlowMotion(7.0f);

		if (PC && PC->GetAchievements())
		{
			PC->LastTRexKill = GetWorld()->GetTimeSeconds();

			if (PC->LastTRexKill - PC->LastNamorKill <= 5.0f)
				PC->GetAchievements()->UnlockAchievement(ACH_TREXNAMOR, PC);
		}
	}
	else if ((PC || (KillerPawn && KillerPawn->bIsHealableMachine)) && DeadDroid && DeadDroid->DroidName.ToString().ToUpper() == TEXT("GRUMPS"))
	{
		PlaySlowMotion(7.0f);
	}
	else if (PC && DeadDino && DeadDino->DinoName.ToString().ToUpper() == TEXT("NAMOR"))
	{
		PC->LastNamorKill = GetWorld()->GetTimeSeconds();
		if (PC->GetAchievements() && PC->LastNamorKill - PC->LastTRexKill <= 5.0f)
			PC->GetAchievements()->UnlockAchievement(ACH_TREXNAMOR, PC);
	}

	if (DeadPawn)
	{
		for (int32 i = 0; i < DeadPawn->Assisters.Num(); i++)
		{
			if (!DeadPawn->Assisters[i] || !DeadPawn->Assisters[i]->IsValidLowLevel())
				continue;

			if ((PC != NULL && PC != DeadPawn->Assisters[i]) || (KillerPawn && KillerPawn->bIsHealableMachine))
			{
				AOrionPlayerController *C = DeadPawn->Assisters[i];
				AOrionPRI *AssistPRI = Cast<AOrionPRI>(C->PlayerState);

				if (AssistPRI)
				{
					if (C && C->GetStats())
					{
						switch (C->ClassIndex)
						{
						case 0:
							C->GetStats()->AddStatValue(STAT_ASSISTSSASASSAULT, 1);
							break;
						case 1:
							C->GetStats()->AddStatValue(STAT_ASSISTSASSUPPORT, 1);
							break;
						case 2:
							C->GetStats()->AddStatValue(STAT_ASSISTSASRECON, 1);
							break;
						case 3:
							C->GetStats()->AddStatValue(STAT_ASSISTSASTECH, 1);
							break;
						case 4:
							C->GetStats()->AddStatValue(STAT_ASSISTSASPYRO, 1);
							break;
						case 5:
							C->GetStats()->AddStatValue(STAT_ASSISTSASMARKSMAN, 1);
							break;
						};

						C->GetStats()->AddStatValue(STAT_ASSISTS, 1);
					}
					AssistPRI->Assists++;

					//assists help you get up
					AOrionCharacter *Pawn = Cast<AOrionCharacter>(C->GetPawn());
					if (Pawn && Pawn->bDowned)
					{
						Pawn->Health = FMath::Min(Pawn->HealthMax, Pawn->Health + (Pawn->HealthMax * 0.4f));

						if (Pawn->Health >= Pawn->HealthMax)
							Pawn->Revived();
					}

					if (AssistPRI->Assists >= 200 && C && C->GetAchievements())
						C->GetAchievements()->UnlockAchievement(ACH_200ASSIST, C);
				}
			}
		}
	}
	
	if (PC)
	{
		if (PC->GetSkillValue(SKILL_KILLLEECH))
		{
			AOrionCharacter *Pawn = Cast<AOrionCharacter>(PC->GetPawn());
			if (Pawn)
			{
				Pawn->AddHealth(Pawn->HealthMax / 10);
				Pawn->AddShield(Pawn->ShieldMax / 20);
			}
		}
	}

	AOrionCharacter *Dino = Cast<AOrionCharacter>(KilledPawn);
	if (Dino)
	{
		//harv kills grant no items
		if (PC)
			SpawnItems(Killer, Dino, DamageType);

		int32 XP = Dino->ExpValue * 2;

		//award some experience to the team
		if (PC && Dino->ExpValue > 0)
		{
			if (PC->HasOrbEffect(ORB_EXP))
				XP *= 2;

			PC->ClientAddXPNumber(XP, KilledPawn->GetActorLocation());
		}

		const UOrionDamageType *dType = Cast<UOrionDamageType>(DamageType);
		AwardXPToAllPlayers(Dino->ExpValue * 2, PC, dType);
	}
}

void AOrionGameMode::PlayRandomVoiceFromPlayer(EVoiceType Type)
{
}

void AOrionGameMode::AwardXPToAllPlayers(int32 Amount, AOrionPlayerController *PC, const UOrionDamageType *DamageType)
{
	//all players on the team receive the same xp
	TArray<AActor*> Controllers;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrionPlayerController::StaticClass(), Controllers);

	for (int32 i = 0; i < Controllers.Num(); i++)
	{
		AOrionPlayerController *C = Cast<AOrionPlayerController>(Controllers[i]);
		if (C)
		{
			int32 XP = Amount;

			if (C->HasOrbEffect(ORB_EXP))
				XP *= 2;

			if (PC && PC == C && DamageType)
			{
				AOrionCharacter *P = Cast<AOrionCharacter>(PC->GetPawn());
				if (P)
					XP += P->XPPerKill;

				AOrionInventoryManager *Inv = PC->GetInventoryManager();
				if (Inv && Inv->HasStat(RARESTAT_BONUSXP) && DamageType->WeaponSlot == 1) XP *= 1.25f;
				if (Inv && Inv->HasStat(RARESTAT_REGENENERGY) && DamageType->WeaponSlot == 1)
				{
					if (P && P->CurrentSkill)
						P->CurrentSkill->SetEnergy(FMath::Min(100.0f, P->CurrentSkill->GetEnergy() + P->CurrentSkill->GetEnergy() * 0.1f));
				}
			}

			C->AddXP(XP);
		}
	}
}

EStatID AOrionGameMode::GetStatID(AController *KilledController, bool bVictim)
{
	AOrionAIController *AI = Cast<AOrionAIController>(KilledController);
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(KilledController);

	if (AI)
	{
		if (AI->AIName == TEXT("Raptor"))
			return bVictim ? (AI->bIsElite ? STAT_ELITERAPTORDEATH : STAT_RAPTORDEATH) : (AI->bIsElite ? STAT_ELITERAPTORKILL : STAT_RAPTORKILL);
		else if (AI->AIName == TEXT("Compy"))
			return bVictim ? (AI->bIsElite ? STAT_ELITECOMPYDEATH : STAT_COMPYDEATH) : (AI->bIsElite ? STAT_ELITECOMPYKILL : STAT_COMPYKILL);
		else if (AI->AIName == TEXT("Dilo"))
			return bVictim ? (AI->bIsElite ? STAT_ELITEDILODEATH : STAT_DILODEATH) : (AI->bIsElite ? STAT_ELITEDILOKILL : STAT_DILOKILL);
		else if (AI->AIName == TEXT("Rham"))
			return bVictim ? (AI->bIsElite ? STAT_ELITERHAMDEATH : STAT_RHAMDEATH) : (AI->bIsElite ? STAT_ELITERHAMKILL : STAT_RHAMKILL);
		else if (AI->AIName == TEXT("Ptera"))
			return bVictim ? (AI->bIsElite ? STAT_ELITEPTERADEATH : STAT_PTERADEATH) : (AI->bIsElite ? STAT_ELITEPTERAKILL : STAT_PTERAKILL);
		else if (AI->AIName == TEXT("Trike"))
			return bVictim ? (AI->bIsElite ? STAT_ELITETRIKEDEATH : STAT_TRIKEDEATH) : (AI->bIsElite ? STAT_ELITETRIKEKILL : STAT_TRIKEKILL);
		else if (AI->AIName == TEXT("Steggo"))
			return bVictim ? (AI->bIsElite ? STAT_ELITESTEGDEATH : STAT_STEGDEATH) : (AI->bIsElite ? STAT_ELITESTEGKILL : STAT_STEGKILL);
		else if (AI->AIName == TEXT("Anky"))
			return bVictim ? (AI->bIsElite ? STAT_ELITEANKYDEATH : STAT_ANKYDEATH) : (AI->bIsElite ? STAT_ELITEANKYKILL : STAT_ANKYKILL);
		else if (AI->AIName == TEXT("Allo"))
			return bVictim ? (AI->bIsElite ? STAT_ELITEALLODEATH : STAT_ALLODEATH) : (AI->bIsElite ? STAT_ELITEALLOKILL : STAT_ALLOKILL);
		else if (AI->AIName == TEXT("Para"))
			return bVictim ? (AI->bIsElite ? STAT_ELITEPARADEATH : STAT_PARADEATH) : (AI->bIsElite ? STAT_ELITEPARAKILL : STAT_PARAKILL);
		else if (AI->AIName == TEXT("TRex"))
			return bVictim ? (AI->bIsElite ? STAT_ELITETREXDEATH : STAT_TREXDEATH) : (AI->bIsElite ? STAT_ELITETREXKILL : STAT_TREXKILL);
		else if (AI->AIName == TEXT("Jeckyl"))
			return bVictim ? (AI->bIsElite ? STAT_ELITEJECKYLDEATH : STAT_JECKYLDEATH) : (AI->bIsElite ? STAT_ELITEJECKYLKILL : STAT_JECKYLKILL);
		else if (AI->AIName == TEXT("Kruger"))
			return bVictim ? (AI->bIsElite ? STAT_ELITEKRUGERDEATH : STAT_KRUGERDEATH) : (AI->bIsElite ? STAT_ELITEKRUGERKILL : STAT_KRUGERKILL);
		else if (AI->AIName == TEXT("Namor"))
			return bVictim ? (AI->bIsElite ? STAT_ELITENAMORDEATH : STAT_NAMORDEATH) : (AI->bIsElite ? STAT_ELITENAMORKILL : STAT_NAMORKILL);
		else if (AI->AIName == TEXT("Viper"))
			return bVictim ? (AI->bIsElite ? STAT_ELITEVIPERDEATH : STAT_VIPERDEATH) : (AI->bIsElite ? STAT_ELITEVIPERKILL : STAT_VIPERKILL);
		else if (AI->AIName == TEXT("Bones"))
			return bVictim ? (AI->bIsElite ? STAT_ELITEBONESDEATH : STAT_BONESDEATH) : (AI->bIsElite ? STAT_ELITEBONESKILL : STAT_BONESKILL);
		else if (AI->AIName == TEXT("Orb"))
			return bVictim ? (AI->bIsElite ? STAT_ELITEORBDEATH : STAT_ORBDEATH) : (AI->bIsElite ? STAT_ELITEORBKILL : STAT_ORBKILL);
		else if (AI->AIName == TEXT("Grumps"))
			return bVictim ? (AI->bIsElite ? STAT_ELITEGRUMPSDEATH : STAT_GRUMPSDEATH) : (AI->bIsElite ? STAT_ELITEGRUMPSKILL : STAT_GRUMPSKILL);
		else if (AI->AIName == TEXT("Spider"))
			return bVictim ? (AI->bIsElite ? STAT_ELITESPIDERDEATH : STAT_SPIDERDEATH) : (AI->bIsElite ? STAT_ELITESPIDERKILL : STAT_SPIDERKILL);
		else if (AI->AIName == TEXT("Assault"))
			return bVictim ? (AI->bIsElite ? STAT_ELITEASSAULTDEATH : STAT_ASSAULTDEATH) : (AI->bIsElite ? STAT_ELITEASSAULTKILL : STAT_ASSAULTKILL);
		else if (AI->AIName == TEXT("Support"))
			return bVictim ? (AI->bIsElite ? STAT_ELITESUPPORTDEATH : STAT_SUPPORTDEATH) : (AI->bIsElite ? STAT_ELITESUPPORTKILL : STAT_SUPPORTKILL);
		else if (AI->AIName == TEXT("Recon"))
			return bVictim ? (AI->bIsElite ? STAT_ELITERECONDEATH : STAT_RECONDEATH) : (AI->bIsElite ? STAT_ELITERECONKILL : STAT_RECONKILL);
		else if (AI->AIName == TEXT("Tech"))
			return bVictim ? (AI->bIsElite ? STAT_ELITETECHDEATH : STAT_TECHDEATH) : (AI->bIsElite ? STAT_ELITETECHKILL : STAT_TECHKILL);
		else if (AI->AIName == TEXT("Pyro"))
			return bVictim ? (AI->bIsElite ? STAT_ELITEPYRODEATH : STAT_PYRODEATH) : (AI->bIsElite ? STAT_ELITEPYROKILL : STAT_PYROKILL);
		else if (AI->AIName == TEXT("Marksman"))
			return bVictim ? (AI->bIsElite ? STAT_ELITEMARKSMANDEATH : STAT_MARKSMANDEATH) : (AI->bIsElite ? STAT_ELITEMARKSMANKILL : STAT_MARKSMANKILL);
	}

	return STAT_ERROR;
}

void AOrionGameMode::SaveAllUsersStats()
{
	TArray<AActor*> Controllers;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrionPlayerController::StaticClass(), Controllers);

	for (int32 i = 0; i < Controllers.Num(); i++)
	{
		AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controllers[i]);
		if (PC)
			PC->GetStats()->FlushPlayerStats(PC);
	}
}

void AOrionGameMode::HandleStats(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType)
{
	AOrionPlayerController *KillerPC = Cast<AOrionPlayerController>(Killer);
	AOrionPlayerController *KilledPC = Cast<AOrionPlayerController>(KilledPlayer);

	EStatID KilledStatID = GetStatID(KilledPlayer, false);
	EStatID KillerStatID = GetStatID(Killer, true);

	if (KillerPC)
	{
		AOrionStats *Stats = KillerPC->GetStats();

		if (Stats)
		{
			Stats->AddStatValue(KilledStatID, 1);
		}

		if (KillerPC->GetAchievements())
			KillerPC->GetAchievements()->CheckForSlayer();
	}

	if (KilledPC)
	{
		AOrionStats *Stats = KilledPC->GetStats();

		if (Stats)
		{
			Stats->AddStatValue(KillerStatID, 1);
		}
	}

	if (Killer && KilledPlayer)
	{
		AOrionPRI *KillerPRI = Cast<AOrionPRI>(Killer->PlayerState);
		AOrionPRI *KilledPRI = Cast<AOrionPRI>(KilledPlayer->PlayerState);

		if (KillerPRI)
		{
			if (KillerPC && KillerPC->GetStats())
			{
				switch (KillerPC->ClassIndex)
				{
				case 0:
					KillerPC->GetStats()->AddStatValue(STAT_KILLSASASSAULT, 1);
					break;
				case 1:
					KillerPC->GetStats()->AddStatValue(STAT_KILLSASSUPPORT, 1);
					break;
				case 2:
					KillerPC->GetStats()->AddStatValue(STAT_KILLSASRECON, 1);
					break;
				case 3:
					KillerPC->GetStats()->AddStatValue(STAT_KILLSASTECH, 1);
					break;
				case 4:
					KillerPC->GetStats()->AddStatValue(STAT_KILLSASPYRO, 1);
					break;
				case 5:
					KillerPC->GetStats()->AddStatValue(STAT_KILLSASMARKSMAN, 1);
					break;
				};
			}
			KillerPRI->Kills++;

			if (KillerPRI->Kills % 25 == 0 && KillerPC)
			{
				AOrionCharacter *KillerPawn = Cast<AOrionCharacter>(KillerPC->GetPawn());
				if (KillerPawn)
					KillerPawn->PlayVoice(VOICE_CANTTOUCHTHIS);
			}

		}
		if (KilledPRI)
		{
			if (KilledPC && KilledPC->GetStats())
			{
				switch (KilledPC->ClassIndex)
				{
				case 0:
					KilledPC->GetStats()->AddStatValue(STAT_DEATHSASASSAULT, 1);
					break;
				case 1:
					KilledPC->GetStats()->AddStatValue(STAT_DEATHSASSUPPORT, 1);
					break;
				case 2:
					KilledPC->GetStats()->AddStatValue(STAT_DEATHSASRECON, 1);
					break;
				case 3:
					KilledPC->GetStats()->AddStatValue(STAT_DEATHSASTECH, 1);
					break;
				case 4:
					KilledPC->GetStats()->AddStatValue(STAT_DEATHSASPYRO, 1);
					break;
				case 5:
					KilledPC->GetStats()->AddStatValue(STAT_DEATHSASMARKSMAN, 1);
					break;
				};
			}
			KilledPRI->Deaths++;
		}
	}
}

void AOrionGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GetGameState());

	//read in our desired difficulty
	FString Diff = UGameplayStatics::ParseOption(Options, TEXT("Difficulty"));

	if (Diff.ToLower() == "easy")
		Difficulty = DIFF_EASY;
	else if (Diff.ToLower() == "medium")
		Difficulty = DIFF_MEDIUM;
	else if (Diff.ToLower() == "hard")
		Difficulty = DIFF_HARD;
	else if (Diff.ToLower() == "insane")
		Difficulty = DIFF_INSANE;
	else if (Diff.ToLower() == "redikulous")
		Difficulty = DIFF_REDIKULOUS;
	else
		Difficulty = DIFF_MEDIUM;

	FString GearLevel = UGameplayStatics::ParseOption(Options, TEXT("GearLevel"));

	ItemLevel = FMath::Max(1, FCString::Atoi(*GearLevel));

	//cap item level based on difficulty
	switch (Difficulty)
	{
	case DIFF_EASY:
	case DIFF_MEDIUM:
		ItemLevel = FMath::Clamp(ItemLevel, 1, 400);
		break;
	case DIFF_HARD:
		ItemLevel = FMath::Clamp(ItemLevel, 100, 400);
		break;
	case DIFF_INSANE:
		ItemLevel = FMath::Clamp(ItemLevel, 200, 500);
		break;
	case DIFF_REDIKULOUS:
		ItemLevel = FMath::Clamp(ItemLevel, 300, 600);
		break;
	}

	FString TOD = UGameplayStatics::ParseOption(Options, TEXT("TOD"));
	bNightTime = TOD.ToUpper().Equals(TEXT("NIGHT"));

	//read in our PlayFab LobbyID
	LobbyID = UGameplayStatics::ParseOption(Options, TEXT("LobbyID"));

	//server name
	ServerInfo.RoomName = UGameplayStatics::ParseOption(Options, TEXT("ServerName")).Append("'S SERVER");
	ServerInfo.MapName = MapName;
	switch(Difficulty)
	{
	case DIFF_EASY:
		ServerInfo.Difficulty = TEXT("EASY");
		break;
	case DIFF_MEDIUM:
		ServerInfo.Difficulty = TEXT("MEDIUM");
		break;
	case DIFF_HARD:
		ServerInfo.Difficulty = TEXT("HARD");
		break;
	case DIFF_INSANE:
		ServerInfo.Difficulty = TEXT("INSANE");
		break;
	case DIFF_REDIKULOUS:
		ServerInfo.Difficulty = TEXT("REDIKULOUS");
		break;
	}

	FString Type = UGameplayStatics::ParseOption(Options, TEXT("GameType"));

	if (Type.ToUpper() == "SINGLEPLAYER")
		bSinglePlayer = true;

	//ServerInfo.IP = UGameplayStatics::ParseOption(Options, TEXT("IP"));
	//ServerInfo.Ticket = UGameplayStatics::ParseOption(Options, TEXT("LobbyTicket"));
	ServerInfo.Privacy = UGameplayStatics::ParseOption(Options, TEXT("Privacy"));
	ServerInfo.LobbyID = LobbyID;
	ServerInfo.GameMode = GetGameName();
	ServerInfo.ItemLevel = FString::Printf(TEXT("%i"), ItemLevel);
	ServerInfo.Region = UGameplayStatics::ParseOption(Options, TEXT("Region"));

	//if (GRI)
	//	GRI->PhotonGUID = LobbyID;
}

float AOrionGameMode::ModifyDamage(float Damage, AOrionCharacter *PawnToDamage, struct FDamageEvent const& DamageEvent, class AController *EventInstigator, class AActor *DamageCauser)
{
	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);

	if (!PawnToDamage || !PawnToDamage->Controller || !EventInstigator)
		return 0.0f;

	AOrionDinoPawn *TRexKiller = Cast<AOrionDinoPawn>(EventInstigator->GetPawn());
	AOrionDinoPawn *TRexKilled = Cast<AOrionDinoPawn>(PawnToDamage);

	bool bDoTRexDamage = false;

	if (TRexKiller && TRexKiller->DinoName.ToString().ToUpper() == "TREX")
		bDoTRexDamage = true;

	//if we're a team game, don't damage your teammates
	if (bTeamGame && !bDoTRexDamage && GRI && GRI->OnSameTeam(Cast<AOrionPRI>(PawnToDamage->Controller->PlayerState), Cast<AOrionPRI>(EventInstigator->PlayerState)))
	{
		Damage = 0.0f;
	}

	//if we're not a team game, don't do player to player damage
	if (!bTeamGame && PawnToDamage->GetPlayerController() && Cast<APlayerController>(EventInstigator))
	{
		Damage = 0.0f;
	}

	//adjust damage based on amount of players in game, mainly just for coop games:p
	if (GRI && PawnToDamage->PlayerState && PawnToDamage->PlayerState->bIsABot)
	{
		int32 NumPlayers = GetOrionNumPlayers();// GRI->PlayerList.Num();

		if (NumPlayers >= 4 || Difficulty == DIFF_REDIKULOUS)
			Damage /= 2.0f;
		else if (NumPlayers >= 3)
			Damage /= 1.5f;
		else if (NumPlayers >= 2)
			Damage /= 1.25f;

		//also adjust based on game difficulty
		if (Difficulty == DIFF_EASY)
			Damage *= 2.0f;
		else if (Difficulty == DIFF_MEDIUM)
			Damage *= 1.0f;
		else if (Difficulty == DIFF_HARD)
			Damage *= 0.9f;
		else if (Difficulty == DIFF_INSANE)
			Damage *= 0.7f;
		else if (Difficulty == DIFF_REDIKULOUS)
			Damage *= 0.5f;
	}
	else
	{
		//adjust damage to humans
		int32 NumPlayers = GRI->PlayerList.Num();

		if (NumPlayers >= 4 || Difficulty == DIFF_REDIKULOUS)
			Damage *= 1.5f;
		else if (NumPlayers >= 3)
			Damage *= 1.25f;
		else if (NumPlayers >= 2)
			Damage *= 1.15f;

		//also adjust based on game difficulty
		if (Difficulty == DIFF_EASY)
			Damage /= 2.0f;
		else if (Difficulty == DIFF_MEDIUM)
			Damage /= 1.0f;
		else if (Difficulty == DIFF_HARD)
			Damage *= 1.3f;
		else if (Difficulty == DIFF_INSANE)
			Damage *= 2.5f;
		else if (Difficulty == DIFF_REDIKULOUS)
			Damage *= 5.0f;
	}

	return Damage;
}

APlayerController* AOrionGameMode::Login(UPlayer* NewPlayer, ENetRole RemoteRole, const FString& Portal, const FString& Options, const TSharedPtr<const FUniqueNetId>& UniqueId, FString& ErrorMessage)
{
	APlayerController *rPC = Super::Login(NewPlayer, RemoteRole, Portal, Options, UniqueId, ErrorMessage);

#if IS_SERVER
	/*AOrionPlayerController *PC = Cast<AOrionPlayerController>(NewPlayer);

	//setup a stats object for this player
	if (PC)
	{
		PC->InitStatsAndAchievements();
		PC->ReadStats();
	}*/
#endif

	//update playerlist
	AOrionGRI *GRI = Cast<AOrionGRI>(GameState);

	if (GRI && rPC)
		GRI->PlayerList.AddUnique(Cast<AOrionPRI>(rPC->PlayerState));

	if (GRI)
	{
		GRI->Difficulty = Difficulty;
		GRI->ItemLevel = ItemLevel;
		GRI->ServerLocation = ServerInfo.Region;
		GRI->OrionGameMode = IsSlaughter() ? "SLAUGHTER" : "SURVIVAL";
#if IS_SERVER
		GRI->bStatsEnabled = true;
#else
		GRI->bStatsEnabled = false;
#endif
	}

	//retrieve player info, do not let them spawn until the data has been fully read in

	SetInitialTeam(rPC);

	return rPC;
}

void AOrionGameMode::PreLogin(const FString& Options, const FString& Address, const TSharedPtr<const FUniqueNetId>& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
}

FString AOrionGameMode::InitNewPlayer(class APlayerController* NewPlayerController, const TSharedPtr<const FUniqueNetId>& UniqueId, const FString& Options, const FString& Portal)
{
	FString ret = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	//grab the playfab info from the Options string and try to validate the player
	FString pfID = UGameplayStatics::ParseOption(Options, TEXT("PlayFabID"));
	FString pfSession = UGameplayStatics::ParseOption(Options, TEXT("PlayFabSession"));
	FString pfChar = UGameplayStatics::ParseOption(Options, TEXT("PlayFabCharacter"));
	FString pfName = UGameplayStatics::ParseOption(Options, TEXT("PlayFabName"));
	FString pfTicket = UGameplayStatics::ParseOption(Options, TEXT("LobbyTicket"));
	FString pfClass = UGameplayStatics::ParseOption(Options, TEXT("CharacterClass"));

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(NewPlayerController);

#if !IS_SERVER
	//these are for local and local coop saved games
	AOrionGRI *GRI = Cast<AOrionGRI>(GameState);

	if (GRI)
	{
		GRI->Player1 = UGameplayStatics::ParseOption(Options, TEXT("Player1"));
		GRI->Player2 = UGameplayStatics::ParseOption(Options, TEXT("Player2"));
		GRI->Player3 = UGameplayStatics::ParseOption(Options, TEXT("Player3"));
		GRI->Player4 = UGameplayStatics::ParseOption(Options, TEXT("Player4"));

		if (PC)
		{
			PC->InitStatsAndAchievements();
			PC->LoadGameFromFile(GRI->Player1);
		}
	}
#endif

	//if this session and id aren't valid, kick the bitch
	if (PC)
	{
		AOrionPRI *PRI = Cast<AOrionPRI>(PC->PlayerState);
		if (PRI)
		{
			PRI->PlayFabID = pfID;
			PRI->SessionTicket = pfSession;
			PRI->CharacterID = pfChar;
			PRI->PlayFabName = pfName;
			PRI->LobbyTicket = pfTicket;
			PRI->CharacterClass = pfClass;
			PRI->ClassType = pfClass;
			//PRI->ServerInfo = ServerInfo;
		}

#if IS_SERVER
		PC->ValidateLobbyTicket(LobbyID, pfTicket);
#else
		//can't do this here, it requires the secret key
		//PC->ValidatePlayFabInfo(pfID, pfSession);
		//hax for now
		PlayerAuthed(PC, true);
#endif

		if (pfClass == TEXT("ASSAULT"))
			PC->ClassIndex = 0;
		else if (pfClass == TEXT("SUPPORT"))
			PC->ClassIndex = 1;
		else if (pfClass == TEXT("RECON"))
			PC->ClassIndex = 2;
		else if (pfClass == TEXT("TECH"))
			PC->ClassIndex = 3;
		else if (pfClass == TEXT("PYRO"))
			PC->ClassIndex = 4;
		else if (pfClass == TEXT("MARKSMAN"))
			PC->ClassIndex = 5;
		else
			PC->ClassIndex = 0;
	}

	return ret;
}

void AOrionGameMode::AddChatMessage(const FString &msg)
{
	TArray<AActor*> Controllers;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrionPlayerController::StaticClass(), Controllers);

	for (int32 i = 0; i < Controllers.Num(); i++)
	{
		AOrionPlayerController *C = Cast<AOrionPlayerController>(Controllers[i]);

		if (C)
			C->ClientReceiveChatMessage(msg);
	}
}

void AOrionGameMode::PlayerAuthed(class AOrionPlayerController *PC, bool bSuccess)
{
#if !WITH_EDITOR && IS_SERVER
	if (!bSuccess)
	{
		//GameSession->KickPlayer(PC, FText::FromString(TEXT("Playfab Authentication Failed.")));
		//return;
	}
	
	if(PC)
	{
		AOrionPRI *PRI = Cast<AOrionPRI>(PC->PlayerState);

		if (PRI)
		{
			UPlayFabRequestProxy::ServerNotifyMatchmakerPlayerJoined(PRI->PlayFabID, LobbyID);
		}
	}

	if (!bSinglePlayer && PC)
	{
		if (!bLobbyCreated)
			PC->CreateInGameLobby(ServerInfo);
		else
			PC->SetServerInfo(ServerInfo); //needed for situation when the original host leaves and new host needs the info
	}

	bLobbyCreated = true;
#endif

	//login was successfull, let the player spawn
	if (PC)
	{
		PC->bAuthenticated = true;
		PC->ClientSetAuthed(true);
		PC->InitStatsAndAchievements();
	}

	//this is for people who load and get authenticated really slow
	SetSpawnTimer();
}

void AOrionGameMode::SetInitialTeam(APlayerController *PC)
{
	int32 index = 0;

	AOrionGRI *GRI = Cast<AOrionGRI>(GameState);

	//slap all players on the same team for now
	if (GRI)
		GRI->AddPlayerToTeam(Cast<AOrionPlayerController>(PC), index);
}

void AOrionGameMode::Logout(AController* Exiting)
{
	if (Cast<AOrionPlayerController>(Exiting))
	{
		SaveAllUsersStats();

		//update playerlist
		AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);

		if (GRI)
			GRI->PlayerList.Remove(Cast<AOrionPRI>(Exiting->PlayerState));

#if IS_SERVER
		//tell the playfab matchmaker that this player is leaving (needed for proper dedicated server shutdown)
		AOrionPRI *PRI = Cast<AOrionPRI>(Exiting->PlayerState);

		if (PRI)
		{
			UPlayFabRequestProxy::ServerNotifyMatchmakerPlayerLeft(PRI->PlayFabID, LobbyID);
		}

		HandleEmptyServer();
#endif
	}

	Super::Logout(Exiting);
}

void AOrionGameMode::HandleEmptyServer()
{
	//if there are 0 players left after this player leaves, close the server
	TArray<AActor*> Controllers;
	int32 Counter = 0;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrionPlayerController::StaticClass(), Controllers);

	for (int32 i = 0; i < Controllers.Num(); i++)
	{
		AOrionPlayerController *C = Cast<AOrionPlayerController>(Controllers[i]);
		if (C && C != Exiting)
			Counter++;
	}

	if (GEngine && Counter == 0)
	{
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, this, &AOrionGameMode::CloseGame, 5.0f, false);
		return;
	}
}

void AOrionGameMode::CloseGame()
{
	if (GEngine)
		GEngine->Exec(nullptr, TEXT("QUIT"), *GLog);
}

void AOrionGameMode::PlaySlowMotion(float Length)
{
	if (GetWorldSettings()->TimeDilation != 1.0f)
		return;

	GetWorldSettings()->TimeDilation = 0.25f;

	TArray<AActor*> Controllers;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrionPlayerController::StaticClass(), Controllers);
	for (int32 i = 0; i < Controllers.Num(); i++)
	{
		Cast<AOrionPlayerController>(Controllers[i])->PlaySlowMotionSound(Length * 0.25f);
	}

	GetWorldTimerManager().SetTimer(SlowMotionTimer, this, &AOrionGameMode::StopSlowMotion, Length * 0.25f, false);
}

void AOrionGameMode::StopSlowMotion()
{
	GetWorldSettings()->TimeDilation = 1.0f;
}

int32 AOrionGameMode::GetEnemyItemLevel()
{
	int32 iLvl = 1;

	switch (Difficulty)
	{
	case DIFF_EASY:
		iLvl = FMath::Clamp(ItemLevel, 1, 400);
		break;
	case DIFF_MEDIUM:
		iLvl = FMath::Clamp(ItemLevel, 1, 400);
		break;
	case DIFF_HARD:
		iLvl = FMath::Clamp(ItemLevel, 100, 400);
		break;
	case DIFF_INSANE:
		iLvl = FMath::Clamp(ItemLevel, 200, 500);
		break;
	case DIFF_REDIKULOUS:
		iLvl = FMath::Clamp(ItemLevel, 300, 600);
		break;
	}

	return iLvl;
}

//spawned from killing various types of enemies, vehicles, opening things, etc.
void AOrionGameMode::SpawnItems(AController *Killer, AActor *Spawner, const UDamageType* DamageType)
{
	bool bSuccess = false;
	//for testing purposes, just spawn random items (turn off for now!)
	if (DefaultPickupClass)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		for (TActorIterator<AOrionPlayerController> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			AOrionPlayerController *PC = *ActorItr;

			if (!PC)
				continue;

			//add some chance based dropping, so not every kill drops stufff
			float DropChance = 0.04f;

			switch (Difficulty)
			{
			case DIFF_HARD:
				DropChance = 0.05f;
				break;
			case DIFF_INSANE:
				DropChance = 0.06f;
				break;
			case DIFF_REDIKULOUS:
				DropChance = 0.07f;
				break;
			}

			if (IsSlaughter())
				DropChance /= 10.0f;

			DropChance /= 1.5f;

			bool bCraftingMat = false;

			if (FMath::FRand() > DropChance)
			{
				//drop some generic crafting mats instead
				if (FMath::FRand() < 0.1f)
				{
					bCraftingMat = true;
				}
				else
					continue;
			}

			AOrionPickup *Pickup = GetWorld()->SpawnActor<AOrionPickup>(DefaultPickupClass, Spawner->GetActorLocation(), Spawner->GetActorRotation(), SpawnInfo);
			if (Pickup)
			{
				Pickup->SetOwner(PC);
				Pickup->bOnlyRelevantToOwner = true;
				Pickup->PickupMesh->bOnlyOwnerSee = true;

				AOrionCharacter *Pawn = Cast<AOrionCharacter>(Spawner);
				if (Pawn)
				{
					int32 Level = 0;

					if (IsSlaughter())
					{
						Level = FMath::Min(FMath::Max(10, ItemLevel - 100) + 10 * FMath::RandRange(0, 2), 600);
					}
					else
					{
						switch (Difficulty)
						{
						case DIFF_EASY:
							Level = FMath::Min(FMath::Max(10, ItemLevel - 10) + 10 * FMath::RandRange(0, 3), 400);
							break;
						case DIFF_MEDIUM:
							Level = FMath::Min(FMath::Max(10, ItemLevel - 10) + 10 * FMath::RandRange(0, 3), 400);
							break;
						case DIFF_HARD:
							Level = FMath::Min(FMath::Max(10, ItemLevel - 10) + 10 * FMath::RandRange(0, 4), 400);
							break;
						case DIFF_INSANE:
							Level = FMath::Min(FMath::Max(10, ItemLevel) + 10 * FMath::RandRange(0, 3), 500);
							break;
						case DIFF_REDIKULOUS:
							Level = FMath::Min(FMath::Max(10, ItemLevel) + 10 * FMath::RandRange(1, 4), 600);
							break;
						}
					}

					if (bCraftingMat)
					{
						if (Pickup->Init(Pawn->CraftingLootTable, Level))
							bSuccess = true;
					}
					else if (Pickup->Init(Pawn->LootTable, Level))
						bSuccess = true;
				}

				if (!bSuccess)
					Pickup->Destroy();
			}
		}
	}

	//spawn some orbies, these are global and are awarded to all teammates in range
	if (DefaultPickupOrbClass)
	{
		for (TActorIterator<AOrionPlayerController> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			AOrionPlayerController *PC = *ActorItr;

			if (!PC)
				continue;

			//this is mainly so orbs don't spawn all over spectator screens
			if (!Cast<AOrionCharacter>(PC->GetPawn()))
				continue;

			float OrbChance = 5.0f;

			//AOrionPlayerController *PC = Cast<AOrionPlayerController>(Killer);
			//if (PC)
			//{
			OrbChance *= 1.0f + float(PC->GetSkillValue(SKILL_ORBDROPRATE)) / 100.0f;

			AOrionCharacter *P = Cast<AOrionCharacter>(PC->GetPawn());
			if (P)
			{
				AOrionWeapon *Weapon = P->GetWeapon();
				if (Weapon)
				{
					if (Weapon->InstantConfig.WeaponSlot == 2)
						OrbChance *= 1.0f + float(PC->GetSkillValue(SKILL_PISTOLORBS)) / 100.0f;
				}
			}

			const UOrionDamageType *DmgType = Cast<UOrionDamageType>(DamageType);
			if (DmgType && DmgType->bIsKnife)
				OrbChance *= 1.0f + float(PC->GetSkillValue(SKILL_KNIFEORBS)) / 100.0f;
			//}

			AOrionInventoryManager *Inv = PC->GetInventoryManager();

			if (Inv && Inv->HasStat(RARESTAT_MOREORBS))
				OrbChance *= 2.0f;

			//10% chance to spawn an orb
			int32 RandNum = FMath::RandRange(1, 100);

			if (RandNum <= OrbChance)
			{
				FActorSpawnParameters SpawnInfo;
				SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnInfo.Owner = PC;

				AOrionPickupOrb *Pickup = GetWorld()->SpawnActor<AOrionPickupOrb>(DefaultPickupOrbClass, Spawner->GetActorLocation(), Spawner->GetActorRotation(), SpawnInfo);

				if (Pickup)
				{
					Pickup->Init();
					Pickup->bOnlyRelevantToOwner = true;
				}
			}
		}
	}

	//drop some coins perhaps
	if (DefaultPickupCoinClass)
	{
		for (TActorIterator<AOrionPlayerController> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			AOrionPlayerController *PC = *ActorItr;

			if (!PC)
				continue;

			//this is mainly so orbs don't spawn all over spectator screens
			if (!Cast<AOrionCharacter>(PC->GetPawn()))
				continue;

			float CoinChance = 20.0f;

			//CoinChance *= 1.0f + float(PC->GetSkillValue(SKILL_ORBDROPRATE)) / 100.0f;

			AOrionCharacter *P = Cast<AOrionCharacter>(PC->GetPawn());

			AOrionInventoryManager *Inv = PC->GetInventoryManager();

			float Rate = 1.0f;

			if (Inv && Inv->HasStat(RARESTAT_GOLDFIND))
				Rate += 1.0f;

			//gold find
			if (P)
				Rate += P->GoldFind / 100.0f;

			CoinChance *= Rate;

			int32 RandNum = FMath::RandRange(1, 100);

			if (RandNum <= CoinChance)
			{
				FActorSpawnParameters SpawnInfo;
				SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnInfo.Owner = PC;

				AOrionPickupOrb *Pickup = GetWorld()->SpawnActor<AOrionPickupOrb>(DefaultPickupCoinClass, Spawner->GetActorLocation(), Spawner->GetActorRotation(), SpawnInfo);

				if (Pickup)
				{
					int32 MinAmount = 50;
					int32 MaxAmount = 50;

					switch (Difficulty)
					{
					case DIFF_EASY:
						MinAmount = 10;
						MaxAmount = 50;
						break;
					case DIFF_MEDIUM:
						MinAmount = 10;
						MaxAmount = 50;
						break;
					case DIFF_HARD:
						MinAmount = 20;
						MaxAmount = 100;
						break;
					case DIFF_INSANE:
						MinAmount = 40;
						MaxAmount = 200;
						break;
					case DIFF_REDIKULOUS:
						MinAmount = 80;
						MaxAmount = 400;
						break;
					}

					Pickup->CoinAmount = int32(float(FMath::RandRange(MinAmount, MaxAmount)) * (1.0f + float(ItemLevel) / 100.0f));
					Pickup->bOnlyRelevantToOwner = true;
				}
			}
		}
	}
}

/*void AOrionGameMode::AutoLoot()
{
	//cycle through the loot and dish it out to the players

}*/
