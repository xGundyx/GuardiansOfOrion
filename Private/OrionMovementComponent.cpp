// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "Orion.h"
#include "OrionCharacter.h"
#include "OrionMovementComponent.h"


UOrionMovementComponent::UOrionMovementComponent(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{

}

float UOrionMovementComponent::GetMaxSpeed() const
{
	float SpeedMod = Super::GetMaxSpeed();

	const AOrionCharacter* OrionCharacterOwner = Cast<AOrionCharacter>(PawnOwner);
	if (OrionCharacterOwner)
	{
		if (OrionCharacterOwner->bDuck)
		{
			SpeedMod *= 0.4f;
			if (OrionCharacterOwner->bAim)
				SpeedMod *= 0.15;
		}
		else if (OrionCharacterOwner->bRun && IsMovingOnGround())
			SpeedMod *= 1.0f;
		else if (OrionCharacterOwner->bAim)
			SpeedMod *= 0.25;
		else
			SpeedMod *= 0.55f;
	}

	return SpeedMod;
}

bool UOrionMovementComponent::IsCrouching() const
{
	const AOrionCharacter* OrionCharacterOwner = Cast<AOrionCharacter>(PawnOwner);
	if (OrionCharacterOwner)
	{
		return OrionCharacterOwner->IsDucking();
	}

	return false;
}

void UOrionMovementComponent::CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration)
{
	const AOrionCharacter* OrionCharacterOwner = Cast<AOrionCharacter>(PawnOwner);
	if (OrionCharacterOwner)
	{
		if (OrionCharacterOwner->IsRolling())
		{
			Velocity.Normalize();
			Velocity = Velocity*800.0f;
			return;
		}
		else if (OrionCharacterOwner->IsSprinting() && IsMovingOnGround())
		{
			Velocity = OrionCharacterOwner->GetActorRotation().Vector()*GetMaxSpeed();
			return;
		}
	}

	Super::CalcVelocity(DeltaTime, Friction, bFluid, BrakingDeceleration);
}