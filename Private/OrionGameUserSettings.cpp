// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionGameUserSettings.h"

void UOrionGameUserSettings::ApplyNonResolutionSettings()
{
	//custom game settings get set here
	IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Streaming.Poolsize"));
	CVar->Set(1000, ECVF_SetByConsole);

	CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.DefaultFeature.Bloom"));
	CVar->Set(true, ECVF_SetByConsole);

	Super::ApplyNonResolutionSettings();
}