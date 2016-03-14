// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "OrionPickup.h"
#include "OrionShipPawn.h"
#include "OrionOutOfBoundsVolume.h"
#include "OrionGameMode.generated.h"

UCLASS()
class ORION_API AOrionGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	AOrionGameMode(const FObjectInitializer& ObjectInitializer);

	virtual void Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType);

	//virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole RemoteRole, const FString& Portal, const FString& Options, const TSharedPtr<FUniqueNetId>& UniqueId, FString& ErrorMessage) override;
	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const TSharedPtr<const FUniqueNetId>& UniqueId, FString& ErrorMessage) override;
	virtual void PreLogin(const FString& Options, const FString& Address, const TSharedPtr<const FUniqueNetId>& UniqueId, FString& ErrorMessage); 
	virtual void Logout(AController* Exiting) override;
	virtual void SetInitialTeam(APlayerController *PC);
	virtual FString InitNewPlayer(class APlayerController* NewPlayerController, const TSharedPtr<const FUniqueNetId>& UniqueId, const FString& Options, const FString& Portal = TEXT(""));

	void CloseServer();

	virtual void AddSpawn(AOrionCharacter* P) {}

	void CloseGame();

	FString Player1;
	FString Player2;
	FString Player3;
	FString Player4;

	bool bLocalCheck;
	void CheckLocal();

	virtual void HandleEmptyServer(AController *Exiting = nullptr);

	virtual void SetSpawnTimer() {}

	virtual void AddChatMessage(const FString &msg, bool bTeamMsg, const FString &PartyName);

	float DinoKillZ;

	UFUNCTION(BlueprintCallable, Category = Auth)
		void PlayerAuthed(class AOrionPlayerController *PC, bool bSuccess);

	void SpawnItems(AController *Killer, AActor *Spawner, const UDamageType* DamageType);
	void SaveAllUsersStats();

	void HandleOutOfBounds();
	AOrionOutOfBoundsVolume *OOBVolume;
	FTimerHandle OOBTimer;

	//keep track of the enemies we have yet to spawn
	int32 SpawnTypes[SPAWN_NUM];

	void AwardXPToAllPlayers(int32 Amount, AOrionPlayerController *PC, const UOrionDamageType *DamageType);

	virtual int32 GetEnemyItemLevel(bool bAdjusted, bool bHalved = false);

	UPROPERTY(BlueprintReadWrite, Category = Gameplay)
		TEnumAsByte<EGameDifficulty> Difficulty;

	UPROPERTY(BlueprintReadWrite, Category = Gameplay)
		FString LobbyID;

	UPROPERTY(BlueprintReadWrite, Category = Gameplay)
		FPhotonServerInfo ServerInfo;

	void TickExitTimer();
	int32 ExitCounter;
	FTimerHandle ExitTimer;

	virtual void PlayerIsReady(AOrionPlayerController *PC) {}

	enum EStatID GetStatID(AController *KilledController, bool bVictim);
	void HandleStats(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes)
		TSubclassOf<class AOrionPickup>  DefaultPickupClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes)
		TSubclassOf<class AOrionPickupOrb>  DefaultPickupOrbClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes)
		TSubclassOf<class AOrionPickupOrb>  DefaultPickupCoinClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes)
		TSubclassOf<class AOrionMusicManager>  DefaultMusicClass;

	UPROPERTY(BlueprintReadOnly, Category = Classes)
		bool bAlwaysShowCursor;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	bool bSinglePlayer;

	virtual FString GetGameName() {
		return "";
	}

	//determines the level of enemies that will spawn
	UPROPERTY(BlueprintReadOnly, Category = RPG)
		int32 ItemLevel;

	virtual int32 GetOrionNumPlayers() 
	{
		AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GetGameState());

		if (Difficulty == DIFF_REDIKULOUS)
			return 4;

		if (GRI)
			return GRI->PlayerList.Num();

		return 4;
	}

	virtual void SetShip(AOrionShipPawn *Ship, int32 Index){}

	UPROPERTY(BlueprintReadOnly, Category = Game)
		bool bTeamGame;

	UPROPERTY(BlueprintReadOnly, Category = Game)
		bool bTopDown;

	virtual float ModifyDamage(float Damage, AOrionCharacter *PawnToDamage, struct FDamageEvent const& DamageEvent, class AController *EventInstigator, class AActor *DamageCauser);

	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual bool IsSlaughter() { return false; }

	virtual void InitGRI();
	void WarmupOver();
	void StartMatch() override;

	bool bNightTime;

	virtual void SpawnWave() {}

	virtual void AddActiveWaveEnemy(AOrionCharacter *Enemy) {}

	UFUNCTION(BlueprintCallable, Category = Generator)
		virtual void FinishCurrentTask() {};

	//how many seconds to wait for players to join before starting the match
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Spawn)
		int32 WarmupTime;

	bool bLobbyCreated;

	UFUNCTION(BlueprintCallable, Category = Voice)
		virtual void PlayRandomVoiceFromPlayer(EVoiceType Type);

	void AddInactivePlayer(APlayerState* PlayerState, APlayerController* PC) override;

	void PlaySlowMotion(float Length);
	void StopSlowMotion();

	FTimerHandle SlowMotionTimer;

	//how many seconds does a match last
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Spawn)
		int32 TimeLimit;

	UPROPERTY(BlueprintReadOnly, Category = Spawn)
		bool bWarmingUp;

	//UFUNCTION(BlueprintNativeEvent, Category = "Game")
		virtual bool ReadyToStartMatch_Implementation() override;

public:
	FTimerHandle GRITimer;
	FTimerHandle WarmupTimer;
	FTimerHandle MatchTimer;
};



