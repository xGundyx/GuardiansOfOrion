// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/EditableTextBox.h"
#include "OrionEditableText.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API UOrionEditableText : public UEditableTextBox
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Customization")
		void SetANewFont(FSlateFontInfo NewFontInfo);
};
