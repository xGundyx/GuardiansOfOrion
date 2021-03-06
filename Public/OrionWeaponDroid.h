// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "OrionWeapon.h"
#include "OrionWeaponDroid.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API AOrionWeaponDroid : public AOrionWeapon
{
	GENERATED_BODY()
public:
	AOrionWeaponDroid(const FObjectInitializer& ObjectInitializer);

	void AttachMeshToPawn() override;
	float PlayWeaponAnimation(const FWeaponAnim& Animation, bool bReplicate, float Scale = 1.0f) override;
	FVector GetBarrelLocation(FName SocketName) override;
	void StartFire() override;
	void StopFire() override;
	//void FireSpecial(FName SocketName, FVector Direction) override;
	//void FireWeapon() override;

	USkeletalMeshComponent* GetWeaponMesh(bool bFirstPerson) const;

	FVector GetMuzzleLocation() const override;
	FVector GetMuzzleDirection() const override;
	void StopWeaponAnimation(const FWeaponAnim& Animation) override;

protected:
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* DroidGunMesh;

	FVector MuzzleLocation;
	FVector MuzzleDirection;
};
