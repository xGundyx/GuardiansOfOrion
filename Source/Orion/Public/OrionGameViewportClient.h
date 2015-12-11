// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameViewportClient.h"
#include "OrionGameViewportClient.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API UOrionGameViewportClient : public UGameViewportClient
{
	GENERATED_BODY()
	
public:
	void UpdateActiveSplitscreenType() override;
	
	virtual TOptional<bool> QueryShowFocus(const EFocusCause InFocusCause) const override;
};
