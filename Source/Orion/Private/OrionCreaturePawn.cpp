// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionDinoMovementComponent.h"
#include "OrionCreaturePawn.h"


AOrionCreaturePawn::AOrionCreaturePawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UOrionDinoMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	Shield = 0.0f;
	ShieldMax = 0.0f;
}

