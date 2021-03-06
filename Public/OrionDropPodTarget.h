// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "OrionDropPodTarget.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API AOrionDropPodTarget : public AActor
{
	GENERATED_BODY()
	
public:

	AOrionDropPodTarget();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawn)
		int32 TeamIndex;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawn)
		bool Coop;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawn)
		bool PVP;
};
