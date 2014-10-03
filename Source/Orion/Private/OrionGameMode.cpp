// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Orion.h"
#include "OrionGameMode.h"
#include "OrionHUD.h"
#include "OrionCharacter.h"

AOrionGameMode::AOrionGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnObject(TEXT("/Game/Character/Blueprints/BaseCharacterBP"));
	if (PlayerPawnObject.Object != NULL)
	{
		DefaultPawnClass = (UClass*)PlayerPawnObject.Object->GeneratedClass;
	}

	PlayerControllerClass = AOrionPlayerController::StaticClass();

	// use our custom HUD class
	HUDClass = AOrionHUD::StaticClass();
}

void AOrionGameMode::Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType)
{
	/*AShooterPlayerState* KillerPlayerState = Killer ? Cast<AShooterPlayerState>(Killer->PlayerState) : NULL;
	AShooterPlayerState* VictimPlayerState = KilledPlayer ? Cast<AShooterPlayerState>(KilledPlayer->PlayerState) : NULL;

	if (KillerPlayerState && KillerPlayerState != VictimPlayerState)
	{
	KillerPlayerState->ScoreKill(VictimPlayerState, KillScore);
	KillerPlayerState->InformAboutKill();
	}

	if (VictimPlayerState)
	{
	VictimPlayerState->ScoreDeath(KillerPlayerState, DeathScore);
	VictimPlayerState->BroadcastDeath(KillerPlayerState, DamageType);
	}*/
}
