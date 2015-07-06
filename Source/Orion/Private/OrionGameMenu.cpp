// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionGameMenu.h"

AOrionGameMenu::AOrionGameMenu(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CharacterPlacers.SetNumUninitialized(5);
	for (int32 i = 0; i < 5; i++)
		CharacterPlacers[i] = nullptr;
}