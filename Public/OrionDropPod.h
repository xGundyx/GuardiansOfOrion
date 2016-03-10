// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "OrionDropPod.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API AOrionDropPod : public AActor
{
	GENERATED_BODY()
public:
	AOrionDropPod(const FObjectInitializer& ObjectInitializer);

	//view when spawning
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class UCameraComponent* SpawnCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class USkeletalMeshComponent* DropPodMesh;

	UPROPERTY(BlueprintReadWrite, Category = Camera)
		float PodFOV;

	UPROPERTY(BlueprintReadWrite, Category = Spawn)
		bool bHasLanded;

	UPROPERTY(BlueprintReadWrite, Category = Spawn)
		FVector LandCameraPos;

	UPROPERTY(BlueprintReadWrite, Category = Spawn)
		FRotator LandCameraRot;
};
