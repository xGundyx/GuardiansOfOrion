// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "OrionTypes.h"
#include "OrionFood.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API AOrionFood : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:
	AOrionFood(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

	void RegenFood();

	UFUNCTION(BlueprintCallable, Category = Food)
		void EatFood(AOrionCharacter *Eater);

	UPROPERTY(EditDefaultsOnly, Category = Food)
		TEnumAsByte<EFoodType> FoodType;

	//eating takes away 1 food, when it's empty the AI will ignore it and find another source
	UPROPERTY(EditDefaultsOnly, Category = Food)
		int32 MaxFoodAmount;

	UPROPERTY(EditDefaultsOnly, Category = Food)
		int32 FoodAmount;

	//how many seconds does it take to regen one food
	UPROPERTY(EditDefaultsOnly, Category = Food)
		int32 FoodRegenInSeconds;
};
