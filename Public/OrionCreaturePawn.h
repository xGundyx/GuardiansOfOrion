// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "OrionDinoPawn.h"
#include "OrionCreaturePawn.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API AOrionCreaturePawn : public AOrionDinoPawn
{
	GENERATED_BODY()
	
public:
	AOrionCreaturePawn(const FObjectInitializer& ObjectInitializer);

	void InitMaterials() {} //no instances needed
};
