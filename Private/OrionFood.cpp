// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionFood.h"

AOrionFood::AOrionFood(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FoodType = FOOD_GRASS;
	MaxFoodAmount = 5;
	FoodAmount = 5;
	FoodRegenInSeconds = 60;

	//make this non solid, but still generate hit events
	GetStaticMeshComponent()->SetCollisionResponseToAllChannels(ECR_Overlap);
}

void AOrionFood::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, this, &AOrionFood::RegenFood, FoodRegenInSeconds, false);
}

void AOrionFood::EatFood(AOrionCharacter *Eater)
{
	if (FoodAmount == 0 || Eater == nullptr)
		return;

	FoodAmount--;

	Eater->EatFood(this);
}

void AOrionFood::RegenFood()
{
	if (FoodAmount < MaxFoodAmount)
		FoodAmount++;

	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, this, &AOrionFood::RegenFood, FoodRegenInSeconds, false);
}




