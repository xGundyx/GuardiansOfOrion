// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionGameMenu.h"

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