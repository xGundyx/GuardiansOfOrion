// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionPRI.h"
#include "OrionGameMode.h"
#include "OrionShipPawn.h"


// Sets default values
AOrionShipPawn::AOrionShipPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

// Called when the game starts or when spawned
void AOrionShipPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOrionShipPawn::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );


}

// Called to bind functionality to input
void AOrionShipPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

void AOrionShipPawn::UnloadPlayers()
{
	for (int32 i = 0; i < Passengers.Num(); i++)
	{
		Passengers[i]->SetShip(Cast<AOrionPlayerController>(Passengers[i]->GetController()), nullptr);
	}

	//remove us from the game mode
	AOrionGameMode *Game = Cast<AOrionGameMode>(GetWorld()->GetAuthGameMode());
	if (Game)
		Game->SetShip(nullptr);
}

void AOrionShipPawn::AddPassenger(AOrionPlayerController *PC, AOrionCharacter *Pawn)
{
	if (Pawn)
	{
		if (Pawn->SetShip(PC, this))
			Passengers.Add(Pawn);
	}
}

