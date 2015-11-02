// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "OrionAchievements.generated.h"

class AOrionPlayerController;

//UENUM()
enum EAchievementID
{
	//difficulty unlocks
	ACH_REACHLEVELTHREE = 0,
	ACH_REACHLEVELTEN,
	ACH_REACHLEVELTWENTY,

	//levels and stufff
	ACH_ASSAULTLEVEL10,
	ACH_ASSAULTLEVEL20,
	ACH_ASSAULTLEVEL30,
	ACH_ASSAULTLEVEL40,
	ACH_ASSAULTLEVEL50,
	ACH_SUPPORTLEVEL10,
	ACH_SUPPORTLEVEL20,
	ACH_SUPPORTLEVEL30,
	ACH_SUPPORTLEVEL40,
	ACH_SUPPORTLEVEL50,
	ACH_RECONLEVEL10,
	ACH_RECONLEVEL20,
	ACH_RECONLEVEL30,
	ACH_RECONLEVEL40,
	ACH_RECONLEVEL50,

	//combat
	ACH_RAPTORSKILLEDI,
	ACH_RAPTORSKILLEDII,
	ACH_RAPTORSKILLEDIII,
	ACH_RAPTORSKILLEDIV,
	ACH_RAPTORSKILLEDV,
	ACH_COMPYSKILLEDI,
	ACH_COMPYSKILLEDII,
	ACH_COMPYSKILLEDIII,
	ACH_COMPYSKILLEDIV,
	ACH_COMPYSKILLEDV,
	ACH_DILOSKILLEDI,
	ACH_DILOSKILLEDII,
	ACH_DILOSKILLEDIII,
	ACH_DILOSKILLEDIV,
	ACH_DILOSKILLEDV,
	ACH_RHAMSKILLEDI,
	ACH_RHAMSKILLEDII,
	ACH_RHAMSKILLEDIII,
	ACH_RHAMSKILLEDIV,
	ACH_RHAMSKILLEDV,
	ACH_ANKYSKILLEDI,
	ACH_ANKYSKILLEDII,
	ACH_ANKYSKILLEDIII,
	ACH_ANKYSKILLEDIV,
	ACH_ANKYSKILLEDV,
	ACH_TREXKILLEDI,
	ACH_TREXKILLEDII,
	ACH_TREXKILLEDIII,
	ACH_TREXKILLEDIV,
	ACH_TREXKILLEDV,
	ACH_TRIKESKILLEDI,
	ACH_TRIKESKILLEDII,
	ACH_TRIKESKILLEDIII,
	ACH_TRIKESKILLEDIV,
	ACH_TRIKESKILLEDV,
	ACH_KRUGERSKILLEDI,
	ACH_KRUGERSKILLEDII,
	ACH_KRUGERSKILLEDIII,
	ACH_KRUGERSKILLEDIV,
	ACH_KRUGERSKILLEDV,
	ACH_JECKYLSKILLEDI,
	ACH_JECKYLSKILLEDII,
	ACH_JECKYLSKILLEDIII,
	ACH_JECKYLSKILLEDIV,
	ACH_JECKYLSKILLEDV,
	ACH_NAMORSKILLEDI,
	ACH_NAMORSKILLEDII,
	ACH_NAMORSKILLEDIII,
	ACH_NAMORSKILLEDIV,
	ACH_NAMORSKILLEDV,
	ACH_ROBOTORBSKILLEDI,
	ACH_ROBOTORBSKILLEDII,
	ACH_ROBOTORBSKILLEDIII,
	ACH_ROBOTORBSKILLEDIV,
	ACH_ROBOTORBSKILLEDV,
	ACH_BONESSKILLEDI,
	ACH_BONESSKILLEDII,
	ACH_BONESSKILLEDIII,
	ACH_BONESSKILLEDIV,
	ACH_BONESSKILLEDV,
	ACH_GRUMPSSKILLEDI,
	ACH_GRUMPSSKILLEDII,
	ACH_GRUMPSSKILLEDIII,
	ACH_GRUMPSSKILLEDIV,
	ACH_GRUMPSSKILLEDV,

	//equipment/weapons
	ACH_AUTORIFLEKILLSI,
	ACH_AUTORIFLEKILLSII,
	ACH_AUTORIFLEKILLSIII,
	ACH_AUTORIFLEKILLSIV,
	ACH_AUTORIFLEKILLSV,
	ACH_PISTOLKILLSI,
	ACH_PISTOLKILLSII,
	ACH_PISTOLKILLSIII,
	ACH_PISTOLKILLSIV,
	ACH_PISTOLKILLSV,
	ACH_COMBATSHOTGUNKILLSI,
	ACH_COMBATSHOTGUNKILLSII,
	ACH_COMBATSHOTGUNKILLSIII,
	ACH_COMBATSHOTGUNKILLSIV,
	ACH_COMBATSHOTGUNKILLSV,
	ACH_MAGNUMKILLSI,
	ACH_MAGNUMKILLSII,
	ACH_MAGNUMKILLSIII,
	ACH_MAGNUMKILLSIV,
	ACH_MAGNUMKILLSV,
	ACH_SILENCEDSMGKILLSI,
	ACH_SILENCEDSMGKILLSII,
	ACH_SILENCEDSMGKILLSIII,
	ACH_SILENCEDSMGKILLSIV,
	ACH_SILENCEDSMGKILLSV,
	ACH_AUTOPISTOLKILLSI,
	ACH_AUTOPISTOLKILLSII,
	ACH_AUTOPISTOLKILLSIII,
	ACH_AUTOPISTOLKILLSIV,
	ACH_AUTOPISTOLKILLSV,

	//misc
	ACH_ORBMASTERI,
	ACH_ORBMASTERII,
	ACH_ORBMASTERIII,
	ACH_ORBMASTERIV, 
	ACH_ORBMASTERV,
	ACH_WELCOMETORELIC,
	ACH_WELCOMETOOUTBACK,
	ACH_WELCOMETOARID,
	ACH_HELPINGHANDI,
	ACH_HELPINGHANDII,
	ACH_HELPINGHANDIII,
	ACH_HELPINGHANDIV,
	ACH_HELPINGHANDV,
	ACH_WINHARD,
	ACH_WININSANE,
	ACH_TREXFATALITY,
	ACH_KRUGERFATALITY,
	ACH_JECKYLFATALITY,
	ACH_OVERTIME,

	//challenges
	ACH_200ASSIST,
	ACH_6ORBS,
	ACH_TREXNAMOR,
	ACH_IRONBALLS,
	ACH_MONSTERFRAG,
	ACH_INHUMANE,
	ACH_UNKILLABLE,
	ACH_REDIKULOUSRELIC,
	ACH_REDIKULOUSOUTBACK,
	ACH_REDIKULOUSARID,
	ACH_MASOCHIST,
	ACH_DINOSLAYER,

	ACH_NUM
};

UENUM(BlueprintType)
enum EAchCategory
{
	ACHCATEGORY_COMBAT,			//kills/deaths/gibs
	ACHCATEGORY_GEAR,			//equipping gear
	ACHCATEGORY_RAID,			//completing raids/dungeons
	ACHCATEGORY_CHARACTERS,		//levels and stuff
	ACHCATEGORY_MISC,			//weird ones
	ACHCATEGORY_CHALLENGES,		//hard achievements to unlock
	ACHCATEGORY_SPECIALEVENTS,	//christmas/easter/halloween etc

	ACHCATEGORY_NUM
};

USTRUCT(BlueprintType)
struct FAchievement
{
	GENERATED_USTRUCT_BODY()

	int32 ID;			//id into array

	UPROPERTY(BlueprintReadOnly, Category = Achievements)
		FString Name;

	UPROPERTY(BlueprintReadWrite, Category = Achievements)
		FString pfName;		//name for playfab

	UPROPERTY(BlueprintReadOnly, Category = Achievements)
		FString Desc;

	UPROPERTY(BlueprintReadOnly, Category = Achievements)
		FString Unlock;		//message informing player of any unlock achieved

	UPROPERTY(BlueprintReadOnly, Category = Achievements)
		int32 Coins;		//amount of store coins this achievement is worth
	
	UPROPERTY(BlueprintReadOnly, Category = Achievements)
		int32 StatID;		//stat we are linked to
	UPROPERTY(BlueprintReadOnly, Category = Achievements)
		int32 Goal;			//amount of stat needed

	UTexture2D *Icon;

	UPROPERTY(BlueprintReadOnly, Category = Achievements)
		TEnumAsByte<EAchCategory> Category;	//for sorting

	UPROPERTY(BlueprintReadWrite, Category = Achievements)
		bool bUnlocked;

	FAchievement(){}
	FAchievement(EAchCategory cat, FString sTitle, FString sPFName, FString sDesc, int32 nID, int32 nStatID, int32 nGoal, UTexture2D *pIcon, FString sUnlock, int32 nCoins)
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
		Category = cat;
	}

	bool operator==(const FAchievement Other) const { return pfName == Other.pfName; }
};

UCLASS()
class ORION_API AOrionAchievements : public AActor
{
	GENERATED_BODY()
public:
	AOrionAchievements(const FObjectInitializer& ObjectInitializer);

	AOrionPlayerController *PCOwner;

	FTimerHandle UnlockTimer;
	void UpdateUnlocks();

	void Init();

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
	void CheckForStatUnlocks();
	void CheckOrbs();
	void CheckForSlayer();

	UPROPERTY(BlueprintReadOnly, Category = Achievements)
		TArray<FAchievement> Achievements;

private:
	bool bInitialized;
};
