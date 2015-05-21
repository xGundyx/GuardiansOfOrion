// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/LocalPlayer.h"
#include "OrionInventoryManager.h"
#include "OrionLocalPlayer.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API UOrionLocalPlayer : public ULocalPlayer
{
	GENERATED_BODY()
public:
	UOrionLocalPlayer(const FObjectInitializer& ObjectInitializer);
};
