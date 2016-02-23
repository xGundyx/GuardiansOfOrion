// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Orion.h"
#include "OrionCharacter.h"
#include "OrionWeapon.h"
#include "OrionPlayerCameraManager.h"

AOrionPlayerCameraManager::AOrionPlayerCameraManager(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
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
	AActor* MyDropPod = PCOwner ? Cast<AOrionPlayerController>(PCOwner)->DropPod : NULL;
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(PCOwner);
	if (MyPawn)// && MyPawn->IsFirstPerson())
	{
		if (PC && PC->bThirdPersonCamera)
		{

		}
		else if (MyPawn->IsTopDown())
		{
			Super::UpdateCamera(DeltaTime);
			return;
		}

		if (MyPawn->GetWeapon())
		{
			TargetingFOV = MyPawn->GetWeapon()->InstantConfig.AimFOV;

			if (PC &&  MyPawn->GetWeapon()->InstantConfig.WeaponName == "SNIPER RIFLE")
			{
				TargetingFOV *= 1.0f - (PC->GetSkillValue(SKILL_THERMALZOOM) / 100.0f);
			}
		}
		else
			TargetingFOV = 60.0f;

		const float TargetFOV = MyPawn->IsAiming() ? TargetingFOV : (MyPawn->IsSprinting() ? 75.0f : 90.0f); //MyPawn->IsAiming() ? TargetingFOV : (MyPawn->IsSprinting() ? RunningFOV : NormalFOV);
		DefaultFOV = FMath::FInterpTo(DefaultFOV, TargetFOV, DeltaTime, 7.5f);
	}
	else if (MyDropPod)
	{
		const float TargetFOV = NormalFOV;
		DefaultFOV = FMath::FInterpTo(DefaultFOV, TargetFOV, DeltaTime, 7.5f);
	}

	Super::UpdateCamera(DeltaTime);

	if (MyPawn)// && MyPawn->IsFirstPerson())
	{
		MyPawn->OnCameraUpdate(GetCameraLocation(), GetCameraRotation(), DefaultFOV);
	}
}

void AOrionPlayerCameraManager::ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(PCOwner);

	if (PC && PC->bThirdPersonCamera)
		ViewPitchMin = -45.0f;
	else
		ViewPitchMin = -87.0f;

	if (GetNetMode() == NM_DedicatedServer)
	{
		Super::ProcessViewRotation(DeltaTime, OutViewRotation, OutDeltaRot);
		return;
	}
	OutViewRotation = OldRotation;

	if (!PC->bThirdPersonCamera)
		OutViewRotation = PC->GetControlRotation();

	Super::ProcessViewRotation(DeltaTime, OutViewRotation, OutDeltaRot);

	OldRotation = OutViewRotation;

	AOrionCharacter* MyPawn = PCOwner ? Cast<AOrionCharacter>(PCOwner->GetPawn()) : NULL;
	if (MyPawn)
	{
		MyPawn->ProcessAimKick(OutViewRotation, OutDeltaRot);
	}
}

