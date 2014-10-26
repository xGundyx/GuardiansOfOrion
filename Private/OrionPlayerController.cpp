// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Orion.h"
#include "OrionCharacter.h"
#include "OrionPlayerCameraManager.h"
#include "OrionPlayerController.h"
#include "OrionArmor.h"
#include "OrionInventoryArmor.h"
#include "OrionTCPLink.h"

AOrionPlayerController::AOrionPlayerController(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{
	PlayerCameraManagerClass = AOrionPlayerCameraManager::StaticClass();
}

void AOrionPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("OpenInventory", IE_Pressed, this, &AOrionPlayerController::OpenInventory);
}

//tell the blueprint to open the inventory
void AOrionPlayerController::OpenInventory()
{
	EventOpenInventoryScreen();
}

void AOrionPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UOrionTCPLink::Init(this);

	//setup our inventory manager
	CreateInventory();

	//create our quests
	InitQuestManager();
}

void AOrionPlayerController::Possess(APawn* aPawn)
{
	Super::Possess(aPawn);

	if (InventoryManager)
	{
		InventoryManager->EquipItems(Cast<AOrionCharacter>(aPawn));
	}
}

void AOrionPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (TheSun == NULL && IsLocalPlayerController())
	{
		for (FActorIterator It(GetWorld()); It; ++It)
		{
			TheSun = Cast<AOrionWeather>(*It);
			if (TheSun != NULL)
			{
				TheSun->PlayerOwner = this;
				TheSun->ChooseWeather();
				break;
			}
		}
	}

	if (TheSun)
		TheSun->UpdateWeather(DeltaTime);

	////UOrionTCPLink::Update();
}

void AOrionPlayerController::PerfectDay()
{
	TheSun->PerfectDay();
}

void AOrionPlayerController::HeavyRain()
{
	TheSun->HeavyRain();
}

void AOrionPlayerController::ClearNight()
{
	TheSun->ClearNight();
}

void AOrionPlayerController::AllArmor(int32 index)
{
	AOrionCharacter* myPawn = Cast<AOrionCharacter>(GetPawn());

	if (myPawn)
	{
		myPawn->EventUpdateHelmet(index);
		myPawn->EventUpdateBody(index);
		myPawn->EventUpdateArms(index);
		myPawn->EventUpdateLegs(index);
		myPawn->EventUpdateFlight1(index);
		myPawn->EventUpdateFlight2(index);
	}
}

void AOrionPlayerController::ArmorColor(int32 index)
{
	AOrionCharacter* myPawn = Cast<AOrionCharacter>(GetPawn());

	if (myPawn)
	{
		myPawn->EventUpdateArmorColor(index);
	}
}

void AOrionPlayerController::CreateInventory()
{
	//make sure our current inventory is null
	if (InventoryManager)
		return;

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.bNoCollisionFail = true;
	InventoryManager = GetWorld()->SpawnActor<AOrionInventoryManager>(AOrionInventoryManager::StaticClass(), SpawnInfo);

	if (!InventoryManager)
		return;

	InventoryManager->Init();
	InventoryManager->OwnerController = this;

	//give us some default inventory
	GetDefaultInventory();
}

void AOrionPlayerController::GetDefaultInventory()
{
	//just hack it for now!
	if (InventoryManager)
	{
		EventGiveDefaultInventory();
	}
}

void AOrionPlayerController::InitQuestManager()
{
	QuestManager = ConstructObject<UOrionQuestManager>(UOrionQuestManager::StaticClass());
}

void AOrionPlayerController::CompleteQuest(AOrionQuest *questToComplete)
{
	if (QuestManager)
	{
		QuestManager->CompleteQuest(questToComplete);
	}
}

TArray<AOrionQuest*> AOrionPlayerController::GetAllQuests()
{
	return QuestManager->GetCurrentQuests();
}

bool AOrionPlayerController::AddNewQuest(AOrionQuest *newQuest)
{
	return QuestManager->AddNewQuest(newQuest);
}