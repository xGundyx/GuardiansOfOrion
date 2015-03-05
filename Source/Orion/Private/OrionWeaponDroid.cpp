// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionWeaponDroid.h"


AOrionWeaponDroid::AOrionWeaponDroid(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DroidGunMesh = ObjectInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("DroidGun"));
	DroidGunMesh->AlwaysLoadOnClient = true;
	DroidGunMesh->AlwaysLoadOnServer = true;
	DroidGunMesh->bOwnerNoSee = false;
	DroidGunMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPose;
	DroidGunMesh->bCastDynamicShadow = true;
	DroidGunMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	DroidGunMesh->bChartDistanceFactor = true;
	DroidGunMesh->bGenerateOverlapEvents = false;
	//DroidGunMesh->AttachParent = Mesh;
	//DroidGunMesh->SetMasterPoseComponent(Mesh);
	DroidGunMesh->SetHiddenInGame(true);
}

USkeletalMeshComponent* AOrionWeaponDroid::GetWeaponMesh(bool bFirstPerson) const
{
	return DroidGunMesh;
}

FVector AOrionWeaponDroid::GetMuzzleLocation() const
{
	return MuzzleLocation;
}

FVector AOrionWeaponDroid::GetMuzzleDirection() const
{
	return MuzzleDirection;
}

FVector AOrionWeaponDroid::GetBarrelLocation(FName SocketName)
{
	//third person stuff
	FVector pos;
	FRotator rot;

	if (MyPawn->BodyMesh)
		MyPawn->BodyMesh->GetSocketWorldLocationAndRotation(SocketName, pos, rot);

	MuzzleLocation = pos;
	MuzzleDirection = rot.Vector();

	return pos;
}

void AOrionWeaponDroid::AttachMeshToPawn()
{
	//DroidGunMesh->AttachParent = MyPawn->Mesh;
	//DroidGunMesh->SetMasterPoseComponent(MyPawn->Mesh);

	MyPawn->SetBodyMesh(DroidGunMesh->SkeletalMesh);
}

float AOrionWeaponDroid::PlayWeaponAnimation(const FWeaponAnim& Animation)
{
	/*if (!MyPawn || !MyPawn->Mesh)
		return 0.0;

	float Duration = MyPawn->Mesh->AnimScriptInstance->Montage_Play(Animation.Pawn3P, 1.0);

	return Duration;*/

	return 0.0;
}

void AOrionWeaponDroid::StopWeaponAnimation(const FWeaponAnim& Animation)
{

}

void AOrionWeaponDroid::StartFire()
{
	if (MyPawn && MyPawn->GetCurrentMontage() != FireAnim.Pawn3P)
	{
		MyPawn->PlayAnimMontage(FireAnim.Pawn3P);
	}
}

void AOrionWeaponDroid::StopFire()
{
	if (MyPawn && MyPawn->GetCurrentMontage() == FireAnim.Pawn3P)
	{
		MyPawn->StopAnimMontage(FireAnim.Pawn3P);
	}
}


