// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "OrionNotifyWeaponFire.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API UOrionNotifyWeaponFire : public UAnimNotify
{
	GENERATED_BODY()
public:
	UOrionNotifyWeaponFire(const FObjectInitializer& ObejctInitializer);

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
};
