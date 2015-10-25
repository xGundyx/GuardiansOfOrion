// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionAchievements.h"

AOrionAchievements::AOrionAchievements(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Achievements.Add(FAchievement(TEXT("PROGRESS"), TEXT("progress"), TEXT("REACH LEVEL 2 WITH ANY CHARACTER"), ACH_REACHLEVELTHREE, -1, 1, NULL, TEXT("HARD MODE UNLOCKED!"), 100));
	Achievements.Add(FAchievement(TEXT("GETTING THE HANG OF THINGS"), TEXT("gettingthehangofthings"), TEXT("REACH LEVEL 5 WITH ANY CHARACTER"), ACH_REACHLEVELTEN, -1, 1, NULL, TEXT("INSANE MODE UNLOCKED!"), 100));
	Achievements.Add(FAchievement(TEXT("NOOB NO LONGER"), TEXT("noobnolonger"), TEXT("REACH LEVEL 10 WITH ANY CHARACTER"), ACH_REACHLEVELTWENTY, -1, 1, NULL, TEXT("REDIKULOUS MODE UNLOCKED!"), 250));

	bInitialized = false;
	bReplicates = true;
}

void AOrionAchievements::UnlockAchievement(int32 AchID, AOrionPlayerController *PC)
{
#if IS_SERVER
	if (!bInitialized)
		return;

	Achievements[AchID].bUnlocked = true;

	PC->UnlockAchievement(TEXT("ACHIEVEMENT UNLOCKED!"), Achievements[AchID].Name);

	EventSavePlayerAchievements(PC);
#endif
}

void AOrionAchievements::ReadPlayerAchievements(AOrionPlayerController *PC)
{
	EventReadPlayerAchievements(PC);
}

void AOrionAchievements::SetAchievementValues(TArray<FAchievement> AchievementsRead)
{
	for (int32 i = 0; i < AchievementsRead.Num(); i++)
	{
		int32 index = Achievements.Find(AchievementsRead[i]);

		if (index != INDEX_NONE)
		{
			Achievements[index].bUnlocked = AchievementsRead[i].bUnlocked;
		}
	}

	bInitialized = true;
}

void AOrionAchievements::CheckForLevelUnlocks(int32 NewLevel, AOrionPlayerController *PC)
{
	if (!bInitialized)
		return;

	if (NewLevel >= 3 && !Achievements[ACH_REACHLEVELTHREE].bUnlocked)
		UnlockAchievement(ACH_REACHLEVELTHREE, PC);
	else if (NewLevel >= 10 && !Achievements[ACH_REACHLEVELTEN].bUnlocked)
		UnlockAchievement(ACH_REACHLEVELTEN, PC);
	else if (NewLevel >= 20 && !Achievements[ACH_REACHLEVELTWENTY].bUnlocked)
		UnlockAchievement(ACH_REACHLEVELTWENTY, PC);
}


