// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "ORionMovementComponent.h"
#include "OrionDinoMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API UOrionDinoMovementComponent : public UOrionMovementComponent
{
	GENERATED_BODY()
public:
	UOrionDinoMovementComponent(const FObjectInitializer& ObjectInitializer);

	virtual float GetMaxSpeed() const override;
	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration) override;
};
