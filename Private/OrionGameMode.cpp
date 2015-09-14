// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Orion.h"
#include "OrionGameMode.h"
#include "OrionHUD.h"
#include "OrionDinoPawn.h"
#include "OrionCharacter.h"
#include "OrionPRI.h"
#include "OrionGRI.h"
#include "OrionStats.h"
#include "OrionAIController.h"
#include "OrionPickupOrb.h"

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

	//no team means every player is on the same side
	bTeamGame = false;

	TimeLimit = 0;
	WarmupTime = 0;
	bWarmingUp = true;

	// do this so the match doesn't accidently start on us
	bDelayedStart = true;
}

void AOrionGameMode::BeginPlay()
{
	Super::BeginPlay();

#if IS_SERVER
	//UOrionTCPLink::Init();
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

	HandleStats(Killer, KilledPlayer, KilledPawn, DamageType);
	
	if (PC)
	{
		AOrionDinoPawn *Dino = Cast<AOrionDinoPawn>(KilledPawn);
		if (Dino)
			SpawnItems(Dino);

		//award some experience to the team
		PC->ClientAddXPNumber(FMath::RandRange(15, 50), KilledPawn->GetActorLocation());
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
	}

	if (KilledPC)
	{
		AOrionStats *Stats = KilledPC->GetStats();

		if (Stats)
		{
			Stats->AddStatValue(KillerStatID, 1);
		}
	}
}

void AOrionGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

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
}

float AOrionGameMode::ModifyDamage(float Damage, AOrionCharacter *PawnToDamage, struct FDamageEvent const& DamageEvent, class AController *EventInstigator, class AActor *DamageCauser)
{
	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);

	if (!PawnToDamage || !PawnToDamage->Controller || !EventInstigator)
		return 0.0f;

	//if we're a team game, don't damage your teammates
	if (bTeamGame && GRI && GRI->OnSameTeam(Cast<AOrionPRI>(PawnToDamage->Controller->PlayerState), Cast<AOrionPRI>(EventInstigator->PlayerState)))
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
		int32 NumPlayers = GRI->PlayerList.Num();

		if (NumPlayers >= 4)
			Damage /= 2.5f;
		else if (NumPlayers >= 3)
			Damage /= 1.75f;
		else if (NumPlayers >= 2)
			Damage /= 1.35f;

		//also adjust based on game difficulty
		if (Difficulty == DIFF_EASY)
			Damage *= 2.0f;
		else if (Difficulty == DIFF_MEDIUM)
			Damage *= 1.0f;
		else if (Difficulty == DIFF_HARD)
			Damage *= 0.8f;
		else if (Difficulty == DIFF_INSANE)
			Damage *= 0.6f;
		else if (Difficulty == DIFF_REDIKULOUS)
			Damage *= 0.4f;
	}
	else
	{
		//adjust damage to humans
		int32 NumPlayers = GRI->PlayerList.Num();

		if (NumPlayers >= 4)
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
			Damage *= 1.1f;
		else if (Difficulty == DIFF_INSANE)
			Damage *= 1.3f;
		else if (Difficulty == DIFF_REDIKULOUS)
			Damage *= 1.5f;
	}

	return Damage;
}

APlayerController* AOrionGameMode::Login(UPlayer* NewPlayer, ENetRole RemoteRole, const FString& Portal, const FString& Options, const TSharedPtr<const FUniqueNetId>& UniqueId, FString& ErrorMessage)
{
	APlayerController *rPC = Super::Login(NewPlayer, RemoteRole, Portal, Options, UniqueId, ErrorMessage);

#if IS_SERVER
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(NewPlayer);

	//setup a stats object for this player
	if (PC)
	{
		PC->InitStatsAndAchievements();
		PC->ReadStats();
	}
#endif

	//update playerlist
	AOrionGRI *GRI = Cast<AOrionGRI>(GameState);

	if (GRI && rPC)
		GRI->PlayerList.AddUnique(Cast<AOrionPRI>(rPC->PlayerState));

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

	//if this session and id aren't valid, kick the bitch
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(NewPlayerController);
	if (PC)
	{
		AOrionPRI *PRI = Cast<AOrionPRI>(PC->PlayerState);
		if (PRI)
		{
			PRI->PlayFabID = pfID;
			PRI->SessionTicket = pfSession;
			PRI->CharacterID = pfChar;
			PRI->PlayFabName = pfName;
		}

		PC->ValidatePlayFabInfo(pfID, pfSession);
	}

	return ret;
}

void AOrionGameMode::PlayerAuthed(class AOrionPlayerController *PC, bool bSuccess)
{
#if !WITH_EDITOR
	if (!bSuccess)
		GameSession->KickPlayer(PC, FText::FromString(TEXT("Playfab Authentication Failed.")));
#endif

	//login was successfull, let the player spawn
	PC->bAuthenticated = true;
	PC->ClientSetAuthed(true);
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
	SaveAllUsersStats();

	Super::Logout(Exiting);

	//update playerlist
	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);

	if (GRI)
		GRI->PlayerList.Remove(Cast<AOrionPRI>(Exiting->PlayerState));
}

//spawned from killing various types of enemies, vehicles, opening things, etc.
void AOrionGameMode::SpawnItems(AActor *Spawner)
{
	bool bSuccess = false;
	//for testing purposes, just spawn random items (turn off for now!)
	if (false && DefaultPickupClass)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		for (TActorIterator<AOrionPlayerController> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			AOrionPlayerController *PC = *ActorItr;

			if (!PC)
				continue;

			AOrionPickup *Pickup = GetWorld()->SpawnActor<AOrionPickup>(DefaultPickupClass, Spawner->GetActorLocation(), Spawner->GetActorRotation(), SpawnInfo);
			if (Pickup)
			{
				Pickup->SetOwner(PC);
				Pickup->bOnlyRelevantToOwner = true;
				Pickup->PickupMesh->bOnlyOwnerSee = true;

				AOrionCharacter *Pawn = Cast<AOrionCharacter>(Spawner);
				if (Pawn)
					if (Pickup->Init(Pawn->LootTable, Pawn->Level))
						bSuccess = true;

				if (!bSuccess)
					Pickup->Destroy();
			}
		}
	}

	//spawn some orbies, these are global and are awarded to all teammates in range
	if (DefaultPickupOrbClass)
	{
		//10% chance to spawn an orb
		int32 RandNum = FMath::RandRange(1, 100);

		if (RandNum <= 10)
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AOrionPickupOrb *Pickup = GetWorld()->SpawnActor<AOrionPickupOrb>(DefaultPickupOrbClass, Spawner->GetActorLocation(), Spawner->GetActorRotation(), SpawnInfo);

			if (Pickup)
			{
				Pickup->Init();
			}
		}
	}
}
