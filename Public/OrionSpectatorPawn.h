// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SpectatorPawn.h"
#include "OrionSpectatorPawn.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API AOrionSpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()
	
public:
	void CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult) override;
	
	
};
