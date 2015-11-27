// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameUserSettings.h"
#include "OrionGameUserSettings.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API UOrionGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	void ApplyNonResolutionSettings() override;

	TArray<FString> GetSupportedResolutions();

	UPROPERTY(config)
		float MasterVolume;

	UPROPERTY(config)
		float MusicVolume;

	UPROPERTY(config)
		float VoiceVolume;

	UPROPERTY(config)
		float WeaponVolume;

	UPROPERTY(config)
		float CreatureVolume;

	UPROPERTY(config)
		float DialogueVolume;

	UPROPERTY(config)
		TArray<FString> Tutorials;

	UPROPERTY(config)
		bool TutorialsEnabled;

	UPROPERTY(config)
		bool GoreEnabled;

	UPROPERTY(config)
		bool AchievementNotifiesEnabled;

	UPROPERTY(config)
		bool ControllerRumbleEnabled;
};
