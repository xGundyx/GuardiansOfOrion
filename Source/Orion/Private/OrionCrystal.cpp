// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionGameMode.h"
#include "OrionCrystal.h"


// Sets default values
AOrionCrystal::AOrionCrystal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AOrionCrystal::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOrionCrystal::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AOrionCrystal::ObjectiveCompleted()
{
	AOrionGameMode *Game = Cast<AOrionGameMode>(GetWorld()->GetAuthGameMode());

	if (Game)
		Game->FinishCurrentTask();
}

