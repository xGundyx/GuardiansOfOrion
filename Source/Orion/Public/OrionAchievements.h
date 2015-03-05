// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "OrionAchievements.generated.h"

/**
 * 
 */

//update backendmappings.h if you add more achievements
UENUM()
enum EAchievementID
{
	ACH_KILLFIVERAPTORS,
	ACH_TREXSLAYER
};

USTRUCT()
struct FAchievement
{
	GENERATED_USTRUCT_BODY()

	int32 ID;

	FString Name;
	FString Desc;
	
	int32 StatID;		//stat we are linked to
	int32 Goal;			//amount of stat needed

	UTexture2D *Icon;

	FAchievement(){}
	FAchievement(FString sTitle, FString sDesc, int32 nID, int32 nStatID, int32 nGoal, UTexture2D *pIcon)
	{
		ID = nID;
		Name = sTitle;
		Desc = sDesc;
		StatID = nStatID;
		Goal = nGoal;
		Icon = pIcon;
	}
};

UCLASS()
class ORION_API UOrionAchievements : public UObject
{
	GENERATED_BODY()
public:
	UOrionAchievements(const FObjectInitializer& ObjectInitializer);
	
private:
	TArray<FAchievement> Achievements;
};
