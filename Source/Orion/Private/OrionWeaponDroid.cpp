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
	//DroidGunMesh->bChartDistanceFactor = true;
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

float AOrionWeaponDroid::PlayWeaponAnimation(const FWeaponAnim& Animation, bool bReplicate, float Scale)
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
		FWeaponAnim Anim;
		Anim.Pawn3P = FireAnim.Pawn3P;

		MyPawn->OrionPlayAnimMontage(Anim, 1.0f, "", true, true, true);// PlayAnimMontage(FireAnim.Pawn3P);
	}
}

void AOrionWeaponDroid::StopFire()
{
	if (MyPawn && MyPawn->GetCurrentMontage() == FireAnim.Pawn3P)
	{
		MyPawn->StopAnimMontage(FireAnim.Pawn3P);
	}
}

/*void AOrionWeaponDroid::FireSpecial()
{
	FireWeapon();
}

void AOrionWeaponDroid::FireWeapon()
{
	if (!CanFire())
		return;

	if (InstantConfig.bAutomatic)
		GetWorldTimerManager().SetTimer(FireTimer, this, &AOrionWeapon::FireWeapon, InstantConfig.TimeBetweenShots, false);

	LastFireTime = GetWorld()->GetTimeSeconds();

	UseAmmo();

	for (int32 i = 0; i < InstantConfig.NumPellets; i++)
	{
		const int32 RandomSeed = FMath::Rand();
		FRandomStream WeaponRandomStream(RandomSeed);
		const float CurrentSpread = GetCurrentSpread();
		const float ConeHalfAngle = FMath::DegreesToRadians(CurrentSpread * 0.5f);

		const FVector AimDir = GetAdjustedAim();
		const FVector StartTrace = GetBarrelLocation(SocketName); //GetCameraDamageStartLocation(AimDir);
		const FVector ShootDir = WeaponRandomStream.VRandCone(AimDir, ConeHalfAngle, ConeHalfAngle);
		const FVector EndTrace = StartTrace + ShootDir * InstantConfig.WeaponRange;

		const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
		ProcessInstantHit(Impact, StartTrace, ShootDir, RandomSeed, CurrentSpread);
	}

	CurrentFiringSpread = FMath::Min(InstantConfig.FiringSpreadMax, CurrentFiringSpread + InstantConfig.FiringSpreadIncrement) * GetSpreadModifier();

	if (MyPawn)
		MyPawn->AddAimKick(FRotator(FMath::FRandRange(1.0f, 2.0f), FMath::FRandRange(-1.0f, 1.0f), 0.0f));
}*/


