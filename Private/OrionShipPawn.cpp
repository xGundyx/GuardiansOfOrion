// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
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

