// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionGameTopDown.h"

AOrionGameTopDown::AOrionGameTopDown(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnObject(TEXT("/Game/Character/Blueprints/TopDownBasePawn"));
	if (PlayerPawnObject.Object != NULL)
	{
		DefaultPawnClass = (UClass*)PlayerPawnObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerControllerObject(TEXT("/Game/Character/Blueprints/TopDownPlayerController"));
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

#if IS_SERVER
	UOrionTCPLink::Init();
#endif
}


