// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "OrionRandomWaveSpawner.h"
#include "OrionSideMission.generated.h"

UCLASS()
class ORION_API AOrionSideMission : public AOrionRandomWaveSpawner
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOrionSideMission(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	//tell the game that this mission is complete
	UFUNCTION(BlueprintCallable, Category = Mission)
		void MissionComplete();

	//start the mission
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "EventStartMission"))
		void StartMission();

	//list of enemies for this mission (if any)
	UPROPERTY(BlueprintReadWrite, Category = Mission)
		TArray<AOrionCharacter*> Enemies;

	UFUNCTION(BlueprintCallable, Category = Mission)
		void SpawnMissionEnemies(FSpawnNumbers TypesToSpawn, AActor *Target);

	void FinishWave();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Mission)
		bool bBossMission;
};
