// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionGameMenu.h"
#include "OrionGameInstance.h"

AOrionGameMenu::AOrionGameMenu(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultPawnClass = nullptr;

	GameStateClass = AOrionGRI::StaticClass();
	PlayerStateClass = AOrionPRI::StaticClass();

	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerControllerObject(TEXT("/Game/Character/Blueprints/BasePlayerController"));
	if (PlayerControllerObject.Object != NULL)
	{
		PlayerControllerClass = (UClass*)PlayerControllerObject.Object->GeneratedClass;
	}
}

void AOrionGameMenu::BeginPlay()
{
	Super::BeginPlay();

	CharacterPlacers.SetNumUninitialized(5);
	for (int32 i = 0; i < 5; i++)
		CharacterPlacers[i] = nullptr;
}

void AOrionGameMenu::AddPlacer(int32 index, AOrionMenuCharacter *Picker)
{
	if (CharacterPlacers.Num() > index )
		CharacterPlacers[index] = Picker;
}

void AOrionGameMenu::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	FString IPName = UGameplayStatics::ParseOption(Options, TEXT("SteamConnectIP"));

	int32 Index = IPName.Find("x8x");

	if (Index != INDEX_NONE)
	{
		LobbyIP = IPName.Mid(0, Index);
		TeamName = IPName.Mid(Index + 3);
	}
	else
		LobbyIP = IPName;

//if a server ever gets here it was a failed space station:( so just force us to go there now
#if IS_SERVER
	UGameplayStatics::OpenLevel(GetWorld(), "OrbitalCommand", true, "Game=Orion.OrionGameLobby");
#endif
}

void AOrionGameMenu::DisplayLoadingScreen()
{
	UOrionGameInstance *GI = Cast<UOrionGameInstance>(GetWorld()->GetGameInstance());

	if (GI)
	{
		GI->bLoading = true;
	}
	/*IOrionGameLoadingScreenModule* LoadingScreenModule = FModuleManager::LoadModulePtr<IOrionGameLoadingScreenModule>("OrionGameLoadingScreen");
	if (LoadingScreenModule != NULL)
	{
		LoadingScreenModule->StartInGameLoadingScreen();
	}*/
}