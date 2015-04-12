// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "OrionPlayerCameraManager.h"
#include "OrionTopDownCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API AOrionTopDownCameraManager : public AOrionPlayerCameraManager
{
	GENERATED_BODY()
	
public:
	AOrionTopDownCameraManager(const FObjectInitializer& ObjectInitializer);
	void UpdateCamera(float DeltaTime);
};
