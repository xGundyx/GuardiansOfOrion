// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "OrionCharacter.h"
#include "OrionTopDownPawn.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API AOrionTopDownPawn : public AOrionCharacter
{
	GENERATED_BODY()
	
public:
	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;
	bool IsFirstPerson() const;
	FVector2D GetAim(float DeltaTime);
};
