// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "OrionWeather.h"
#include "GameFramework/PlayerController.h"
#include "OrionInventoryManager.h"
#include "OrionQuest.h"
#include "OrionQuestManager.h"
#include "OrionStats.h"
#include "OrionAchievements.h"
#include "OrionDroidPawn.h"
#include "OrionPlayerController.generated.h"

/**
*
*/
UCLASS()
class AOrionPlayerController : public APlayerController
{
	GENERATED_UCLASS_BODY()

	virtual void PlayerTick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;

	void SetupInputComponent() override;

	void OpenInventory();

	void CreateInventory();
	void GetDefaultInventory();

	virtual void Possess(APawn* aPawn) override;

	AOrionWeather* TheSun;

	UFUNCTION(exec)
		virtual void PerfectDay();

	UFUNCTION(exec)
		virtual void HeavyRain();

	UFUNCTION(exec)
		virtual void ClearNight();

	UFUNCTION(exec)
		virtual void AllArmor(int32 index);

	UFUNCTION(exec)
		virtual void ArmorColor(int32 index);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "Give Default Inventory"))
		void EventGiveDefaultInventory();

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "Open Inventory Screen"))
		void EventOpenInventoryScreen();

	//try to start this quest
	UFUNCTION(BlueprintCallable, Category = Quests)
		bool AddNewQuest(AOrionQuest *newQuest);

	//called when a player finishes a quest
	UFUNCTION(BlueprintCallable, Category = Quests)
		void CompleteQuest(AOrionQuest *questToComplete);

	//retrieve a list of quests for this player
	UFUNCTION(BlueprintCallable, Category = Quests)
		TArray<AOrionQuest*> GetAllQuests();

	//retrieve our quest info and set it for the game to use
	void InitQuestManager();
	
	//stats and achievements
	void InitStatsAndAchievements();

	//read in the stats from playfab
	void ReadStats();

	//only the server can actually save stats
	void SaveStats();

	void SetStatValue(FStatID id, int32 iAmount, int32 fAmount);
	void IncreaseStatValue(FStatID id, int32 iAmount, int32 fAmount);

	AOrionDroidPawn *DroidBuddy;

	UFUNCTION(BlueprintCallable, Category = PlayFab)
		void AttemptLogin(FString UserName, FString Password);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "LoginComplete"))
		void EventLoginComplete(const bool bResult, const FString &ID, const FString &Error);

	UFUNCTION(BlueprintCallable, Category = PlayFab)
		void CreateNewAccount(FString UserName, FString Password, FString Email);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "AccountCreateComplete"))
		void EventAccountCreationComplete(const bool bResult, const FString &Error);

	UFUNCTION(BlueprintCallable, Category = PlayFab)
		void CreateNewCharacter(FString UserName, FString Gender, FString BaseColor);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "CharacterCreateComplete"))
		void EventCharacterCreationComplete(const bool bResult, const FString &Error);

	void PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel) override;

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "CleanupUMG"))
		void EventCleanupUMG();

	UFUNCTION(BlueprintCallable, Category = Inventory)
		AOrionInventoryManager *GetInventoryManager();

	void CleanupGameViewport() override;
	void SeamlessTravelTo(class APlayerController* NewPC) override;
	void SeamlessTravelFrom(class APlayerController* OldPC) override;

	//UFUNCTION(exec)
		virtual void ClearUMG();

	virtual void Destroyed() override;

private:
	UOrionQuestManager *QuestManager;
	UOrionStats *Stats;
	UOrionAchievements *Achievements;
};
