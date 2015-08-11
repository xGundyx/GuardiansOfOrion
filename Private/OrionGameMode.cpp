// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Orion.h"
#include "OrionGameMode.h"
#include "OrionHUD.h"
#include "OrionDinoPawn.h"
#include "OrionCharacter.h"
#include "OrionPRI.h"
#include "OrionGRI.h"
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
		PC->ClientAddXPNumber(FMath::RandRange(15, 50), KilledPawn->GetActorLocation());
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

	//if this session and id aren't valid, kick the bitch
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(NewPlayerController);
	if (PC)
	{
		PC->ValidatePlayFabInfo(pfID, pfSession);
	}

	return ret;
}

void AOrionGameMode::PlayerAuthed(class AOrionPlayerController *PC, bool bSuccess)
{
	if (!bSuccess)
		GameSession->KickPlayer(PC, FText::FromString(TEXT("Playfab Authentication Failed.")));

	//login was successfull, let the player spawn
	PC->bAuthenticated = true;
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
	Super::Logout(Exiting);
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
