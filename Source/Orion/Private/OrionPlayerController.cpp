// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Orion.h"
#include "OrionCharacter.h"
#include "OrionPlayerCameraManager.h"
#include "OrionPlayerController.h"
#include "OrionArmor.h"
#include "OrionInventoryArmor.h"
#include "OrionLocalPlayer.h"
#include "ClientConnector.h"

AOrionPlayerController::AOrionPlayerController(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PlayerCameraManagerClass = AOrionPlayerCameraManager::StaticClass();

	//RainPSC = ObjectInitializer.CreateOptionalDefaultSubobject<UParticleSystemComponent>(this, TEXT("Rain"));
}

void AOrionPlayerController::AttemptLogin(FString UserName, FString Password)
{
#if !IS_SERVER
	UOrionTCPLink::Login(UserName, Password);
#endif
}

void AOrionPlayerController::CreateNewAccount(FString UserName, FString Password, FString Email)
{
#if !IS_SERVER
	UOrionTCPLink::CreateAccount(UserName, Password, Email);
#endif
}

void AOrionPlayerController::CreateNewCharacter(FString UserName, FString Gender, FString BaseColor)
{
#if !IS_SERVER
	UOrionTCPLink::CreateCharacter(UserName, Gender, BaseColor);
#endif
}

void AOrionPlayerController::SelectCharacterAtIndex(int32 Index)
{
#if !IS_SERVER
	UOrionTCPLink::SelectCharacter(Index);
#endif
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

void AOrionPlayerController::PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel)
{
	Super::PreClientTravel(PendingURL, TravelType, bIsSeamlessTravel);
}

void AOrionPlayerController::CleanupGameViewport()
{
	Super::CleanupGameViewport();
}

void AOrionPlayerController::SeamlessTravelTo(class APlayerController* NewPC)
{
	Super::SeamlessTravelTo(NewPC);
}

void AOrionPlayerController::SeamlessTravelFrom(class APlayerController* OldPC)
{
	Super::SeamlessTravelFrom(OldPC);
}

void AOrionPlayerController::ClearUMG()
{
	EventCleanupUMG();

	if (Cast<UOrionLocalPlayer>(Player) && Cast<UOrionLocalPlayer>(Player)->InventoryManager)
	{
		Cast<UOrionLocalPlayer>(Player)->InventoryManager->Destroy();
		Cast<UOrionLocalPlayer>(Player)->InventoryManager = NULL;
	}
}

void AOrionPlayerController::GetPlayerViewPoint(FVector& OutCamLoc, FRotator& OutCamRot) const
{
	AOrionCharacter* MyPawn = Cast<AOrionCharacter>(GetPawn());

	if(MyPawn && !MyPawn->IsFirstPerson())
	{
		Super::GetPlayerViewPoint(OutCamLoc, OutCamRot);
		OutCamLoc = MyPawn->CameraLocation;
	}
	else
	{
		Super::GetPlayerViewPoint(OutCamLoc, OutCamRot);
	}
}

void AOrionPlayerController::Destroyed()
{
	//cleanup our menus so we don't get a garbage collection crash
	ClearUMG();

	Super::Destroyed();
}

void AOrionPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

#if !IS_SERVER
	UOrionTCPLink::Init(this);
#endif

	//setup our inventory manager
	CreateInventory();

	//create our quests
	InitQuestManager();
}

void AOrionPlayerController::SetDropPod(AOrionDropPod *Pod)
{
	DropPod = Pod;
}

void AOrionPlayerController::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult)
{
	if (DropPod)
		DropPod->SpawnCameraComponent->GetCameraView(DeltaTime, OutResult);
	else if (MenuCamera)
		MenuCamera->GetCameraView(DeltaTime, OutResult);
	else
		Super::CalcCamera(DeltaTime, OutResult);
}

void AOrionPlayerController::PawnPendingDestroy(APawn* P)
{
	//LastDeathLocation = P->GetActorLocation();
	//FVector CameraLocation = LastDeathLocation + FVector(0, 0, 300.0f);
	//FRotator CameraRotation(-90.0f, 0.0f, 0.0f);
	//FindDeathCameraSpot(CameraLocation, CameraRotation);

	Super::PawnPendingDestroy(P);

	//tell our blueprint to respawn us
	EventRespawn();

	//ClientSetSpectatorCamera(CameraLocation, CameraRotation);
}

void AOrionPlayerController::Possess(APawn* aPawn)
{
	Super::Possess(aPawn);

	AOrionCharacter *newPawn = Cast<AOrionCharacter>(aPawn);
	if (newPawn && GetInventoryManager())
	{
		GetInventoryManager()->EquipItems(newPawn);
	}
}

void AOrionPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (TheSun == NULL)// && IsLocalPlayerController())
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

#if !IS_SERVER
	UOrionTCPLink::Update();
	UClientConnector::Update();

	int32 x, y;
	//check if we need to update our hud scaling
	GetViewportSize(x,y);
	if (x != OldViewportSizeX || y != OldViewportSizeY)
	{
		OldViewportSizeX = x;
		OldViewportSizeY = y;
		EventResizeHUD();
	}
#endif
}

void AOrionPlayerController::TestSettings()
{
	/*FString RenderSection = "/Script/Engine.RendererSettings";

	GConfig->SetBool(
		*RenderSection,
		TEXT("r.DefaultFeature.Bloom"),
		false,
		GEngineIni
		);

	GConfig->SetInt(
		*RenderSection,
		TEXT("r.Streaming.Poolsize"),
		1000,
		GEngineIni);

	GConfig->Flush(true, GEngineIni);*/

	if (GEngine && GEngine->GameUserSettings)
	{
		GEngine->GameUserSettings->ApplySettings(true);
	}
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

bool AOrionPlayerController::ServerAllArmor_Validate(int32 index)
{
	return true;
}

void AOrionPlayerController::ServerAllArmor_Implementation(int32 index)
{
	AllArmor(index);
}

void AOrionPlayerController::AllArmor(int32 index)
{
	if (Role < ROLE_Authority)
	{
		ServerAllArmor(index);
		return;
	}

	AOrionCharacter* myPawn = Cast<AOrionCharacter>(GetPawn());

	if (myPawn)
		myPawn->SetArmor(index);
}

void AOrionPlayerController::ArmorColor(int32 index)
{
	AOrionCharacter* myPawn = Cast<AOrionCharacter>(GetPawn());

	if (myPawn)
	{
		myPawn->EventUpdateArmorColor(index);
	}
}

AOrionInventoryManager *AOrionPlayerController::GetInventoryManager()
{
	UOrionLocalPlayer *myPlayer = Cast<UOrionLocalPlayer>(Player);

	if (!myPlayer)
		return nullptr;

	if (myPlayer->InventoryManager)
		return myPlayer->InventoryManager;

	return nullptr;
}

TArray<FOptionsData> AOrionPlayerController::GetGameplayOptions()
{
	TArray<FOptionsData> Options;
	FOptionsData NewOption;

	NewOption.Options.Empty();
	NewOption.Title = TEXT("GAMEPLAYOPTION 1");
	NewOption.Options.Add("DISABLED");
	NewOption.Options.Add("ENABLED");
	Options.Add(NewOption);

	NewOption.Title = TEXT("GAMEPLAYOPTION 2");
	Options.Add(NewOption);

	NewOption.Title = TEXT("GAMEPLAYOPTION 3");
	Options.Add(NewOption);

	NewOption.Title = TEXT("GAMEPLAYOPTION 4");
	Options.Add(NewOption);

	NewOption.Title = TEXT("GAMEPLAYOPTION 5");
	Options.Add(NewOption);

	NewOption.Title = TEXT("GAMEPLAYOPTION 6");
	Options.Add(NewOption);

	NewOption.Title = TEXT("GAMEPLAYOPTION 7");
	Options.Add(NewOption);

	NewOption.Options.Empty();
	NewOption.Title = TEXT("TEXTURE QUALITY");
	NewOption.Options.Add("LOW");
	NewOption.Options.Add("MEDIUM");
	NewOption.Options.Add("HIGH");
	Options.Add(NewOption);

	NewOption.Options.Empty();
	NewOption.Title = TEXT("CHARACTER QUALITY");
	NewOption.Options.Add("LOW");
	NewOption.Options.Add("MEDIUM");
	NewOption.Options.Add("HIGH");
	Options.Add(NewOption);

	return Options;
}

void AOrionPlayerController::UpdateRotation(float DeltaTime)
{
	// Calculate Delta to be applied on ViewRotation
	FRotator DeltaRot(RotationInput);

	FRotator ViewRotation = GetControlRotation();

	if (PlayerCameraManager)
	{
		PlayerCameraManager->ProcessViewRotation(DeltaTime, ViewRotation, DeltaRot);
	}

	/*if (!PlayerCameraManager || !PlayerCameraManager->bFollowHmdOrientation)
	{
		if (IsLocalPlayerController() && GEngine->HMDDevice.IsValid() && GEngine->HMDDevice->IsHeadTrackingAllowed())
		{
			GEngine->HMDDevice->ApplyHmdRotation(this, ViewRotation);
		}
	}*/

	//if (IsLocalPlayerController())
		SetControlRotation(ViewRotation);

	APawn* const P = GetPawnOrSpectator();
	if (P)
	{
		P->FaceRotation(ViewRotation, DeltaTime);
	}
}

void AOrionPlayerController::CreateInventory()
{
	//make sure our current inventory is null
	if (GetInventoryManager())
		return;

	UOrionLocalPlayer *myPlayer = Cast<UOrionLocalPlayer>(Player);

	if (!myPlayer)
	{
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, this, &AOrionPlayerController::CreateInventory, 0.01, false);
		return;
	}

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.bNoCollisionFail = true;
	myPlayer->InventoryManager = GetWorld()->SpawnActor<AOrionInventoryManager>(AOrionInventoryManager::StaticClass(), SpawnInfo);

	if (!myPlayer->InventoryManager)
		return;

	myPlayer->InventoryManager->Init();
	myPlayer->InventoryManager->OwnerController = this;

	//give us some default inventory
	GetDefaultInventory();
}

void AOrionPlayerController::GetDefaultInventory()
{
	//just hack it for now!
	if (GetInventoryManager())
	{
		EventGiveDefaultInventory();
	}
}

void AOrionPlayerController::InitStatsAndAchievements()
{
	Stats = ConstructObject<UOrionStats>(UOrionStats::StaticClass());
	Achievements = ConstructObject<UOrionAchievements>(UOrionAchievements::StaticClass());
}

void AOrionPlayerController::ReadStats()
{
#if IS_SERVER
	//Stats->GetPlayerStats(this);
#endif
}

void AOrionPlayerController::SaveStats()
{
	//can only be called from the server!
#if IS_SERVER
	Stats->FlushPlayerStats(this);
#endif
}

void AOrionPlayerController::IncreaseStatValue(FStatID id, int32 iAmount, int32 fAmount)
{
	if (!Stats || Role != ROLE_Authority)
		return;

	int32 i = int32(id) - 1;

	if (Stats->aStats[i].StatType == STATTYPE_INT)
		Stats->aStats[i].StatValueInt += iAmount;
	else if (Stats->aStats[i].StatType == STATTYPE_FLOAT)
		Stats->aStats[i].StatValueFloat += iAmount;

	//for testing purposes only, save here
	SaveStats();
}

void AOrionPlayerController::SetStatValue(FStatID id, int32 iAmount, int32 fAmount)
{
	if (!Stats || Role != ROLE_Authority)
		return;

	int32 i = int32(id) - 1;

	if (Stats->aStats[i].StatType == STATTYPE_INT)
		Stats->aStats[i].StatValueInt = iAmount;
	else if (Stats->aStats[i].StatType == STATTYPE_FLOAT)
		Stats->aStats[i].StatValueFloat = iAmount;
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