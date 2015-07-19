// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionBotSpawner.h"


// Sets default values
AOrionBotSpawner::AOrionBotSpawner(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PreviewMesh = ObjectInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Preview"));
	PreviewMesh->AlwaysLoadOnClient = false;
	PreviewMesh->AlwaysLoadOnServer = true;
	PreviewMesh->bHiddenInGame = true;

	RootComponent = PreviewMesh;

	bCreateSquad = true;

	NumToKeepAlive = 5;
	SpawnDelay = 60;
	MaxDistance = 20000.0f;
	MinDistance = 5000.0f;
	FirstSpawnDelay = 1.0f;
	SpawnRadius = 1000.0f;

	Squad = nullptr;
}

void AOrionBotSpawner::SpawnBots()
{
	if (Role != ROLE_Authority)
		return;

	if (GetWorld() == nullptr || GetWorld()->GetNavigationSystem() == nullptr)
		return;

	for (int32 i = ActivePawns.Num(); i < NumToKeepAlive || NumToKeepAlive < 0; i++)
	{
		if (Pawn && GetWorld() && GetWorld()->GetNavigationSystem())
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.bNoCollisionFail = false;

			FNavLocation Loc;

			GetWorld()->GetNavigationSystem()->GetRandomReachablePointInRadius(GetActorLocation(), SpawnRadius, Loc);

			AOrionCharacter* NewPawn = GetWorld()->SpawnActor<AOrionCharacter>(Pawn, Loc.Location + FVector(0, 0, 150.0f), GetActorRotation(), SpawnInfo);
			if (NewPawn)
			{
				NewPawn->SpawnDefaultController();
				NewPawn->SetAIType(AIType);
			}
			else
				continue;

			if (Squad == nullptr && bCreateSquad)
			{
				Squad = GetWorld()->SpawnActor<AOrionSquad>(AOrionSquad::StaticClass(), Loc.Location + FVector(0, 0, 150.0f), GetActorRotation(), SpawnInfo); //, this, NAME_None, RF_NoFlags, NULL, false, NULL);
			}

			if (Squad && NewPawn)
			{
				Squad->AddSquadMember(Cast<AOrionAIController>(NewPawn->Controller));
			}

			ActivePawns.Add(NewPawn);

			//this means it was just a one time spawn and can be removed
			if (NumToKeepAlive < 0)
			{
				Destroy();
				return;
			}
		}
		else if (NumToKeepAlive < 0)
			return;
	}
}

// Called when the game starts or when spawned
void AOrionBotSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	//delay 5 seconds to spawn the creatures
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &AOrionBotSpawner::SpawnBots, FirstSpawnDelay, false);
}

// Called every frame
void AOrionBotSpawner::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	//check our active pawns for any that need to be removed
	CheckActivePawns();
}

void AOrionBotSpawner::CheckActivePawns()
{
	if (Role != ROLE_Authority)
		return;

	//limit how often we check
	if (GetWorld()->TimeSeconds - LastPawnCheckTime >= 1.0f)
	{
		LastPawnCheckTime = GetWorld()->TimeSeconds;

		//check to see if we need to spawn any bots or not
		for (int32 i = 0; i < ActivePawns.Num(); i++)
		{
			if (!ActivePawns[i] || ActivePawns[i]->Health <= 0)
			{
				//if (Squad && ActivePawns[i])
				//	Squad->RemoveSquadMember(Cast<AOrionAIController>(ActivePawns[i]->Controller));

				ActivePawns.RemoveAt(i);
				i = -1;
			}
		}

		if ((ActivePawns.Num() < NumToKeepAlive || NumToKeepAlive < 0) && !GetWorldTimerManager().IsTimerActive(SpawnTimer))
			GetWorldTimerManager().SetTimer(SpawnTimer, this, &AOrionBotSpawner::SpawnBots, SpawnDelay, false);
	}
}

AOrionSquad *AOrionBotSpawner::GetSquad()
{
	return Squad;
}

