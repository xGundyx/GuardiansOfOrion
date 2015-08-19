// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "OrionRandomWaveSpawner.generated.h"

UENUM()
enum EAISpawnType
{
	SPAWN_RAPTOR,
	SPAWN_COMPY,
	SPAWN_DILO,
	SPAWN_TREX,
	SPAWN_TRIKE,
	SPAWN_RHAM,
	SPAWN_PTERA,
	SPAWN_ANKY,
	SPAWN_KRUGER,
	SPAWN_JECKYL,
	SPAWN_NAMOR,
	SPAWN_VIPER,
	SPAWN_ORBDROID,
	SPAWN_BONES,
	SPAWN_ASSAULT,
	SPAWN_SUPPORT,
	SPAWN_RECON,
	SPAWN_DEMO,
	SPAWN_TECH,

	SPAWN_NUM
};

UCLASS()
class ORION_API AOrionRandomWaveSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AOrionRandomWaveSpawner(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class USkeletalMeshComponent* PreviewMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = RandomWave)
		bool bRaptor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = RandomWave)
		bool bCompy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = RandomWave)
		bool bDilo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = RandomWave)
		bool bTRex;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = RandomWave)
		bool bTriceratops;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = RandomWave)
		bool bRham;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = RandomWave)
		bool bPtera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = RandomWave)
		bool bAnky;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = RandomWave)
		bool bKruger;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = RandomWave)
		bool bJeckyl;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = RandomWave)
		bool bNamor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = RandomWave)
		bool bViper;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = RandomWave)
		bool bOrbDroid;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = RandomWave)
		bool bBones;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = RandomWave)
		bool bAssault;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = RandomWave)
		bool bSupport;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = RandomWave)
		bool bRecon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = RandomWave)
		bool bTech;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = RandomWave)
		bool bDemo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = RandomWave)
		float SpawnRadius;

	void SpawnWave(int32 TypesToSpawn[SPAWN_NUM], FVector FocusLocation);
	void SpawnFailures();

	int32 MaxToSpawnPerFrame;

	FVector FocusArea;

	static TArray< TSubclassOf<class AOrionCharacter> > SpawnClasses;

	TSubclassOf<class AOrionCharacter> GetBlueprintFromPath(FString path);

private:
	TArray<bool> ClassEnabled;
	TArray<int32> FailedToSpawn;
	FTimerHandle FailedTimer;
};
