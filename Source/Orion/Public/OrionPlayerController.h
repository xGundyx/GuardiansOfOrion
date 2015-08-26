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
#include "OrionPlayFabInventoryMapper.h"
#include "OrionPlayerController.generated.h"

/**
*
*/

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

	AOrionWeather* TheSun;
	bool bHideWeapons;

	//server hacks for now
	UFUNCTION(exec)
		void SlowMotion(float Value) { ServerSlowMotion(Value); }

	UFUNCTION(Reliable, server, WithValidation)
		void ServerSlowMotion(float Value);
		bool ServerSlowMotion_Validate(float Value) { return true; }
		void ServerSlowMotion_Implementation(float Value) { ConsoleCommand(FString::Printf(TEXT("slomo %f"), Value)); }

	UFUNCTION(exec)
		void ToggleHUD();

	UPROPERTY(BlueprintReadOnly, Category = HUD)
		bool bToggleHUD;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ToggleHUD"))
		void EventToggleHUD();

	//0 = assault, 1 = support, 2 = recon
	UFUNCTION(exec)
		virtual void ChangeClass(int32 index);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ChangeClass"))
		void EventChangeClass(int32 index);

	UFUNCTION(exec)
		virtual void HideWeapons();

	UFUNCTION(exec)
		virtual void ShowWeapons();

	UFUNCTION(exec)
		virtual void PerfectDay();

	UFUNCTION(exec)
		virtual void HeavyRain();

	UFUNCTION(exec)
		virtual void ClearNight();

	UFUNCTION(exec)
		virtual void AllArmor(int32 index);

	UFUNCTION(exec)
		virtual void ArmorColor(int32 index);

	UFUNCTION(exec)
		void ChangeCamera(int32 TeamIndex);

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
	
	//stats and achievements
	UFUNCTION(BlueprintCallable, Category = PlayFab)
		void InitStatsAndAchievements();

	//read in the stats from playfab
	void ReadStats();

	//only the server can actually save stats
	void SaveStats();

	void SetStatValue(FStatID id, int32 iAmount, int32 fAmount);
	void IncreaseStatValue(FStatID id, int32 iAmount, int32 fAmount);

	AOrionDroidPawn *DroidBuddy;

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

	UFUNCTION(BlueprintCallable, Category = Menu)
		TArray<FKeyboardOptionsData> GetKeyboardOptions();

	UFUNCTION(BlueprintCallable, Category = Menu)
		TArray<FControllerOptionsData> GetControllerOptions();

	EControllerButton ConvertControllerButtonToIndex(FString ButtonName);

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

	void GetAudioListenerPosition(FVector& OutLocation, FVector& OutFrontDir, FVector& OutRightDir) override;

	UFUNCTION(client, reliable)
		void ClientAddDamageNumber(int32 Damage, FVector Pos);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "SpawnDamageNumber"))
		void EventAddDamageNumber(int32 Damage, FVector Pos);

	void AddDamageNumber(int32 Damage, FVector Pos);

	//pointer to ragdoll for camera when we die
	AOrionCharacter *Ragdoll;

	UFUNCTION(client, reliable)
		void ClientAddXPNumber(int32 Damage, FVector Pos);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "SpawnXPNumber"))
		void EventAddXPNumber(int32 Damage, FVector Pos);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "VictoryMessage"))
		void ShowVictoryMessage();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "DefeatMessage"))
		void ShowDefeatedMessage();

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

	//UFUNCTION(exec)
		virtual void ClearUMG();

	UFUNCTION(exec)
		void TestSettings();

	UFUNCTION(exec)
		void SpawnWave();

	UFUNCTION(exec)
		void DaveyCam();

	UFUNCTION(Reliable, server, WithValidation)
		void ServerSetDaveyCam(bool bOn);
		bool ServerSetDaveyCam_Validate(bool bOn) { return true; }
		void ServerSetDaveyCam_Implementation(bool bOn) { bDaveyCam = bOn; }

	bool bDaveyCam;

	virtual void Destroyed() override;

	UFUNCTION(Blueprintcallable, Category = Playfab)
		void ConnectToIP(FString IP);

	void UpdateRotation(float DeltaTime) override;

	UFUNCTION(Reliable, server, WithValidation)
		void ServerAllArmor(int32 index);
		bool ServerAllArmor_Validate(int32 index);
		void ServerAllArmor_Implementation(int32 index);

	UPROPERTY(BlueprintReadWrite, Category = Playfab)
		int32 ClassIndex;

public:
	virtual void BeginPlay();
	virtual void StartFire(uint8 FireModeNum);

	bool bAuthenticated;

	bool InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad) override;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "AuthenticatePF"))
		void EventValidateSession(const FString &Session);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OrionKeyPressed"))
		void EventPressKey(FKey Key, bool Gamepad);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OrionChat"))
		void EventSay();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OrionTeamChat"))
		void EventTeamSay();

	void ValidatePlayFabInfo(FString pfID, FString pfSession);

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
	FTimerHandle ServerTickTimer;
	UOrionQuestManager *QuestManager;
	UOrionStats *Stats;
	UOrionAchievements *Achievements;

	AOrionChatManager *ChatManager;

	bool CreateAndGiveInventoryItem(TSharedPtr<FJsonObject> Data, AOrionInventoryGrid *theGrid, FString Slot, int32 Index);

public:
	UPROPERTY(BlueprintReadWrite, Category = Inventory)
		TArray<FInventoryMapping> InventoryMapppings;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "CreateHealthBar"))
		void EventCreateHealthBar(class AOrionCharacter *PawnOwner);

	UFUNCTION()
		void ServerTick();
};
