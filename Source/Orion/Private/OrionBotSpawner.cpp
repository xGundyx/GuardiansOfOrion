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

	for (int32 i = ActivePawns.Num(); i < NumToKeepAlive; i++)
	{
		if (Pawn)
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.bNoCollisionFail = true;

			FNavLocation Loc;

			GetWorld()->GetNavigationSystem()->GetRandomPointInRadius(GetActorLocation(), SpawnRadius, Loc);

			AOrionCharacter* NewPawn = GetWorld()->SpawnActor<AOrionCharacter>(Pawn, Loc.Location + FVector(0, 0, 150.0f), GetActorRotation(), SpawnInfo);
			if (NewPawn)
				NewPawn->SpawnDefaultController();

			if (Squad == nullptr && bCreateSquad)
			{
				Squad = ConstructObject<UOrionSquad>(UOrionSquad::StaticClass(), this, NAME_None, RF_NoFlags, NULL, false, NULL);
			}

			if (Squad && NewPawn)
			{
				Squad->AddSquadMember(Cast<AOrionAIController>(NewPawn->Controller));
			}

			ActivePawns.Add(NewPawn);
		}
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

		if (ActivePawns.Num() < NumToKeepAlive && !GetWorldTimerManager().IsTimerActive(SpawnTimer))
			GetWorldTimerManager().SetTimer(SpawnTimer, this, &AOrionBotSpawner::SpawnBots, SpawnDelay, false);
	}
}

UOrionSquad *AOrionBotSpawner::GetSquad()
{
	return Squad;
}

