// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Orion.h"
#include "OrionCharacter.h"
#include "OrionWeapon.h"
#include "OrionPlayerCameraManager.h"

AOrionPlayerCameraManager::AOrionPlayerCameraManager(const FObjectInitializer& ObejctInitializer)
: Super(ObejctInitializer)
{
	NormalFOV = 90.0f;
	TargetingFOV = 60.0f;
	RunningFOV = 75.0f;
	ViewPitchMin = -87.0f;
	ViewPitchMax = 87.0f;
	bAlwaysApplyModifiers = true;
}

void AOrionPlayerCameraManager::UpdateCamera(float DeltaTime)
{
	AOrionCharacter* MyPawn = PCOwner ? Cast<AOrionCharacter>(PCOwner->GetPawn()) : NULL;
	if (MyPawn)// && MyPawn->IsFirstPerson())
	{
		if (MyPawn->GetWeapon())
			TargetingFOV = MyPawn->GetWeapon()->InstantConfig.AimFOV;
		else
			TargetingFOV = 60.0f;

		const float TargetFOV = MyPawn->IsAiming() ? TargetingFOV : (MyPawn->IsSprinting() ? RunningFOV : NormalFOV);
		DefaultFOV = FMath::FInterpTo(DefaultFOV, TargetFOV, DeltaTime, 7.5f);
	}

	Super::UpdateCamera(DeltaTime);

	if (MyPawn)// && MyPawn->IsFirstPerson())
	{
		MyPawn->OnCameraUpdate(GetCameraLocation(), GetCameraRotation(), DefaultFOV);
	}
}

void AOrionPlayerCameraManager::ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot)\
{
	OutViewRotation = OldRotation;

	Super::ProcessViewRotation(DeltaTime, OutViewRotation, OutDeltaRot);

	OldRotation = OutViewRotation;

	AOrionCharacter* MyPawn = PCOwner ? Cast<AOrionCharacter>(PCOwner->GetPawn()) : NULL;
	if (MyPawn)
	{
		MyPawn->ProcessAimKick(OutViewRotation, OutDeltaRot);
	}
}

