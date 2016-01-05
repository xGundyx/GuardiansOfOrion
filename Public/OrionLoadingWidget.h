// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "OrionLoadingWidget.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API UOrionLoadingWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;
};
