// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionNotifyWeaponFire.h"


UOrionNotifyWeaponFire::UOrionNotifyWeaponFire(const FObjectInitializer& ObejctInitializer)
	: Super(ObejctInitializer)
{

}

void UOrionNotifyWeaponFire::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
}


