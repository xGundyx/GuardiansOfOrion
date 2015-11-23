// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "MoviePlayer.h"
#include "OrionGameInstance.h"


UOrionGameInstance::UOrionGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOrionGameInstance::ShowLoadingScreen()
{
	FLoadingScreenAttributes LoadingScreen;

	LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
	LoadingScreen.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget(); // <-- test screen that comes with UE

	GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
	GetMoviePlayer()->PlayMovie();
}

void UOrionGameInstance::StopLoadingScreen()
{
	GetMoviePlayer()->StopMovie();
}

//overridden to 
void UOrionGameInstance::StartGameInstance()
{
	UEngine* const Engine = GetEngine();

	// Create default URL.
	// @note: if we change how we determine the valid start up map update LaunchEngineLoop's GetStartupMap()
	FURL DefaultURL;
	DefaultURL.LoadURLConfig(TEXT("DefaultPlayer"), GGameIni);

	// Enter initial world.
	EBrowseReturnVal::Type BrowseRet = EBrowseReturnVal::Failure;
	FString Error;
	TCHAR Parm[4096] = TEXT("");
	const TCHAR* Tmp = FCommandLine::Get();

	const UGameMapsSettings* GameMapsSettings = GetDefault<UGameMapsSettings>();
	const FString& DefaultMap = GameMapsSettings->GetGameDefaultMap();
	if (!FParse::Token(Tmp, Parm, ARRAY_COUNT(Parm), 0) || Parm[0] == '-')
	{
		FCString::Strcpy(Parm, *(DefaultMap + GameMapsSettings->LocalMapOptions));
	}

	FURL URL(&DefaultURL, Parm, TRAVEL_Partial);
	if (URL.Valid)
	{
		BrowseRet = Engine->Browse(*WorldContext, URL, Error);
	}

	// If waiting for a network connection, go into the starting level.
	if (BrowseRet != EBrowseReturnVal::Success)
	{
		UE_LOG(LogLoad, Error, TEXT("%s"), *FString::Printf(TEXT("Failed to enter %s: %s. Please check the log for errors."), *URL.Map, *Error));

		// the map specified on the command-line couldn't be loaded.  ask the user if we should load the default map instead
		if (FCString::Stricmp(Parm, *DefaultMap) != 0)
		{
			const FText Message = FText::Format(NSLOCTEXT("Engine", "MapNotFound", "The map specified on the commandline '{0}' could not be found. Would you like to load the default map instead?"), FText::FromString(URL.Map));
			if (FCString::Stricmp(*URL.Map, *DefaultMap) != 0
				&& FMessageDialog::Open(EAppMsgType::OkCancel, Message) != EAppReturnType::Ok)
			{
				// user canceled (maybe a typo while attempting to run a commandlet)
				FPlatformMisc::RequestExit(false);
				return;
			}
			else
			{
				BrowseRet = Engine->Browse(*WorldContext, FURL(&DefaultURL, *(DefaultMap + GameMapsSettings->LocalMapOptions), TRAVEL_Partial), Error);
			}
		}
		else
		{
			const FText Message = FText::Format(NSLOCTEXT("Engine", "MapNotFoundNoFallback", "The map specified on the commandline '{0}' could not be found. Exiting."), FText::FromString(URL.Map));
			FMessageDialog::Open(EAppMsgType::Ok, Message);
			FPlatformMisc::RequestExit(false);
			return;
		}
	}

	// Handle failure.
	if (BrowseRet != EBrowseReturnVal::Success)
	{
		UE_LOG(LogLoad, Error, TEXT("%s"), *FString::Printf(TEXT("Failed to enter %s: %s. Please check the log for errors."), *DefaultMap, *Error));
		const FText Message = FText::Format(NSLOCTEXT("Engine", "DefaultMapNotFound", "The default map '{0}' could not be found. Exiting."), FText::FromString(DefaultMap));
		FMessageDialog::Open(EAppMsgType::Ok, Message);
		FPlatformMisc::RequestExit(false);
		return;
	}

}


