// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#if IS_SERVER
#include "playfab/PlayFabServerAPI.h"
#else
#include "playfab/PlayFabClientAPI.h"
#endif
#include "OrionWeather.h"
#include "GameFramework/PlayerController.h"
#include "OrionInventoryManager.h"
#include "OrionQuest.h"
#include "OrionQuestManager.h"
#include "OrionStats.h"
#include "OrionAchievements.h"
#include "OrionDroidPawn.h"
#include "OrionDropPod.h"
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
	BUTTON_START
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
struct FCharacterData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Character)
		FString pName;

	UPROPERTY(BlueprintReadOnly, Category = Character)
		FString pClass;

	UPROPERTY(BlueprintReadOnly, Category = Character)
		FString CharacterID;

	UPROPERTY(BlueprintReadOnly, Category = Character)
		FString Sex;

	UPROPERTY(BlueprintReadOnly, Category = Character)
		int32 Level;

	UPROPERTY(BlueprintReadOnly, Category = Character)
		int32 Exp;

	UPROPERTY(BlueprintReadOnly, Category = Character)
		int32 HelmetID;

	UPROPERTY(BlueprintReadOnly, Category = Character)
		int32 ChestID;

	UPROPERTY(BlueprintReadOnly, Category = Character)
		int32 HandsID;

	UPROPERTY(BlueprintReadOnly, Category = Character)
		int32 LegsID;

	UPROPERTY(BlueprintReadOnly, Category = Character)
		int32 Weapon;

	void Reset()
	{
		pName = FString("");
		pClass = FString("");
		CharacterID = FString("0");
		Level = 0;
		Exp = 0;
		HelmetID = 0;
		ChestID = 0;
		HandsID = 0;
		LegsID = 0;
		Sex = FString("");
		Weapon = 0;
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

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "Give Default Inventory"))
		void EventGiveDefaultInventory();

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "Open Inventory Screen"))
		void EventOpenInventoryScreen();

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

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "LoginComplete"))
		void EventLoginComplete(const bool bResult, const FString &Msg);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "SetCharacterDatas"))
		void EventSetCharacterDatas(const TArray<FCharacterData> &Data);

	UFUNCTION(BlueprintCallable, Category = PlayFab)
		void CreateNewAccount(FString UserName, FString Password, FString Email);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "AccountCreateComplete"))
		void EventAccountCreationComplete(const bool bResult, const FString &Error);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "AccountCreated"))
		void EventAccountCreated(const bool bResult, const FString &Error);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "CharacterFinalized"))
		void EventCharacterFinalized(const bool bResult, const FString &Error);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "ReceiveErrorMessage"))
		void ReceiveErrorMessage(const FString &Message);

	UFUNCTION(BlueprintCallable, Category = PlayFab)
		void CreateNewCharacter(FString UserName, FString Gender, FString BaseColor, FString CharacterClass);

	UFUNCTION(BlueprintCallable, Category = PlayFab)
		void DeleteCharacter(FString CharacterID);

	UFUNCTION(BlueprintCallable, Category = PlayFab)
		void SelectCharacterAtIndex(int32 Index);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "SelectCharacterComplete"))
		void EventSelectCharacterComplete(const bool bResult, const FString &Error);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "CharacterCreateComplete"))
		void EventCharacterCreationComplete(const bool bResult, const FString &Error);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "CharacterDeleteComplete"))
		void EventCharacterDeletionComplete(const bool bResult);

	void PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel) override;

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "CleanupUMG"))
		void EventCleanupUMG();

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "Respawn"))
		void EventRespawn();

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "RedrawInventory"))
		void EventRedrawInventory();

	UFUNCTION(BlueprintCallable, Category = Inventory)
		AOrionInventoryManager *GetInventoryManager();

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

	UFUNCTION(BlueprintCallable, Category = Menu)
		TArray<FOptionsData> GetCreateCharacterOptions();

	UFUNCTION(BlueprintCallable, Category = Menu)
		FOptionsValueData GetMouseSensitivity();

	UFUNCTION(BlueprintCallable, Category = Menu)
		FOptionsValueData GetMouseReverse();

	UFUNCTION(BlueprintCallable, Category = Menu)
		FOptionsValueData GetMouseSmooth();

	UPROPERTY()
		AOrionDropPod *DropPod;

	UPROPERTY(BlueprintReadWrite, Category = MainMenu)
		UCameraComponent *MenuCamera;

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "ResizeHUD"))
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

	virtual void Destroyed() override;

	void UpdateRotation(float DeltaTime) override;

	UFUNCTION(Reliable, server, WithValidation)
		void ServerAllArmor(int32 index);
		bool ServerAllArmor_Validate(int32 index);
		void ServerAllArmor_Implementation(int32 index);

public:
	virtual void BeginPlay();
	virtual void StartFire(uint8 FireModeNum);

	UFUNCTION(Reliable, server, WithValidation)
		void ServerSetPlayFabInfo(const FString &ID, const FString &SessionID, const FString &cID);
		bool ServerSetPlayFabInfo_Validate(const FString &ID, const FString &SessionID, const FString &cID);
		void ServerSetPlayFabInfo_Implementation(const FString &ID, const FString &SessionID, const FString &cID);

	UPROPERTY()//VisibleAnywhere, BlueprintReadOnly, Category = Rain)
		UParticleSystemComponent *RainPSC;

	std::map<std::string,std::string> GetInventoryData();
	void PopulateInventory(TSharedPtr<FJsonObject> Data);
#if IS_SERVER
	void PopulateInventory(std::map<std::string, PlayFab::ServerModels::UserDataRecord> Data);
#else
#endif

private:
	UOrionQuestManager *QuestManager;
	UOrionStats *Stats;
	UOrionAchievements *Achievements;

	bool CreateAndGiveInventoryItem(TSharedPtr<FJsonObject> Data, AOrionInventoryGrid *theGrid, FString Slot, int32 Index);
};