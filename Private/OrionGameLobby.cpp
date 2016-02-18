// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionGameLobby.h"

AOrionGameLobby::AOrionGameLobby(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnObject(TEXT("/Game/Character/Blueprints/BaseLobbyPawn"));
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
	//HUDClass = AOrionHUD::StaticClass();

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

void AOrionGameLobby::InitGRI()
{
	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);

	if (!GRI)
	{
		GetWorldTimerManager().SetTimer(GRITimer, this, &AOrionGameLobby::InitGRI, 0.1, false);
		return;
	}

	GRI->bTeamGame = bTeamGame;
	GRI->bAlwaysShowCursor = bAlwaysShowCursor;
	GRI->bTopDown = bTopDown;
	GRI->bIsLobby = true;

	TArray<AActor*> Controllers;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrionPlayerController::StaticClass(), Controllers);

	for (int32 i = 0; i < Controllers.Num(); i++)
	{
		AOrionPlayerController *C = Cast<AOrionPlayerController>(Controllers[i]);
		if (C)
		{
			C->SetThirdPerson();
		}
	}
}

void AOrionGameLobby::BeginPlay()
{
	Super::BeginPlay();

	if (GameSession)
		GameSession->MaxPlayers = 40;
}


