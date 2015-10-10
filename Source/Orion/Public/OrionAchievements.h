// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "OrionAchievements.generated.h"

class AOrionPlayerController;

//UENUM()
enum EAchievementID
{
	ACH_REACHLEVELTWO = 0,
	ACH_REACHLEVELFIVE,
	ACH_REACHLEVELTEN
};

USTRUCT(BlueprintType)
struct FAchievement
{
	GENERATED_USTRUCT_BODY()

	int32 ID;			//id into array

	FString Name;

	UPROPERTY(BlueprintReadWrite, Category = Achievements)
		FString pfName;		//name for playfab

	FString Desc;

	FString Unlock;		//message informing player of any unlock achieved

	int32 Coins;		//amount of store coins this achievement is worth
	
	int32 StatID;		//stat we are linked to
	int32 Goal;			//amount of stat needed

	UTexture2D *Icon;

	UPROPERTY(BlueprintReadWrite, Category = Achievements)
		bool bUnlocked;

	FAchievement(){}
	FAchievement(FString sTitle, FString sPFName, FString sDesc, int32 nID, int32 nStatID, int32 nGoal, UTexture2D *pIcon, FString sUnlock, int32 nCoins)
	{
		ID = nID;
		Name = sTitle;
		pfName = sPFName;
		Desc = sDesc;
		StatID = nStatID;
		Goal = nGoal;
		Icon = pIcon;
		bUnlocked = false;
		Coins = nCoins;
		Unlock = sUnlock;
	}

	bool operator==(const FAchievement Other) const { return Name == Other.Name; }
};

UCLASS()
class ORION_API AOrionAchievements : public AActor
{
	GENERATED_BODY()
public:
	AOrionAchievements(const FObjectInitializer& ObjectInitializer);

	//server only
	void UnlockAchievement(int32 AchID, AOrionPlayerController *PC);

	UFUNCTION(BlueprintCallable, Category = Achievements)
		void ReadPlayerAchievements(AOrionPlayerController *PC);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Read Player Achievements From Playfab"))
		void EventReadPlayerAchievements(AOrionPlayerController *PC);

	UFUNCTION(BlueprintCallable, Category = Achievements)
		void SetAchievementValues(TArray<FAchievement> AchievementsRead);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Save Player Achievements to Playfab"))
		void EventSavePlayerAchievements(AOrionPlayerController* PC);

	void CheckForLevelUnlocks(int32 NewLevel, AOrionPlayerController *PC);

	UPROPERTY(BlueprintReadWrite, Category = Achievements)
		TArray<FAchievement> Achievements;

private:
	bool bInitialized;
};
