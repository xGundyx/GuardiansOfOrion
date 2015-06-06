// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "OrionHealthBar.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API UOrionHealthBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "Change 1P Armor"))
	//	void EventUpdate1PArmor(int32 index);

	UPROPERTY(BlueprintReadOnly, Category = Health)
		class AOrionCharacter *OwningPawn;
};
