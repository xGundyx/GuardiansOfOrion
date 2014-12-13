// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "OrionQuest.h"
#include "OrionQuestManager.generated.h"

/**
 * This class holds a player's specific quests that are currently underway or that have been completed
 */
UCLASS()
class ORION_API UOrionQuestManager : public UObject
{
	GENERATED_BODY()
public:
	UOrionQuestManager(const FObjectInitializer& ObejctInitializer);

	UPROPERTY(BlueprintReadWrite, Category = Inventory)
		TArray<AOrionQuest*> CompletedQuests;

	UPROPERTY(BlueprintReadWrite, Category = Inventory)
		TArray<AOrionQuest*> ActiveQuests;

	TArray<AOrionQuest*> GetCurrentQuests();
	void CompleteQuest(AOrionQuest* questToComplete);
	bool AddNewQuest(AOrionQuest* newQuest);
};
