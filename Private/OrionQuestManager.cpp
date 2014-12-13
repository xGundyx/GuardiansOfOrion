// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionQuestManager.h"


UOrionQuestManager::UOrionQuestManager(const FObjectInitializer& ObejctInitializer)
	: Super(ObejctInitializer)
{

}

TArray<AOrionQuest*> UOrionQuestManager::GetCurrentQuests()
{
	TArray<AOrionQuest*> theQuests;

	theQuests = CompletedQuests;

	theQuests.Append(ActiveQuests);

	return theQuests;
}

void UOrionQuestManager::CompleteQuest(AOrionQuest* questToComplete)
{
	CompletedQuests.Add(questToComplete);
	ActiveQuests.Remove(questToComplete);
}

bool UOrionQuestManager::AddNewQuest(AOrionQuest* newQuest)
{
	ActiveQuests.Add(newQuest);

	return true;
}