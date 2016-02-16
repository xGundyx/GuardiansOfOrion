// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "OrionGameMode.h"
#include "OrionGameLobby.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API AOrionGameLobby : public AOrionGameMode
{
	GENERATED_BODY()
public:
	AOrionGameLobby(const FObjectInitializer& ObjectInitializer);
	void InitGRI();
	
};
