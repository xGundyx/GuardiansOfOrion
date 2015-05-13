// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Orion.h"
#include "OrionGameMode.h"
#include "OrionHUD.h"
#include "OrionDinoPawn.h"
#include "OrionCharacter.h"
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

	// use our custom HUD class
	HUDClass = AOrionHUD::StaticClass();

	//this will keep players connected and keep their data persistent across map changes
	bUseSeamlessTravel = true; 

	bAlwaysShowCursor = false;

#if IS_SERVER
	UOrionTCPLink::Init();
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
	}
}

APlayerController* AOrionGameMode::Login(class UPlayer* NewPlayer, const FString& Portal, const FString& Options, const TSharedPtr<class FUniqueNetId>& UniqueId, FString& ErrorMessage)
{
	APlayerController *rPC = Super::Login(NewPlayer, Portal, Options, UniqueId, ErrorMessage);

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
	//before this player logs out, save any data they have
	UOrionTCPLink::SaveCharacter(Cast<AOrionPlayerController>(Exiting));

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

		AOrionPickup *Pickup = GetWorld()->SpawnActor<AOrionPickup>(DefaultPickupClass, Spawner->GetActorLocation(), Spawner->GetActorRotation(), SpawnInfo);
		if (Pickup)
		{
			AOrionCharacter *Pawn = Cast<AOrionCharacter>(Spawner);
			if (Pawn)
				if (Pickup->Init(Pawn->LootTable, Pawn->Level))
					bSuccess = true;

			if (!bSuccess)
				Pickup->Destroy();
		}
	}
}
