// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/TextBlock.h"
#include "OrionTextBlock.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API UOrionTextBlock : public UTextBlock
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Customization")
		void SetANewFont(FSlateFontInfo NewFontInfo);
	UFUNCTION(BlueprintCallable, Category = "Customization")
		void SetANewHeightPercentage(float height);
	UFUNCTION(BlueprintCallable, Category = "Customization")
		void SetANewWrapWidth(float width);
};
