// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionGameViewportClient.h"

void UOrionGameViewportClient::UpdateActiveSplitscreenType()
{
	ActiveSplitscreenType = ESplitScreenType::Type::None;

	//Super::UpdateActiveSplitscreenType();
}

TOptional<bool> UOrionGameViewportClient::QueryShowFocus(const EFocusCause InFocusCause) const
{
	// Consider your own special logic, you might want to call the super method first.
	return false;
}

