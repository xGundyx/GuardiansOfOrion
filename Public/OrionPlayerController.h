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
#include "OrionDropPod.h"
#include "OrionPlayerController.generated.h"

/**
*
*/

USTRUCT(BlueprintType)
struct FCharacterData
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(BlueprintReadOnly, Category = Character)
		FString pName;

	UPROPERTY(BlueprintReadOnly, Category = Character)
		FString Sex;

	UPROPERTY(BlueprintReadOnly, Category = Character)
		int32 Level;

	UPROPERTY(BlueprintReadOnly, Category = Character)
		int32 Exp;

	UPROPERTY(BlueprintReadOnly, Category = Character)
		int32 HelmetID;

	UPROPERTY(BlueprintReadOnly, Category = Character)
		int32 ChestID;

	UPROPERTY(BlueprintReadOnly, Category = Character)
		int32 HandsID;

	UPROPERTY(BlueprintReadOnly, Category = Character)
		int32 LegsID;

	UPROPERTY(BlueprintReadOnly, Category = Character)
		int32 Weapon;

	void Reset()
	{
		pName = FString("");
		Level = 0;
		Exp = 0;
		HelmetID = 0;
		ChestID = 0;
		HandsID = 0;
		LegsID = 0;
		Sex = FString("");
		Weapon = 0;
	}
};

UCLASS()
class AOrionPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AOrionPlayerController(const FObjectInitializer& ObejctInitializer);

	virtual void PlayerTick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;

	void SetupInputComponent() override;

	void GetPlayerViewPoint(FVector& OutCamLoc, FRotator& OutCamRot) const override;

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
		void EventLoginComplete(const bool bResult, const FString &Msg);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "DrawCharacterDatas"))
		void EventDrawCharacterDatas(FCharacterData charData1, FCharacterData charData2, FCharacterData charData3, FCharacterData charData4);

	UFUNCTION(BlueprintCallable, Category = PlayFab)
		void CreateNewAccount(FString UserName, FString Password, FString Email);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "AccountCreateComplete"))
		void EventAccountCreationComplete(const bool bResult, const FString &Error);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "AccountCreated"))
		void EventAccountCreated(const bool bResult, const FString &Error);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "CharacterFinalized"))
		void EventCharacterFinalized(const bool bResult, const FString &Error);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "ReceiveErrorMessage"))
		void ReceiveErrorMessage(const FString &Message);

	UFUNCTION(BlueprintCallable, Category = PlayFab)
		void CreateNewCharacter(FString UserName, FString Gender, FString BaseColor);

	UFUNCTION(BlueprintCallable, Category = PlayFab)
		void SelectCharacterAtIndex(int32 Index);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "SelectCharacterComplete"))
		void EventSelectCharacterComplete(const bool bResult, const FString &Error);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "CharacterCreateComplete"))
		void EventCharacterCreationComplete(const bool bResult, const FString &Error);

	void PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel) override;

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "CleanupUMG"))
		void EventCleanupUMG();

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "Respawn"))
		void EventRespawn();

	UFUNCTION(BlueprintCallable, Category = Inventory)
		AOrionInventoryManager *GetInventoryManager();

	UFUNCTION(BlueprintCallable, Category = Respawn)
		void SetDropPod(AOrionDropPod *Pod);

	UPROPERTY()
		AOrionDropPod *DropPod;

	void PawnPendingDestroy(APawn* P) override;
	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;

	void CleanupGameViewport() override;
	void SeamlessTravelTo(class APlayerController* NewPC) override;
	void SeamlessTravelFrom(class APlayerController* OldPC) override;

	//UFUNCTION(exec)
		virtual void ClearUMG();

	virtual void Destroyed() override;

	void UpdateRotation(float DeltaTime) override;

	UFUNCTION(Reliable, server, WithValidation)
		void ServerAllArmor(int32 index);

public:
	UPROPERTY()//VisibleAnywhere, BlueprintReadOnly, Category = Rain)
		UParticleSystemComponent *RainPSC;

private:
	UOrionQuestManager *QuestManager;
	UOrionStats *Stats;
	UOrionAchievements *Achievements;
};
