#pragma once

#include "OrionGameUserSettings.h"
#include "OrionGameSettingsManager.generated.h"

USTRUCT(BlueprintType)
struct FQualitySettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quality Settings")
		int32 AntiAliasingQuality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quality Settings")
		int32 EffectsQuality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quality Settings")
		int32 PostProcessQuality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quality Settings")
		int32 ResolutionQuality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quality Settings")
		int32 ShadowQuality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quality Settings")
		int32 TextureQuality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quality Settings")
		int32 ViewDistanceQuality;

	FQualitySettings()
	{
		//do nothing
	}
};

USTRUCT(BlueprintType)
struct FScreenSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screen Settings")
		int32 ScreenSizeX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screen Settings")
		int32 ScreenSizeY;

	/** Game window fullscreen mode 0 = Fullscreen 1 = Windowed fullscreen 2 = Windowed 3 = WindowedMirror */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screen Settings")
		int32 ScreenMode;

	/** Whether to use VSync or not */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screen Settings")
		bool bUseVSync;

	FScreenSettings()
	{

	}
};

UCLASS()
class ORION_API UOrionGameSettingsManager : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	static void InitSettings();
	static void InitInputSettings();

	/** Gets the current Scalability Quality in the engine */
	UFUNCTION(BlueprintCallable, Category = "Quality Settings")
		static FQualitySettings GetQualityLevels();

	/** Sets the introduced ScalabilityQuality Settings */
	UFUNCTION(BlueprintCallable, Category = "Quality Settings")
		static void SetQualityLevels(FQualitySettings InSettings);

	/** Gets the current Screen settings in the engine */
	UFUNCTION(BlueprintCallable, Category = "Screen Settings")
		static FScreenSettings GetScreenSettings();

	/** Sets the screen settings */
	UFUNCTION(BlueprintCallable, Category = "Screen Settings")
		static void SetScreenSettings(FScreenSettings InSettings);

	/** Applies all non resolution settings */
	UFUNCTION(BlueprintCallable, Category = "Settings")
		static void ApplyNonScreenSettings();

	/** Applies the screen settings */
	UFUNCTION(BlueprintCallable, Category = "Settings")
		static void ApplyScreenSettings();

	/** Applies all the graphic settings */
	UFUNCTION(BlueprintCallable, Category = "Settings")
		static void ApplyGraphicSettings();

	static UOrionGameUserSettings *Settings;
	static UInputSettings *InputSettings;

	/** Rebind a key */
	UFUNCTION(BlueprintCallable, Category = "Settings")
		static bool RebindKey(FString ActionName, FKey NewKey, FName OriginalKey, float NewScale = 1.0f);

	/** Save changes to disk */
	UFUNCTION(BlueprintCallable, Category = "Settings")
		static void SaveInput();

	/** Read in current value */
	UFUNCTION(BlueprintCallable, Category = "Settings")
		static FString GetKeyForAction(FString ActionName, bool bAxis, float Scale);

	/** Create a key from name */
	UFUNCTION(BlueprintCallable, Category = "Settings")
		static FKey GetKeyFromString(FName KeyName) { return FKey(KeyName); }
};