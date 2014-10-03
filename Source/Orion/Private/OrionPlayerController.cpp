// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Orion.h"
#include "OrionCharacter.h"
#include "OrionPlayerCameraManager.h"
#include "OrionPlayerController.h"

AOrionPlayerController::AOrionPlayerController(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{
	PlayerCameraManagerClass = AOrionPlayerCameraManager::StaticClass();
}

void AOrionPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AOrionPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (TheSun == NULL && IsLocalPlayerController())
	{
		for (FActorIterator It(GetWorld()); It; ++It)
		{
			TheSun = Cast<AOrionWeather>(*It);
			if (TheSun != NULL)
			{
				TheSun->PlayerOwner = this;
				TheSun->ChooseWeather();
				break;
			}
		}
	}

	if (TheSun)
		TheSun->UpdateWeather(DeltaTime);
}

void AOrionPlayerController::PerfectDay()
{
	TheSun->PerfectDay();
}

void AOrionPlayerController::HeavyRain()
{
	TheSun->HeavyRain();
}

void AOrionPlayerController::ClearNight()
{
	TheSun->ClearNight();
}

void AOrionPlayerController::AllArmor(int32 index)
{
	AOrionCharacter* myPawn = Cast<AOrionCharacter>(GetPawn());

	if (myPawn)
	{
		myPawn->EventUpdateHelmet(index);
		myPawn->EventUpdateBody(index);
		myPawn->EventUpdateArms(index);
		myPawn->EventUpdateLegs(index);
		myPawn->EventUpdateFlight1(index);
		myPawn->EventUpdateFlight2(index);
	}
}

void AOrionPlayerController::ArmorColor(int32 index)
{
	AOrionCharacter* myPawn = Cast<AOrionCharacter>(GetPawn());

	if (myPawn)
	{
		myPawn->EventUpdateArmorColor(index);
	}
}