// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "OrionCharacter.h"
#include "Components/SplineComponent.h"
#include "OrionLaneSpawner.generated.h"

UCLASS()
class ORION_API AOrionLaneSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOrionLaneSpawner(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class USceneComponent* TheScene;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Mesh)
		class USkeletalMeshComponent* PreviewMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class USplineComponent* TheSpline;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
		TSubclassOf<class AOrionCharacter> ClassToSpawn;

	UFUNCTION(BlueprintCallable, Category = Spawn)
		AOrionCharacter *GetSpawnObject();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Change Preview Mesh"))
		void EventChangeMesh();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void LoadedFromAnotherClass(const FName& OldClassName) override;
#endif
};
