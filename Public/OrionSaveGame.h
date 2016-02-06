// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SaveGame.h"
#include "OrionSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API UOrionSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UOrionSaveGame(){};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = ID)
		FString SavedGameName;
	

};
