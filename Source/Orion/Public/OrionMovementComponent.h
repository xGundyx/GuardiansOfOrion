// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "OrionMovementComponent.generated.h"

/**
*
*/
UCLASS()
class UOrionMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
	UOrionMovementComponent(const FObjectInitializer& ObejctInitializer);

	virtual float GetMaxSpeed() const override;
	virtual bool IsCrouching() const override;
	virtual void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration);
};