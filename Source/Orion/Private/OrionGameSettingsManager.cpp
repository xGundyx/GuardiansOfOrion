#include "Orion.h"
#include "OrionGameSettingsManager.h"

UOrionGameUserSettings *UOrionGameSettingsManager::Settings = nullptr;
UInputSettings *UOrionGameSettingsManager::InputSettings = nullptr;

// Sets default values
UOrionGameSettingsManager::UOrionGameSettingsManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UOrionGameSettingsManager::InitSettings()
{
	if (GEngine)
		Settings = Cast<UOrionGameUserSettings>(GEngine->GameUserSettings);
}

void UOrionGameSettingsManager::InitInputSettings()
{
	InputSettings = const_cast<UInputSettings*>(GetDefault<UInputSettings>());
}

FQualitySettings UOrionGameSettingsManager::GetQualityLevels()
{
	if (!Settings)
		InitSettings();

	FQualitySettings OutQuality;

	OutQuality.AntiAliasingQuality = Settings->ScalabilityQuality.AntiAliasingQuality;
	OutQuality.EffectsQuality = Settings->ScalabilityQuality.EffectsQuality;
	OutQuality.PostProcessQuality = Settings->ScalabilityQuality.PostProcessQuality;
	OutQuality.ResolutionQuality = Settings->ScalabilityQuality.ResolutionQuality;
	OutQuality.ShadowQuality = Settings->ScalabilityQuality.ShadowQuality;
	OutQuality.TextureQuality = Settings->ScalabilityQuality.TextureQuality;
	OutQuality.ViewDistanceQuality = Settings->ScalabilityQuality.ViewDistanceQuality;

	return OutQuality;
}

void UOrionGameSettingsManager::SetQualityLevels(FQualitySettings InSettings)
{
	if (!Settings)
		InitSettings();

	Settings->ScalabilityQuality.AntiAliasingQuality = InSettings.AntiAliasingQuality;
	Settings->ScalabilityQuality.EffectsQuality = InSettings.EffectsQuality;
	Settings->ScalabilityQuality.PostProcessQuality = InSettings.PostProcessQuality;
	Settings->ScalabilityQuality.ResolutionQuality = InSettings.ResolutionQuality;
	Settings->ScalabilityQuality.ShadowQuality = InSettings.ShadowQuality;
	Settings->ScalabilityQuality.TextureQuality = InSettings.TextureQuality;
	Settings->ScalabilityQuality.ViewDistanceQuality = InSettings.ViewDistanceQuality;
}


FScreenSettings UOrionGameSettingsManager::GetScreenSettings()
{
	if (!Settings)
		InitSettings();

	FScreenSettings OutSettings;
	FIntPoint Resolution = Settings->GetScreenResolution();

	OutSettings.ScreenSizeX = Resolution.X;
	OutSettings.ScreenSizeY = Resolution.Y;
	OutSettings.ScreenMode = int32(Settings->GetFullscreenMode());
	OutSettings.bUseVSync = Settings->bUseVSync;

	return OutSettings;
}

void UOrionGameSettingsManager::SetScreenSettings(FScreenSettings InSettings)
{
	if (!Settings)
		InitSettings();

	Settings->SetFullscreenMode(static_cast<EWindowMode::Type>(InSettings.ScreenMode));
	Settings->SetScreenResolution(FIntPoint(InSettings.ScreenSizeX, InSettings.ScreenSizeY));
	Settings->bUseVSync = InSettings.bUseVSync;
}

void UOrionGameSettingsManager::ApplyNonScreenSettings()
{
	if (!Settings)
		InitSettings();

	Settings->ApplyNonResolutionSettings();
}

void UOrionGameSettingsManager::ApplyScreenSettings()
{
	if (!Settings)
		InitSettings();

	Settings->ApplyResolutionSettings(true);
}

void UOrionGameSettingsManager::ApplyGraphicSettings()
{
	if (!Settings)
		InitSettings();

	Settings->ApplyNonResolutionSettings();
	Settings->ApplyResolutionSettings(true);
}

bool UOrionGameSettingsManager::RebindKey(FString ActionName, FKey NewKey, FName OriginalKey, float NewScale)
{
	if (!InputSettings)
		InitInputSettings();

	TArray<FInputActionKeyMapping>& Actions = InputSettings->ActionMappings;

	bool bFound = false;
	for (FInputActionKeyMapping& Each : Actions)
	{
		if (Each.ActionName.ToString() == ActionName && Each.Key == FKey(OriginalKey))
		{
			Each.Key = NewKey;
			bFound = true;
			break;
		}
	}

	if (!bFound)
	{
		TArray<FInputAxisKeyMapping>& Axis = InputSettings->AxisMappings;

		for (FInputAxisKeyMapping& Each : Axis)
		{
			if (Each.AxisName.ToString() == ActionName && Each.Scale == NewScale && Each.Key == FKey(OriginalKey))
			{
				Each.Key = NewKey;
				bFound = true;
				break;
			}
		}
	}

	return bFound;
}

FString UOrionGameSettingsManager::GetKeyForAction(FString ActionName, bool bAxis, float Scale)
{
	if (!InputSettings)
		InitInputSettings();

	TArray<FInputActionKeyMapping>& Actions = InputSettings->ActionMappings;

	for (FInputActionKeyMapping& Each : Actions)
	{
		if (Each.ActionName.ToString() == ActionName)
		{
			return Each.Key.GetFName().ToString();
		}
	}

	TArray<FInputAxisKeyMapping>& Axis = InputSettings->AxisMappings;

	for (FInputAxisKeyMapping& Each : Axis)
	{
		if (Each.AxisName.ToString() == ActionName && Each.Scale == Scale)
		{
			return Each.Key.GetFName().ToString();
		}
	}

	return TEXT("ERROR");
}

void UOrionGameSettingsManager::SaveInput()
{
	if (!InputSettings)
		InitInputSettings();

	const_cast<UInputSettings*>(InputSettings)->SaveKeyMappings();

	//REBUILDS INPUT, creates modified config in Saved/Config/Windows/Input.ini
	for (TObjectIterator<UPlayerInput> It; It; ++It)
	{
		It->ForceRebuildingKeyMaps(true);
	}
}