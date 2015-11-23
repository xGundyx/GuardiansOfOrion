// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "OrionVoice.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API AOrionVoice : public AActor
{
	GENERATED_BODY()
	
public:
	
	void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Voice)
		TArray<FVoiceHelper> Voices;
};
