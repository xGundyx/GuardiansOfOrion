// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionGameMode.h"
#include "OrionSideMission.h"


// Sets default values
AOrionSideMission::AOrionSideMission(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UBlueprint> CrateObject(TEXT("/Game/OrionActors/LootCrate/Blueprint/LargeLootCrate"));
	if (CrateObject.Object != NULL)
	{
		DefaultCrateClass = (UClass*)CrateObject.Object->GeneratedClass;
	}

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bBossMission = false;

	Tier = 0;
}

// Called when the game starts or when spawned
void AOrionSideMission::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOrionSideMission::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AOrionSideMission::MissionComplete()
{
	AOrionGameMode *Game = Cast<AOrionGameMode>(GetWorld()->GetAuthGameMode());
	if (Game)
		Game->FinishCurrentTask();
}

void AOrionSideMission::SpawnMissionEnemies(FSpawnNumbers TypesToSpawn, AActor *Target)
{
	int32 Types[SPAWN_NUM];

	for (int32 i = 0; i < SPAWN_NUM; i++)
		Types[i] = 0;

	Types[SPAWN_RAPTOR] = TypesToSpawn.NumRaptor; 
	Types[SPAWN_COMPY] = TypesToSpawn.NumCompy;
	Types[SPAWN_DILO] = TypesToSpawn.NumDilo;
	Types[SPAWN_TREX] = TypesToSpawn.NumTRex;
	Types[SPAWN_TRIKE] = TypesToSpawn.NumTrike;
	Types[SPAWN_RHAM] = TypesToSpawn.NumRham;
	Types[SPAWN_PTERA] = TypesToSpawn.NumPtera;
	Types[SPAWN_ANKY] = TypesToSpawn.NumAnky;
	Types[SPAWN_KRUGER] = TypesToSpawn.NumKruger;
	Types[SPAWN_JECKYL] = TypesToSpawn.NumJeckyl;
	Types[SPAWN_NAMOR] = TypesToSpawn.NumNamor;
	Types[SPAWN_VIPER] = TypesToSpawn.NumViper;
	Types[SPAWN_ORBDROID] = TypesToSpawn.NumOrb;
	Types[SPAWN_BONES] = TypesToSpawn.NumBones;
	Types[SPAWN_ASSAULT] = TypesToSpawn.NumAssault;
	Types[SPAWN_SUPPORT] = TypesToSpawn.NumSupport;
	Types[SPAWN_RECON] = TypesToSpawn.NumRecon;
	Types[SPAWN_DEMO] = TypesToSpawn.NumDemo;
	Types[SPAWN_TECH] = TypesToSpawn.NumTech;
	Types[SPAWN_ALLO] = TypesToSpawn.NumAllo;
	Types[SPAWN_PARA] = TypesToSpawn.NumPara;
	Types[SPAWN_STEG] = TypesToSpawn.NumSteg;
	Types[SPAWN_GRUMPS] = TypesToSpawn.NumGrumps;

	AOrionGameMode *Game = Cast<AOrionGameMode>(GetWorld()->GetAuthGameMode());

	if (Game)
	{
		for (int32 i = 0; i < SPAWN_NUM; i++)
			Game->SpawnTypes[i] = Types[i];
	}

	//SpawnWave(Types, Target);

	float BonusTime = 600.0f;

	if (Game)
	{
		if (Game->Difficulty == DIFF_HARD)
			BonusTime = 300.0f;
		else if (Game->Difficulty == DIFF_INSANE)
			BonusTime = 180.0f;
		else if (Game->Difficulty == DIFF_REDIKULOUS)
			BonusTime = 120.0f;
	}

	//offset for when it actually becomes visible;
	BonusTime += 10.0f;

	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);
	if (GRI)
		GRI->bBonusWon = false;

	bBonus = true;
	GetWorldTimerManager().SetTimer(BonusTimer, this, &AOrionSideMission::BonusFailed, BonusTime, false);
}

void AOrionSideMission::BonusFailed()
{
	bBonus = false;
}

//just single wave for testing
void AOrionSideMission::FinishWave()
{
	if (GetWorldTimerManager().IsTimerActive(BonusTimer))
		GetWorldTimerManager().ClearTimer(BonusTimer);

	//spawn a reward chest if completed on time
	if (bBonus && DefaultCrateClass)
	{
		//spawn us around the center point of the side objective, 1 for each player
		AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);
		if (GRI)
			GRI->bBonusWon = true;

		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		int32 Counter = 0;
		FVector Offset[4];
		Offset[0] = FVector(0.707f, 0.707f, 0.0f);
		Offset[1] = FVector(0.707f, -0.707f, 0.0f);
		Offset[2] = FVector(-0.707f, 0.707f, 0.0f);
		Offset[3] = FVector(-0.707f, -0.707f, 0.0f);

		for (TActorIterator<AOrionPlayerController> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			AOrionPlayerController *PC = *ActorItr;
			if (PC)
			{
				SpawnInfo.Owner = PC;
				AOrionLootCrate *Crate = GetWorld()->SpawnActor<AOrionLootCrate>(DefaultCrateClass, GetActorLocation() + Offset[Counter] * 0.0f, Offset[Counter].Rotation(), SpawnInfo);

				Counter++;

				if (Crate)
				{
				}
			}

			//only spawn one chest in local coop, mwahahaha
			AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);
			if (GRI && GRI->bIsLocalCoop)
				break;
		}
	}

	//if this is the last side mission before the match is over, tell the gri about it
	/*if (Tier == 2)
	{
		AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);
		if (GRI)
		{
			GRI->
		}
	}*/

	//let the gamemode know we're done, if this is the end, make sure to keep all players together and send them back to the space station
	MissionComplete();
}

float AOrionSideMission::GetBonusTimeRemaining()
{
	if (GetWorldTimerManager().IsTimerActive(BonusTimer))
		return GetWorldTimerManager().GetTimerRemaining(BonusTimer);

	return 0.0f;
}
