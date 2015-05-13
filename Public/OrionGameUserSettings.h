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
};