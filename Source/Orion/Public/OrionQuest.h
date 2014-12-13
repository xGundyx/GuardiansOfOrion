// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "OrionQuest.generated.h"

/**
 * 
 */

UENUM()
enum QuestType
{
	QUEST_KILL,
	QUEST_RETRIEVE
};

USTRUCT()
struct FQuestObjective
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Quest)
		TEnumAsByte<QuestType> qType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Quest)
		int32 GoalAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Quest)
		UClass *QuestActor;
};

UCLASS()
class ORION_API AOrionQuest : public AActor
{
	GENERATED_BODY()
public:
	AOrionQuest(const FObjectInitializer& ObejctInitializer);

	//next quest to start as part of a chain (if any)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Quest)
		AOrionQuest *NextQuestInChain;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Quest)
		FString Title;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Quest)
		FString Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Quest)
		int32 RecommendedLevel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Quest)
		TArray<FQuestObjective> Objectives;
};
