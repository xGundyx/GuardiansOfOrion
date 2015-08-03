// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionGameUserSettings.h"

/*static TAutoConsoleVariable<int32> CVarResolutionQuality(
	TEXT("sg.ResolutionQuality"),
	100,
	TEXT("Scalability quality state (internally used by scalability system, ini load/save or using SCALABILITY console command)\n")
	TEXT(" 50..100, default: 100"),
	ECVF_ScalabilityGroup);*/

void UOrionGameUserSettings::ApplyNonResolutionSettings()
{
	ValidateSettings();

	bool bIsDirty = IsDirty();
	EWindowMode::Type NewWindowMode = GetFullscreenMode();

	// Update vsync cvar
	{
		static auto CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.VSync"));
		CVar->Set(IsVSyncEnabled(), ECVF_SetByGameSetting);
	}

	// in init those are loaded earlier, after that we apply consolevariables.ini
	if (GEngine->IsInitialized())
	{
		Scalability::SetQualityLevels(ScalabilityQuality);
	}

	IConsoleManager::Get().CallAllConsoleVariableSinks();

	//custom game settings get set here
	/*IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Streaming.Poolsize"));
	CVar->Set(1000, ECVF_SetByConsole);

	CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.DefaultFeature.Bloom"));
	CVar->Set(true, ECVF_SetByConsole);*/
}

TArray<FString> UOrionGameUserSettings::GetSupportedResolutions()
{
	FScreenResolutionArray ResList;
	TArray<FString> ResolutionString;

	if (RHIGetAvailableResolutions(ResList, false))
	{
		for (const FScreenResolutionRHI& EachResolution : ResList)
		{
			ResolutionString.AddUnique(FString::Printf(TEXT("%ix%i"), EachResolution.Width, EachResolution.Height));
		}
	}

	return ResolutionString;
}