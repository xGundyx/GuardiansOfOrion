// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionTopDownPawn.h"

void AOrionTopDownPawn::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	OutResult.Rotation = ThirdPersonCameraComponent->GetComponentRotation();
	OutResult.Location = ThirdPersonCameraComponent->GetComponentLocation();

	CameraLocation = OutResult.Location;
}

bool AOrionTopDownPawn::IsFirstPerson() const
{
	return false;
}

FVector2D AOrionTopDownPawn::GetAim(float DeltaTime)
{
	FVector pos;
	FRotator rot;

	if (CurrentWeapon == NULL)
		return FVector2D(0.0f, 0.0f);

	if (IsSprinting())
		return FVector2D(0.0f, 0.0f);

	////if (!Controller || !IsLocallyControlled())
	////	return FVector2D(AimYaw, AimPitch);

	GetPawnMesh()->GetSocketWorldLocationAndRotation(FName("Aim"), pos, rot);

	//GetWeapon()->GetWeaponMesh(false)->GetSocketWorldLocationAndRotation(FName("MuzzleFlashSocket"), pos, rot);
	FVector AimDirWS = /*(CameraLocation + Controller->GetControlRotation().Vector()*5000.0) - pos;*/ GetBaseAimRotation().Vector();
	//DrawDebugBox(GWorld, CameraLocation + Controller->GetControlRotation().Vector()*5000.0, FVector(10, 10, 10), FColor(255, 0, 0, 255), false);
	AimDirWS.Normalize();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	//we need to offset the aim a bit based on player third person camera offsets
	/////AimYaw = (2.5f + AimRotLS.Yaw) / 90.0f;
	////AimPitch = AimRotLS.Pitch / 90.0;

	////if (Role < ROLE_Authority)
	////	ServerSetAimYaw(AimYaw, AimPitch);

	////return FVector2D(AimYaw, AimPitch);

	return FVector2D((2.5f + AimRotLS.Yaw) / 90.0f, 0.0f);// AimRotLS.Pitch / 90.0);
}


