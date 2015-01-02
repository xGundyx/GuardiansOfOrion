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
	AOrionDropPod(const FObjectInitializer& ObejctInitializer);

	//view when spawning
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class UCameraComponent* SpawnCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class USkeletalMeshComponent* DropPodMesh;
};
