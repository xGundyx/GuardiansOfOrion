// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SaveGame.h"
#include "OrionStats.h"
#include "OrionAchievements.h"
#include "OrionSkillTree.h"
#include "OrionSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FSavedCharacterData
{
	GENERATED_USTRUCT_BODY();

public:
	UPROPERTY(BlueprintReadWrite, Category = Save)
		TArray<FSkillGroup> SkillTree;

	//these next entries are so umg can grab them easy
	UPROPERTY(BlueprintReadWrite, Category = Save)
		int32 Level;

	UPROPERTY(BlueprintReadWrite, Category = Save)
		int32 ItemLevel;
};

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
		TArray<FString> SavedGames;

	//stats, mainly for player progression
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = ID)
		TArray<FPlayerStats> Stats;

	//inventory data
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = ID)
		TArray<FInventoryItem> InventoryData;

	//gold 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = ID)
		int32 Gold;

	//saved file name
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = ID)
		FString FileName;

	//chosen character
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = ID)
		FString CharacterClass;

	//character data like skill trees
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = ID)
		TArray<FSavedCharacterData> CharacterData;
};
