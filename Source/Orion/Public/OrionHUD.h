// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once 
#include "GameFramework/HUD.h"
#include "OrionHUD.generated.h"

UCLASS()
class ORION_API AOrionHUD : public AHUD
{
	GENERATED_BODY()
public:
	AOrionHUD(const FObjectInitializer& ObjectInitializer);

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	virtual void BeginPlay() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;
	class UTexture2D* LoadingTex;

};

