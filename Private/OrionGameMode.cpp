// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Orion.h"
#include "OrionGameMode.h"
#include "OrionHUD.h"
#include "OrionDinoPawn.h"
#include "OrionCharacter.h"
#include "OrionPRI.h"
#include "OrionGRI.h"

AOrionGameMode::AOrionGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnObject(TEXT("/Game/Character/Blueprints/BasePawn"));
	if (PlayerPawnObject.Object != NULL)
	{
		DefaultPawnClass = (UClass*)PlayerPawnObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> PickupObject(TEXT("/Game/Pickups/Blueprints/BasePickup"));
	if (PickupObject.Object != NULL)
	{
		DefaultPickupClass = (UClass*)PickupObject.Object->GeneratedClass;
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
}

void AOrionGameMode::BeginPlay()
{
	Super::BeginPlay();

#if IS_SERVER
	UOrionTCPLink::Init();
#endif

	InitGRI();
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
	UOrionTCPLink::Update();
#endif
}

void AOrionGameMode::Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType)
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Killer);

	if (PC)
	{
		//make sure to update this player's stats
		//PC->IncreaseStatValue(STAT_RAPTORKILL, 1, 0.0f);
	}
	
	if (PC)
	{
		AOrionDinoPawn *Dino = Cast<AOrionDinoPawn>(KilledPawn);
		if (Dino)
			SpawnItems(Dino);

		//award some experience to the team
	}
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

	return Damage;
}

APlayerController* AOrionGameMode::Login(UPlayer* NewPlayer, ENetRole RemoteRole, const FString& Portal, const FString& Options, const TSharedPtr<FUniqueNetId>& UniqueId, FString& ErrorMessage)
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

	return rPC;
}

void AOrionGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}

//spawned from killing various types of enemies, vehicles, opening things, etc.
void AOrionGameMode::SpawnItems(AActor *Spawner)
{
	bool bSuccess = false;
	//for testing purposes, just spawn random items
	if (DefaultPickupClass)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.bNoCollisionFail = true;

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
}
