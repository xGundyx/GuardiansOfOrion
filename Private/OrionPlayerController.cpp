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

void AOrionPlayerController::CreateNewCharacter(FString UserName, FString Gender, FString BaseColor, FString CharacterClass)
{
#if !IS_SERVER
	UOrionTCPLink::CreateCharacter(UserName, Gender, BaseColor, CharacterClass);
#endif
}

void AOrionPlayerController::DeleteCharacter(FString CharacterID)
{
#if !IS_SERVER
	UOrionTCPLink::DeleteCharacter(CharacterID);
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

//convert data for playfab storage
std::map<std::string, std::string> AOrionPlayerController::GetInventoryData()
{
	std::map<std::string, std::string> InvMap;

	AOrionInventoryManager *InvManager = GetInventoryManager();

	if (InvManager)
	{
		for (int32 i = 0; i < InvManager->Grid->Width * InvManager->Grid->Height; i++)
		{
			InvMap[std::string("InventorySlot") + std::to_string(i)] = InvManager->Grid->Inventory[i] ? TCHAR_TO_UTF8(*InvManager->Grid->Inventory[i]->EncodedValue) : std::string("Empty");
		}

		InvMap[std::string("HelmetSlot")] = InvManager->HelmetSlot->Inventory[0] ? TCHAR_TO_UTF8(*InvManager->HelmetSlot->Inventory[0]->EncodedValue) : std::string("Empty");
		InvMap[std::string("HandsSlot")] = InvManager->HandsSlot->Inventory[0] ? TCHAR_TO_UTF8(*InvManager->HandsSlot->Inventory[0]->EncodedValue) : std::string("Empty");
		InvMap[std::string("BodySlot")] = InvManager->BodySlot->Inventory[0] ? TCHAR_TO_UTF8(*InvManager->BodySlot->Inventory[0]->EncodedValue) : std::string("Empty");
		InvMap[std::string("LegsSlot")] = InvManager->LegsSlot->Inventory[0] ? TCHAR_TO_UTF8(*InvManager->LegsSlot->Inventory[0]->EncodedValue) : std::string("Empty");
		InvMap[std::string("PrimaryWeaponSlot")] = InvManager->WeaponSlot1->Inventory[0] ? TCHAR_TO_UTF8(*InvManager->WeaponSlot1->Inventory[0]->EncodedValue) : std::string("Empty");
		InvMap[std::string("SecondaryWeaponSlot")] = InvManager->WeaponSlot2->Inventory[0] ? TCHAR_TO_UTF8(*InvManager->WeaponSlot2->Inventory[0]->EncodedValue) : std::string("Empty");
		InvMap[std::string("RingSlot1")] = InvManager->RingSlot1->Inventory[0] ? TCHAR_TO_UTF8(*InvManager->RingSlot1->Inventory[0]->EncodedValue) : std::string("Empty");
		InvMap[std::string("RingSlot2")] = InvManager->RingSlot2->Inventory[0] ? TCHAR_TO_UTF8(*InvManager->RingSlot2->Inventory[0]->EncodedValue) : std::string("Empty");
		InvMap[std::string("ShieldSlot")] = InvManager->ShieldSlot->Inventory[0] ? TCHAR_TO_UTF8(*InvManager->ShieldSlot->Inventory[0]->EncodedValue) : std::string("Empty");
		InvMap[std::string("NeckSlot")] = InvManager->NeckSlot->Inventory[0] ? TCHAR_TO_UTF8(*InvManager->NeckSlot->Inventory[0]->EncodedValue) : std::string("Empty");
		InvMap[std::string("BeltSlot")] = InvManager->BeltSlot->Inventory[0] ? TCHAR_TO_UTF8(*InvManager->BeltSlot->Inventory[0]->EncodedValue) : std::string("Empty");
		InvMap[std::string("GadgetSlot")] = InvManager->GadgetSlot->Inventory[0] ? TCHAR_TO_UTF8(*InvManager->GadgetSlot->Inventory[0]->EncodedValue) : std::string("Empty");
	}

	return InvMap;
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

void AOrionPlayerController::ChangeCamera(int32 TeamIndex)
{
	AOrionCharacter *MyPawn = Cast<AOrionCharacter>(GetPawn());

	if (MyPawn)
		MyPawn->CameraIndex = TeamIndex;
}

void AOrionPlayerController::Possess(APawn* aPawn)
{
	Super::Possess(aPawn);

	AOrionCharacter *newPawn = Cast<AOrionCharacter>(aPawn);
	if (newPawn && GetInventoryManager())
	{
		GetInventoryManager()->EquipItems(newPawn, ITEM_ANY);
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

	return Options;
}

TArray<FOptionsData> AOrionPlayerController::GetCreateCharacterOptions()
{
	TArray<FOptionsData> Options;
	FOptionsData NewOption;

	NewOption.Options.Empty();
	NewOption.Title = TEXT("CLASS");
	NewOption.Options.Add("ASSAULT");
	NewOption.Options.Add("SUPPORT");
	NewOption.Options.Add("RECON");
	Options.Add(NewOption);

	NewOption.Options.Empty();
	NewOption.Title = TEXT("GENDER");
	NewOption.Options.Add("MALE");
	NewOption.Options.Add("FEMALE");
	Options.Add(NewOption);

	NewOption.Options.Empty();
	NewOption.Title = TEXT("SUIT COLOR");
	NewOption.Options.Add("AQUA");
	NewOption.Options.Add("NAVY BLUE");
	Options.Add(NewOption);

	return Options;
}
TArray<FOptionsData> AOrionPlayerController::GetVideoOptions(bool Basic)
{
	TArray<FOptionsData> Options;
	FOptionsData NewOption;

	if (Basic)
	{
		NewOption.Options.Empty();
		NewOption.Title = TEXT("RESOLUTION");
		NewOption.Options.Add("1920x1080");
		NewOption.Options.Add("1280x1024");
		Options.Add(NewOption);

		NewOption.Options.Empty();
		NewOption.Title = TEXT("ASPECT RATIO");
		NewOption.Options.Add("16:10");
		NewOption.Options.Add("16:9");
		NewOption.Options.Add("4:3");
		Options.Add(NewOption);

		NewOption.Options.Empty();
		NewOption.Title = TEXT("WINDOW MODE");
		NewOption.Options.Add("FULLSCREEN");
		NewOption.Options.Add("BORDERLESS WINDOW");
		NewOption.Options.Add("WINDOWED");
		Options.Add(NewOption);
	}
	else
	{
		NewOption.Options.Empty();
		NewOption.Options.Add("ENABLED");
		NewOption.Options.Add("DISABLED");

		NewOption.Title = TEXT("VSYNC");
		Options.Add(NewOption);

		NewOption.Title = TEXT("BLOOM");
		Options.Add(NewOption);

		NewOption.Title = TEXT("DEPTH OF FIELD");
		Options.Add(NewOption);

		NewOption.Title = TEXT("MOTION BLUR");
		Options.Add(NewOption);

		NewOption.Title = TEXT("SPEEDTREE LEAVES");
		Options.Add(NewOption);

		NewOption.Title = TEXT("DYNAMIC LIGHTS");
		Options.Add(NewOption);

		NewOption.Title = TEXT("DYNAMIC SHADOWS");
		Options.Add(NewOption);

		NewOption.Options.Empty();
		NewOption.Title = TEXT("ANTI ALIASING");
		NewOption.Options.Add("FXAA");
		NewOption.Options.Add("TEMPORAL");
		NewOption.Options.Add("OFF");
		Options.Add(NewOption);

		NewOption.Options.Empty();
		NewOption.Options.Add("HIGH");
		NewOption.Options.Add("MEDIUM");
		NewOption.Options.Add("LOW");

		NewOption.Title = TEXT("SHADOW QUALITY");
		Options.Add(NewOption);

		NewOption.Title = TEXT("FOLIAGE QUALITY");
		Options.Add(NewOption);

		NewOption.Title = TEXT("WORLD TEXTURES");
		Options.Add(NewOption);

		NewOption.Title = TEXT("CHARACTER TEXTURES");
		Options.Add(NewOption);
	}

	return Options;
}

TArray<FOptionsValueData> AOrionPlayerController::GetSoundOptions()
{
	TArray<FOptionsValueData> Options;
	FOptionsValueData NewOption;

	NewOption.Title = TEXT("MASTER");
	NewOption.Value = 1.0f;
	Options.Add(NewOption);

	NewOption.Title = TEXT("MUSIC");
	NewOption.Value = 1.0f;
	Options.Add(NewOption);

	NewOption.Title = TEXT("WEAPONS");
	NewOption.Value = 1.0f;
	Options.Add(NewOption);

	NewOption.Title = TEXT("CREATURES/DINOS");
	NewOption.Value = 1.0f;
	Options.Add(NewOption);

	NewOption.Title = TEXT("VOICES");
	NewOption.Value = 1.0f;
	Options.Add(NewOption);

	return Options;
}

TArray<FKeyboardOptionsData> AOrionPlayerController::GetKeyboardOptions()
{
	TArray<FKeyboardOptionsData> Options;
	FKeyboardOptionsData NewOption;

	NewOption.Title = TEXT("MOVE FORWARDS");
	NewOption.Key = TEXT("W");
	Options.Add(NewOption);

	NewOption.Title = TEXT("MOVE BACKWARDS");
	NewOption.Key = TEXT("S");
	Options.Add(NewOption);

	NewOption.Title = TEXT("MOVE LEFT");
	NewOption.Key = TEXT("A");
	Options.Add(NewOption);

	NewOption.Title = TEXT("MOVE RIGHT");
	NewOption.Key = TEXT("D");
	Options.Add(NewOption);

	NewOption.Title = TEXT("JUMP");
	NewOption.Key = TEXT("SPACEBAR");
	Options.Add(NewOption);

	NewOption.Title = TEXT("SPRINT");
	NewOption.Key = TEXT("LEFT SHIFT");
	Options.Add(NewOption);

	NewOption.Title = TEXT("CROUCH/ROLL");
	NewOption.Key = TEXT("C");
	Options.Add(NewOption);

	NewOption.Title = TEXT("GENERIC USE");
	NewOption.Key = TEXT("E");
	Options.Add(NewOption);

	NewOption.Title = TEXT("FIRE WEAPON");
	NewOption.Key = TEXT("MOUSE 1");
	Options.Add(NewOption);

	NewOption.Title = TEXT("AIM WEAPON");
	NewOption.Key = TEXT("MOUSE 2");
	Options.Add(NewOption);
	
	NewOption.Title = TEXT("RELOAD");
	NewOption.Key = TEXT("R");
	Options.Add(NewOption);

	NewOption.Title = TEXT("MELEE ATTACK");
	NewOption.Key = TEXT("V");
	Options.Add(NewOption);

	NewOption.Title = TEXT("USE ABILITY");
	NewOption.Key = TEXT("LEFT CTRL");
	Options.Add(NewOption);

	NewOption.Title = TEXT("PRIMARY WEAPON");
	NewOption.Key = TEXT("1");
	Options.Add(NewOption);

	NewOption.Title = TEXT("SECONDARY WEAPON");
	NewOption.Key = TEXT("2");
	Options.Add(NewOption);

	NewOption.Title = TEXT("GADGET");
	NewOption.Key = TEXT("3");
	Options.Add(NewOption);

	NewOption.Title = TEXT("LAST WEAPON");
	NewOption.Key = TEXT("Q");
	Options.Add(NewOption);

	NewOption.Title = TEXT("GRENADE");
	NewOption.Key = TEXT("G");
	Options.Add(NewOption);

	NewOption.Title = TEXT("GLOBAL SAY");
	NewOption.Key = TEXT("Y");
	Options.Add(NewOption);

	NewOption.Title = TEXT("TEAM SAY");
	NewOption.Key = TEXT("U");
	Options.Add(NewOption);

	NewOption.Title = TEXT("INVENTORY");
	NewOption.Key = TEXT("I");
	Options.Add(NewOption);

	return Options;
}

TArray<FControllerOptionsData> AOrionPlayerController::GetControllerOptions()
{
	TArray<FControllerOptionsData> Options;
	FControllerOptionsData NewOption;

	NewOption.Title = TEXT("JUMP");
	NewOption.Button = BUTTON_A;
	Options.Add(NewOption);

	NewOption.Title = TEXT("SPRINT");
	NewOption.Button = BUTTON_LEFTSTICK;
	Options.Add(NewOption);

	NewOption.Title = TEXT("CROUCH/ROLL");
	NewOption.Button = BUTTON_B;
	Options.Add(NewOption);

	NewOption.Title = TEXT("GENERIC USE(HOLD)");
	NewOption.Button = BUTTON_RIGHTSHOULDER;
	Options.Add(NewOption);

	NewOption.Title = TEXT("FIRE WEAPON");
	NewOption.Button = BUTTON_RIGHTTRIGGER;
	Options.Add(NewOption);

	NewOption.Title = TEXT("AIM WEAPON");
	NewOption.Button = BUTTON_LEFTTRIGGER;
	Options.Add(NewOption);

	NewOption.Title = TEXT("RELOAD(TAP)");
	NewOption.Button = BUTTON_RIGHTSHOULDER;
	Options.Add(NewOption);

	NewOption.Title = TEXT("MELEE ATTACK");
	NewOption.Button = BUTTON_RIGHTSTICK;
	Options.Add(NewOption);

	NewOption.Title = TEXT("USE ABILITY");
	NewOption.Button = BUTTON_LEFTSHOULDER;
	Options.Add(NewOption);

	NewOption.Title = TEXT("GADGET");
	NewOption.Button = BUTTON_RIGHT;
	Options.Add(NewOption);

	NewOption.Title = TEXT("LAST WEAPON");
	NewOption.Button = BUTTON_Y;
	Options.Add(NewOption);

	NewOption.Title = TEXT("GRENADE");
	NewOption.Button = BUTTON_X;
	Options.Add(NewOption);

	NewOption.Title = TEXT("INVENTORY");
	NewOption.Button = BUTTON_UP;
	Options.Add(NewOption);

	NewOption.Title = TEXT("OPEN MENU");
	NewOption.Button = BUTTON_START;
	Options.Add(NewOption);

	return Options;
}

FOptionsValueData AOrionPlayerController::GetMouseSensitivity()
{
	FOptionsValueData Data;

	Data.Title = TEXT("MOUSE SENSITIVITY");
	Data.Value = 1.0f;

	return Data;
}

FOptionsValueData AOrionPlayerController::GetMouseReverse()
{
	FOptionsValueData Data;

	Data.Title = TEXT("REVERSE MOUSE");
	Data.Value = 0.0f;

	return Data;
}

FOptionsValueData AOrionPlayerController::GetMouseSmooth()
{
	FOptionsValueData Data;

	Data.Title = TEXT("SMOOTH MOUSE");
	Data.Value = 1.0f;

	return Data;
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