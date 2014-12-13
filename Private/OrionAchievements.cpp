// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionAchievements.h"


UOrionAchievements::UOrionAchievements(const FObjectInitializer& ObejctInitializer)
	: Super(ObejctInitializer)
{
	Achievements.Add(FAchievement(TEXT("RAPTOR HUNTER"), TEXT("KILL 5 RAPTORS"), ACH_KILLFIVERAPTORS, STAT_RAPTORKILL, 5, NULL));
	Achievements.Add(FAchievement(TEXT("TREX SLAYER"), TEXT("KILL A TRex"), ACH_KILLFIVERAPTORS, STAT_TREXKILL, 1, NULL));
}


