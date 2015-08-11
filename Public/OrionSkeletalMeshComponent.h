// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SkeletalMeshComponent.h"
#include "OrionSkeletalMeshComponent.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API UOrionSkeletalMeshComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()
	
public:
	FMatrix GetRenderMatrix() const override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = FOV)
		float WeaponFOV;
};
