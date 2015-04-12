// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionTopDownCameraManager.h"

AOrionTopDownCameraManager::AOrionTopDownCameraManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NormalFOV = 75.0f;// 90.0f;
	TargetingFOV = 90.0f;// 60.0f;
	RunningFOV = 65.0f;// 75.0f;
	ViewPitchMin = -87.0f;
	ViewPitchMax = 87.0f;
	bAlwaysApplyModifiers = true;
}

void AOrionTopDownCameraManager::UpdateCamera(float DeltaTime)
{
	AOrionCharacter* MyPawn = PCOwner ? Cast<AOrionCharacter>(PCOwner->GetPawn()) : NULL;
	AActor* MyDropPod = PCOwner ? Cast<AOrionPlayerController>(PCOwner)->DropPod : NULL;
	if (MyPawn)// && MyPawn->IsFirstPerson())
	{
		if (MyPawn->GetWeapon())
			TargetingFOV = 90.0f + (90.0f - MyPawn->GetWeapon()->InstantConfig.AimFOV);
		else
			TargetingFOV = 60.0f;

		const float TargetFOV = MyPawn->IsAiming() ? TargetingFOV : (MyPawn->IsSprinting() ? RunningFOV : NormalFOV);
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


