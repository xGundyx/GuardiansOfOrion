// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "OrionSquad.h"
#include "OrionBotSpawner.generated.h"

UCLASS()
class ORION_API AOrionBotSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOrionBotSpawner(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	//visible only in editor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class USkeletalMeshComponent* PreviewMesh;

	UPROPERTY(Transient, EditDefaultsOnly, Category = Mesh)
		TSubclassOf<class AOrionCharacter> Pawn;

	//if true, all pawns spawned from this point will be in the same squad
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Squad)
		bool bCreateSquad;

	UFUNCTION(BlueprintCallable, Category = Squad)
		AOrionSquad *GetSquad();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Squad)
		int32 NumToKeepAlive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Squad)
		int32 SpawnDelay;

	//if no one is inside this range, make dinos become inactive
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Squad)
		float MaxDistance;

	//will not spawn if there is a player within this radius
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Squad)
		float MinDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Squad)
		float FirstSpawnDelay;

	//randomly spawn inside a radius
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Squad)
		float SpawnRadius;

private:
	AOrionSquad *Squad;
	FTimerHandle SpawnTimer;
	TArray<AOrionCharacter*> ActivePawns;
	float LastPawnCheckTime;

	void SpawnBots();
	void CheckActivePawns();
};
