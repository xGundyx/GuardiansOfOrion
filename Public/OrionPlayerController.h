// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

/*#if IS_SERVER
#include "playfab/PlayFabServerAPI.h"
#else
#include "playfab/PlayFabClientAPI.h"
#endif*/
#include "PlayFabRequestProxy.h"
#include "OrionWeather.h"
#include "GameFramework/PlayerController.h"
#include "OrionInventoryManager.h"
#include "OrionQuest.h"
#include "OrionQuestManager.h"
#include "OrionStats.h"
#include "OrionAchievements.h"
#include "OrionDroidPawn.h"
#include "OrionDropPod.h"
#include "OrionChatManager.h"
#include "OrionTypes.h"
#include "OrionPlayFabInventoryMapper.h"
#include "OrionSkillTree.h"
//#include "SPanel.h"
#include "OrionPlayerController.generated.h"

/**
*
*/

USTRUCT()
struct FNotificationHelper
{
	GENERATED_USTRUCT_BODY()

	FString Header;
	FString Body;
};

USTRUCT(BlueprintType)
struct FLobbyPlayer
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Lobby)
		int32 PlayerNumber;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Lobby)
		FString PlayerName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Lobby)
		FString PlayerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Lobby)
		FString PlayerLevel;
};

USTRUCT(BlueprintType)
struct FLobbyData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Lobby)
		FString ServerName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Lobby)
		FString Difficulty;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Lobby)
		FString MapName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Lobby)
		FString PlayerCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Lobby)
		FString Progress;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Lobby)
		FString Ping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Lobby)
		FString IP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Lobby)
		FString LobbyTicket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Lobby)
		FString GUID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Lobby)
		FString RoomName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Lobby)
		FString GameMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Lobby)
		FString ItemLevel;
};

USTRUCT(BlueprintType)
struct FAnimTester
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Anim)
		USkeletalMesh *Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Anim)
		USkeletalMesh *ExtraMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Anim)
		TArray<class UAnimationAsset*> Animations;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Anim)
		FName Type;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Anim)
		float Dist;

	FAnimTester()
	{
		Dist = 1500.0f;
	}
};

USTRUCT(BlueprintType)
struct FOptionsData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Option)
		FString Title;

	UPROPERTY(BlueprintReadOnly, Category = Option)
		TArray<FString> Options;

	UPROPERTY(BlueprintReadOnly, Category = Option)
		FString Value;
};

USTRUCT(BlueprintType)
struct FOptionsValueData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Option)
		FString Title;

	UPROPERTY(BlueprintReadOnly, Category = Option)
		float Value;
};

USTRUCT(BlueprintType)
struct FKeyboardOptionsData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Option)
		FString Title;

	UPROPERTY(BlueprintReadOnly, Category = Option)
		FString Key;

	UPROPERTY(BlueprintReadOnly, Category = Option)
		FString Action;

	UPROPERTY(BlueprintReadOnly, Category = Option)
		float Scale;
};

UENUM(BlueprintType)
enum EControllerButton
{
	BUTTON_A,
	BUTTON_B,
	BUTTON_X,
	BUTTON_Y,
	BUTTON_DOWN,
	BUTTON_LEFT,
	BUTTON_UP,
	BUTTON_RIGHT,
	BUTTON_LEFTSTICK,
	BUTTON_RIGHTSTICK,
	BUTTON_LEFTSHOULDER,
	BUTTON_LEFTTRIGGER,
	BUTTON_RIGHTSHOULDER,
	BUTTON_RIGHTTRIGGER,
	BUTTON_START,
	BUTTON_HOME
};

USTRUCT(BlueprintType)
struct FControllerOptionsData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Option)
		FString Title;

	UPROPERTY(BlueprintReadOnly, Category = Option)
		TEnumAsByte<EControllerButton> Button;

	UPROPERTY(BlueprintReadOnly, Category = Option)
		FString Action;
};

USTRUCT(BlueprintType)
struct FInventoryMapper
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Category = Character)
		FString InstanceID;

	UPROPERTY(BlueprintReadWrite, Category = Character)
		FString ItemName;
};

USTRUCT(BlueprintType)
struct FCharacterData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Category = Character)
		FString pName;

	UPROPERTY(BlueprintReadWrite, Category = Character)
		FString pClass;

	UPROPERTY(BlueprintReadWrite, Category = Character)
		FString CharacterID;

	UPROPERTY(BlueprintReadWrite, Category = Character)
		FString Sex;

	UPROPERTY(BlueprintReadWrite, Category = Character)
		FString SuitColor;

	UPROPERTY(BlueprintReadWrite, Category = Character)
		int32 Level;

	UPROPERTY(BlueprintReadWrite, Category = Character)
		int32 Exp;

	UPROPERTY(BlueprintReadWrite, Category = Character)
		FInventoryMapper HelmetID;

	UPROPERTY(BlueprintReadWrite, Category = Character)
		FInventoryMapper ChestID;

	UPROPERTY(BlueprintReadWrite, Category = Character)
		FInventoryMapper HandsID;

	UPROPERTY(BlueprintReadWrite, Category = Character)
		FInventoryMapper LegsID;

	UPROPERTY(BlueprintReadWrite, Category = Character)
		FInventoryMapper PrimaryWeaponID;

	UPROPERTY(BlueprintReadWrite, Category = Character)
		FInventoryMapper SecondaryWeaponID;

	void Reset()
	{
		pName = FString("");
		pClass = FString("");
		CharacterID = FString("0");
		Level = 0;
		Exp = 0;
		HelmetID.ItemName = FString("");
		ChestID.ItemName = FString("");
		HandsID.ItemName = FString("");
		LegsID.ItemName = FString("");
		PrimaryWeaponID.ItemName = FString("");
		SecondaryWeaponID.ItemName = FString("");
		HelmetID.InstanceID = FString("");
		ChestID.InstanceID = FString("");
		HandsID.InstanceID = FString("");
		LegsID.InstanceID = FString("");
		PrimaryWeaponID.InstanceID = FString("");
		SecondaryWeaponID.InstanceID = FString("");
		SuitColor = FString("");
		Sex = FString("");
	}
};

UCLASS()
class ORION_API AOrionPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AOrionPlayerController(const FObjectInitializer& ObjectInitializer);

	virtual void PlayerTick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;

	void SetupInputComponent() override;

	void GetPlayerViewPoint(FVector& OutCamLoc, FRotator& OutCamRot) const override;

	void OpenInventory();

	void CreateInventory();
	void GetDefaultInventory();

	virtual void Possess(APawn* aPawn) override;

	UFUNCTION(BlueprintCallable, Category = Steam)
		FString GetSteamID();

	UFUNCTION(BlueprintCallable, Category = Steam)
		void GetFriends();

	void ReadFriendsDelegate(int32 LocalUserNum, bool bSuccessful, const FString& ListName, const FString& ErrorString);

	UPROPERTY(BlueprintReadOnly, Category = Steam)
		TArray<FFriendListData> Friends;

	UTexture2D *GetFriendAvatar(TSharedRef<FOnlineFriend> Friend);

	UFUNCTION(BlueprintCallable, Category = Steam)
		void SetConnectInfo(const FString &RoomName);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "DrawFriends"))
		void EventDrawFriends();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "SetServerInfo"))
		void EventSetServerInfo();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "SetWave"))
		void EventSetWave(const FString& Wave);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "JoinServer"))
		void EventJoinServerID(const FString &ID);

	UFUNCTION(BlueprintCallable, Category = HUD)
		FVector2D GetTextSize(FString str, FSlateFontInfo Font);

	AOrionWeather* TheSun;
	bool bHideWeapons;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
		TArray<FAnimTester> AnimationTests;

	//UFUNCTION(exec)
	//	void TestSkillTree() {EventOpenSkillTree();}

	void OpenSkillTree() { /*EventOpenSkillTree();*/ }

	UPROPERTY(BlueprintReadWrite, Category = Movement)
		bool bMenuOpen;

	UFUNCTION(exec)
		void ReadFriends();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ToggleSkillTree"))
		void EventOpenSkillTree();

	//
	UFUNCTION(exec)
		void SpawnSkeletalActor(FName Type, int32 Index);
#if WITH_CHEATS
	UFUNCTION(exec)
		void ChangeDifficulty(int32 Index);

	UFUNCTION(Reliable, server, WithValidation)
	void ServerChangeDifficulty(int32 Index);
	bool ServerChangeDifficulty_Validate(int32 Index) { return true; }
	void ServerChangeDifficulty_Implementation(int32 Index);

	UFUNCTION(exec)
		void IceAge();

	UFUNCTION(Reliable, server, WithValidation)
		void ServerIceAge();
		bool ServerIceAge_Validate() { return true; }
		void ServerIceAge_Implementation();
#endif

	//server hacks for now
	UFUNCTION(exec)
		void SlowMotion(float Value);

	UFUNCTION(Reliable, server, WithValidation)
		void ServerSlowMotion(float Value);
		bool ServerSlowMotion_Validate(float Value) { return true; }
		void ServerSlowMotion_Implementation(float Value) { GetWorldSettings()->TimeDilation =  Value; }

	UFUNCTION(Reliable, client)
		void PlaySlowMotionSound(float Length);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "PlaySlowMotion"))
		void EventPlaySlowMotionSound(float Length);

	UFUNCTION(exec)
		void ToggleHUD();

	UPROPERTY(BlueprintReadOnly, Category = HUD)
		bool bToggleHUD;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ToggleHUD"))
		void EventToggleHUD();

	UFUNCTION(client, reliable)
		void ClientItemAddedToInventory(FInventoryItem Item, bool bSuccess);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ItemAddedToInventory"))
		void EventItemAddedToInventory(FInventoryItem Item, bool bSuccess);

	void UpdateOrbEffects();

	UFUNCTION(BlueprintCallable, Category = ORB)
		bool HasOrbEffect(EOrbType Type);

	FTimerHandle OrbTimer;

	//0 = assault, 1 = support, 2 = recon, 3 = tech
	UFUNCTION(exec)
		virtual void ChangeClass(int32 index);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ChangeClass"))
		void EventChangeClass(int32 index);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ServerGetSkills"))
		void EventServerGetSkillTreeInfo();

	float LastLobbyTime;

	UPROPERTY(BlueprintReadWrite, Category = Playfab)
		FPhotonServerInfo ServerInfo;

	void CreateServerRoom();

	UFUNCTION(client, reliable)
		void ClientSetLastCharacterID(const FString &newID);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "SetLastCharID"))
		void EventSetLastCharacterID();

	UFUNCTION(BlueprintCallable, Category = UI)
		void ResetControllerLayout();
	UFUNCTION(BlueprintCallable, Category = UI)
		void ResetKeyboardLayout();

	UPROPERTY(BlueprintReadWrite, Category = PHOTON)
		bool bPhotonReady;

	UFUNCTION(client, reliable)
		void CreateInGameLobby(FPhotonServerInfo Info);

	UFUNCTION(client, reliable)
		void SetServerInfo(FPhotonServerInfo Info);

	UFUNCTION(client, reliable)
		void UnlockAchievement(const FString &Header, const FString &Body);

	UFUNCTION(client, reliable)
		void PlayLevelUpEffect(int32 NewLevel);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ShowLevelUpEffect"))
		void EventPlayLevelUpEffect(int32 NewLevel);

	UFUNCTION(client, reliable)
		void ShowLevelUpMessage(int32 NewLevel);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ShowLevelUpMessage"))
		void EventShowLevelUpMessage(int32 NewLevel);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ShowNotification"))
		void EventShowNotification(const FString &Header, const FString &Body);

	UFUNCTION(exec)
		void TestLobby();

	UFUNCTION(exec)
		virtual void HideWeapons();

	UFUNCTION(exec)
		virtual void ShowWeapons();
#if WITH_CHEATS
	UFUNCTION(exec)
		virtual void PerfectDay();

	UFUNCTION(exec)
		virtual void HeavyRain();

	UFUNCTION(exec)
		virtual void ClearNight();

	UFUNCTION(Reliable, server, WithValidation)
		void ServerSetWeather(int32 index);
		bool ServerSetWeather_Validate(int32 index) { return true; }
		void ServerSetWeather_Implementation(int32 index);

	UFUNCTION(exec)
		virtual void AllArmor(int32 index);

	UFUNCTION(exec)
		virtual void ArmorColor(int32 index);

	UFUNCTION(exec)
		void ChangeCamera(int32 TeamIndex);
#endif

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Give Default Inventory"))
		void EventGiveDefaultInventory();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Open Inventory Screen"))
		void EventOpenInventoryScreen();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Update Chat Window"))
		void EventUpdateChatWindows();

	//try to start this quest
	UFUNCTION(BlueprintCallable, Category = Quests)
		bool AddNewQuest(AOrionQuest *newQuest);

	//called when a player finishes a quest
	UFUNCTION(BlueprintCallable, Category = Quests)
		void CompleteQuest(AOrionQuest *questToComplete);

	//retrieve a list of quests for this player
	UFUNCTION(BlueprintCallable, Category = Quests)
		TArray<AOrionQuest*> GetAllQuests();

	//retrieve our quest info and set it for the game to use
	void InitQuestManager();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = PlayFab)
		TSubclassOf<class AOrionStats> StatsClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = PlayFab)
		TSubclassOf<class AOrionAchievements> AchievementsClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = PlayFab)
		TSubclassOf<class AOrionInventoryManager> InventoryManagerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = RPG)
		TSubclassOf<class AOrionGrenade> FragGrenadeClass;
	
	//stats and achievements
	UFUNCTION(BlueprintCallable, Category = PlayFab)
		void InitStatsAndAchievements();

	//read in the stats from playfab
	void ReadStats();

	//only the server can actually save stats
	void SaveStats();

	AOrionDroidPawn *DroidBuddy;

	TArray<FNotificationHelper> Notifications;
	void ProcessNotifications();
	float LastNotificationTime;

	UFUNCTION(BlueprintCallable, Category = PlayFab)
		void AttemptLogin(FString UserName, FString Password);

	UFUNCTION(BlueprintCallable, Category = PlayFab)
		void SendChatMessage(FString Channel, FString Message);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "LoginComplete"))
		void EventLoginComplete(const bool bResult, const FString &Msg);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "SetCharacterDatas"))
		void EventSetCharacterDatas(const TArray<FCharacterData> &Data);

	UFUNCTION(BlueprintCallable, Category = PlayFab)
		void CreateNewAccount(FString UserName, FString Password, FString Email);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "AccountCreateComplete"))
		void EventAccountCreationComplete(const bool bResult, const FString &Error);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "AccountCreated"))
		void EventAccountCreated(const bool bResult, const FString &Error);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "CharacterFinalized"))
		void EventCharacterFinalized(const bool bResult, const FString &Error);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ReceiveErrorMessage"))
		void ReceiveErrorMessage(const FString &Message);

	UFUNCTION(BlueprintCallable, Category = PlayFab)
		void CreateNewCharacter(FString UserName, FString Gender, FString BaseColor, FString CharacterClass);

	UFUNCTION(BlueprintCallable, Category = PlayFab)
		void DeleteCharacter(FString CharacterID);

	UFUNCTION(BlueprintCallable, Category = PlayFab)
		void SelectCharacterAtIndex(int32 Index);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "SelectCharacterComplete"))
		void EventSelectCharacterComplete(const bool bResult, const FString &Error);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "CharacterCreateComplete"))
		void EventCharacterCreationComplete(const bool bResult, const FString &Error);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "CharacterDeleteComplete"))
		void EventCharacterDeletionComplete(const bool bResult);

	void PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel) override;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "CleanupUMG"))
		void EventCleanupUMG();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Respawn"))
		void EventRespawn();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "RedrawInventory"))
		void EventRedrawInventory();

	UFUNCTION(BlueprintCallable, Category = Inventory)
		AOrionInventoryManager *GetInventoryManager();

	UFUNCTION(BlueprintCallable, Category = Chat)
		AOrionChatManager *GetChatManager();

	UFUNCTION(BlueprintCallable, Category = Respawn)
		void SetDropPod(AOrionDropPod *Pod);

	UFUNCTION(BlueprintCallable, Category = Menu)
		TArray<FOptionsData> GetGameplayOptions();

	UFUNCTION(BlueprintCallable, Category = Menu)
		TArray<FOptionsData> GetVideoOptions(bool Basic);

	UFUNCTION(BlueprintCallable, Category = Menu)
		TArray<FOptionsValueData> GetSoundOptions();

	void ClientPlayForceFeedback_Implementation(class UForceFeedbackEffect* ForceFeedbackEffect, bool bLooping, FName Tag) override;

	UFUNCTION(BlueprintCallable, Category = Menu)
		TArray<FKeyboardOptionsData> GetKeyboardOptions();

	UFUNCTION(BlueprintCallable, Category = Menu)
		TArray<FOptionsValueData> GetControllerSensitivities();

	UFUNCTION(BlueprintCallable, Category = Menu)
		TArray<FOptionsValueData> GetKeyboardInverted();

	UFUNCTION(BlueprintCallable, Category = Menu)
		TArray<FControllerOptionsData> GetControllerOptions();

	UFUNCTION(BlueprintCallable, Category = Menu)
		FString GetBuildVersion();

	void AddXP(int32 Value);
	void DoLevelUp(int32 NewLevel);

	EControllerButton ConvertControllerButtonToIndex(FString ButtonName);

	UFUNCTION(BlueprintCallable, Category = Menu)
		TArray<FString> GetDifficultySettings();

	UFUNCTION(BlueprintCallable, Category = Menu)
		TArray<FString> GetMaps();

	UFUNCTION(BlueprintCallable, Category = Menu)
		TArray<FString> GetGameModeSettings();

	UFUNCTION(BlueprintCallable, Category = Menu)
		TArray<FString> GetPrivacySettings();

	UFUNCTION(BlueprintCallable, Category = Menu)
		TArray<FString> GetCharacters();

	UFUNCTION(BlueprintCallable, Category = Menu)
		TArray<FOptionsData> GetCreateCharacterOptions();

	UFUNCTION(BlueprintCallable, Category = Menu)
		FOptionsValueData GetMouseSensitivity();

	UFUNCTION(BlueprintCallable, Category = Menu)
		FOptionsValueData GetMouseReverse();

	UFUNCTION(BlueprintCallable, Category = Menu)
		FOptionsValueData GetMouseSmooth();

	UPROPERTY(BlueprintReadWrite, Category = PlayFab)
		TArray<FCharacterData> CharacterDatas;

	UPROPERTY()
		AOrionDropPod *DropPod;

	UFUNCTION(BlueprintCallable, Category = Photon)
		void OpenLobby(FString MapName, FString MapDifficulty, FString Gamemode, FString Privacy, FString TOD, FString ItemLevel);

	UFUNCTION(BlueprintCallable, Category = Photon)
		void LeaveLobby();

	UFUNCTION(BlueprintCallable, Category = Button)
		FString GetReviveButtonKeyboard();

	UFUNCTION(BlueprintCallable, Category = Button)
		int32 GetReviveButtonController();

	UFUNCTION(BlueprintCallable, Category = Button)
		FString GetMeleeButtonKeyboard();

	UFUNCTION(BlueprintCallable, Category = Button)
		int32 GetMeleeButtonController();

	UFUNCTION(BlueprintCallable, Category = Playfab)
		int32 GetMaxLevel();

	//UFUNCTION(BlueprintCallable, Category = Loading)
	//	void PlayLoadingScreen(UUserWidget *Widget);

	float LastCameraToggleTime;

	bool bLobbyLeader;

	void HandleGUID(FString GUID);

	UPROPERTY(BlueprintReadOnly, Category = PlayFab)
		int32 ILevel;

	UFUNCTION(client, reliable)
		void ClientSetItemLevel(int32 Level);

	UFUNCTION(Server, reliable, WithValidation)
		void ServerSendGUID(const FString &ID);
		bool ServerSendGUID_Validate(const FString &ID) { return true; }
		void ServerSendGUID_Implementation(const FString &ID);

	UFUNCTION(BlueprintImplementableEvent, Category = Photon)
		void UpdateLobbySettings(const FString& MapName, const FString& Difficulty, const FString& Gamemode, const FString& Privacy, const FString& IP, const FString& Ticket, const FString& Progress, const FString& Version, const FString &GUID, const FString& RoomName, const FString& TOD, const FString& ItemLevel);

	UFUNCTION(BlueprintCallable, Category = Photon)
		void FlushLobbySettings(FString MapName, FString Difficulty, FString Gamemode, FString Privacy, FString IP, FString Ticket, FString Wave, FString Version, FString RoomName, FString TOD, FString ItemLevel);

	void JoinChatRoom(FString Room);

	//update our view to our saved option
	UFUNCTION(BlueprintCallable, Category = FPS)
		void SetThirdPerson();

	UFUNCTION(BlueprintCallable, Category = Photon)
		bool IsLobbyLeader();

	void GetAudioListenerPosition(FVector& OutLocation, FVector& OutFrontDir, FVector& OutRightDir) override;

	UFUNCTION(client, reliable)
		void ClientSetDeathSpectate(APawn *DeadPawn);

	UFUNCTION(client, reliable)
		void ClientAddDamageNumber(int32 Damage, FVector Pos, bool bCrit);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "SpawnDamageNumber"))
		void EventAddDamageNumber(int32 Damage, FVector Pos, bool bCrit);

	void AddDamageNumber(int32 Damage, FVector Pos, bool bCrit);

	//pointer to ragdoll for camera when we die
	AOrionCharacter *Ragdoll;

	UPROPERTY(BlueprintReadWrite, Category = Camera)
		bool bThirdPersonCamera;

	UFUNCTION(exec)
		void ToggleThirdPerson();

	UFUNCTION(client, reliable)
		void ClientAddXPNumber(int32 Damage, FVector Pos);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "SpawnXPNumber"))
		void EventAddXPNumber(int32 Damage, FVector Pos);

	UFUNCTION(client, reliable)
		void ClientAddCoinAmount(int32 Amount);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "SpawnCoinAmount"))
		void EventAddCoinAmount(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = Coins)
		void AddCoinAmount(int32 Amount);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "GlobalMessage"))
		void ShowGlobalMessage(const FString &Msg);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "GlobalMessageHeader"))
		void ShowGlobalMessageHeader(const FString &Title, const FString &Desc);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ShowScoreScreen"))
		void ShowScoreScreen(bool bShow);

	void TryToCreateLobbyAgain();
	void CreateLobbyForReal();

	void ShowScores();
	void HideScores();

	void ShowCharacterSelect();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Show Character Select"))
		void EventShowCharacterSelect();

	void PlayShieldEffect(bool bFull);

	UFUNCTION(client, reliable)
		void ClientPlayShieldEffect(bool bFull);

	void PlayHUDHit();

	UFUNCTION(client, reliable)
		void ClientPlayHUDHit();

	void AddXPNumber(int32 Damage, FVector Pos);

	UPROPERTY(BlueprintReadWrite, Category = MainMenu)
		UCameraComponent *MenuCamera;

	UPROPERTY(BlueprintReadWrite, Category = MainMenu)
		UCameraComponent *OverviewCamera;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ResizeHUD"))
		void EventResizeHUD();

	int32 OldViewportSizeX;
	int32 OldViewportSizeY;

	void PawnPendingDestroy(APawn* P) override;
	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;

	void CleanupGameViewport() override;
	void SeamlessTravelTo(class APlayerController* NewPC) override;
	void SeamlessTravelFrom(class APlayerController* OldPC) override;

//#if WITH_CHEATS
	UFUNCTION(exec)
		void TestLevel();
//#endif

	//UFUNCTION(exec)
		virtual void ClearUMG();
#if WITH_CHEATS
	UFUNCTION(exec)
		void TestSettings();

	UFUNCTION(exec)
		void SpawnWave();
#endif

	bool bSpawnHax;

	//delegates for steam friends stuff
	void OnInviteAccepted(const bool bWasSuccessful, const int32 LocalUserNum, TSharedPtr<const FUniqueNetId> FriendID, const FOnlineSessionSearchResult &SessionToJoin);
	void OnInviteReceived(const FUniqueNetId& UserId, const FUniqueNetId& FromId, const FString& AppId, const FOnlineSessionSearchResult& InviteResult);
	void OnSessionCreated(FName SessionName, bool bSuccessful);
	void OnJoinGame(int32 LocalUserNum, bool bWasSuccessful, const FOnlineSessionSearchResult& SearchResult);
	void OnFindSession(int32 LocalUserNum, bool bWasSuccessful, const FOnlineSessionSearchResult& SearchResult);

	void OnFriendInviteAccepted(const FUniqueNetId& UserID, const FUniqueNetId& FriendID);
	void OnAcceptFriendInvite(int32 UserNum, bool bWasSuccessful, const FUniqueNetId& FriendId, const FString& ListName, const FString& ErrorStr);
	void OnJoinSessionCompleted(int32 LocalUserNum, bool bWasSuccessful, const FOnlineSessionSearchResult& SearchResult);

	void CreateLobbySession(FString RoomName);
	void DestroyLobbySession();

	UFUNCTION(BlueprintCallable, Category = Raptor)
		void DoPounceTutorial();

	void SetLobbyName(FString lName);
	FString LobbyName;

	UFUNCTION(BlueprintCallable, Category = Steam)
		void InviteFriendToLobby(FString FriendSteamID);

	UFUNCTION(BlueprintCallable, Category = Steam)
		void SetPresenceInfo(FString LobbyID);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ReceiveChat"))
		void EventAddChatMessage(const FString &msg);

	UFUNCTION(Reliable, client)
		void ClientReceiveChatMessage(const FString &msg);

	UFUNCTION(BlueprintCallable, Category = Chat)
		void OrionSendChatMessage(const FString &msg);

	UFUNCTION(Reliable, server, WithValidation)
		void ServerSetThirdPersonCamera(bool bOn);
		bool ServerSetThirdPersonCamera_Validate(bool bOn) { return true; }
		void ServerSetThirdPersonCamera_Implementation(bool bOn);

	UFUNCTION(Reliable, server, WithValidation)
		void ServerSendChatMessage(const FString &msg);
		bool ServerSendChatMessage_Validate(const FString &msg) { return true; }
		void ServerSendChatMessage_Implementation(const FString &msg);

	UFUNCTION(Reliable, server, WithValidation)
		void ServerSetDaveyCam(bool bOn);
		bool ServerSetDaveyCam_Validate(bool bOn) { return true; }
		void ServerSetDaveyCam_Implementation(bool bOn) { bDaveyCam = bOn; }

	UFUNCTION(exec)
		void DaveyCam();

	bool bDaveyCam;

	virtual void Destroyed() override;

	UFUNCTION(Blueprintcallable, Category = Playfab)
		void ConnectToIP(FString IP);

	UFUNCTION(Blueprintcallable, Category = Playfab)
		void StartSoloMap(FString MapName);

	UFUNCTION(Blueprintcallable, Category = Audio)
		void SaveSoundOptions(FString ClassName, float Volume);

	void UpdateRotation(float DeltaTime) override;
#if WITH_CHEATS
	UFUNCTION(Reliable, server, WithValidation)
		void ServerAllArmor(int32 index);
		bool ServerAllArmor_Validate(int32 index);
		void ServerAllArmor_Implementation(int32 index);
#endif
	UPROPERTY(BlueprintReadWrite, Category = Playfab)
		int32 ClassIndex;

	UPROPERTY(BlueprintReadWrite, Category = Lobby)
		TArray<FLobbyPlayer> LobbyPlayers;

	void ResetLobbyPlayers() { LobbyPlayers.Empty(); }
	void AddLobbyPlayer(int32 pNumber, FString pName, FString pClass, FString pLevel);
	
	UFUNCTION(BlueprintImplementableEvent, Category = Camera)
		void EventHUDSpawn(bool bStart);

	//redraw the lobby player info
	UFUNCTION(BlueprintImplementableEvent, Category = Lobby)
		void FlushLobbyPlayers();

	UFUNCTION(BlueprintImplementableEvent, Category = Lobby)
		void JoinLobbyFailed(int32 ErrorCode, const FString &Reason);

	UFUNCTION(BlueprintImplementableEvent, Category = Lobby)
		void LobbyCreationFailed(int32 ErrorCode, const FString &Reason);

	UFUNCTION(BlueprintImplementableEvent, Category = Lobby)
		void EventJoinLobbySuccess();

	UFUNCTION(BlueprintImplementableEvent, Category = Spawn)
		void EventBlackFade();

	UFUNCTION(BlueprintCallable, Category = Options)
		void ResetInput();

	UPROPERTY(BlueprintReadOnly, Category = Lobby)
		int32 LobbyPlayerNumber;

	void JoinLobbySuccess(int32 PlayerNumber);

	UFUNCTION(BlueprintCallable, Category = Photon)
		void SendPlayerInfoToPhoton();

	UFUNCTION(BlueprintCallable, Category = Photon)
		void RefreshLobbyList();

	UFUNCTION(BlueprintImplementableEvent, Category = Photon)
		void EventUpdateLobbyList();

	UFUNCTION(BlueprintCallable, Category = Photon)
		void JoinLobby(FString ServerName);

	//list of visible lobbies for us to choose from
	UPROPERTY(BlueprintReadWrite, Category = Photon)
		TArray<FLobbyData> Lobbies;

	//list of in game servers
	UPROPERTY(BlueprintReadWrite, Category = Photon)
		TArray<FLobbyData> GameServers;

	void TickPhoton();

public:
	UFUNCTION(BlueprintCallable, Category = PlayFab)
		int32 GetClassIndex();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Skill)
		TSubclassOf<UOrionSkillTree> SkillTrees;

	UPROPERTY(BlueprintReadWrite, Category = Skill)
		UOrionSkillTree *MySkillTree;

	UFUNCTION(BlueprintCallable, Category = Skill)
		void SetSkillTreeValuesForUse();

	void SetDefaultSkills();

	UFUNCTION(server, reliable, WithValidation)
		void ServerSetSkillTreeValuesForUse();
		bool ServerSetSkillTreeValuesForUse_Validate() { return true; }
		void ServerSetSkillTreeValuesForUse_Implementation();

	UPROPERTY(BlueprintReadWrite, Category = Skill)
		TArray<FUnlockedSkills> CharacterSkills;

	UFUNCTION(client, reliable)
		void ClientSetCharacterSkills(const TArray<FUnlockedSkills> &Skills);

	UFUNCTION(BlueprintCallable, Category = Skill)
		int32 GetSkillValue(ESkillTreeUnlocks Skill);

	virtual void BeginPlay();
	virtual void StartFire(uint8 FireModeNum);

	UFUNCTION(BlueprintCallable, Category = Stats)
		AOrionStats *GetStats() { return Stats; }

	UFUNCTION(BlueprintCallable, Category = Achievements)
		AOrionAchievements *GetAchievements() { return Achievements; }

	UPROPERTY(BlueprintReadOnly, Category = PlayFab)
		bool bAuthenticated;

	float LastTRexKill;
	float LastNamorKill;
	bool bHereFromStart;

	float LastGrenadeKillTime;
	int32 GrenadeKills;

	UFUNCTION(client, reliable)
		void ClientSetAuthed(bool bAuthed);

	UFUNCTION(BlueprintCallable, Category = PlayFab)
		void SetCharacterClass(int32 Index, FString CharID);

	UFUNCTION(server, reliable, WithValidation)
		void ServerSetCharacterClass(int32 Index, const FString &CharID);
		bool ServerSetCharacterClass_Validate(int32 Index, const FString &CharID);
		void ServerSetCharacterClass_Implementation(int32 Index, const FString &CharID);

	int32 NextSpawnClass;
	FString NextSpawnID;

	float LastTutorialTime;
	void TickGenericTutorials();
	FTimerHandle TutorialTimer;
	FString FormatTutorial(FString Desc);

	UFUNCTION(client, reliable)
		void ClientSendTutorialMessage(const FString &Title, const FString &Desc);

	bool InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad) override;

	void SendTutorialMessage(FString Title, FString Desc);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OrbMessage"))
		void EventSetOrbMessage(const FString &Message, EOrbType Type);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ShowTutorial"))
		void EventShowTutorial(const FString &Title, const FString &Desc);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "SelectWeapon"))
		void EventSelectWeapon(int32 index, const FString &WeaponName);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "AuthenticatePF"))
		void EventValidateSession(const FString &Session);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "AuthenticateTicket"))
		void EventValidateTicket(const FString &LobbyID, const FString &Ticket);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OrionKeyPressed"))
		void EventPressKey(FKey Key, bool Gamepad);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OrionChat"))
		void EventSay();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OrionTeamChat"))
		void EventTeamSay();

	void ValidatePlayFabInfo(FString pfID, FString pfSession);
	void ValidateLobbyTicket(FString LobbyID, FString pfTicket);

	UFUNCTION(Reliable, server, WithValidation)
		void ServerSetPlayFabInfo(const FString &ID, const FString &SessionID, const FString &cID);
		bool ServerSetPlayFabInfo_Validate(const FString &ID, const FString &SessionID, const FString &cID);
		void ServerSetPlayFabInfo_Implementation(const FString &ID, const FString &SessionID, const FString &cID);

	UPROPERTY()//VisibleAnywhere, BlueprintReadOnly, Category = Rain)
		UParticleSystemComponent *RainPSC;

	std::map<std::string,std::string> GetInventoryData();
	void PopulateInventory(TSharedPtr<FJsonObject> Data);
#if IS_SERVER
	////void PopulateInventory(std::map<std::string, PlayFab::ServerModels::UserDataRecord> Data);
#else
#endif

private:
	//for animation testing
	class ASkeletalMeshActor *TestActor;
	class ASkeletalMeshActor *TestActor2;

	FTimerHandle ServerTickTimer;
	UOrionQuestManager *QuestManager;
	AOrionStats *Stats;
	AOrionAchievements *Achievements;

	AOrionChatManager *ChatManager;

	bool CreateAndGiveInventoryItem(TSharedPtr<FJsonObject> Data, AOrionInventoryGrid *theGrid, FString Slot, int32 Index);

public:
	UPROPERTY(BlueprintReadWrite, Category = Inventory)
		TArray<FInventoryMapping> InventoryMapppings;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "CreateHealthBar"))
		void EventCreateHealthBar(class AOrionCharacter *PawnOwner);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Take Hit"))
		void EventTakeHit(bool bFull);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Take HUD Hit"))
		void EventPlayHUDHit();

	UFUNCTION()
		void ServerTick();
};
