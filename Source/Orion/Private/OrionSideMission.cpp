// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionGameMode.h"
#include "OrionSideMission.h"


// Sets default values
AOrionSideMission::AOrionSideMission(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bBossMission = false;
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
	//Types[SPAWN_GRUMPS] = TypesToSpawn.NumTech;
	Types[SPAWN_ALLO] = TypesToSpawn.NumAllo;
	Types[SPAWN_PARA] = TypesToSpawn.NumPara;
	Types[SPAWN_STEG] = TypesToSpawn.NumSteg;

	SpawnWave(Types, Target);
}

//just single wave for testing
void AOrionSideMission::FinishWave()
{
	MissionComplete();
}
