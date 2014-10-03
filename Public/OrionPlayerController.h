// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "OrionWeather.h"
#include "GameFramework/PlayerController.h"
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
};
