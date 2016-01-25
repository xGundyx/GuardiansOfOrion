// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "OrionWeapon.h"
#include "OrionWeaponFlameThrower.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API AOrionWeaponFlameThrower : public AOrionWeapon
{
	GENERATED_BODY()
	
public:
	virtual void FireWeapon() override;
	virtual void StopFire() override;
	
	UFUNCTION(BlueprintImplementableEvent, Category = FlameThrower)
		void EventPlayFlames();

	UFUNCTION(BlueprintImplementableEvent, Category = FlameThrower)
		void EventStopFlames();
};
