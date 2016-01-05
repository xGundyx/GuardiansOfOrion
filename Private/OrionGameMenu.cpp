// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionGameLoadingScreen.h"
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

	LobbyIP = UGameplayStatics::ParseOption(Options, TEXT("SteamConnectIP"));
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