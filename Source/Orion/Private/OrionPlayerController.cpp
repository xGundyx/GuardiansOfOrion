// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

//files editted for steam
//onlinesessionasyncserversteam.cpp

#include "Orion.h"
#include "OrionCharacter.h"
#include "OrionPlayerCameraManager.h"
#include "OrionPlayerController.h"
#include "OrionArmor.h"
#include "OrionInventoryArmor.h"
#include "OrionLocalPlayer.h"
#include "OrionGameUserSettings.h"
#include "OrionGameSettingsManager.h"
#include "OrionSpectatorPawn.h"
#include "OrionGameInstance.h"
#include "AudioDevice.h"
#include "Animation/SkeletalMeshActor.h"
#include <steam/steam_api.h>
#include "OnlineSubsystem.h"
#include "OnlineFriendsInterface.h"
#include "OnlinePresenceInterface.h"
#include "OnlineIdentityInterface.h"
#include "OnlineSubsystemTypes.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemSteam.h"
#include "OrionCinematicCamera.h"
//#include "OnlineSubsystemSteamClasses.h"
#include "OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
//#include "MoviePlayer.h"
//#include "OnlineFriendsInterfaceSteam.h"
#include "OnlineSubsystemSteam.h"
#if !IS_SERVER
#include "PhotonProxy.h"
#include "LoadBalancing.h"
#endif
//#include "ClientConnector.h"
#include "OrionInventoryItem.h"
#include "OrionPRI.h"
#include "OrionGameMode.h"
#include "PhotonProxy.h"
#include "PlayFabRequestProxy.h"

#pragma warning( disable : 4996 )

class AOrionDinoPawn;
class AOrionGameMenu;
class AOrionAchievements;

AOrionPlayerController::AOrionPlayerController(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PlayerCameraManagerClass = AOrionPlayerCameraManager::StaticClass();

	//RainPSC = ObjectInitializer.CreateOptionalDefaultSubobject<UParticleSystemComponent>(this, TEXT("Rain"));

	bHideWeapons = false;
	bAuthenticated = false;

	bForceFeedbackEnabled = true;

	NextSpawnClass = -1;

	LastNotificationTime = -20.0f;

	LastTutorialTime = -20.0f;

	bSpawnHax = true;
}

void AOrionPlayerController::AttemptLogin(FString UserName, FString Password)
{
#if !IS_SERVER
	//UOrionTCPLink::Login(UserName, Password);
#endif
}

void AOrionPlayerController::SendChatMessage(FString Channel, FString Message)
{
#if !IS_SERVER
	//UOrionTCPLink::SendChatMessage(Channel, Message);
#endif
}

void AOrionPlayerController::CreateNewAccount(FString UserName, FString Password, FString Email)
{
#if !IS_SERVER
	//UOrionTCPLink::CreateAccount(UserName, Password, Email);
#endif
}

void AOrionPlayerController::CreateNewCharacter(FString UserName, FString Gender, FString BaseColor, FString CharacterClass)
{
#if !IS_SERVER
	//UOrionTCPLink::CreateCharacter(UserName, Gender, BaseColor, CharacterClass);
#endif
}

void AOrionPlayerController::DeleteCharacter(FString CharacterID)
{
#if !IS_SERVER
	//UOrionTCPLink::DeleteCharacter(CharacterID);
#endif
}

void AOrionPlayerController::SelectCharacterAtIndex(int32 Index)
{
#if !IS_SERVER
	//UOrionTCPLink::SelectCharacter(Index);
#endif
}

void AOrionPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("OpenInventory", IE_Pressed, this, &AOrionPlayerController::OpenInventory);
	InputComponent->BindAction("Gamepad_OpenInventory", IE_Pressed, this, &AOrionPlayerController::OpenInventory);

	InputComponent->BindAction("OpenScores", IE_Pressed, this, &AOrionPlayerController::ShowScores);
	InputComponent->BindAction("OpenScores", IE_Released, this, &AOrionPlayerController::HideScores);

	InputComponent->BindAction("Gamepad_OpenScores", IE_Pressed, this, &AOrionPlayerController::ShowScores);
	InputComponent->BindAction("Gamepad_OpenScores", IE_Released, this, &AOrionPlayerController::HideScores);

	//voice chat
	InputComponent->BindAction("StartVoiceChat", IE_Pressed, this, &APlayerController::StartTalking);
	InputComponent->BindAction("StartVoiceChat", IE_Released, this, &APlayerController::StopTalking);

	//escape
	InputComponent->BindAction("OpenCharacterSelect", IE_Pressed, this, &AOrionPlayerController::ShowCharacterSelect);
	InputComponent->BindAction("Gamepad_OpenCharacterSelect", IE_Pressed, this, &AOrionPlayerController::ShowCharacterSelect);

	//skilltree
	InputComponent->BindAction("OpenSkillTree", IE_Pressed, this, &AOrionPlayerController::OpenSkillTree);
	InputComponent->BindAction("Gamepad_OpenSkillTree", IE_Pressed, this, &AOrionPlayerController::OpenSkillTree);

	//camera mode change
	InputComponent->BindAction("ToggleCamera", IE_Pressed, this, &AOrionPlayerController::ToggleThirdPerson);
	InputComponent->BindAction("Gamepad_ToggleCamera", IE_Pressed, this, &AOrionPlayerController::ToggleThirdPerson);

	//readyup, controller holds reload to trigger
	//InputComponent->BindAction("ReadyUp", IE_Pressed, this, &AOrionPlayerController::ReadyUp);
}

void AOrionPlayerController::ShowCharacterSelect()
{
	//don't show in main menu
	if (Cast<AOrionGameMenu>(GetWorld()->GetAuthGameMode()))
		return;

	EventShowCharacterSelect();
}

void AOrionPlayerController::SetCharacterClass(int32 Index, FString CharID)
{
	if (Role < ROLE_Authority)
		ServerSetCharacterClass(Index, CharID);
	else
	{
		//update our character class id and type for next spawn
		NextSpawnClass = Index;
		NextSpawnID = CharID;

		//AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);
		//if (PRI)
		//	PRI->CharacterID = CharID;
	}
}

bool AOrionPlayerController::ServerSetCharacterClass_Validate(int32 Index, const FString &CharID)
{
	return true;
}

void AOrionPlayerController::ServerSetCharacterClass_Implementation(int32 Index, const FString &CharID)
{
	SetCharacterClass(Index, CharID);
}

void AOrionPlayerController::ShowScores()
{
	ShowScoreScreen(true);
}

void AOrionPlayerController::HideScores()
{
	ShowScoreScreen(false);
}

//tell the blueprint to open the inventory
void AOrionPlayerController::OpenInventory()
{
	EventOpenInventoryScreen();
}

void AOrionPlayerController::PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel)
{
	UOrionGameInstance *GI = Cast<UOrionGameInstance>(GetGameInstance());

	//add our playfab credentials to the url so the server can verify us
	FString newURL;
	if (GI)
		newURL = FString::Printf(TEXT("%s?PlayFabID=%s?PlayFabSession=%s?PlayFabCharacter=%s?PlayFabName=%s?LobbyTicket=%s?CharacterClass=%s"), *PendingURL, *GI->PlayFabID, *GI->SessionTicket, *GI->CharacterID, *GI->PlayFabName, *GI->LobbyTicket, *GI->CharacterClass);

	//UE_LOG(LogPath, Log, TEXT("GundyTravel: %s"), *newURL);

	Super::PreClientTravel(newURL, TravelType, bIsSeamlessTravel);
}

void AOrionPlayerController::StartSoloMap(FString MapName)
{
	UOrionGameInstance *GI = Cast<UOrionGameInstance>(GetGameInstance());

	//add our playfab credentials to the url so the server can verify us
	FString newURL;

	if (GI)
		newURL = FString::Printf(TEXT("open %s?PlayFabID=%s?PlayFabSession=%s?PlayFabCharacter=%s?PlayFabName=%s?LobbyTicket=%s?CharacterClass=%s"), *MapName, *GI->PlayFabID, *GI->SessionTicket, *GI->CharacterID, *GI->PlayFabName, *GI->LobbyTicket, *GI->CharacterClass);

	ConsoleCommand(newURL, true);
}

void AOrionPlayerController::ConnectToIP(FString IP)
{
	UOrionGameInstance *GI = Cast<UOrionGameInstance>(GetGameInstance());

	//add our playfab credentials to the url so the server can verify us
	FString newURL;

	if (GI)
	{
		GI->ServerIP = IP;
		newURL = FString::Printf(TEXT("open %s?PlayFabID=%s?PlayFabSession=%s?PlayFabCharacter=%s?PlayFabName=%s?LobbyTicket=%s?CharacterClass=%s"), *IP, *GI->PlayFabID, *GI->SessionTicket, *GI->CharacterID, *GI->PlayFabName, *GI->LobbyTicket, *GI->CharacterClass);
	}

	//UE_LOG(LogTemp, Log, TEXT("GundyReallyTravel: %s"), *newURL);

	LeaveLobby();

	ConsoleCommand(newURL, true);
}

//tell the server what our photon guid is, so other players can join it
void AOrionPlayerController::HandleGUID(FString GUID)
{
	if (bLobbyLeader)
	{
		ServerSendGUID(GUID);
	}
}

void AOrionPlayerController::ServerSendGUID_Implementation(const FString &ID)
{
	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);

	if (GRI)
	{
		GRI->PhotonGUID = ID;
	}
}

void AOrionPlayerController::SetServerInfo_Implementation(FPhotonServerInfo Info)
{
	ServerInfo = Info;

	UOrionGameInstance *GI = Cast<UOrionGameInstance>(GetWorld()->GetGameInstance());

	if (GI)
		ServerInfo.RoomName = GI->PlayFabName.Append("'S SERVER");

	EventSetServerInfo();

	SetLobbyName(Info.LobbyID);

	//ServerSendGUID(Info.LobbyID);
}

void AOrionPlayerController::CreateInGameLobby_Implementation(FPhotonServerInfo Info)
{
#if !IS_SERVER
	if (Info.LobbyID != TEXT(""))
	{
		ServerInfo = Info;

		UOrionGameInstance *GI = Cast<UOrionGameInstance>(GetWorld()->GetGameInstance());

		if (GI)
			ServerInfo.RoomName = GI->PlayFabName.Append("'S SERVER");

		EventSetServerInfo();

		SetLobbyName(Info.LobbyID);

		//ServerSendGUID(Info.LobbyID);

		//CreateLobbyForReal();
	}
#endif
}

void AOrionPlayerController::TryToCreateLobbyAgain()
{
	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, this, &AOrionPlayerController::CreateLobbyForReal, 0.1f, false);
}

void AOrionPlayerController::CreateLobbyForReal()
{
#if !IS_SERVER
	/*if (!bPhotonReady)
	{
		TryToCreateLobbyAgain();
		return;
	}

	if (UPhotonProxy::GetListener())
	{
		UPhotonProxy::GetListener()->PCOwner = this;

		UOrionGameInstance *GI = Cast<UOrionGameInstance>(GetWorld()->GetGameInstance());

		//create a new room that other players can join
		if (GI)
		{
			FString RoomName = GI->PlayFabName;// Info.RoomName;
			RoomName.Append(TEXT("'s Server"));

			FString ChatRoom = "";// ServerInfo.RoomName;
			//ChatRoom.Append(TEXT("Chat"));

			FString Version = GetBuildVersion();

			bLobbyLeader = true;

			UPhotonProxy::GetListener()->createRoom(TCHAR_TO_UTF8(*RoomName), TCHAR_TO_UTF8(*ServerInfo.MapName), TCHAR_TO_UTF8(*ServerInfo.Difficulty), "Survival", TCHAR_TO_UTF8(*ServerInfo.Privacy), TCHAR_TO_UTF8(*GI->ServerIP), TCHAR_TO_UTF8(*ServerInfo.LobbyID), TCHAR_TO_UTF8(*ChatRoom), TCHAR_TO_UTF8(*Version), "", TCHAR_TO_UTF8(*TOD));
		}
	}*/
#endif
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

	/*if (GetInventoryManager())
	{
		GetInventoryManager()->Destroy();
		AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);
		if (PRI)
			PRI->InventoryManager = NULL;
	}*/
}

void AOrionPlayerController::GetAudioListenerPosition(FVector& OutLocation, FVector& OutFrontDir, FVector& OutRightDir)
{
	if (bThirdPersonCamera && GetPawn())
	{
		FVector ViewLocation = GetPawn()->GetActorLocation() + FVector(0.0f, 0.0f, 45.0f);
		FRotator ViewRotation = GetControlRotation();

		OutLocation = ViewLocation;

		const FRotationTranslationMatrix ViewRotationMatrix(ViewRotation, ViewLocation);

		OutFrontDir = ViewRotationMatrix.GetUnitAxis(EAxis::X);
		OutRightDir = ViewRotationMatrix.GetUnitAxis(EAxis::Y);
	}
	else if (GetPawn())
	{
		FVector ViewLocation = GetPawn()->GetActorLocation() + FVector(0.0f, 0.0f, 45.0f);
		FRotator ViewRotation = GetPawn()->GetActorRotation();

		//const FRotationTranslationMatrix ViewRotationMatrix(ViewRotation, ViewLocation);

		OutLocation = ViewLocation;// -ViewRotation.Vector() * 50.0f;
		//OutFrontDir = ViewRotationMatrix.GetUnitAxis(EAxis::X);
		//OutRightDir = ViewRotationMatrix.GetUnitAxis(EAxis::Y);

		FRotator YawRotation(0, 45.0f, 0);

		// Get forward vector
		OutFrontDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		OutRightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	}
	else
		Super::GetAudioListenerPosition(OutLocation, OutFrontDir, OutRightDir);
}

void AOrionPlayerController::GetPlayerViewPoint(FVector& OutCamLoc, FRotator& OutCamRot) const
{
	AOrionCharacter* MyPawn = Cast<AOrionCharacter>(GetPawn());

	if(MyPawn && !MyPawn->IsFirstPerson())
	{
		Super::GetPlayerViewPoint(OutCamLoc, OutCamRot);
		if (IsLocalPlayerController())
			OutCamLoc = MyPawn->CameraLocation;
	}
	else
	{
		Super::GetPlayerViewPoint(OutCamLoc, OutCamRot);
	}
}

void AOrionPlayerController::ReadyUp(bool bPlayerIsReady)
{
	//only do this if game is in readyup mode
	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GetGameState());
	if (!GRI || !GRI->bReadyingUp)
		return;

	AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);
	if (PRI)
		PRI->bReady = !PRI->bReady;

	if (Role < ROLE_Authority)
	{
		ServerSetReady(PRI->bReady);
		return;
	}

	if (!IsLocalController() && PRI)
		PRI->bReady = bPlayerIsReady;

	AOrionGameMode *Game = Cast<AOrionGameMode>(GetWorld()->GetAuthGameMode());

	if (Game)
		Game->PlayerIsReady(this);
}

void AOrionPlayerController::ServerSetReady_Implementation(bool bPlayerIsReady)
{
	ReadyUp(bPlayerIsReady);
}

void AOrionPlayerController::Destroyed()
{
#if IS_SERVER
	//UOrionTCPLink::SaveCharacter(this);
#else
	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);
	if(Cast<AOrionGameMenu>(GetWorld()->GetAuthGameMode()) || (GRI && GRI->bIsLobby))
		LeaveLobby();
#endif

	//cleanup our menus so we don't get a garbage collection crash
	ClearUMG();

	Super::Destroyed();
}

void AOrionPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

#if !IS_SERVER
	//UOrionTCPLink::Init(this);

	if (IsLocalPlayerController())
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ChatManager = GetWorld()->SpawnActor<AOrionChatManager>(AOrionChatManager::StaticClass(), SpawnInfo);

		if (ChatManager)
			ChatManager->pPlayerOwner = this;
	}
#else
#endif

	//setup our inventory manager
	CreateInventory();

	//create our quests
	InitQuestManager();

	//read in our stats and achievements
	////InitStatsAndAchievements();
}

void AOrionPlayerController::SetDropPod(AOrionDropPod *Pod)
{
	DropPod = Pod;
}

void AOrionPlayerController::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult)
{
	AOrionCharacter *MyPawn = Cast<AOrionCharacter>(GetPawn());
	if (DropPod)
		DropPod->SpawnCameraComponent->GetCameraView(DeltaTime, OutResult);
	else if (MenuCamera)
		MenuCamera->GetCameraView(DeltaTime, OutResult);
	else if (Ragdoll && Ragdoll->IsValidLowLevel())
		Ragdoll->CalcCamera(DeltaTime, OutResult);
	else if (OverviewCamera)
	{
		OverviewCamera->GetCameraView(DeltaTime, OutResult);
		bSpawnHax = false;
	}
	else if (MyPawn && MyPawn->bNotSpawnedYet)
		return;
	else if (bSpawnHax)
	{
		TArray<AActor*> Camera;

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrionCinematicCamera::StaticClass(), Camera);

		if (Camera.Num() > 0)
		{
			OutResult.Location = Camera[0]->GetActorLocation();
			OutResult.Rotation = Camera[0]->GetActorRotation();;
		}
		else
		{
			OutResult.Location = FVector(0.0f, 0.0f, 10000.0f);
			OutResult.Rotation = FRotator(-90.0f, 0.0f, 0.0f);
		}
	}
	else
		Super::CalcCamera(DeltaTime, OutResult);
}

//convert data for playfab storage
std::map<std::string, std::string> AOrionPlayerController::GetInventoryData()
{
	std::map<std::string, std::string> InvMap;

	/*AOrionInventoryManager *InvManager = GetInventoryManager();

	if (InvManager)
	{
		for (int32 i = 0; i < InvManager->Grid->Width * InvManager->Grid->Height; i++)
		{
			InvMap[std::string("InventorySlot") + std::to_string(i)] = InvManager->Grid->Inventory[i].ItemClass ? TCHAR_TO_UTF8(*InvManager->Grid->Inventory[i]->EncodedValue) : std::string("Empty");
		}

		InvMap[std::string("HelmetSlot")] = InvManager->HelmetSlot->Inventory[0] ? TCHAR_TO_UTF8(*InvManager->HelmetSlot->Inventory[0]->EncodedValue) : std::string("Empty");
		InvMap[std::string("HandsSlot")] = InvManager->HandsSlot->Inventory[0] ? TCHAR_TO_UTF8(*InvManager->HandsSlot->Inventory[0]->EncodedValue) : std::string("Empty");
		InvMap[std::string("BodySlot")] = InvManager->BodySlot->Inventory[0] ? TCHAR_TO_UTF8(*InvManager->BodySlot->Inventory[0]->EncodedValue) : std::string("Empty");
		InvMap[std::string("LegsSlot")] = InvManager->LegsSlot->Inventory[0] ? TCHAR_TO_UTF8(*InvManager->LegsSlot->Inventory[0]->EncodedValue) : std::string("Empty");
		InvMap[std::string("PrimaryWeaponSlot")] = InvManager->WeaponSlot1->Inventory[0] ? TCHAR_TO_UTF8(*InvManager->WeaponSlot1->Inventory[0]->EncodedValue) : std::string("Empty");
		InvMap[std::string("SecondaryWeaponSlot")] = InvManager->WeaponSlot2->Inventory[0] ? TCHAR_TO_UTF8(*InvManager->WeaponSlot2->Inventory[0]->EncodedValue) : std::string("Empty");
		InvMap[std::string("BeltSlot")] = InvManager->BeltSlot->Inventory[0] ? TCHAR_TO_UTF8(*InvManager->BeltSlot->Inventory[0]->EncodedValue) : std::string("Empty");
		InvMap[std::string("GadgetSlot")] = InvManager->GadgetSlot->Inventory[0] ? TCHAR_TO_UTF8(*InvManager->GadgetSlot->Inventory[0]->EncodedValue) : std::string("Empty");
		InvMap[std::string("ShaderSlot")] = InvManager->ShaderSlot->Inventory[0] ? TCHAR_TO_UTF8(*InvManager->ShaderSlot->Inventory[0]->EncodedValue) : std::string("Empty");
		InvMap[std::string("DisplaySlot")] = InvManager->DisplaySlot->Inventory[0] ? TCHAR_TO_UTF8(*InvManager->DisplaySlot->Inventory[0]->EncodedValue) : std::string("Empty");
		InvMap[std::string("AbilitySlot")] = InvManager->AbilitySlot->Inventory[0] ? TCHAR_TO_UTF8(*InvManager->AbilitySlot->Inventory[0]->EncodedValue) : std::string("Empty");
	}*/

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
	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);

	if (!GRI || !GRI->IsTopDownGame())
		EventRespawn();

	//ClientSetSpectatorCamera(CameraLocation, CameraRotation);
}
#if WITH_CHEATS
void AOrionPlayerController::ChangeCamera(int32 TeamIndex)
{
	AOrionCharacter *MyPawn = Cast<AOrionCharacter>(GetPawn());

	if (MyPawn)
		MyPawn->CameraIndex = TeamIndex;
}

void AOrionPlayerController::ChangeDifficulty(int32 Index)
{
	ServerChangeDifficulty(Index);
}

void AOrionPlayerController::ServerChangeDifficulty_Implementation(int32 Index)
{
	AOrionGameMode *Game = Cast<AOrionGameMode>(GetWorld()->GetAuthGameMode());

	if (!Game)
		return;

	switch (Index)
	{
	case 0:
		Game->Difficulty = DIFF_EASY;
		break;
	case 1:
		Game->Difficulty = DIFF_MEDIUM;
		break;
	case 2:
		Game->Difficulty = DIFF_HARD;
		break;
	case 3:
		Game->Difficulty = DIFF_INSANE;
		break;
	case 4:
		Game->Difficulty = DIFF_REDIKULOUS;
		break;
	}
}
#endif

void AOrionPlayerController::ClientSetAuthed_Implementation(bool bAuthed)
{
	bAuthenticated = bAuthed;
}

#if WITH_CHEATS
void AOrionPlayerController::ServerSetWeather_Implementation(int32 index)
{
	if (GetWorld())
	{
		AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);
		if (GRI && GRI->Weather)
		{
			switch (index)
			{
			case 0:
				GRI->Weather->PerfectDay();
				break;
			case 1:
				GRI->Weather->HeavyRain();
				break;
			case 2:
				GRI->Weather->ClearNight();
				break;
			}
		}
	}
}
#endif

void AOrionPlayerController::ClientSetLastCharacterID_Implementation(const FString &newID)
{
	UOrionGameInstance *GI = Cast<UOrionGameInstance>(GetWorld()->GetGameInstance());

	if (GI)
	{
		GI->CharacterID = newID;

		for (int32 i = 0; i < CharacterDatas.Num(); i++)
		{
			if (CharacterDatas[i].CharacterID == newID)
			{
				GI->CharacterClass = CharacterDatas[i].pClass;
			}
		}
	}

	EventSetLastCharacterID();

	SetIgnoreMoveInput(false);
	SetIgnoreLookInput(false);
}

//call this re-init inputs
void AOrionPlayerController::ResetInput()
{
	/*SetupInputComponent();

	AOrionCharacter *P = Cast<AOrionCharacter>(GetPawn());

	if (P)
		P->ResetInput();*/

	for (TObjectIterator<UPlayerInput> It; It; ++It)
	{
		It->ForceRebuildingKeyMaps(true);
	}
}

void AOrionPlayerController::Possess(APawn* aPawn)
{
	Ragdoll = nullptr;

	Super::Possess(aPawn);

	SetIgnoreMoveInput(false);
	SetIgnoreLookInput(false);

	if (Role == ROLE_Authority)
	{
		AOrionCharacter *newPawn = Cast<AOrionCharacter>(GetPawn());
		AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);

		//hax fix for now, remove when team games are ready and not broken
		PRI->SetTeamIndex(0);

		if (newPawn && PRI && PRI->InventoryManager)
		{
			if (NextSpawnClass > -1)
			{
				ClassIndex = NextSpawnClass;
				NextSpawnClass = -1;
			}

			if (NextSpawnID.Len() > 0)
			{
				if (PRI)
					PRI->CharacterID = NextSpawnID;
			}

			newPawn->bNotSpawnedYet = false;
			EventBlackFade();

			if (!IsLocalPlayerController())
				ClientSetLastCharacterID(PRI->CharacterID);
			else
			{
				UOrionGameInstance *GI = Cast<UOrionGameInstance>(GetWorld()->GetGameInstance());

				if (GI)
				{
					GI->CharacterID = PRI->CharacterID;

					for (int32 i = 0; i < CharacterDatas.Num(); i++)
					{
						if (CharacterDatas[i].CharacterID == PRI->CharacterID)
						{
							GI->CharacterClass = CharacterDatas[i].pClass;
						}
					}
				}

				EventSetLastCharacterID();
			}

			NextSpawnID = TEXT("");

			////PRI->InventoryManager->EquipItems(newPawn, ITEM_ANY);
			if (ClassIndex < 0)
				ClassIndex = FMath::RandRange(0, 4);
			
			ChangeClass(ClassIndex);

			switch (ClassIndex)
			{
			case 0:
				PRI->ClassType = TEXT("ASSAULT");
				PRI->CharacterClass = TEXT("ASSAULT");
				break;
			case 1:
				PRI->ClassType = TEXT("SUPPORT");
				PRI->CharacterClass = TEXT("SUPPORT");
				break;
			case 2:
				PRI->ClassType = TEXT("RECON");
				PRI->CharacterClass = TEXT("RECON");
				break;
			case 3:
				PRI->ClassType = TEXT("TECH");
				PRI->CharacterClass = TEXT("TECH");
				break;
			case 4:
				PRI->ClassType = TEXT("PYRO");
				PRI->CharacterClass = TEXT("PYRO");
				break;
			case 5:
				PRI->ClassType = TEXT("MARKSMAN");
				PRI->CharacterClass = TEXT("MARKSMAN");
				break;
			default:
				PRI->ClassType = TEXT("NONE");
				PRI->CharacterClass = TEXT("NONE");
				break;
			}

			if (SavedGame)
			{
				SavedGame->CharacterClass = PRI->ClassType;

				if (MySkillTree)
				{
					MySkillTree->Skills = SavedGame->CharacterData[ClassIndex].SkillTree;
					SetSkillTreeValuesForUse();
				}
			}
			else
				EventServerGetSkillTreeInfo();

			PRI->InventoryManager->UpdateEquippedSlots();
			PRI->InventoryManager->EquipItems();
		}
	}
}

void AOrionPlayerController::ClientSetItemLevel_Implementation(int32 Level)
{
	ILevel = Level;
}

int32 AOrionPlayerController::GetClassIndex()
{
	FString ClassType;

	if (Cast<AOrionGameMenu>(GetWorld()->GetAuthGameMode()))
	{
		UOrionGameInstance *GI = Cast<UOrionGameInstance>(GetWorld()->GetGameInstance());
		if (GI)
			ClassType = GI->CharacterClass;
	}
	else
	{
		AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);
		if (PRI)
			ClassType = PRI->ClassType;
	}

	if (ClassType == TEXT("ASSAULT"))
		return 0;
	else if (ClassType == TEXT("SUPPORT"))
		return 1;
	else if (ClassType == TEXT("RECON"))
		return 2;
	else if (ClassType == TEXT("TECH"))
		return 3;
	else if (ClassType == TEXT("PYRO"))
		return 4;
	else if (ClassType == TEXT("MARKSMAN"))
		return 5;

	return 0;
}

void AOrionPlayerController::ToggleHUD()
{
	bToggleHUD = !bToggleHUD;

	EventToggleHUD();
}

bool AOrionPlayerController::InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad)
{
	bool bRet = Super::InputKey(Key, EventType, AmountDepressed, bGamepad);

	if (!bGamepad || EventType == EInputEvent::IE_Pressed)
		EventPressKey(Key, bGamepad);

	return bRet;
}

void AOrionPlayerController::ChangeClass(int32 index)
{
	if (Role != ROLE_Authority)
		return;

	AOrionCharacter *P = Cast<AOrionCharacter>(GetPawn());

	if (!P)
		return;

	P->DestroyInventory();
	P->SpawnClassWeapons(index);

	//change our armor and weapons
	P->SetClassArmor(index);
	/*switch (index)
	{
	case 0: //assault
		P->SetClassArmor(0);
		break;
	case 1: //support
		P->SetClassArmor(1);
		break;
	case 2: //recon
		P->SetClassArmor(2);
		break;
	case 3: //tech
		P->SetClassArmor(3);
		break;
	};*/

	if (P)
	{
		P->InitMaterials();
		if (P->GetWeapon() && index == 2)
		{
			P->GetWeapon()->InitMaterials();
		}
	}

	//let blueprints change the ability
	EventChangeClass(index);
}

void AOrionPlayerController::AddDamageNumber(int32 Damage, FVector Pos, bool bCrit)
{
	if (bToggleHUD)
		return;

	if (GetNetMode() == NM_DedicatedServer)
	{
		ClientAddDamageNumber(Damage, Pos, bCrit);
		return;
	}

	EventAddDamageNumber(Damage, Pos, bCrit);
}

void AOrionPlayerController::ClientAddDamageNumber_Implementation(int32 Damage, FVector Pos, bool bCrit)
{
	if (bToggleHUD)
		return;

	AddDamageNumber(Damage, Pos, bCrit);
}

void AOrionPlayerController::AddXPNumber(int32 XP, FVector Pos)
{
	if (bToggleHUD)
		return;

	if (GetNetMode() == NM_DedicatedServer)
	{
		ClientAddXPNumber(XP, Pos);
		return;
	}

	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GetGameState());

	if (GRI)
	{
		switch (GRI->Difficulty)
		{
		case DIFF_EASY:
		case DIFF_MEDIUM:
			XP *= 1.0f;
			break;
		case DIFF_HARD:
			XP *= 1.5f;
			break;
		case DIFF_INSANE:
			XP *= 2.0f;
			break;
		case DIFF_REDIKULOUS:
			XP *= 3.0f;
			break;
		}
	}

	EventAddXPNumber(XP, Pos);
}

void AOrionPlayerController::ClientAddXPNumber_Implementation(int32 XP, FVector Pos)
{
	if (bToggleHUD)
		return;

	AddXPNumber(XP, Pos);
}

void AOrionPlayerController::ClientAddCoinAmount_Implementation(int32 Amount)
{
	AddCoinAmount(Amount);
}

void AOrionPlayerController::AddCoinAmount(int32 Amount)
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		ClientAddCoinAmount(Amount);
		return;
	}

	EventAddCoinAmount(Amount);
}

void AOrionPlayerController::PlaySlowMotionSound_Implementation(float Length)
{
	EventPlaySlowMotionSound(Length);
}

void AOrionPlayerController::SlowMotion(float Value) 
{ 
	UOrionGameInstance *GI = Cast<UOrionGameInstance>(GetWorld()->GetGameInstance());
	if (GI && GI->PlayFabID == "C93236F8795D15C7" || GI->PlayFabID == "E43BB9D949860565")
		ServerSlowMotion(Value); 
}

void AOrionPlayerController::ClientPlayForceFeedback_Implementation(class UForceFeedbackEffect* ForceFeedbackEffect, bool bLooping, FName Tag)
{
	UOrionGameUserSettings *Settings = nullptr;

	if (GEngine && GEngine->GameUserSettings)
		Settings = Cast<UOrionGameUserSettings>(GEngine->GameUserSettings);

	if (Settings->ControllerRumbleEnabled == false)
		return;

	Super::ClientPlayForceFeedback_Implementation(ForceFeedbackEffect, bLooping, Tag);
}

void AOrionPlayerController::PlayShieldEffect(bool bFull)
{
	if (IsLocalPlayerController())
		EventTakeHit(bFull);
	else
		ClientPlayShieldEffect(bFull);
}

void AOrionPlayerController::ClientPlayShieldEffect_Implementation(bool bFull)
{
	EventTakeHit(bFull);
}

void AOrionPlayerController::PlayHUDHit()
{
	if (IsLocalPlayerController())
		EventPlayHUDHit();
	else
		ClientPlayHUDHit();
}

void AOrionPlayerController::ClientPlayHUDHit_Implementation()
{
	EventPlayHUDHit();
}

//only called by the server
void AOrionPlayerController::ServerTick()
{
	if (Role == ROLE_Authority)
	{
		AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);
		if (PRI)
			PRI->ControlledPawn = Cast<AOrionCharacter>(GetPawn());
	}
}

void AOrionPlayerController::ClientSetDeathSpectate_Implementation(APawn *DeadPawn)
{
	Ragdoll = Cast<AOrionCharacter>(DeadPawn);

	PlayerState->bIsSpectator = true;
	bPlayerIsWaiting = true;
	ChangeState(NAME_Spectating);
}

void AOrionPlayerController::UpdateOrbEffects()
{
	AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);

	if (PRI)
		PRI->UpdateOrbEffects();
}

bool AOrionPlayerController::HasOrbEffect(EOrbType Type)
{
	AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);

	if (PRI)
	{
		for (int32 i = 0; i < PRI->OrbEffects.Num(); i++)
		{
			if (PRI->OrbEffects[i].Type == Type)
				return true;
		}
	}

	return false;
}

void AOrionPlayerController::TestLobby()
{
#if !IS_SERVER
	//if (ServerInfo.LobbyID != TEXT(""))
	//{
		/*if (UPhotonProxy::GetListener())
		{
			UPhotonProxy::GetListener()->PCOwner = this;

			UOrionGameInstance *GI = Cast<UOrionGameInstance>(GetWorld()->GetGameInstance());

			//create a new room that other players can join
			if (GI)
			{
				FString RoomName = ServerInfo.RoomName;
				RoomName.Append(TEXT("'s Server"));

				FString ChatRoom = "";// ServerInfo.RoomName;
				//ChatRoom.Append(TEXT("Chat"));

				FString Version = GetBuildVersion();

				UPhotonProxy::GetListener()->createRoom(TCHAR_TO_UTF8(*RoomName), "GOO-RELIC", "MEDIUM", "Survival", "", TCHAR_TO_UTF8(*GI->ServerIP), TCHAR_TO_UTF8(*GI->LobbyTicket), TCHAR_TO_UTF8(*ChatRoom), TCHAR_TO_UTF8(*Version), "");
			}
		}*/
	//}
#endif
}

//only gets called on the local controller
void AOrionPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	/*if (bMenuOpen || Cast<AOrionGameMenu>(GetWorld()->GetAuthGameMode()))
	{
		FInputModeGameAndUI Data;

		SetInputMode(Data);
	}
	else if (!bThirdPersonCamera)
	{
		//bShowMouseCursor = true;

		FInputModeGameOnly Data;
		//Data.SetLockMouseToViewport(true);

		SetInputMode(Data);
	}
	else
	{
		//bShowMouseCursor = false;

		FInputModeGameOnly Data;
		//Data.SetLockMouseToViewport(true);

		SetInputMode(Data);
	}*/

#if !IS_SERVER
	//UOrionTCPLink::Update();
	//UClientConnector::Update();

	//photon only in menu for now
	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);

	if (Cast<AOrionGameMenu>(GetWorld()->GetAuthGameMode()) || (GRI && GRI->bIsLobby))
		UPhotonProxy::Update(DeltaTime);

	ProcessNotifications();

	int32 x, y;
	//check if we need to update our hud scaling
	GetViewportSize(x,y);
	if (x != OldViewportSizeX || y != OldViewportSizeY)
	{
		OldViewportSizeX = x;
		OldViewportSizeY = y;
		EventResizeHUD();
	}

	//AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);

	//if(PRI)
	//	ServerInfo = PRI->ServerInfo;

	/*AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);

	if (bPhotonReady && IsLocalPlayerController() && bAuthenticated && GRI && GRI->PhotonGUID != TEXT("") && GetWorld()->GetTimeSeconds() - LastLobbyTime >= 1.0f)
	{
		LastLobbyTime = GetWorld()->GetTimeSeconds();

		if (UPhotonProxy::GetListener())
		{
			UPhotonProxy::GetListener()->PCOwner = this;

			//do nothing if we're already in the room
			if (!UPhotonProxy::GetListener()->IsInRoom(TCHAR_TO_UTF8(*GRI->PhotonGUID)))
			{
				UOrionGameInstance *GI = Cast<UOrionGameInstance>(GetWorld()->GetGameInstance());

				//create a new room that other players can join
				if (GI)
				{
					UPhotonProxy::GetListener()->JoinRoom(TCHAR_TO_UTF8(*GRI->PhotonGUID), false);// , true);
				}
			}
		}
	}*/
#else
#endif
}

void AOrionPlayerController::CreateServerRoom()
{
#if !IS_SERVER
	/*if (ServerInfo.LobbyID != TEXT(""))
	{
		if (UPhotonProxy::GetListener())
		{
			UPhotonProxy::GetListener()->PCOwner = this;

			UOrionGameInstance *GI = Cast<UOrionGameInstance>(GetWorld()->GetGameInstance());

			//create a new room that other players can join
			if (GI)
			{
				FString RoomName = ServerInfo.RoomName;
				RoomName.Append(TEXT("'s Server"));

				FString ChatRoom = "";// ServerInfo.RoomName;
				//ChatRoom.Append(TEXT("Chat"));

				FString Version = GetBuildVersion();

				UPhotonProxy::GetListener()->createRoom(TCHAR_TO_UTF8(*RoomName), TCHAR_TO_UTF8(*ServerInfo.MapName), TCHAR_TO_UTF8(*ServerInfo.Difficulty), "Survival", TCHAR_TO_UTF8(*ServerInfo.Privacy), TCHAR_TO_UTF8(*GI->ServerIP), TCHAR_TO_UTF8(*GI->LobbyTicket), TCHAR_TO_UTF8(*ChatRoom), TCHAR_TO_UTF8(*Version), TCHAR_TO_UTF8(*ServerInfo.LobbyID));
			}
		}
	}*/
#endif
}

void AOrionPlayerController::JoinLobbySuccess(int32 PlayerNumber)
{
	LobbyPlayerNumber = PlayerNumber;

	//send our player info the the lobby
	SendPlayerInfoToPhoton();

	EventJoinLobbySuccess();
}

bool AOrionPlayerController::IsLobbyLeader()
{
#if !IS_SERVER
	if (UPhotonProxy::GetListener())
	{
		int32 Leader = UPhotonProxy::GetListener()->GetRoomLeader();

		return Leader == LobbyPlayerNumber && Leader > 0;
	}
#endif

	return false;
}

void AOrionPlayerController::SendPlayerInfoToPhoton()
{
#if !IS_SERVER
	if (UPhotonProxy::GetListener())
	{
		UPhotonProxy::GetListener()->PCOwner = this;

		UOrionGameInstance *GI = Cast<UOrionGameInstance>(GetGameInstance());

		AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);

		if (GI && PRI)
		{
			int32 Level = 1;

			if (GI->CharacterClass == TEXT("ASSAULT"))
				Level = CalculateLevel(PRI->AssaultXP);
			else if (GI->CharacterClass == TEXT("SUPPORT"))
				Level = CalculateLevel(PRI->SupportXP);
			else if (GI->CharacterClass == TEXT("RECON"))
				Level = CalculateLevel(PRI->ReconXP);
			else if (GI->CharacterClass == TEXT("TECH"))
				Level = CalculateLevel(PRI->TechXP);
			else if (GI->CharacterClass == TEXT("PYRO"))
				Level = CalculateLevel(PRI->PyroXP);
			else if (GI->CharacterClass == TEXT("MARKSMAN"))
				Level = CalculateLevel(PRI->MarksmanXP);

			FString strLevel = FString::Printf(TEXT("%i"),Level);

			UPhotonProxy::GetListener()->SetPlayerSettings(LobbyPlayerNumber, TCHAR_TO_UTF8(*GI->PlayFabName), TCHAR_TO_UTF8(*GI->CharacterClass), TCHAR_TO_UTF8(*strLevel));
		}
	}
#endif
}

void AOrionPlayerController::AddLobbyPlayer(int32 pNumber, FString pName, FString pClass, FString pLevel)
{
	FLobbyPlayer Player;

	Player.PlayerNumber = pNumber;
	//need to trim the " from the start and end
	Player.PlayerName = pName.Mid(1, pName.Len() - 2);
	Player.PlayerClass = pClass.Mid(1, pClass.Len() - 2);
	Player.PlayerLevel = pLevel.Mid(1, pLevel.Len() - 2);

	LobbyPlayers.Add(Player);
}

void AOrionPlayerController::DaveyCam()
{
	bDaveyCam = !bDaveyCam;

	if (bDaveyCam)
	{
		SetCinematicMode(false, true, true);
		bShowMouseCursor = false;
	}

	FRotator rot;
	AOrionSpectatorPawn *P = Cast<AOrionSpectatorPawn>(GetPawn());
	if (P)
		PlayerCameraManager->GetCameraViewPoint(P->SpecCameraLocation, rot);

	if (GetWorld()->GetNetMode() == NM_Client)
		ServerSetDaveyCam(bDaveyCam);
}

#if WITH_CHEATS
void AOrionPlayerController::SpawnWave()
{
	if (GetWorld())
	{
		AOrionGameMode *Game = Cast<AOrionGameMode>(GetWorld()->GetAuthGameMode());
		if (Game)
			Game->SpawnWave();
	}
}

//for various testing things
void AOrionPlayerController::TestSettings()
{
	FNotificationHelper Notify;
	Notify.Header = TEXT("YOU LEVELED");
	Notify.Body = TEXT("LEVEL 3");

	Notifications.Add(Notify);

	Notify.Header = TEXT("SOMETHING UNLOCKED");
	Notify.Body = TEXT("IT'S BEEN UNLOCKED");

	Notifications.Add(Notify);

	Notify.Header = TEXT("YOU LEVELED");
	Notify.Body = TEXT("LEVEL 4");

	Notifications.Add(Notify);
}
#endif

void AOrionPlayerController::AddXP(int32 Value)
{
//#if IS_SERVER
	AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);
	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GetGameState());

	if(GRI)
	{
		switch (GRI->Difficulty)
		{
		case DIFF_EASY:
		case DIFF_MEDIUM:
			Value *= 1.0f;
			break;
		case DIFF_HARD:
			Value *= 1.25f;
			break;
		case DIFF_INSANE:
			Value *= 1.5f;
			break;
		case DIFF_REDIKULOUS:
			Value *= 2.0f;
			break;
		}
	}

	AOrionGameMode *Game = Cast<AOrionGameMode>(GetWorld()->GetAuthGameMode());

	if(Game)
		Value *= 1.0f + Game->ItemLevel / 300.0f;

	//make sure stats are valid
	if (!Stats)
		return;

	if (Value > 0)
	{
		int32 OldCharacterXP = 0;
		int32 NewCharacterXP = 0;

		if (ClassIndex == 0)
		{
			OldCharacterXP = Stats->aStats[STAT_ASSAULTEXP].StatValue;
			Stats->AddStatValue(STAT_ASSAULTEXP, Value); 
			PRI->AssaultXP = Stats->aStats[STAT_ASSAULTEXP].StatValue;
			NewCharacterXP = Stats->aStats[STAT_ASSAULTEXP].StatValue;
		}
		else if (ClassIndex == 1)
		{
			OldCharacterXP = Stats->aStats[STAT_SUPPORTEXP].StatValue;
			Stats->AddStatValue(STAT_SUPPORTEXP, Value); 
			PRI->SupportXP = Stats->aStats[STAT_SUPPORTEXP].StatValue;
			NewCharacterXP = Stats->aStats[STAT_SUPPORTEXP].StatValue;
		}
		else if (ClassIndex == 2)
		{
			OldCharacterXP = Stats->aStats[STAT_RECONEXP].StatValue;
			Stats->AddStatValue(STAT_RECONEXP, Value); 
			PRI->ReconXP = Stats->aStats[STAT_RECONEXP].StatValue;
			NewCharacterXP = Stats->aStats[STAT_RECONEXP].StatValue;
		}
		else if (ClassIndex == 3)
		{
			OldCharacterXP = Stats->aStats[STAT_TECHEXP].StatValue;
			Stats->AddStatValue(STAT_TECHEXP, Value);
			PRI->TechXP = Stats->aStats[STAT_TECHEXP].StatValue;
			NewCharacterXP = Stats->aStats[STAT_TECHEXP].StatValue;
		}
		else if (ClassIndex == 4)
		{
			OldCharacterXP = Stats->aStats[STAT_PYROEXP].StatValue;
			Stats->AddStatValue(STAT_PYROEXP, Value);
			PRI->PyroXP = Stats->aStats[STAT_PYROEXP].StatValue;
			NewCharacterXP = Stats->aStats[STAT_PYROEXP].StatValue;
		}
		else if (ClassIndex == 5)
		{
			OldCharacterXP = Stats->aStats[STAT_MARKSMANEXP].StatValue;
			Stats->AddStatValue(STAT_MARKSMANEXP, Value);
			PRI->MarksmanXP = Stats->aStats[STAT_MARKSMANEXP].StatValue;
			NewCharacterXP = Stats->aStats[STAT_MARKSMANEXP].StatValue;
		}

		int32 OldLevel = CalculateLevel(OldCharacterXP);
		int32 NewLevel = CalculateLevel(NewCharacterXP);
		//check for level up
		if (OldLevel < NewLevel)
		{
			DoLevelUp(NewLevel);
		}
	}
//#endif
}

void AOrionPlayerController::DoLevelUp(int32 NewLevel)
{
	//spawn some effects
	TArray<AActor*> Controllers;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrionPlayerController::StaticClass(), Controllers);

	PlayLevelUpEffect(NewLevel);
	ShowLevelUpMessage(NewLevel);

	if (Achievements)
		Achievements->CheckForLevelUnlocks(NewLevel, this);

	/*for (int32 i = 0; i < Controllers.Num(); i++)
	{
		AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controllers[i]);
		if (PC)
		{
			PC->ShowLevelUpMessage(NewLevel); //text message to show others that this player has leveled up
		}
	}*/
}

void AOrionPlayerController::HideWeapons()
{
	bHideWeapons = true;
}

void AOrionPlayerController::ShowWeapons()
{
	bHideWeapons = false;
}

#if WITH_CHEATS
void AOrionPlayerController::PerfectDay()
{
	if (Role < ROLE_Authority)
	{
		ServerSetWeather(0);
		return;
	}

	if (GetWorld())
	{
		AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);
		if (GRI && GRI->Weather)
			GRI->Weather->PerfectDay();
	}
}

void AOrionPlayerController::HeavyRain()
{
	if (Role < ROLE_Authority)
	{
		ServerSetWeather(1);
		return;
	}


	if (GetWorld())
	{
		AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);
		if (GRI && GRI->Weather)
			GRI->Weather->HeavyRain();
	}
}

void AOrionPlayerController::ClearNight()
{
	if (Role < ROLE_Authority)
	{
		ServerSetWeather(2);
		return;
	}

	if (GetWorld())
	{
		AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);
		if (GRI && GRI->Weather)
			GRI->Weather->ClearNight();
	}
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
#endif

AOrionChatManager *AOrionPlayerController::GetChatManager()
{
	return ChatManager;
}

AOrionInventoryManager *AOrionPlayerController::GetInventoryManager()
{
	AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);

	if (PRI && PRI->InventoryManager)
		return PRI->InventoryManager;

	return nullptr;

	/*UOrionLocalPlayer *myPlayer = Cast<UOrionLocalPlayer>(Player);

	if (!myPlayer)
		return nullptr;

	if (myPlayer->InventoryManager)
		return myPlayer->InventoryManager;

	return nullptr;*/
}

//this version is for filling out the player's inventory as a non dedicated server, and also for the character select screen
void AOrionPlayerController::PopulateInventory(TSharedPtr<FJsonObject> Data)
{
	/*AOrionInventoryManager *InvMan = GetInventoryManager();

	if (InvMan)
	{
		CreateAndGiveInventoryItem(Data, InvMan->HelmetSlot, "HelmetSlot", 0);
		CreateAndGiveInventoryItem(Data, InvMan->BodySlot, "BodySlot", 0);
		CreateAndGiveInventoryItem(Data, InvMan->HandsSlot, "HandsSlot", 0);
		CreateAndGiveInventoryItem(Data, InvMan->LegsSlot, "LegsSlot", 0);
		CreateAndGiveInventoryItem(Data, InvMan->WeaponSlot1, "PrimaryWeaponSlot", 0);
		CreateAndGiveInventoryItem(Data, InvMan->WeaponSlot2, "SecondaryWeaponSlot", 0);
		CreateAndGiveInventoryItem(Data, InvMan->BeltSlot, "BeltSlot", 0);
		CreateAndGiveInventoryItem(Data, InvMan->GadgetSlot, "GadgetSlot", 0);
		CreateAndGiveInventoryItem(Data, InvMan->ShaderSlot, "ShaderSlot", 0);
		CreateAndGiveInventoryItem(Data, InvMan->DisplaySlot, "DisplaySlot", 0);
		CreateAndGiveInventoryItem(Data, InvMan->AbilitySlot, "AbilitySlot", 0);

		for (int32 i = 0; i < 75; i++)
		{
			FString Slot = UTF8_TO_TCHAR((std::string("InventorySlot") + std::to_string(i)).c_str());
			CreateAndGiveInventoryItem(Data, InvMan->Grid, Slot, i);
		}

		//equip us fully
		InvMan->EquipItems();
		EventRedrawInventory();
	}*/
}

FVector2D AOrionPlayerController::GetTextSize(FString str, FSlateFontInfo Font)
{
	TSharedRef< FSlateFontMeasure > FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	return FontMeasure->Measure(str, Font);
}

bool AOrionPlayerController::CreateAndGiveInventoryItem(TSharedPtr<FJsonObject> Data, AOrionInventoryGrid *theGrid, FString Slot, int32 Index)
{
	/*AOrionInventoryManager *InvMan = GetInventoryManager();

	if (InvMan)
	{
		TSharedPtr<FJsonObject> TestObject = Data->GetObjectField(TCHAR_TO_UTF8(*Slot));
		if (TestObject.IsValid())
		{
			FString tString = TestObject->GetStringField("Value");

			TSharedPtr<FJsonObject> JsonParsed;
			TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(tString);
			if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
			{
				FString pItemName = JsonParsed->GetStringField("ItemName");
				FString pItemPath = JsonParsed->GetStringField("ItemPath");
				FString pItemRarity = JsonParsed->GetStringField("ItemQuality");
				FString pItemLevel = JsonParsed->GetStringField("ItemLevel");

				FString FullPath;
				FullPath += "Blueprint";
				FullPath += "'";
				FullPath += TCHAR_TO_UTF8(*pItemPath);
				FullPath += "'";

				//FStringAssetReference itemRef = FullPath;
				//UObject *ItemObj = itemRef.ResolveObject();

				UObject *ItemObj = Cast<UObject>(StaticLoadObject(UObject::StaticClass(), NULL, *FullPath));

				if (ItemObj)
				{
					FActorSpawnParameters SpawnInfo;
					SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
					UOrionInventoryItem *Inv = Cast<UOrionInventoryItem>(ItemObj);
					if (Inv)
					{
						AOrionInventory *Inventory = GetWorld()->SpawnActor<AOrionInventory>(Inv->ItemClass, SpawnInfo);
						if (Inventory)
						{
							Inventory->Image = Inv->ItemIcon;
							Inventory->InventoryType = Inv->ItemType;
							Inventory->ItemName = pItemName;
							Inventory->ItemDescription = Inv->ItemDesc;
							Inventory->bStackable = Inv->bStackable;
							Inventory->StackAmount = 1;
							Inventory->RequiredLevel = FCString::Atoi(*pItemLevel);
							Inventory->Rarity = GetRarityFromFString(pItemRarity);
							Inventory->EncodedValue = tString;

							AOrionArmor *Armor = Cast<AOrionArmor>(Inventory);
							if (Armor)
							{
								Armor->Mesh = Inv->ItemMesh;
								Armor->Mesh1P = Inv->ItemMesh1P;
							}

							//give it to them at a specific inventory slot!
							if (InvMan->AddItemToInventory(theGrid, Inventory, Index) >= 0)
								return true;
						}
					}
				}
			}
		}
	}*/

	return false;
}

void AOrionPlayerController::SpawnSkeletalActor(FName Type, int32 Index)
{
#if WITH_EDITOR
	for (int32 i = 0; i < AnimationTests.Num(); i++)
	{
		if (AnimationTests[i].Type == Type)
		{
			if (AnimationTests[i].Animations.Num() <= Index)
				return;

			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnInfo.Owner = this;

			if (TestActor)
			{
				TestActor->Destroy();
				TestActor = nullptr;
			}

			if (TestActor2)
			{
				TestActor2->Destroy();
				TestActor2 = nullptr;
			}

			FVector pos;
			FRotator rot;

			AOrionCharacter *P = Cast<AOrionCharacter>(GetPawn());

			if (P)
			{
				pos = P->GetActorLocation();// +P->GetActorRotation().Vector() * AnimationTests[i].Dist;
				rot = P->GetActorRotation();// (pos - P->GetActorLocation()).Rotation();
			}
			else if(PlayerCameraManager)
			{
				PlayerCameraManager->GetCameraViewPoint(pos, rot);
				//pos = pos + rot.Vector() * AnimationTests[i].Dist;
				rot = ControlRotation;// GetControlRotation();
			}

			//rot.Pitch = 0.0f;
			rot.Roll = 0.0f;

			//try to slap pos to the ground
			FCollisionQueryParams TraceParams("Feet", false, this);
			TraceParams.bTraceAsyncScene = false;
			TraceParams.bReturnPhysicalMaterial = false;

			FHitResult Hit;

			//if (GetWorld()->SweepSingleByChannel(Hit, pos + FVector(0.0f, 0.0f, 100.0f), pos - FVector(0.0f, 0.0f, 1250.0f), FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(10.0f), TraceParams))
			if (GetWorld()->SweepSingleByChannel(Hit, pos, pos + rot.Vector() * AnimationTests[i].Dist, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(10.0f), TraceParams))
			{
				pos = Hit.ImpactPoint;
			}
			else
				pos = pos + rot.Vector() * AnimationTests[i].Dist;

			rot.Pitch = 0.0f;

			TestActor = GetWorld()->SpawnActor<ASkeletalMeshActor>(ASkeletalMeshActor::StaticClass(), pos, rot, SpawnInfo);

			if (TestActor && TestActor->GetSkeletalMeshComponent())
			{
				TestActor->GetSkeletalMeshComponent()->SetSkeletalMesh(AnimationTests[i].Mesh);
				TestActor->GetSkeletalMeshComponent()->PlayAnimation(AnimationTests[i].Animations[Index], true);

				if (AnimationTests[i].ExtraMesh)
				{
					TestActor2 = GetWorld()->SpawnActor<ASkeletalMeshActor>(ASkeletalMeshActor::StaticClass(), FVector(0.0f), FRotator(0.0f), SpawnInfo);

					if (TestActor2 && TestActor2->GetSkeletalMeshComponent())
					{
						TestActor2->GetSkeletalMeshComponent()->SetSkeletalMesh(AnimationTests[i].ExtraMesh);
						TestActor2->GetSkeletalMeshComponent()->AttachTo(TestActor->GetSkeletalMeshComponent(), NAME_None, EAttachLocation::KeepRelativeOffset);
						TestActor2->GetSkeletalMeshComponent()->SetMasterPoseComponent(TestActor->GetSkeletalMeshComponent());
					}
				}
			}

			return;
		}
	}
#endif
}

//this version is for the actual in game inventory, server version
#if IS_SERVER
/*void AOrionPlayerController::PopulateInventory(std::map<std::string, PlayFab::ServerModels::UserDataRecord> Data)
{
}*/
#else
#endif

TArray<FOptionsData> AOrionPlayerController::GetGameplayOptions()
{
	TArray<FOptionsData> Options;
	FOptionsData NewOption;

	UOrionGameUserSettings *Settings = nullptr;

	if (GEngine && GEngine->GameUserSettings)
		Settings = Cast<UOrionGameUserSettings>(GEngine->GameUserSettings);

	if (!Settings)
		return Options;

	NewOption.Options.Empty();
	NewOption.Title = TEXT("TUTORIAL");
	NewOption.Value = Settings->TutorialsEnabled ? "ENABLED" : "DISABLED";
	NewOption.Options.Add("DISABLED");
	NewOption.Options.Add("ENABLED");
	Options.Add(NewOption);

	NewOption.Title = TEXT("GORE");
	NewOption.Value = Settings->GoreEnabled ? "ENABLED" : "DISABLED";
	Options.Add(NewOption);

	//NewOption.Title = TEXT("TOGGLE SPRINT");
	//NewOption.Value = Settings->ToggleSprintEnabled ? "ENABLED" : "DISABLED";
	//Options.Add(NewOption);

	NewOption.Title = TEXT("ACHIEVEMENT NOTIFIES");
	NewOption.Value = Settings->AchievementNotifiesEnabled ? "ENABLED" : "DISABLED";
	Options.Add(NewOption);

	NewOption.Title = TEXT("CONTROLLER RUMBLE");
	NewOption.Value = Settings->ControllerRumbleEnabled ? "ENABLED" : "DISABLED";
	Options.Add(NewOption);

	//NewOption.Title = TEXT("THIRD PERSON CAMERA");
	//NewOption.Value = Settings->ThirdPersonEnabled ? "ENABLED" : "DISABLED";
	//Options.Add(NewOption);

	return Options;
}

TArray<FOptionsData> AOrionPlayerController::GetCreateCharacterOptions()
{
	TArray<FOptionsData> Options;
	/*FOptionsData NewOption;

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
	Options.Add(NewOption);*/

	return Options;
}

TArray<FOptionsData> AOrionPlayerController::GetVideoOptions(bool Basic)
{
	TArray<FOptionsData> Options;
	FOptionsData NewOption;

	UOrionGameUserSettings *Settings = nullptr;

	if (GEngine && GEngine->GameUserSettings)
		Settings = Cast<UOrionGameUserSettings>(GEngine->GameUserSettings);

	if (!Settings)
		return Options;

	if (Basic)
	{
		FIntPoint Res = Settings->GetScreenResolution();

		NewOption.Options.Empty();
		NewOption.Title = TEXT("RESOLUTION");
		NewOption.Options = Settings->GetSupportedResolutions();
		NewOption.Value = FString::Printf(TEXT("%ix%i"), Res.X, Res.Y);
		Options.Add(NewOption);

		float Aspect = float(Res.X) / float(Res.Y);

		NewOption.Options.Empty();
		NewOption.Title = TEXT("ASPECT RATIO");
		NewOption.Options.Add("16:10");
		NewOption.Options.Add("16:9");
		NewOption.Options.Add("4:3");
		NewOption.Options.Add("ALL");
		NewOption.Value = Aspect == 1.6f ? "16:10" : (FMath::Abs(Aspect - 1.333333f) < 0.001f ? "4:3" : (FMath::Abs(Aspect - 1.777778f) < 0.001f ? "16:9" : "ALL"));// < 1.35f ? "4:3" : (Aspect < 1.62f ? "16:10" : "16:9");
		Options.Add(NewOption);

		int32 Window = int32(Settings->GetFullscreenMode());

		NewOption.Options.Empty();
		NewOption.Title = TEXT("WINDOW MODE");
		NewOption.Options.Add("FULLSCREEN");
		NewOption.Options.Add("BORDERLESS WINDOW");
		NewOption.Options.Add("WINDOWED");
		NewOption.Options.Add("WINDOWED MIRROR");
		NewOption.Value = Window == 0 ? "FULLSCREEN" : (Window == 1 ? "BORDERLESS WINDOW" : (Window == 2 ? "WINDOWED" : "WINDOWED MIRROR"));
		Options.Add(NewOption);
	}
	else
	{
		NewOption.Options.Empty();
		NewOption.Options.Add("ENABLED");
		NewOption.Options.Add("DISABLED");

		NewOption.Title = TEXT("VSYNC");
		NewOption.Value = Settings->bUseVSync ? TEXT("ENABLED") : TEXT("DISABLED");
		Options.Add(NewOption);

		/*NewOption.Title = TEXT("BLOOM");
		NewOption.Value = TEXT("ENABLED");
		Options.Add(NewOption);

		NewOption.Title = TEXT("DEPTH OF FIELD");
		NewOption.Value = TEXT("ENABLED");
		Options.Add(NewOption);

		NewOption.Title = TEXT("MOTION BLUR");
		NewOption.Value = TEXT("ENABLED");
		Options.Add(NewOption);

		NewOption.Title = TEXT("SPEEDTREE LEAVES");
		NewOption.Value = TEXT("ENABLED");
		Options.Add(NewOption);

		NewOption.Title = TEXT("DYNAMIC LIGHTS");
		NewOption.Value = TEXT("ENABLED");
		Options.Add(NewOption);

		NewOption.Title = TEXT("DYNAMIC SHADOWS");
		NewOption.Value = TEXT("ENABLED");
		Options.Add(NewOption);

		NewOption.Options.Empty();
		NewOption.Title = TEXT("ANTI ALIASING");
		NewOption.Options.Add("FXAA");
		NewOption.Options.Add("TEMPORAL");
		NewOption.Options.Add("OFF");
		NewOption.Value = TEXT("FXAA");
		Options.Add(NewOption);*/

		NewOption.Options.Empty();
		NewOption.Options.Add("LOW");
		NewOption.Options.Add("MEDIUM");
		NewOption.Options.Add("HIGH");
		NewOption.Options.Add("SUPER HIGH");

		NewOption.Title = TEXT("ANTI ALIASING QUALITY");
		NewOption.Value = NewOption.Options[Settings->ScalabilityQuality.AntiAliasingQuality];
		Options.Add(NewOption);

		NewOption.Title = TEXT("EFFECTS QUALITY");
		NewOption.Value = NewOption.Options[Settings->ScalabilityQuality.EffectsQuality];
		Options.Add(NewOption);

		NewOption.Title = TEXT("POST PROCESSING QUALITY");
		NewOption.Value = NewOption.Options[Settings->ScalabilityQuality.PostProcessQuality];
		Options.Add(NewOption);

		NewOption.Title = TEXT("RESOLUTION QUALITY");
		NewOption.Value = NewOption.Options[Settings->ScalabilityQuality.ResolutionQuality >= 100 ? 3 : (Settings->ScalabilityQuality.ResolutionQuality >= 90 ? 2 : (Settings->ScalabilityQuality.ResolutionQuality >= 75 ? 1 : 0))];
		Options.Add(NewOption);

		NewOption.Title = TEXT("SHADOW QUALITY");
		NewOption.Value = NewOption.Options[Settings->ScalabilityQuality.ShadowQuality];
		Options.Add(NewOption);

		NewOption.Title = TEXT("TEXTURE QUALITY");
		NewOption.Value = NewOption.Options[Settings->ScalabilityQuality.TextureQuality];
		Options.Add(NewOption);

		NewOption.Title = TEXT("VIEW DISTANCE QUALITY");
		NewOption.Value = NewOption.Options[Settings->ScalabilityQuality.ViewDistanceQuality];
		Options.Add(NewOption);

		/*NewOption.Title = TEXT("WORLD TEXTURES");
		NewOption.Value = TEXT("HIGH");
		Options.Add(NewOption);

		NewOption.Title = TEXT("CHARACTER TEXTURES");
		NewOption.Value = TEXT("HIGH");
		Options.Add(NewOption);*/
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

	NewOption.Title = TEXT("WEAPON");
	NewOption.Value = 1.0f;
	Options.Add(NewOption);

	NewOption.Title = TEXT("CREATURE");
	NewOption.Value = 1.0f;
	Options.Add(NewOption);

	NewOption.Title = TEXT("VOICE");
	NewOption.Value = 1.0f;
	Options.Add(NewOption);

	NewOption.Title = TEXT("DIALOGUE");
	NewOption.Value = 1.0f;
	Options.Add(NewOption);

	return Options;
}

void AOrionPlayerController::SaveSoundOptions(FString ClassName, float Volume)
{
	FAudioDevice* Device = GEngine->GetMainAudioDevice();
	if (!Device)
	{
		return;
	}

	for (TMap<USoundClass*, FSoundClassProperties>::TIterator It(Device->SoundClasses); It; ++It)
	{
		USoundClass* SoundClass = It.Key();

		if (SoundClass && SoundClass->GetFullName().Find(ClassName) != INDEX_NONE)
		{
			SoundClass->Properties.Volume = Volume;
			return;
		}
	}
}

TArray<FString> AOrionPlayerController::GetSavedGames()
{
	TArray<FString> Games;

	//load up the master save and get a list of all save games
	//if (!MasterSaveFile)
	MasterSaveFile = Cast<UOrionSaveGame>(UGameplayStatics::LoadGameFromSlot("Master", 0));

	if (MasterSaveFile)
	{
		Games = MasterSaveFile->SavedGames;
	}

	return Games;
}

bool AOrionPlayerController::CreateGameFile(FString Slot)
{
	//see if this file already exists
	if (!UGameplayStatics::DoesSaveGameExist(Slot, 0))
	{
		if (Slot == "Master")
		{
			if (!MasterSaveFile)
			{
				MasterSaveFile = Cast<UOrionSaveGame>(UGameplayStatics::CreateSaveGameObject(UOrionSaveGame::StaticClass()));
				//MasterSaveFile = Cast<UOrionSaveGame>(UGameplayStatics::LoadGameFromSlot(Slot, 0));
			}

			return UGameplayStatics::SaveGameToSlot(MasterSaveFile, Slot, 0);
		}
		else
		{
			if (MasterSaveFile)
			{
				MasterSaveFile->SavedGames.AddUnique(Slot);
				UGameplayStatics::SaveGameToSlot(MasterSaveFile, "Master", 0);
			}

			if (!SavedGame)
			{
				SavedGame = Cast<UOrionSaveGame>(UGameplayStatics::CreateSaveGameObject(UOrionSaveGame::StaticClass()));

				SavedGame->FileName = Slot;
				SavedGame->CharacterClass = "ASSAULT";

				SavedGame->Gold = 0;

				//setup some default values
				if (SavedGame)
				{
					if (GetStats())
						SavedGame->Stats = GetStats()->aStats;

					AOrionInventoryManager *Inv = GetInventoryManager();

					if (Inv)
					{
						TArray<FInventoryItem> Items;

						/*FInventoryItem Item;

						for (int32 i = 0; i < 100 + 6 * 14; i++)
						{
							Items.Add(Item);
						}*/

						SavedGame->InventoryData = Items;
					}
				}

				TArray<FSavedCharacterData> CharacterData;

				for (int32 i = 0; i < 6; i++)
				{
					FSavedCharacterData Data;

					Data.Level = 1;
					Data.ItemLevel = 0;
					Data.SkillTree = MySkillTree->Skills;

					CharacterData.Add(Data);
				}

				SavedGame->CharacterData = CharacterData;
			}

			return UGameplayStatics::SaveGameToSlot(SavedGame, Slot, 0);
		}
	}
	else if (Slot == "Master")
	{
		if (!MasterSaveFile)
		{
			MasterSaveFile = Cast<UOrionSaveGame>(UGameplayStatics::CreateSaveGameObject(UOrionSaveGame::StaticClass()));
			MasterSaveFile = Cast<UOrionSaveGame>(UGameplayStatics::LoadGameFromSlot(Slot, 0));
		}

		return true;
	}

	return false;
}

void AOrionPlayerController::RetryLoad()
{
	LoadGameFromFile(SavedGameFile);
}

void AOrionPlayerController::LoadGameFromFile(FString Slot)
{
	if (Slot == "NONE")
	{
		SavedGame = nullptr;
		return;
	}

	SavedGameFile = Slot;
	//make sure we're fully ready to load

	AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);

	if (!Stats || !GetInventoryManager() || !MySkillTree || !PRI)
	{
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, this, &AOrionPlayerController::RetryLoad, 0.1, false);
		return;
	}

	SavedGame = Cast<UOrionSaveGame>(UGameplayStatics::LoadGameFromSlot(Slot, 0));

	if (SavedGame && !Cast<AOrionGameMenu>(GetWorld()->GetAuthGameMode()))
	{
		//grab our desired class
		if (SavedGame->CharacterClass == "ASSAULT")
		{
			ClassIndex = 0;
			PRI->ClassType = SavedGame->CharacterClass;
			PRI->CharacterClass = SavedGame->CharacterClass;
		}
		else if (SavedGame->CharacterClass == "SUPPORT")
		{
			ClassIndex = 1;
			PRI->ClassType = SavedGame->CharacterClass;
			PRI->CharacterClass = SavedGame->CharacterClass;
		}
		else if (SavedGame->CharacterClass == "RECON")
		{
			ClassIndex = 2;
			PRI->ClassType = SavedGame->CharacterClass;
			PRI->CharacterClass = SavedGame->CharacterClass;
		}
		else if (SavedGame->CharacterClass == "TECH")
		{
			ClassIndex = 3;
			PRI->ClassType = SavedGame->CharacterClass;
			PRI->CharacterClass = SavedGame->CharacterClass;
		}
		else if (SavedGame->CharacterClass == "PYRO")
		{
			ClassIndex = 4;
			PRI->ClassType = SavedGame->CharacterClass;
			PRI->CharacterClass = SavedGame->CharacterClass;
		}
		else if (SavedGame->CharacterClass == "MARKSMAN")
		{
			ClassIndex = 5;
			PRI->ClassType = SavedGame->CharacterClass;
			PRI->CharacterClass = SavedGame->CharacterClass;
		}
		else
		{
			ClassIndex = 0;
			PRI->ClassType = "ASSAULT";
			PRI->CharacterClass = "ASSAULT";
		}

		if (Stats)
		{
			Stats->aStats = SavedGame->Stats;
			Stats->FillInLevels();
			Stats->SetInitialized(true);
		}

		//player inventory
		AOrionInventoryManager *Inv = GetInventoryManager();
		if (Inv && Inv->IsFullyInitialized())
		{
			for (int32 i = 0; i < SavedGame->InventoryData.Num(); i++)
			{
				FInventoryItem Item = SavedGame->InventoryData[i];

				Inv->OwnerController = this;
				Inv->ForceAddInventoryItem(Item, Inv->GetPathToClass(Item));
			}

			Inv->Money = SavedGame->Gold;

			Inv->UpdateEquippedSlots();
			Inv->EquipItems();
		}

		//character datas
		if (MySkillTree)
		{
			MySkillTree->Skills = SavedGame->CharacterData[ClassIndex].SkillTree;
		}
	}
}

void AOrionPlayerController::SaveGameToFile(FString Slot)
{
	if (SavedGame && GetStats())
	{
		//character class
		switch (ClassIndex)
		{
		case 0:
			SavedGame->CharacterClass = TEXT("ASSAULT");
			break;
		case 1:
			SavedGame->CharacterClass = TEXT("SUPPORT");
			break;
		case 2:
			SavedGame->CharacterClass = TEXT("RECON");
			break;
		case 3:
			SavedGame->CharacterClass = TEXT("TECH");
			break;
		case 4:
			SavedGame->CharacterClass = TEXT("PYRO");
			break;
		case 5:
			SavedGame->CharacterClass = TEXT("MARKSMAN");
			break;
		default:
			SavedGame->CharacterClass = TEXT("ASSAULT");
			break;
		}

		//don't override our stats from inside the main menu
		if (!Cast<AOrionGameMenu>(GetWorld()->GetAuthGameMode()))
		{
			SavedGame->Stats = GetStats()->aStats;

			TArray<FInventoryItem> Items;

			//inventory
			AOrionInventoryManager *Inv = GetInventoryManager();
			if (Inv && Inv->IsFullyInitialized())
			{
				for (int32 i = 0; i < Inv->Grid->Inventory.Num(); i++)
				{
					if (Inv->Grid->Inventory[i].ItemClass)
						Items.Add(Inv->Grid->Inventory[i]);
				}

				//add all equipped stuff
				for (int32 i = 0; i < Inv->EquippedSlots.Num(); i++)
				{
					if (Inv->EquippedSlots[i].Item.ItemClass)
						Items.Add(Inv->EquippedSlots[i].Item);
				}

				SavedGame->Gold = Inv->Money;
			}

			SavedGame->InventoryData = Items;

			//character data
			AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);

			if (SavedGame->CharacterData.Num() > ClassIndex && MySkillTree && PRI)
			{
				int32 Level = 1;

				if (PRI)
				{
					switch (ClassIndex)
					{
					case CLASS_ASSAULT:
						Level = CalculateLevel(PRI->AssaultXP);
						break;
					case CLASS_SUPPORT:
						Level = CalculateLevel(PRI->SupportXP);
						break;
					case CLASS_RECON:
						Level = CalculateLevel(PRI->ReconXP);
						break;
					case CLASS_TECH:
						Level = CalculateLevel(PRI->TechXP);
						break;
					case CLASS_PYRO:
						Level = CalculateLevel(PRI->PyroXP);
						break;
					case CLASS_MARKSMAN:
						Level = CalculateLevel(PRI->MarksmanXP);
						break;
					};
				}

				SavedGame->CharacterData[ClassIndex].SkillTree = MySkillTree->Skills;
				SavedGame->CharacterData[ClassIndex].ItemLevel = ILevel;
				SavedGame->CharacterData[ClassIndex].Level = Level;
			}
		}

		UGameplayStatics::SaveGameToSlot(SavedGame, Slot, 0);
	}
}

TArray<FString> AOrionPlayerController::GetDifficultySettings()
{
	TArray<FString> Difficulties;

	if (Achievements && Achievements->Achievements[ACH_REACHLEVELTHREE].bUnlocked)
		Difficulties.Add(TEXT("DIFFICULTY - HARD"));
	if (Achievements && Achievements->Achievements[ACH_REACHLEVELTEN].bUnlocked)
		Difficulties.Add(TEXT("DIFFICULTY - INSANE"));
	if (Achievements && Achievements->Achievements[ACH_REACHLEVELTWENTY].bUnlocked)
		Difficulties.Add(TEXT("DIFFICULTY - REDIKULOUS"));

	Difficulties.Add(TEXT("DIFFICULTY - EASY"));
	Difficulties.Add(TEXT("DIFFICULTY - NORMAL"));

	return Difficulties;
}

TArray<FString> AOrionPlayerController::GetMaps()
{
	TArray<FString> Maps;

	Maps.Add(TEXT("GOO-RELIC"));
	Maps.Add(TEXT("GOO-OUTBACK"));
	Maps.Add(TEXT("GOO-ARID"));
	Maps.Add(TEXT("GOO-SUMMIT"));
	Maps.Add(TEXT("GOO-WHITEOUT"));

	return Maps;
}

//must match the class index
TArray<FString> AOrionPlayerController::GetCharacters()
{
	TArray<FString> Characters;

	Characters.Add(TEXT("ASSAULT"));
	Characters.Add(TEXT("SUPPORT"));
	Characters.Add(TEXT("RECON"));
	Characters.Add(TEXT("TECH"));
	Characters.Add(TEXT("PYRO"));
	Characters.Add(TEXT("MARKSMAN"));

	return Characters;
}

TArray<FString> AOrionPlayerController::GetGameModeSettings()
{
	TArray<FString> GameModes;

	GameModes.Add(TEXT("MODE - SLAUGHTER"));
	GameModes.Add(TEXT("MODE - SURVIVAL"));

	return GameModes;
}

TArray<FString> AOrionPlayerController::GetPrivacySettings()
{
	TArray<FString> PrivacySettings;

	PrivacySettings.Add(TEXT("PRIVACY - PRIVATE"));
	PrivacySettings.Add(TEXT("PRIVACY - FRIENDS"));
	PrivacySettings.Add(TEXT("PRIVACY - PUBLIC"));

	return PrivacySettings;
}

FString AOrionPlayerController::GetReadyButtonKeyboard()
{
	return UOrionGameSettingsManager::GetKeyForAction("ReadyUp", false, 0.0f);
}

FString AOrionPlayerController::GetReviveButtonKeyboard()
{
	return UOrionGameSettingsManager::GetKeyForAction("Reload", false, 0.0f);
}

int32 AOrionPlayerController::GetReviveButtonController()
{
	return ConvertControllerButtonToIndex(UOrionGameSettingsManager::GetKeyForAction("Gamepad_Reload", false, 0.0f));
}

FString AOrionPlayerController::GetMeleeButtonKeyboard()
{
	return UOrionGameSettingsManager::GetKeyForAction("Melee", false, 0.0f);
}

int32 AOrionPlayerController::GetMeleeButtonController()
{
	return ConvertControllerButtonToIndex(UOrionGameSettingsManager::GetKeyForAction("Gamepad_Melee", false, 0.0f));
}

TArray<FOptionsValueData> AOrionPlayerController::GetControllerSensitivities()
{
	TArray<FOptionsValueData> Options;
	FOptionsValueData NewOption;

	FInputAxisProperties RightX = UOrionGameSettingsManager::GetAxisConfig("Gamepad_RightX");
	FInputAxisProperties RightY = UOrionGameSettingsManager::GetAxisConfig("Gamepad_RightY");

	NewOption.Title = TEXT("LOOK X SENSITIVITY");
	NewOption.Value = RightX.Sensitivity;
	Options.Add(NewOption);

	NewOption.Title = TEXT("LOOK Y SENSITIVITY");
	NewOption.Value = RightY.Sensitivity;
	Options.Add(NewOption);

	NewOption.Title = TEXT("INVERT X AXIS");
	NewOption.Value = RightX.bInvert;
	Options.Add(NewOption);

	NewOption.Title = TEXT("INVERT Y AXIS");
	NewOption.Value = RightY.bInvert;
	Options.Add(NewOption);

	return Options;
}

TArray<FOptionsValueData> AOrionPlayerController::GetKeyboardInverted()
{
	TArray<FOptionsValueData> Options;
	FOptionsValueData NewOption;

	FInputAxisProperties RightX = UOrionGameSettingsManager::GetAxisConfig("MouseX");
	FInputAxisProperties RightY = UOrionGameSettingsManager::GetAxisConfig("MouseY");

	NewOption.Title = TEXT("INVERT X AXIS");
	NewOption.Value = RightX.bInvert;
	Options.Add(NewOption);

	NewOption.Title = TEXT("INVERT Y AXIS");
	NewOption.Value = RightY.bInvert;
	Options.Add(NewOption);

	return Options;
}

TArray<FKeyboardOptionsData> AOrionPlayerController::GetKeyboardOptions()
{
	TArray<FKeyboardOptionsData> Options;
	FKeyboardOptionsData NewOption;

	NewOption.Title = TEXT("MOVE FORWARDS");
	NewOption.Key = UOrionGameSettingsManager::GetKeyForAction("MoveForward", true, 1.0f);
	NewOption.Action = TEXT("MoveForward");
	NewOption.Scale = 1.0f;
	Options.Add(NewOption);

	NewOption.Title = TEXT("MOVE BACKWARDS");
	NewOption.Key = UOrionGameSettingsManager::GetKeyForAction("MoveForward", true, -1.0f);
	NewOption.Action = TEXT("MoveForward");
	NewOption.Scale = -1.0f;
	Options.Add(NewOption);

	NewOption.Title = TEXT("MOVE LEFT");
	NewOption.Key = UOrionGameSettingsManager::GetKeyForAction("MoveRight", true, -1.0f);
	NewOption.Action = TEXT("MoveRight");
	NewOption.Scale = -1.0f;
	Options.Add(NewOption);

	NewOption.Title = TEXT("MOVE RIGHT");
	NewOption.Key = UOrionGameSettingsManager::GetKeyForAction("MoveRight", true, 1.0f);
	NewOption.Action = TEXT("MoveRight");
	NewOption.Scale = 1.0f;
	Options.Add(NewOption);

	NewOption.Title = TEXT("ROLL");
	NewOption.Key = UOrionGameSettingsManager::GetKeyForAction("Roll", false, 0.0f);
	NewOption.Action = TEXT("Roll");
	NewOption.Scale = 0.0f;
	Options.Add(NewOption);

	NewOption.Title = TEXT("SPRINT");
	NewOption.Key = UOrionGameSettingsManager::GetKeyForAction("Run", false, 0.0f);
	NewOption.Action = TEXT("Run");
	NewOption.Scale = 0.0f;
	Options.Add(NewOption);

	NewOption.Title = TEXT("BLINK");
	NewOption.Key = UOrionGameSettingsManager::GetKeyForAction("Blink", false, 0.0f);
	NewOption.Action = TEXT("Blink");
	NewOption.Scale = 0.0f;
	Options.Add(NewOption);

	NewOption.Title = TEXT("USE");
	NewOption.Key = UOrionGameSettingsManager::GetKeyForAction("Use", false, 0.0f);
	NewOption.Action = TEXT("Use");
	NewOption.Scale = 0.0f;
	Options.Add(NewOption);

	NewOption.Title = TEXT("FIRE WEAPON");
	NewOption.Key = UOrionGameSettingsManager::GetKeyForAction("Fire", false, 0.0f);
	NewOption.Action = TEXT("Fire");
	NewOption.Scale = 0.0f;
	Options.Add(NewOption);

	NewOption.Title = TEXT("AIM WEAPON");
	NewOption.Key = UOrionGameSettingsManager::GetKeyForAction("Aim", false, 0.0f);
	NewOption.Action = TEXT("Aim");
	NewOption.Scale = 0.0f;
	Options.Add(NewOption);
	
	NewOption.Title = TEXT("RELOAD / REVIVE");
	NewOption.Key = UOrionGameSettingsManager::GetKeyForAction("Reload", false, 0.0f);
	NewOption.Action = TEXT("Reload");
	NewOption.Scale = 0.0f;
	Options.Add(NewOption);

	NewOption.Title = TEXT("MELEE");
	NewOption.Key = UOrionGameSettingsManager::GetKeyForAction("Melee", false, 0.0f);
	NewOption.Action = TEXT("Melee");
	NewOption.Scale = 0.0f;
	Options.Add(NewOption);

	NewOption.Title = TEXT("USE ABILITY");
	NewOption.Key = UOrionGameSettingsManager::GetKeyForAction("ActivateSkill", false, 0.0f);
	NewOption.Action = TEXT("ActivateSkill");
	NewOption.Scale = 0.0f;
	Options.Add(NewOption);

	NewOption.Title = TEXT("PRIMARY WEAPON");
	NewOption.Key = UOrionGameSettingsManager::GetKeyForAction("WeaponSlot1", false, 0.0f);
	NewOption.Action = TEXT("WeaponSlot1");
	NewOption.Scale = 0.0f;
	Options.Add(NewOption);

	NewOption.Title = TEXT("SECONDARY WEAPON");
	NewOption.Key = UOrionGameSettingsManager::GetKeyForAction("WeaponSlot2", false, 0.0f);
	NewOption.Action = TEXT("WeaponSlot2");
	NewOption.Scale = 0.0f;
	Options.Add(NewOption);

	NewOption.Title = TEXT("GADGET");
	NewOption.Key = UOrionGameSettingsManager::GetKeyForAction("WeaponSlot3", false, 0.0f);
	NewOption.Action = TEXT("WeaponSlot3");
	NewOption.Scale = 0.0f;
	Options.Add(NewOption);

	NewOption.Title = TEXT("LAST WEAPON");
	NewOption.Key = UOrionGameSettingsManager::GetKeyForAction("LastWeapon", false, 0.0f);
	NewOption.Action = TEXT("LastWeapon");
	NewOption.Scale = 0.0f;
	Options.Add(NewOption);

	NewOption.Title = TEXT("GRENADE");
	NewOption.Key = UOrionGameSettingsManager::GetKeyForAction("ThrowGrenade", false, 0.0f);
	NewOption.Action = TEXT("ThrowGrenade");
	NewOption.Scale = 0.0f;
	Options.Add(NewOption);

	NewOption.Title = TEXT("GLOBAL SAY");
	NewOption.Key = UOrionGameSettingsManager::GetKeyForAction("Say", false, 0.0f);
	NewOption.Action = TEXT("Say");
	NewOption.Scale = 0.0f;
	Options.Add(NewOption);

	NewOption.Title = TEXT("TEAM SAY");
	NewOption.Key = UOrionGameSettingsManager::GetKeyForAction("TeamSay", false, 0.0f);
	NewOption.Action = TEXT("TeamSay");
	NewOption.Scale = 0.0f;
	Options.Add(NewOption);

	NewOption.Title = TEXT("INVENTORY");
	NewOption.Key = UOrionGameSettingsManager::GetKeyForAction("OpenInventory", false, 0.0f);
	NewOption.Action = TEXT("OpenInventory");
	NewOption.Scale = 0.0f;
	Options.Add(NewOption);

	NewOption.Title = TEXT("NEXT WEAPON");
	NewOption.Key = UOrionGameSettingsManager::GetKeyForAction("NextWeapon", false, 0.0f);
	NewOption.Action = TEXT("NextWeapon");
	NewOption.Scale = 0.0f;
	Options.Add(NewOption);

	NewOption.Title = TEXT("PREV WEAPON");
	NewOption.Key = UOrionGameSettingsManager::GetKeyForAction("PrevWeapon", false, 0.0f);
	NewOption.Action = TEXT("PrevWeapon");
	NewOption.Scale = 0.0f;
	Options.Add(NewOption);

	/*NewOption.Title = TEXT("VOICE");
	NewOption.Key = UOrionGameSettingsManager::GetKeyForAction("StartVoiceChat", false, 0.0f);
	NewOption.Action = TEXT("StartVoiceChat");
	NewOption.Scale = 0.0f;
	Options.Add(NewOption);*/

	NewOption.Title = TEXT("SHOW SCORES");
	NewOption.Key = UOrionGameSettingsManager::GetKeyForAction("OpenScores", false, 0.0f);
	NewOption.Action = TEXT("OpenScores");
	NewOption.Scale = 0.0f;
	Options.Add(NewOption);

	NewOption.Title = TEXT("SHOW CHARACTER SELECT");
	NewOption.Key = UOrionGameSettingsManager::GetKeyForAction("OpenCharacterSelect", false, 0.0f);
	NewOption.Action = TEXT("OpenCharacterSelect");
	NewOption.Scale = 0.0f;
	Options.Add(NewOption);

	/*NewOption.Title = TEXT("SHOW SKILLTREE");
	NewOption.Key = UOrionGameSettingsManager::GetKeyForAction("OpenSkillTree", false, 0.0f);
	NewOption.Action = TEXT("OpenSkillTree");
	NewOption.Scale = 0.0f;
	Options.Add(NewOption);*/

	NewOption.Title = TEXT("TOGGLE CAMERA VIEW");
	NewOption.Key = UOrionGameSettingsManager::GetKeyForAction("ToggleCamera", false, 0.0f);
	NewOption.Action = TEXT("ToggleCamera");
	NewOption.Scale = 0.0f;
	Options.Add(NewOption);

	/*NewOption.Title = TEXT("READY UP");
	NewOption.Key = UOrionGameSettingsManager::GetKeyForAction("ReadyUp", false, 0.0f);
	NewOption.Action = TEXT("ReadyUp");
	NewOption.Scale = 0.0f;
	Options.Add(NewOption);*/

	return Options;
}

EControllerButton AOrionPlayerController::ConvertControllerButtonToIndex(FString ButtonName)
{
	EControllerButton Index = BUTTON_A;

	if (ButtonName == TEXT("Gamepad_LeftThumbStick"))
		Index = BUTTON_LEFTSTICK;
	else if (ButtonName == TEXT("Gamepad_RightThumbStick"))
		Index = BUTTON_RIGHTSTICK;
	else if (ButtonName == TEXT("Gamepad_Special_Left"))
		Index = BUTTON_HOME;
	else if (ButtonName == TEXT("Gamepad_Special_Right"))
		Index = BUTTON_START;
	else if (ButtonName == TEXT("Gamepad_FaceButton_Bottom"))
		Index = BUTTON_A;
	else if (ButtonName == TEXT("Gamepad_FaceButton_Right"))
		Index = BUTTON_B;
	else if (ButtonName == TEXT("Gamepad_FaceButton_Left"))
		Index = BUTTON_X;
	else if (ButtonName == TEXT("Gamepad_FaceButton_Top"))
		Index = BUTTON_Y;
	else if (ButtonName == TEXT("Gamepad_LeftShoulder"))
		Index = BUTTON_LEFTSHOULDER;
	else if (ButtonName == TEXT("Gamepad_RightShoulder"))
		Index = BUTTON_RIGHTSHOULDER;
	else if (ButtonName == TEXT("Gamepad_LeftTrigger"))
		Index = BUTTON_LEFTTRIGGER;
	else if (ButtonName == TEXT("Gamepad_RightTrigger"))
		Index = BUTTON_RIGHTTRIGGER;
	else if (ButtonName == TEXT("Gamepad_DPad_Up"))
		Index = BUTTON_UP;
	else if (ButtonName == TEXT("Gamepad_DPad_Down"))
		Index = BUTTON_DOWN;
	else if (ButtonName == TEXT("Gamepad_DPad_Left"))
		Index = BUTTON_LEFT;
	else if (ButtonName == TEXT("Gamepad_DPad_Right"))
		Index = BUTTON_RIGHT;

	return Index;
}

TArray<FControllerOptionsData> AOrionPlayerController::GetControllerOptions()
{
	TArray<FControllerOptionsData> Options;
	FControllerOptionsData NewOption;

	NewOption.Title = TEXT("ROLL");
	NewOption.Button = ConvertControllerButtonToIndex(UOrionGameSettingsManager::GetKeyForAction("Gamepad_Roll", false, 0.0f));
	NewOption.Action = TEXT("Gamepad_Roll");
	Options.Add(NewOption);

	NewOption.Title = TEXT("SPRINT");
	NewOption.Button = ConvertControllerButtonToIndex(UOrionGameSettingsManager::GetKeyForAction("Gamepad_Run", false, 0.0f));
	NewOption.Action = TEXT("Gamepad_Run");
	Options.Add(NewOption);

	NewOption.Title = TEXT("BLINK");
	NewOption.Button = ConvertControllerButtonToIndex(UOrionGameSettingsManager::GetKeyForAction("Gamepad_Blink", false, 0.0f));
	NewOption.Action = TEXT("Gamepad_Blink");
	Options.Add(NewOption);

	NewOption.Title = TEXT("RELOAD / REVIVE");
	NewOption.Button = ConvertControllerButtonToIndex(UOrionGameSettingsManager::GetKeyForAction("Gamepad_Reload", false, 0.0f));
	NewOption.Action = TEXT("Gamepad_Reload");
	Options.Add(NewOption);

	NewOption.Title = TEXT("FIRE WEAPON");
	NewOption.Button = ConvertControllerButtonToIndex(UOrionGameSettingsManager::GetKeyForAction("Gamepad_Fire", false, 0.0f));
	NewOption.Action = TEXT("Gamepad_Fire");
	Options.Add(NewOption);

	NewOption.Title = TEXT("AIM WEAPON");
	NewOption.Button = ConvertControllerButtonToIndex(UOrionGameSettingsManager::GetKeyForAction("Gamepad_Aim", false, 0.0f));
	NewOption.Action = TEXT("Gamepad_Aim");
	Options.Add(NewOption);

	NewOption.Title = TEXT("MELEE ATTACK");
	NewOption.Button = ConvertControllerButtonToIndex(UOrionGameSettingsManager::GetKeyForAction("Gamepad_Melee", false, 0.0f));
	NewOption.Action = TEXT("Gamepad_Melee");
	Options.Add(NewOption);

	NewOption.Title = TEXT("USE ABILITY");
	NewOption.Button = ConvertControllerButtonToIndex(UOrionGameSettingsManager::GetKeyForAction("Gamepad_ActivateSkill", false, 0.0f));
	NewOption.Action = TEXT("Gamepad_ActivateSkill");
	Options.Add(NewOption);

	NewOption.Title = TEXT("GADGET");
	NewOption.Button = ConvertControllerButtonToIndex(UOrionGameSettingsManager::GetKeyForAction("Gamepad_WeaponSlot3", false, 0.0f));
	NewOption.Action = TEXT("Gamepad_WeaponSlot3");
	Options.Add(NewOption);

	NewOption.Title = TEXT("LAST WEAPON");
	NewOption.Button = ConvertControllerButtonToIndex(UOrionGameSettingsManager::GetKeyForAction("Gamepad_LastWeapon", false, 0.0f));
	NewOption.Action = TEXT("Gamepad_LastWeapon");
	Options.Add(NewOption);

	NewOption.Title = TEXT("GRENADE");
	NewOption.Button = ConvertControllerButtonToIndex(UOrionGameSettingsManager::GetKeyForAction("Gamepad_ThrowGrenade", false, 0.0f));
	NewOption.Action = TEXT("Gamepad_ThrowGrenade");
	Options.Add(NewOption);

	NewOption.Title = TEXT("INVENTORY");
	NewOption.Button = ConvertControllerButtonToIndex(UOrionGameSettingsManager::GetKeyForAction("Gamepad_OpenInventory", false, 0.0f));
	NewOption.Action = TEXT("Gamepad_OpenInventory");
	Options.Add(NewOption);

	NewOption.Title = TEXT("CHARACTER SELECT");
	NewOption.Button = ConvertControllerButtonToIndex(UOrionGameSettingsManager::GetKeyForAction("Gamepad_OpenCharacterSelect", false, 0.0f));
	NewOption.Action = TEXT("Gamepad_OpenCharacterSelect");
	Options.Add(NewOption);

	/*NewOption.Title = TEXT("SHOW SKILLTREE");
	NewOption.Button = ConvertControllerButtonToIndex(UOrionGameSettingsManager::GetKeyForAction("Gamepad_OpenSkillTree", false, 0.0f));
	NewOption.Action = TEXT("Gamepad_OpenSkillTree");
	Options.Add(NewOption);*/

	NewOption.Title = TEXT("SHOW SCORES");
	NewOption.Button = ConvertControllerButtonToIndex(UOrionGameSettingsManager::GetKeyForAction("Gamepad_OpenScores", false, 0.0f));
	NewOption.Action = TEXT("Gamepad_OpenScores");
	Options.Add(NewOption);

	//NewOption.Title = TEXT("OPEN MENU");
	//NewOption.Button = BUTTON_HOME;
	//Options.Add(NewOption);

	NewOption.Title = TEXT("TOGGLE CAMERA");
	NewOption.Button = ConvertControllerButtonToIndex(UOrionGameSettingsManager::GetKeyForAction("Gamepad_ToggleCamera", false, 0.0f));
	NewOption.Action = TEXT("Gamepad_ToggleCamera");
	Options.Add(NewOption);

	return Options;
}

void AOrionPlayerController::ResetControllerLayout()
{
	UOrionGameSettingsManager::RebindKey("Gamepad_Roll", "Gamepad_FaceButton_Bottom", "Button", 1.0f);
	UOrionGameSettingsManager::RebindKey("Gamepad_Run", "Gamepad_LeftThumbStick", "Button", 1.0f);
	UOrionGameSettingsManager::RebindKey("Gamepad_Reload", "Gamepad_FaceButton_Left", "Button", 1.0f);
	UOrionGameSettingsManager::RebindKey("Gamepad_Fire", "Gamepad_RightTrigger", "Button", 1.0f);
	UOrionGameSettingsManager::RebindKey("Gamepad_Aim", "Gamepad_LeftTrigger", "Button", 1.0f);
	UOrionGameSettingsManager::RebindKey("Gamepad_Melee", "Gamepad_RightThumbStick", "Button", 1.0f);
	UOrionGameSettingsManager::RebindKey("Gamepad_ActivateSkill", "Gamepad_LeftShoulder", "Button", 1.0f);
	UOrionGameSettingsManager::RebindKey("Gamepad_WeaponSlot3", "Gamepad_DPad_Right", "Button", 1.0f);
	UOrionGameSettingsManager::RebindKey("Gamepad_LastWeapon", "Gamepad_FaceButton_Top", "Button", 1.0f);
	UOrionGameSettingsManager::RebindKey("Gamepad_ThrowGrenade", "Gamepad_RightShoulder", "Button", 1.0f);
	UOrionGameSettingsManager::RebindKey("Gamepad_OpenInventory", "Gamepad_DPad_Left", "Button", 1.0f);
	UOrionGameSettingsManager::RebindKey("Gamepad_OpenCharacterSelect", "Gamepad_Special_right", "Button", 1.0f);
	//UOrionGameSettingsManager::RebindKey("Gamepad_OpenSkillTree", "Gamepad_DPad_Right", "Button", 1.0f);
	UOrionGameSettingsManager::RebindKey("Gamepad_OpenScores", "Gamepad_Special_Left", "Button", 1.0f);
	UOrionGameSettingsManager::RebindKey("Gamepad_ToggleCamera", "Gamepad_DPad_Down", "Button", 1.0f);

	UOrionGameSettingsManager::SaveInput();
}

void AOrionPlayerController::ResetKeyboardLayout()
{
}

void AOrionPlayerController::StartFire(uint8 FireModeNum)
{
	if (((IsInState(NAME_Spectating) && bPlayerIsWaiting) || IsInState(NAME_Inactive)) && !IsFrozen())
	{
		AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);
		if (!GRI || !GRI->IsTopDownGame())
			ServerRestartPlayer();
	}
	else if (IsInState(NAME_Spectating))
	{
		ServerViewNextPlayer();
	}
	else if (GetPawn() && !bCinematicMode && !GetWorld()->bPlayersOnly)
	{
		GetPawn()->PawnStartFire(FireModeNum);
	}
}

FOptionsValueData AOrionPlayerController::GetMouseSensitivity()
{
	FOptionsValueData Data;

	FInputAxisProperties RightX = UOrionGameSettingsManager::GetAxisConfig("MouseX");

	Data.Title = TEXT("MOUSE SENSITIVITY");
	Data.Value = RightX.Sensitivity;

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
	Data.Value = UOrionGameSettingsManager::GetMouseSmoothing();

	return Data;
}

int32 AOrionPlayerController::GetMaxLevel()
{
	AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);

	int32 XP = 0;

	if (PRI)
	{
		XP = PRI->AssaultXP;
		XP = FMath::Max(XP, PRI->SupportXP);
		XP = FMath::Max(XP, PRI->ReconXP);
		XP = FMath::Max(XP, PRI->TechXP);
	}

	return CalculateLevel(XP);
}

//let photon handle this, so players can join and talk before a match actually starts, will allow multiple platforms to mingle
void AOrionPlayerController::OpenLobby(FString MapName, FString MapDifficulty, FString Gamemode, FString Privacy, FString TOD, FString ItemLevel, FString Region)
{
#if !IS_SERVER
	if (UPhotonProxy::GetListener())
	{
		UPhotonProxy::GetListener()->PCOwner = this;

		AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);
		UOrionGameInstance *GI = Cast<UOrionGameInstance>(GetGameInstance());

		//create a new room that other players can join
		if (PRI && GI)
		{
			FString RoomName = GI->PlayFabName;
			RoomName.Append(TEXT("'s Game"));

			FString ChatRoom = GI->PlayFabName;

			FString Version = GetBuildVersion();

			UPhotonProxy::GetListener()->createRoom(TCHAR_TO_UTF8(*RoomName), TCHAR_TO_UTF8(*MapName), TCHAR_TO_UTF8(*MapDifficulty), TCHAR_TO_UTF8(*Gamemode), TCHAR_TO_UTF8(*Privacy), "", "", TCHAR_TO_UTF8(*ChatRoom), TCHAR_TO_UTF8(*Version), "", TCHAR_TO_UTF8(*TOD), TCHAR_TO_UTF8(*ItemLevel), TCHAR_TO_UTF8(*Region));
		}
	}
#endif
}

void AOrionPlayerController::CreateLobbySession(FString RoomName)
{
	return;
	if (SteamAPI_Init())
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

		if (OnlineSub)
		{
			FOnlineSessionSettings Settings;

			Settings.NumPublicConnections = 4;
			Settings.NumPrivateConnections = 0;
			Settings.bShouldAdvertise = true;
			Settings.bAllowJoinInProgress = true;
			Settings.bIsLANMatch = false;
			Settings.bIsDedicated = false;
			Settings.bUsesStats = false;
			Settings.bAllowInvites = true;
			Settings.bUsesPresence = true;
			Settings.bAllowJoinViaPresence = true;
			Settings.bAllowJoinViaPresenceFriendsOnly = false;
			Settings.bAntiCheatProtected = false;
			//Settings.BuildUniqueId = 3;

			Settings.Settings.Add(TEXT("ROOMNAME"), RoomName);

			FString ConnectionURL = FString::Printf(TEXT("?SteamConnectIP=\"%s\""), *RoomName);
			Settings.Settings.Add(TEXT("connect"), ConnectionURL);

			IOnlineSessionPtr Sess = OnlineSub->GetSessionInterface();

			if (!Sess.IsValid())
				return;
			
			//FOnCreateSessionCompleteDelegate OnSessionCreateCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &AOrionPlayerController::OnSessionCreated);

			//Sess->AddOnCreateSessionCompleteDelegate_Handle(OnSessionCreateCompleteDelegate);

			Sess->CreateSession(0, "Lobby", Settings);
		}
	}
}

void AOrionPlayerController::OnSessionCreated(FName SessionName, bool bSuccessful)
{
	/*if (SteamAPI_Init())
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

		if (OnlineSub)
		{
			IOnlineSessionPtr Sess = OnlineSub->GetSessionInterface();

			if (Sess.IsValid())
			{
				FOnlineSubsystemSteam *SteamOnline = static_cast(OnlineSub);

				if (SteamOnline)
				{
					FNamedOnlineSession* Session = SteamOnline->SessionInterface->GetNamedSession("Lobby");
					if (Session && Session->SessionInfo.IsValid())
					{
						FOnlineSessionInfoSteam* SessionInfo = (FOnlineSessionInfoSteam*)(Session->SessionInfo.Get());

						//uint8* OutBytes = new uint8(32);
						//int32 Size = StringToBytes(RoomName, OutBytes, 128);

						SessionInfo->SessionId = FUniqueNetIdString(RoomName);// *Identity->CreateUniquePlayerId(OutBytes, Size + 1);
					}
				}
			}
		}
	}*/
}

void AOrionPlayerController::DestroyLobbySession()
{
	return;
	if (SteamAPI_Init())
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

		if (OnlineSub)
		{
			IOnlineSessionPtr Sess = OnlineSub->GetSessionInterface();

			if (!Sess.IsValid())
				return;

			Sess->DestroySession("Lobby");
		}
	}
}

/*void AOrionPlayerController::PlayLoadingScreen(UUserWidget *Widget)
{
	if (!Widget)
		return;

	FLoadingScreenAttributes LoadingScreen;

	LoadingScreen.bAutoCompleteWhenLoadingCompletes = true;
	LoadingScreen.WidgetLoadingScreen = MakeShareable(Widget); // <-- test screen that comes with UE

	GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
}*/

void AOrionPlayerController::JoinLobby(FString ServerName)
{
#if !IS_SERVER
	if (UPhotonProxy::GetListener())
	{
		UPhotonProxy::GetListener()->JoinRoom(TCHAR_TO_UTF8(*ServerName), false);

		//EventShowLobbyScreen();
	}
#endif
}

//leave the lobby
void AOrionPlayerController::LeaveLobby()
{
#if !IS_SERVER
	if (UPhotonProxy::GetListener())
	{
		UPhotonProxy::GetListener()->leave();

		JoinChatRoom(TEXT("Global"));

		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

		/*if (OnlineSub)
		{
			IOnlineSessionPtr Sess = OnlineSub->GetSessionInterface();

			if (!Sess.IsValid())
				return;

			Sess->EndSession("Lobby");
		}*/
	}
#endif
}

void AOrionPlayerController::SetLobbyName(FString lName)
{
	LobbyName = lName;

	if (SteamAPI_Init())
	{
		FString ConnectionURL = FString::Printf(TEXT("?SteamConnectIP=\"%s\""), *LobbyName);
		if (SteamFriends()->SetRichPresence("connect", TCHAR_TO_UTF8(*ConnectionURL)))
		{
			SteamFriends()->SetRichPresence("ROOMNAME", TCHAR_TO_UTF8(*LobbyName));
			//UE_LOG(LogTemp, Log, TEXT("Rich Presence Info %s"), *lName);

			if (lName == "")
			{
				SteamFriends()->ClearRichPresence();
			}
		}
	}
}

void AOrionPlayerController::InviteFriendToLobby(FString FriendSteamID)
{
	if (SteamAPI_Init())
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

		if (OnlineSub)
		{
			IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();

			if (Session.IsValid())
			{
				IOnlineIdentityPtr User = OnlineSub->GetIdentityInterface();

				if (User.IsValid())
				{
					IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();

					if (Identity.IsValid())
					{
						uint8* OutBytes = new uint8(32);
						int32 Size = StringToBytes(FriendSteamID, OutBytes, 32);
						//Session->SendSessionInviteToFriend(int32(0), FName("Lobby"), *Identity->CreateUniquePlayerId(OutBytes, Size + 1));

						bool bSuccess = false;

						//FNamedOnlineSession* Session = Session->GetNamedSession(SessionName);
	
						//FOnlineSessionInfoSteam* SessionInfo = (FOnlineSessionInfoSteam*)(Session->SessionInfo.Get());

						// Create the connection string
						FString ConnectionURL = FString::Printf(TEXT("?SteamConnectIP=\"%s\""), *LobbyName);

						CSteamID steamID(*(uint64*)OutBytes);

						// Outside game accept -> the ConnectionURL gets added on client commandline
						// Inside game accept -> GameRichPresenceJoinRequested_t callback on client
						if (SteamFriends()->InviteUserToGame(steamID, TCHAR_TO_UTF8(*ConnectionURL)))
						{
							UE_LOG(LogTemp, Log, TEXT("Inviting %s to session Lobby with %s"), *FriendSteamID, *ConnectionURL);
						}
						else
						{
							UE_LOG_ONLINE(Warning, TEXT("Error inviting %s to session Lobby"), *FriendSteamID);
						}

						delete OutBytes;
					}
				}
			}
		}
	}
}

void AOrionPlayerController::SetPresenceInfo(FString LobbyID)
{
	if (SteamAPI_Init() && LobbyID.Len() > 0)
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

		if (OnlineSub)
		{
			IOnlinePresencePtr Presence = OnlineSub->GetPresenceInterface();

			if (!Presence.IsValid())
				return;

			FOnlineUserPresenceStatus Info;
			FPresenceProperties Props;

			Props.Add(TEXT("LOBBYID"), LobbyID);

			Info.Properties = Props;
			Info.State = EOnlinePresenceState::Online;
			Info.StatusStr = TEXT("ONLINE");

			IOnlineIdentityPtr User = OnlineSub->GetIdentityInterface();

			if (!User.IsValid())
				return;

			Presence->SetPresence(*User->GetUniquePlayerId(0), Info);
		}
	}
}

void AOrionPlayerController::ClientReceiveChatMessage_Implementation(const FString &msg)
{ 
	EventAddChatMessage(msg); 
}

void AOrionPlayerController::OrionSendChatMessage(const FString &msg)
{
	if (Role < ROLE_Authority)
	{
		ServerSendChatMessage(msg);
		return;
	}

	AOrionGameMode *Game = Cast<AOrionGameMode>(GetWorld()->GetAuthGameMode());

	if (Game)
		Game->AddChatMessage(msg);
}

void AOrionPlayerController::ServerSendChatMessage_Implementation(const FString &msg)
{
	OrionSendChatMessage(msg);
}

void AOrionPlayerController::JoinChatRoom(FString Room)
{
#if !IS_SERVER
	if (Room.Len() > 1)
	{
		//only join chat rooms while in the main menu for lobbies, no chat channels while in game for now
		AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);

		if (Cast<AOrionGameMenu>(GetWorld()->GetAuthGameMode()) || (GRI && GRI->bIsLobby))
			UPhotonProxy::JoinChannel(TCHAR_TO_UTF8(*Room));
	}
#endif
}

void AOrionPlayerController::GrabLobbySettings()
{
#if !IS_SERVER
	if (UPhotonProxy::GetListener())
	{
		UPhotonProxy::GetListener()->GrabRoomSettings();
	}
#endif
}

void AOrionPlayerController::FlushLobbySettings(FString MapName, FString Difficulty, FString Gamemode, FString Privacy, FString IP, FString Ticket, FString Wave, FString Version, FString RoomName, FString TOD, FString ItemLevel, FString Region)
{
#if !IS_SERVER
	if (UPhotonProxy::GetListener())
	{
		UPhotonProxy::GetListener()->SetLobbySettings(TCHAR_TO_UTF8(*MapName), TCHAR_TO_UTF8(*Difficulty), TCHAR_TO_UTF8(*Gamemode), TCHAR_TO_UTF8(*Privacy), TCHAR_TO_UTF8(*IP), TCHAR_TO_UTF8(*Ticket), TCHAR_TO_UTF8(*Wave), TCHAR_TO_UTF8(*Version), TCHAR_TO_UTF8(*RoomName), TCHAR_TO_UTF8(*TOD), TCHAR_TO_UTF8(*ItemLevel), TCHAR_TO_UTF8(*Region));
	}
#endif
}

void AOrionPlayerController::UpdateRotation(float DeltaTime)
{
	if (bMenuOpen)
		return;

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

void AOrionPlayerController::ValidatePlayFabInfo(FString pfID, FString pfSession)
{
	if (Role != ROLE_Authority)
		return;

	EventValidateSession(pfSession);
}

void AOrionPlayerController::ValidateLobbyTicket(FString pfLobbyID, FString pfTicket)
{
	if (Role != ROLE_Authority)
		return;

	EventValidateTicket(pfLobbyID, pfTicket);
}

void AOrionPlayerController::ClientItemAddedToInventory_Implementation(FInventoryItem Item, bool bSuccess)
{
	EventItemAddedToInventory(Item, bSuccess);
}

void AOrionPlayerController::CreateInventory()
{
	//only server does this here
	if (Role != ROLE_Authority)
		return;

	//make sure our current inventory is null
	AOrionInventoryManager *InvMan = GetInventoryManager();
	if (InvMan || !InventoryManagerClass)
		return;

	AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);

	if (!PRI)
	{
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, this, &AOrionPlayerController::CreateInventory, 0.01, false);
		return;
	}

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.Owner = this;
	PRI->InventoryManager = GetWorld()->SpawnActor<AOrionInventoryManager>(InventoryManagerClass, SpawnInfo);

	if (!PRI->InventoryManager)
		return;

	PRI->InventoryManager->OwnerController = this;
	PRI->InventoryManager->Init(this);

	//give us some default inventory
	GetDefaultInventory();
}

bool AOrionPlayerController::ServerSetPlayFabInfo_Validate(const FString &ID, const FString &SessionID, const FString &cID)
{
	return true;
}

void AOrionPlayerController::ServerSetPlayFabInfo_Implementation(const FString &ID, const FString &SessionID, const FString &cID)
{
	AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);
	if (PRI)
	{
		PRI->PlayFabID = ID;
		PRI->SessionTicket = SessionID;
		PRI->CharacterID = cID;

		//UOrionTCPLink::GetCharacterData(this);
	}
}

void AOrionPlayerController::TickGenericTutorials()
{
	SendTutorialMessage(TEXT("ROLLING"), TEXT("PRESS x8xROLLx8x TO ROLL TO AVOID ATTACKS"));

	SendTutorialMessage(TEXT("BLINKING"), TEXT("PRESS x8xBLINKx8x TO TELEPORT A SHORT DISTANCE IN THE DIRECTION OF MOVEMVENT"));

	SendTutorialMessage(TEXT("GRENADES"), TEXT("PRESS x8xGRENADEx8x TO THROW A GRENADE AT YOUR ENEMIES, EACH CLASS HAS IT'S OWN GRENADE"));

	SendTutorialMessage(TEXT("RELOADING"), TEXT("PRESS x8xRELOADx8x TO RELOAD YOUR WEAPON, WEAPONS HAVE INFINITE AMMO, SO SPRAY AWAY"));

	SendTutorialMessage(TEXT("MELEE"), TEXT("PRESS x8xMELEEx8x TO PERFORM A KNIFE MELEE ATTACK"));

	GetWorldTimerManager().SetTimer(TutorialTimer, this, &AOrionPlayerController::TickGenericTutorials, 20.0f, false);
}

void AOrionPlayerController::DoPounceTutorial()
{
	AOrionCharacter *Pawn = Cast<AOrionCharacter>(GetPawn());
	if (Pawn && Pawn->bKnockedDown)
		SendTutorialMessage(TEXT("POUNCED"), TEXT("PRESS x8xMELEEx8x TO ESCAPE FROM THE RAPTOR"));
}

void AOrionPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bHereFromStart = false;
	bPhotonReady = false;

	AOrionGameMenu *Menu = Cast<AOrionGameMenu>(GetWorld()->GetAuthGameMode());
	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);

#if !IS_SERVER
	if (IsLocalPlayerController())
	{
		//FInputModeGameAndUI Data;
		//Data.SetLockMouseToViewport(true);

		//SetInputMode(Data);

		SendTutorialMessage(TEXT("WAITING TO SPAWN"), TEXT("YOU ARE WAITING TO SPAWN.  GAME WILL COMMENCE SHORTLY"));

		AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);
		if (PRI)
		{
			//PRI->PlayFabID = UOrionTCPLink::PlayFabID;
			//PRI->SessionTicket = UOrionTCPLink::SessionTicket;
			//PRI->CharacterID = UOrionTCPLink::CurrentCharacterID;
		}

		//UOrionTCPLink::GetCharacterData(this);

		if (Menu || (GRI && GRI->bIsLobby))
		{
			FTimerHandle Handle;
			GetWorldTimerManager().SetTimer(Handle, this, &AOrionPlayerController::TickPhoton, 0.5f, true);
		}

		GetWorldTimerManager().SetTimer(TutorialTimer, this, &AOrionPlayerController::TickGenericTutorials, 35.0f, false);

		//register some steam delegates
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

		if (OnlineSub)
		{
			FOnSessionUserInviteAcceptedDelegate OnSessionInviteAcceptedDelegate = FOnSessionUserInviteAcceptedDelegate::CreateUObject(this, &AOrionPlayerController::OnInviteAccepted);
			FOnSessionInviteReceivedDelegate OnSessionInviteReceivedDelegate = FOnSessionInviteReceivedDelegate::CreateUObject(this, &AOrionPlayerController::OnInviteReceived);
			//FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &AOrionPlayerController::OnFindSession);
			FOnFindFriendSessionCompleteDelegate OnSingleSessionResultCompleteDelegate = FOnFindFriendSessionCompleteDelegate::CreateUObject(this, &AOrionPlayerController::OnFindSession);
			//FOnJoinSessionCompleteDelegate OnJoinSessionDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &AOrionPlayerController::OnFindSession);

			IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();

			if (Session.IsValid())
			{
				Session->AddOnSessionUserInviteAcceptedDelegate_Handle(OnSessionInviteAcceptedDelegate);
				Session->AddOnSessionInviteReceivedDelegate_Handle(OnSessionInviteReceivedDelegate);

				//Session->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionDelegate);

				//FOnJoinSessionCompleteDelegate joinDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &AOrionPlayerController::OnJoinSessionCompleted);
				//Session->AddOnJoinSessionCompleteDelegate_Handle(joinDelegate);

				//FOnSingleSessionResultComplete singleDelegate = FOnSingleSessionResultComplete::

				//FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &AOrionPlayerController::OnJoinSessionCompleted);

				//Session->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
			}

			IOnlineFriendsPtr Friend = OnlineSub->GetFriendsInterface();

			if (Friend.IsValid())
			{
				//FOnInviteAcceptedDelegate OnInviteAcceptedDelegate = FOnInviteAcceptedDelegate::CreateUObject(this, &AOrionPlayerController::OnFriendInviteAccepted);
				//FOnAcceptInviteComplete OnAcceptInvite = FOnAcceptInviteComplete::CreateUObject(this, &AOrionPlayerController::OnAcceptFriendInvite);

				//Friend->AddOnInviteAcceptedDelegate_Handle(OnInviteAcceptedDelegate);
				//Friend->AddOnAcceptInviteComplete_Handle(OnAcceptInvite);

				Friend->OnInviteReceivedDelegates.AddUObject(this, &AOrionPlayerController::OnFriendInviteAccepted);
				//Friend->OnInviteAcceptedDelegates.AddUObject(this, &AOrionPlayerController::)
			}

			//FOnSingleSessionResultCompleteDelegate OnFindFriendDelegate = FOnSingleSessionResultCompleteDelegate::CreateUObject(this, &AOrionPlayerController::OnFindFriend);
			//FOnSingleSessionResultComplete.AddUObject(this, &AOrionPlayerController::OnJoinGame);
		}

		//clear our steam rich presence if needed
		SetLobbyName("");

		//if we're no in the main menu, reset photon junk
		if (Cast<AOrionGameMenu>(GetWorld()->GetAuthGameMode()) == nullptr && GRI && !GRI->bIsLobby)
			UPhotonProxy::Reset();

		SetThirdPerson();
	}
#endif

	if (SkillTrees)
	{
		MySkillTree = NewObject<UOrionSkillTree>(this, SkillTrees);

		//set to defaults to avoid nasty crashes
		SetDefaultSkills();
	}

	//run this on the client too, it'll give a skill points popup if we have some skill points!
	EventServerGetSkillTreeInfo();

	if (Role == ROLE_Authority)
	{
		//EventServerGetSkillTreeInfo();

		GetWorldTimerManager().SetTimer(ServerTickTimer, this, &AOrionPlayerController::ServerTick, 0.35f, true);
		GetWorldTimerManager().SetTimer(OrbTimer, this, &AOrionPlayerController::UpdateOrbEffects, 0.01f, true);
	}

#if !IS_SERVER
	if (Menu && !Menu->LobbyIP.IsEmpty())
		JoinLobby(Menu->LobbyIP);
#endif
}

void AOrionPlayerController::SetThirdPerson()
{
	if (Cast<AOrionGameMenu>(GetWorld()->GetAuthGameMode()) != nullptr)
	{
		bThirdPersonCamera = false;

		bShowMouseCursor = true;

		//FInputModeGameOnly Data;
		//Data.SetLockMouseToViewport(true);

		//SetInputMode(Data);

		return;
	}

	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);

	if (GRI && GRI->bIsLobby)
	{
		bThirdPersonCamera = true;

		if (Role < ROLE_Authority)
			ServerSetThirdPersonCamera(bThirdPersonCamera);

		return;
	}

	UOrionGameUserSettings *Settings = nullptr;

	if (GEngine && GEngine->GameUserSettings)
		Settings = Cast<UOrionGameUserSettings>(GEngine->GameUserSettings);

	if (Settings)
		bThirdPersonCamera = Settings->ThirdPersonEnabled;
	else
		bThirdPersonCamera = false;

	if (Role < ROLE_Authority)
		ServerSetThirdPersonCamera(bThirdPersonCamera);
}

void AOrionPlayerController::ToggleThirdPerson() 
{ 
	if (bMenuOpen)
		return;

	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);

	if (GRI && GRI->bIsLobby)
		return;

	bThirdPersonCamera = !bThirdPersonCamera; 

	float Length = 0.75f - FMath::Min(GetWorld()->TimeSeconds - LastCameraToggleTime, 0.75f);

	LastCameraToggleTime = GetWorld()->TimeSeconds - Length;

	if (!bThirdPersonCamera)
	{
		//bShowMouseCursor = true;

		//FInputModeGameOnly Data;
		//Data.SetLockMouseToViewport(true);

		//SetInputMode(Data);
	}
	else
	{
		//bShowMouseCursor = false;

		FInputModeGameOnly Data;
		//Data.SetLockMouseToViewport(true);

		SetInputMode(Data);
	}

	//save for us
	UOrionGameUserSettings *Settings = nullptr;

	if (GEngine && GEngine->GameUserSettings)
		Settings = Cast<UOrionGameUserSettings>(GEngine->GameUserSettings);

	if (Settings)
	{
		Settings->ThirdPersonEnabled = bThirdPersonCamera;
		Settings->SaveConfig();
	}

	AOrionCharacter *P = Cast<AOrionCharacter>(GetPawn());

	if (P)
		P->bThirdPersonCamera = bThirdPersonCamera;

	ServerSetThirdPersonCamera(bThirdPersonCamera);
}

void AOrionPlayerController::ServerSetThirdPersonCamera_Implementation(bool bOn)
{
	AOrionCharacter *P = Cast<AOrionCharacter>(GetPawn());

	if (P)
		P->bThirdPersonCamera = bOn;

	bThirdPersonCamera = bOn;
}

void AOrionPlayerController::OnFindSession(int32 LocalUserNum, bool bWasSuccessful, const FOnlineSessionSearchResult& SearchResult)
{
	FString LobbyID = SearchResult.Session.SessionSettings.Settings.Find(FName("ROOMNAME"))->ToString();

	//trim off the end shenanigans
	int32 index = LobbyID.Find(" :");

	if (index != INDEX_NONE)
	{
		LobbyID = LobbyID.Left(index);
	}

	JoinLobby(LobbyID);

	UE_LOG(LogTemp, Log, TEXT("Joining Lobby: %s"), *LobbyID);
}

void AOrionPlayerController::OnJoinSessionCompleted(int32 LocalUserNum, bool bWasSuccessful, const FOnlineSessionSearchResult& SearchResult)
{
}

void AOrionPlayerController::OnJoinGame(int32 LocalUserNum, bool bWasSuccessful, const FOnlineSessionSearchResult& SearchResult)
{

}

void AOrionPlayerController::OnAcceptFriendInvite(int32 UserNum, bool bWasSuccessful, const FUniqueNetId& FriendId, const FString& ListName, const FString& ErrorStr)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		TSharedPtr<FOnlineUserPresence> Presence;

		IOnlinePresencePtr IPresence = OnlineSub->GetPresenceInterface();

		if (IPresence.IsValid())
		{
			IPresence->GetCachedPresence(FriendId, Presence);

			FString LobbyID = Presence->Status.Properties[TEXT("ROOMNAME")].ToString();

			JoinLobby(LobbyID);

			UE_LOG(LogTemp, Log, TEXT("GundyInviteAcceptedReal: %s"), *LobbyID);
		}
	}
}

void AOrionPlayerController::OnFriendInviteAccepted(const FUniqueNetId& UserID, const FUniqueNetId& FriendID)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		TSharedPtr<FOnlineUserPresence> Presence;

		IOnlinePresencePtr IPresence = OnlineSub->GetPresenceInterface();
		
		if (IPresence.IsValid())
		{
			IPresence->GetCachedPresence(FriendID, Presence);

			FString LobbyID = Presence->Status.Properties[TEXT("ROOMNAME")].ToString();

			JoinLobby(LobbyID);

			UE_LOG(LogTemp, Log, TEXT("GundyInviteAcceptedReal: %s"), *LobbyID);
		}

	}
}

void AOrionPlayerController::OnInviteAccepted(const bool bWasSuccessful, const int32 LocalUserNum, TSharedPtr<const FUniqueNetId> FriendID, const FOnlineSessionSearchResult &SessionToJoin)
{
	//add some error here?
	if (!SessionToJoin.Session.SessionSettings.Settings.Contains("ROOMNAME"))
	{
		if (SteamAPI_Init())
		{
			const uint8* OutBytes = FriendID.Get()->GetBytes();

			CSteamID steamID = (*(uint64*)OutBytes);

			const char* ID = SteamFriends()->GetFriendRichPresence(steamID, "ROOMNAME");

			/*int32 Num = SteamFriends()->GetFriendRichPresenceKeyCount(steamID);

			for (int32 i = 0; i < Num; i++)
			{
				const char* Test = SteamFriends()->GetFriendRichPresenceKeyByIndex(steamID, i);

				UE_LOG(LogTemp, Log, TEXT("GundyInviteAcceptedReal: %s"), Test);
			}*/

			EventJoinServerID(ID);
			//JoinLobby(ID);
		}
		return;
	}

	FString LobbyID = SessionToJoin.Session.SessionSettings.Settings.Find(FName("ROOMNAME"))->ToString();

	//trim off the end shenanigans
	int32 index = LobbyID.Find(" :");

	if (index != INDEX_NONE)
	{
		LobbyID = LobbyID.Left(index);
	}

	//JoinLobby(LobbyID);
	EventJoinServerID(LobbyID);

	UE_LOG(LogTemp, Log, TEXT("Invite Accepted to Lobby: %s"), *LobbyID);

	/*IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		TSharedPtr<FOnlineUserPresence> Presence;

		IOnlinePresencePtr IPresence = OnlineSub->GetPresenceInterface();

		if (IPresence.IsValid())
		{
			IPresence->GetCachedPresence(*FriendID.Get(), Presence);

			FString LobbyID = Presence->Status.Properties[TEXT("ROOMNAME")].ToString();

			JoinLobby(LobbyID);

			UE_LOG(LogTemp, Log, TEXT("GundyInviteAcceptedReal: %s"), *LobbyID);
		}
		else
		{
			if (SteamAPI_Init())
			{
				const uint8* OutBytes = FriendID.Get()->GetBytes();

				CSteamID steamID(*(uint64*)OutBytes);

				const char* ID = SteamFriends()->GetFriendRichPresence(steamID, "ROOMNAME");

				int32 Num = SteamFriends()->GetFriendRichPresenceKeyCount(steamID);

				for (int32 i = 0; i < Num; i++)
				{
					const char* Test = SteamFriends()->GetFriendRichPresenceKeyByIndex(steamID, i);

					UE_LOG(LogTemp, Log, TEXT("GundyInviteAcceptedReal: %s"), Test);
				}
				
				JoinLobby(ID);
			}
		}
	}*/
}

void AOrionPlayerController::OnInviteReceived(const FUniqueNetId& UserId, const FUniqueNetId& FromId, const FString& AppId, const FOnlineSessionSearchResult& InviteResult)
{
	FString LobbyID = InviteResult.Session.SessionSettings.Settings.Find(FName("ROOMNAME"))->ToString();

	UE_LOG(LogTemp, Log, TEXT("GundyInviteReceived: %s"), *LobbyID);
}

void AOrionPlayerController::SetDefaultSkills()
{
	if (MySkillTree)
	{
		//reset the values
		CharacterSkills.Empty();
		CharacterSkills.SetNum(SKILL_NUMPLUSONE);

		for (int32 i = 0; i < MySkillTree->Skills[0].SkillCategory1.Num(); i++)
		{
			FSkillCategory Category = MySkillTree->Skills[0].SkillCategory1[i];

			CharacterSkills[Category.Skill1.Category].Points = Category.Skill1.Points;
			CharacterSkills[Category.Skill1.Category].MaxPoints = Category.Skill1.MaxPoints;
			CharacterSkills[Category.Skill1.Category].Modifier = FMath::Max(1, Category.Skill1.Upgrade1);

			CharacterSkills[Category.Skill2.Category].Points = Category.Skill2.Points;
			CharacterSkills[Category.Skill2.Category].MaxPoints = Category.Skill2.MaxPoints;
			CharacterSkills[Category.Skill2.Category].Modifier = FMath::Max(1, Category.Skill2.Upgrade1);

			CharacterSkills[Category.Skill3.Category].Points = Category.Skill3.Points;
			CharacterSkills[Category.Skill3.Category].MaxPoints = Category.Skill3.MaxPoints;
			CharacterSkills[Category.Skill3.Category].Modifier = FMath::Max(1, Category.Skill3.Upgrade1);
		}
		for (int32 i = 0; i < MySkillTree->Skills[0].SkillCategory2.Num(); i++)
		{
			FSkillCategory Category = MySkillTree->Skills[0].SkillCategory2[i];

			CharacterSkills[Category.Skill1.Category].Points = Category.Skill1.Points;
			CharacterSkills[Category.Skill1.Category].MaxPoints = Category.Skill1.MaxPoints;
			CharacterSkills[Category.Skill1.Category].Modifier = FMath::Max(1, Category.Skill1.Upgrade1);

			CharacterSkills[Category.Skill2.Category].Points = Category.Skill2.Points;
			CharacterSkills[Category.Skill2.Category].MaxPoints = Category.Skill2.MaxPoints;
			CharacterSkills[Category.Skill2.Category].Modifier = FMath::Max(1, Category.Skill2.Upgrade1);

			CharacterSkills[Category.Skill3.Category].Points = Category.Skill3.Points;
			CharacterSkills[Category.Skill3.Category].MaxPoints = Category.Skill3.MaxPoints;
			CharacterSkills[Category.Skill3.Category].Modifier = FMath::Max(1, Category.Skill3.Upgrade1);
		}
		for (int32 i = 0; i < MySkillTree->Skills[0].SkillCategory3.Num(); i++)
		{
			FSkillCategory Category = MySkillTree->Skills[0].SkillCategory3[i];

			CharacterSkills[Category.Skill1.Category].Points = Category.Skill1.Points;
			CharacterSkills[Category.Skill1.Category].MaxPoints = Category.Skill1.MaxPoints;
			CharacterSkills[Category.Skill1.Category].Modifier = FMath::Max(1, Category.Skill1.Upgrade1);

			CharacterSkills[Category.Skill2.Category].Points = Category.Skill2.Points;
			CharacterSkills[Category.Skill2.Category].MaxPoints = Category.Skill2.MaxPoints;
			CharacterSkills[Category.Skill2.Category].Modifier = FMath::Max(1, Category.Skill2.Upgrade1);

			CharacterSkills[Category.Skill3.Category].Points = Category.Skill3.Points;
			CharacterSkills[Category.Skill3.Category].MaxPoints = Category.Skill3.MaxPoints;
			CharacterSkills[Category.Skill3.Category].Modifier = FMath::Max(1, Category.Skill3.Upgrade1);
		}
	}
}

void AOrionPlayerController::SetSkillTreeValuesForUse()
{
	if (Role != ROLE_Authority)
	{
		ServerSetSkillTreeValuesForUse();
		return;
	}

	//cycle through our skill tree and convert the data into a quicker lookup version
	if (MySkillTree)
	{
		//reset the values
		CharacterSkills.Empty();
		CharacterSkills.SetNum(SKILL_NUMPLUSONE);

		int32 cIndex = GetClassIndex();

		for (int32 i = 0; i < MySkillTree->Skills[cIndex].SkillCategory1.Num(); i++)
		{
			FSkillCategory Category = MySkillTree->Skills[cIndex].SkillCategory1[i];

			CharacterSkills[Category.Skill1.Category].Points = Category.Skill1.Points;
			CharacterSkills[Category.Skill1.Category].MaxPoints = Category.Skill1.MaxPoints;
			CharacterSkills[Category.Skill1.Category].Modifier = FMath::Max(1,Category.Skill1.Upgrade1);

			CharacterSkills[Category.Skill2.Category].Points = Category.Skill2.Points;
			CharacterSkills[Category.Skill2.Category].MaxPoints = Category.Skill2.MaxPoints;
			CharacterSkills[Category.Skill2.Category].Modifier = FMath::Max(1, Category.Skill2.Upgrade1);

			CharacterSkills[Category.Skill3.Category].Points = Category.Skill3.Points;
			CharacterSkills[Category.Skill3.Category].MaxPoints = Category.Skill3.MaxPoints;
			CharacterSkills[Category.Skill3.Category].Modifier = FMath::Max(1, Category.Skill3.Upgrade1);
		}
		for (int32 i = 0; i < MySkillTree->Skills[cIndex].SkillCategory2.Num(); i++)
		{
			FSkillCategory Category = MySkillTree->Skills[cIndex].SkillCategory2[i];

			CharacterSkills[Category.Skill1.Category].Points = Category.Skill1.Points;
			CharacterSkills[Category.Skill1.Category].MaxPoints = Category.Skill1.MaxPoints;
			CharacterSkills[Category.Skill1.Category].Modifier = FMath::Max(1, Category.Skill1.Upgrade1);

			CharacterSkills[Category.Skill2.Category].Points = Category.Skill2.Points;
			CharacterSkills[Category.Skill2.Category].MaxPoints = Category.Skill2.MaxPoints;
			CharacterSkills[Category.Skill2.Category].Modifier = FMath::Max(1, Category.Skill2.Upgrade1);

			CharacterSkills[Category.Skill3.Category].Points = Category.Skill3.Points;
			CharacterSkills[Category.Skill3.Category].MaxPoints = Category.Skill3.MaxPoints;
			CharacterSkills[Category.Skill3.Category].Modifier = FMath::Max(1, Category.Skill3.Upgrade1);
		}
		for (int32 i = 0; i < MySkillTree->Skills[cIndex].SkillCategory3.Num(); i++)
		{
			FSkillCategory Category = MySkillTree->Skills[cIndex].SkillCategory3[i];

			CharacterSkills[Category.Skill1.Category].Points = Category.Skill1.Points;
			CharacterSkills[Category.Skill1.Category].MaxPoints = Category.Skill1.MaxPoints;
			CharacterSkills[Category.Skill1.Category].Modifier = FMath::Max(1, Category.Skill1.Upgrade1);

			CharacterSkills[Category.Skill2.Category].Points = Category.Skill2.Points;
			CharacterSkills[Category.Skill2.Category].MaxPoints = Category.Skill2.MaxPoints;
			CharacterSkills[Category.Skill2.Category].Modifier = FMath::Max(1, Category.Skill2.Upgrade1);

			CharacterSkills[Category.Skill3.Category].Points = Category.Skill3.Points;
			CharacterSkills[Category.Skill3.Category].MaxPoints = Category.Skill3.MaxPoints;
			CharacterSkills[Category.Skill3.Category].Modifier = FMath::Max(1, Category.Skill3.Upgrade1);
		}
	}

	ClientSetCharacterSkills(CharacterSkills);
}

void AOrionPlayerController::ClientSetCharacterSkills_Implementation(const TArray<FUnlockedSkills> &Skills)
{
	CharacterSkills = Skills;
}

int32 AOrionPlayerController::GetSkillValue(ESkillTreeUnlocks Skill)
{
	if (CharacterSkills.Num() <= int32(Skill))
		return 0;

	return FMath::Clamp(CharacterSkills[Skill].Points, 0, 10) * FMath::Max(1, CharacterSkills[Skill].Modifier);

	//for testing
	//return CharacterSkills[Skill].MaxPoints * FMath::Max(1, CharacterSkills[Skill].Modifier);
}

void AOrionPlayerController::ServerSetSkillTreeValuesForUse_Implementation()
{
	//call playfab to make sure we get the correct info, don't trust those filthy clients
	EventServerGetSkillTreeInfo();
	//SetSkillTreeValuesForUse();
}

FString AOrionPlayerController::FormatTutorial(FString Desc)
{
	FString Ret = Desc;

	int32 Start = Desc.Find("x8x") + 3;

	if (Start > INDEX_NONE)
	{
		int32 End = Desc.Find("x8x", ESearchCase::IgnoreCase, ESearchDir::FromStart, Start);

		if (End > INDEX_NONE)
		{
			FString Key = Desc.Mid(Start, End - Start);

			bool bIsController = false;

			AOrionCharacter *Pawn = Cast<AOrionCharacter>(GetPawn());

			if (Pawn)
				bIsController = Pawn->bController;

			if (bIsController)
			{
				if (Key == "ROLL")
					Key = UOrionGameSettingsManager::GetKeyForAction("Gamepad_Roll", false, 0.0f);
				else if (Key == "BLINK")
					Key = UOrionGameSettingsManager::GetKeyForAction("Gamepad_Blink", false, 0.0f);
				else if (Key == "REGEN")
					Key = UOrionGameSettingsManager::GetKeyForAction("Gamepad_WeaponSlot3", false, 0.0f);
				else if (Key == "GRENADE")
					Key = UOrionGameSettingsManager::GetKeyForAction("Gamepad_ThrowGrenade", false, 0.0f);
				else if (Key == "RELOAD")
					Key = UOrionGameSettingsManager::GetKeyForAction("Gamepad_Reload", false, 0.0f);
				else if (Key == "MELEE")
					Key = UOrionGameSettingsManager::GetKeyForAction("Gamepad_Melee", false, 0.0f);
				else if (Key == "LEVEL")
					Key = UOrionGameSettingsManager::GetKeyForAction("Gamepad_OpenSkillTree", false, 0.0f);
			}
			else
			{
				if (Key == "ROLL")
					Key = UOrionGameSettingsManager::GetKeyForAction("Roll", false, 0.0f);
				else if (Key == "BLINK")
					Key = UOrionGameSettingsManager::GetKeyForAction("Blink", false, 0.0f);
				else if (Key == "REGEN")
					Key = UOrionGameSettingsManager::GetKeyForAction("WeaponSlot3", false, 0.0f);
				else if (Key == "GRENADE")
					Key = UOrionGameSettingsManager::GetKeyForAction("ThrowGrenade", false, 0.0f);
				else if (Key == "RELOAD")
					Key = UOrionGameSettingsManager::GetKeyForAction("Reload", false, 0.0f);
				else if (Key == "MELEE")
					Key = UOrionGameSettingsManager::GetKeyForAction("Melee", false, 0.0f);
				else if (Key == "LEVEL")
					Key = UOrionGameSettingsManager::GetKeyForAction("OpenSkillTree", false, 0.0f);
			}

			Ret = Desc.Left(Start - 3).Append("[").Append(Key.ToUpper()).Append("]").Append(Desc.Mid(End + 3));
		}
	}

	return Ret;
}

void AOrionPlayerController::SendTutorialMessage(FString Title, FString Desc)
{
	if (IsLocalPlayerController())
	{
		UOrionGameUserSettings *Settings = nullptr;

		if (GEngine && GEngine->GameUserSettings)
			Settings = Cast<UOrionGameUserSettings>(GEngine->GameUserSettings);

		if (Settings->TutorialsEnabled == false)
			return;

		if (GetWorld()->GetTimeSeconds() - LastTutorialTime >= 10.0f)
		{
			if (!UOrionGameSettingsManager::GetTutorial(Title))
			{
				EventShowTutorial(Title, FormatTutorial(Desc));

				UOrionGameSettingsManager::SetTutorial(Title);

				LastTutorialTime = GetWorld()->GetTimeSeconds();
			}
		}
	}
}

void AOrionPlayerController::ClientSendTutorialMessage_Implementation(const FString &Title, const FString &Desc)
{ 
	SendTutorialMessage(Title, Desc); 
}

void AOrionPlayerController::SetConnectInfo(const FString &RoomName)
{
	if (SteamAPI_Init())
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

		if (OnlineSub)
		{
			IOnlineIdentityPtr User = OnlineSub->GetIdentityInterface();

			if (!User.IsValid())
				return;

			IOnlinePresencePtr Presence = OnlineSub->GetPresenceInterface();

			if (!Presence.IsValid())
				return;

			FOnlineKeyValuePairs<FPresenceKey, FVariantData> props;

			props.Add(TEXT("LobbyInfo"), RoomName);

			FOnlineUserPresenceStatus P;

			P.State = EOnlinePresenceState::Online;
			P.Properties = props;

			Presence->SetPresence(*User->GetUniquePlayerId(0), P);
		}
	}
}

FString AOrionPlayerController::GetSteamID()
{
	if (SteamAPI_Init())
	{
		//set the lobby info to blank so we don't try to join an old match that wasn't closed properly
		SetConnectInfo(TEXT("0"));

		//CSteamID steamID = SteamUser()->GetSteamID();

		//uint64 Return = steamID.CSteamID::ConvertToUint64();

		//Return ID as String if Found
		//FString ID = FString::Printf(TEXT("%016llX"), Return);
		//FString ID = FString::Printf(TEXT("%llu"), Return);
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		FString ID = "";

		if (OnlineSub)
		{
			IOnlineIdentityPtr User = OnlineSub->GetIdentityInterface();

			if (!User.IsValid())
				return TEXT("");

			ID = User->GetAuthToken(0);
		}

		return ID;
	}

	return TEXT("0");
}

void AOrionPlayerController::ReadFriends()
{
	GetFriends();
}

void AOrionPlayerController::GetFriends()
{
	if (SteamAPI_Init())
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

		if (OnlineSub)
		{
			Friends.Empty();

			IOnlineFriendsPtr FriendsInt = OnlineSub->GetFriendsInterface();

			FOnReadFriendsListComplete ReadFriendsDelegate = FOnReadFriendsListComplete::CreateUObject(this, &AOrionPlayerController::ReadFriendsDelegate);

			if (FriendsInt.IsValid())
				FriendsInt->ReadFriendsList(0, EFriendsLists::ToString(EFriendsLists::Default), ReadFriendsDelegate);
		}
	}
}

UTexture2D *AOrionPlayerController::GetFriendAvatar(TSharedRef<FOnlineFriend> Friend)
{
	uint32 Width;
	uint32 Height;

	bool IsValid = false;

	if (SteamAPI_Init())
	{
		//Getting the PictureID from the SteamAPI and getting the Size with the ID
		const char *SteamID = TCHAR_TO_UTF8(*Friend->GetUserId()->ToString());
		CSteamID ID = CSteamID(strtoull(SteamID, nullptr, 10)); //convert string to uint64
		int Picture = SteamFriends()->GetMediumFriendAvatar(ID);
		SteamUtils()->GetImageSize(Picture, &Width, &Height);

		if (Width > 0 && Height > 0)
		{
			IsValid = true;

			//Creating the buffer "oAvatarRGBA" and then filling it with the RGBA Stream from the Steam Avatar
			BYTE *oAvatarRGBA = new BYTE[Width * Height * 4];

			//Filling the buffer with the RGBA Stream from the Steam Avatar and creating a UTextur2D to parse the RGBA Steam in
			SteamUtils()->GetImageRGBA(Picture, (uint8*)oAvatarRGBA, 4 * Height * Width * sizeof(char));
			UTexture2D* Avatar = UTexture2D::CreateTransient(Width, Height, PF_R8G8B8A8);

			//MAGIC!
			uint8* MipData = (uint8*)Avatar->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(MipData, (void*)oAvatarRGBA, Height * Width * 4);
			Avatar->PlatformData->Mips[0].BulkData.Unlock();

			//Setting some Parameters for the Texture and finally returning it
			Avatar->PlatformData->NumSlices = 1;
			Avatar->NeverStream = true;
			//Avatar->Rename(*Friend->GetUserId()->ToString());
			Avatar->CompressionSettings = TC_EditorIcon;

			Avatar->UpdateResource();

			//SteamAPI_Shutdown();
			return Avatar;
		}
	}

	return nullptr;
}

void AOrionPlayerController::ReadFriendsDelegate(int32 LocalUserNum, bool bSuccessful, const FString& ListName, const FString& ErrorString)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		IOnlineFriendsPtr FriendsInt = OnlineSub->GetFriendsInterface();

		TArray<TSharedRef<FOnlineFriend> > FFriends;

		TArray<FFriendListData> JoinableFriends;
		TArray<FFriendListData> PlayingFriends;
		TArray<FFriendListData> OnlineFriends;
		TArray<FFriendListData> OfflineFriends;

		if (FriendsInt->GetFriendsList(0, EFriendsLists::ToString(EFriendsLists::Default), FFriends))
		{
			for (int32 i = 0; i < FFriends.Num(); i++)
			{
				FFriendListData newFriend;

				newFriend.PlayerName = FFriends[i]->GetDisplayName();
				newFriend.bOnline = FFriends[i]->GetPresence().bIsOnline;
				newFriend.bPlayingGame = FFriends[i]->GetPresence().bIsPlayingThisGame;
				if (!FFriends[i]->GetUserAttribute(TEXT("LobbyInfo"), newFriend.LobbyID))
					newFriend.LobbyID = TEXT("0");
				newFriend.SteamID = BytesToString(FFriends[i]->GetUserId()->GetBytes(), FFriends[i]->GetUserId()->GetSize());//->ToString();

				//try to grab the avatar for this player
				newFriend.Avatar = GetFriendAvatar(FFriends[i]);

				if (!newFriend.bOnline)
					OfflineFriends.Add(newFriend);
				else if (!newFriend.bPlayingGame)
					OnlineFriends.Add(newFriend);
				else
					PlayingFriends.Add(newFriend);
			}
		}

		//sort each sub category by name
		JoinableFriends.Sort();
		PlayingFriends.Sort();
		OnlineFriends.Sort();
		OfflineFriends.Sort();

		//sort the friends list, with online friends playing games at the top and offlines friends at the bottom
		Friends = JoinableFriends;
		Friends.Append(PlayingFriends);
		Friends.Append(OnlineFriends);
		Friends.Append(OfflineFriends);

		EventDrawFriends();
	}
}

void AOrionPlayerController::TickPhoton()
{
#if !IS_SERVER
	//only tick photon during the main menu
	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);

	if (Cast<AOrionGameMenu>(GetWorld()->GetAuthGameMode()) == nullptr && (!GRI || !GRI->bIsLobby))
		return;

	if (UPhotonProxy::GetListener())
	{
		UPhotonProxy::GetListener()->PCOwner = this;
		UPhotonProxy::GetListener()->UpdatePlayerSettings();
		SendPlayerInfoToPhoton();

		UPhotonProxy::GetListener()->GrabRoomSettings();
	
		//update our lobby settings while in game
		/*if (IsLobbyLeader())
		{
			AOrionGameMode *Game = Cast<AOrionGameMode>(GetWorld()->GetAuthGameMode());
			AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);
			UOrionGameInstance *GI = Cast<UOrionGameInstance>(GetGameInstance());
			if (!Game && GRI && GI)
			{
				FString RoomName = GI->PlayFabName;// Info.RoomName;
				RoomName.Append(TEXT("'s Server"));
				FString Version = GetBuildVersion();

				//if (ServerInfo.GameMode.Find("SLAUGHTER") >= 0)
				//	ServerInfo.Difficulty = "SLAUGHTER";

				FString Wave = GRI->WaveNum > 9 ? FString::Printf(TEXT("WAVE %i"), GRI->WaveNum) : FString::Printf(TEXT("WAVE 0%i"), GRI->WaveNum);
				UPhotonProxy::GetListener()->SetLobbySettings(TCHAR_TO_UTF8(*ServerInfo.MapName), TCHAR_TO_UTF8(*ServerInfo.Difficulty), TCHAR_TO_UTF8(*ServerInfo.GameMode),
					TCHAR_TO_UTF8(*ServerInfo.Privacy), TCHAR_TO_UTF8(*GI->ServerIP), TCHAR_TO_UTF8(*ServerInfo.LobbyID), TCHAR_TO_UTF8(*Wave), TCHAR_TO_UTF8(*Version),
					TCHAR_TO_UTF8(*RoomName), TCHAR_TO_UTF8(*ServerInfo.TOD), TCHAR_TO_UTF8(*ServerInfo.ItemLevel), TCHAR_TO_UTF8(*ServerInfo.Region));
			}
		}*/
	}
#endif
}

void AOrionPlayerController::RefreshLobbyList()
{
#if !IS_SERVER
	if (UPhotonProxy::GetListener())
	{
		UPhotonProxy::GetListener()->PCOwner = this;
		UPhotonProxy::GetListener()->RefreshRoomList();
	}
#endif
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
	if (StatsClass)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnInfo.Owner = this;

		Stats = GetWorld()->SpawnActor<AOrionStats>(StatsClass, SpawnInfo);

		if (Stats && Role == ROLE_Authority)
		{
			if (!Cast<AOrionGameMenu>(GetWorld()->GetAuthGameMode()))
				Stats->ReadPlayerStats(this);

			Stats->PCOwner = this;
		}

		//if (!SavedGame)
		{
			Achievements = GetWorld()->SpawnActor<AOrionAchievements>(AchievementsClass, SpawnInfo);

			if (Achievements)
				Achievements->Init();

			if (Stats && Role == ROLE_Authority)
			{
				if (!Cast<AOrionGameMenu>(GetWorld()->GetAuthGameMode()))
					Achievements->ReadPlayerAchievements(this);

				Achievements->PCOwner = this;
			}
		}
	}
}

#if WITH_CHEATS
void AOrionPlayerController::IceAge()
{
	ServerIceAge();
}
#endif
void AOrionPlayerController::UnlockAchievement_Implementation(const FString &Header, const FString &Body)
{
	UOrionGameUserSettings *Settings = nullptr;

	if (GEngine && GEngine->GameUserSettings)
		Settings = Cast<UOrionGameUserSettings>(GEngine->GameUserSettings);

	if (Settings->AchievementNotifiesEnabled == false)
		return;

	FNotificationHelper Notify;
	Notify.Header = Header;
	Notify.Body = Body;
	
	Notifications.Add(Notify);

	//ProcessNotifications();
}

//#if WITH_CHEATS
void AOrionPlayerController::TestLevel()
{
	//connect to gundy's local server for testing
	ConnectToIP("192.168.1.64");
}
//#endif

void AOrionPlayerController::ProcessNotifications()
{
	if (Notifications.Num() > 0 && GetWorld()->GetTimeSeconds() - LastNotificationTime > 10.0f)
	{
		LastNotificationTime = GetWorld()->GetTimeSeconds();

		EventShowNotification(Notifications[0].Header, Notifications[0].Body);

		Notifications.RemoveAt(0);
	}
}

void AOrionPlayerController::PlayLevelUpEffect_Implementation(int32 NewLevel)
{
	EventPlayLevelUpEffect(NewLevel);
}

void AOrionPlayerController::ShowLevelUpMessage_Implementation(int32 NewLevel)
{
	//EventShowLevelUpMessage(NewLevel);
	FNotificationHelper Notify;

	Notify.Header = TEXT("LEVEL UP");
	Notify.Body = FString::Printf(TEXT("LEVEL %i"), NewLevel);

	//remove this until the button is brought back?
	////if (IsLocalPlayerController())
	////	SendTutorialMessage("LEVELING UP", "YOU HAVE LEVELED UP!  PRESS x8xLEVELx8x TO SPEND YOUR SKILL POINTS");

	Notifications.Add(Notify);
}
#if WITH_CHEATS
void AOrionPlayerController::ServerIceAge_Implementation()
{
	TArray<AActor*> Dinos;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrionCharacter::StaticClass(), Dinos);

	for (int32 i = 0; i < Dinos.Num(); i++)
	{
		AOrionDinoPawn *Dino = Cast<AOrionDinoPawn>(Dinos[i]);
		if (Dino)
		{
			AOrionPRI *PRI = Cast<AOrionPRI>(Dino->PlayerState);

			if (PRI && !PRI->bIsABot)
				continue;

			FDamageEvent dEvent = FDamageEvent::FDamageEvent();
			dEvent.DamageTypeClass = UOrionDamageType::StaticClass();

			Dino->Die(500000.0f, dEvent, nullptr, Dino);
		}
	}
}
#endif
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

void AOrionPlayerController::InitQuestManager()
{
	QuestManager = NewObject<UOrionQuestManager>(this, UOrionQuestManager::StaticClass());
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