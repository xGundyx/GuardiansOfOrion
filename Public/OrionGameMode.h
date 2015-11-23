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

	virtual void AddSpawn(AOrionCharacter* P) {}

	virtual void SetSpawnTimer() {}

	void AddChatMessage(const FString &msg);

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

	void AwardXPToAllPlayers(int32 Amount);

	UPROPERTY(BlueprintReadWrite, Category = Gameplay)
		TEnumAsByte<EGameDifficulty> Difficulty;

	UPROPERTY(BlueprintReadWrite, Category = Gameplay)
		FString LobbyID;

	UPROPERTY(BlueprintReadWrite, Category = Gameplay)
		FPhotonServerInfo ServerInfo;

	void TickExitTimer();
	int32 ExitCounter;
	FTimerHandle ExitTimer;

	enum EStatID GetStatID(AController *KilledController, bool bVictim);
	void HandleStats(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes)
		TSubclassOf<class AOrionPickup>  DefaultPickupClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes)
		TSubclassOf<class AOrionPickupOrb>  DefaultPickupOrbClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes)
		TSubclassOf<class AOrionMusicManager>  DefaultMusicClass;

	UPROPERTY(BlueprintReadOnly, Category = Classes)
		bool bAlwaysShowCursor;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetShip(AOrionShipPawn *Ship, int32 Index){}

	UPROPERTY(BlueprintReadOnly, Category = Game)
		bool bTeamGame;

	UPROPERTY(BlueprintReadOnly, Category = Game)
		bool bTopDown;

	float ModifyDamage(float Damage, AOrionCharacter *PawnToDamage, struct FDamageEvent const& DamageEvent, class AController *EventInstigator, class AActor *DamageCauser);

	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	void InitGRI();
	void WarmupOver();
	void StartMatch() override;

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

	//how many seconds does a match last
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Spawn)
		int32 TimeLimit;

	UPROPERTY(BlueprintReadOnly, Category = Spawn)
		bool bWarmingUp;

	//UFUNCTION(BlueprintNativeEvent, Category = "Game")
		virtual bool ReadyToStartMatch_Implementation() override;

private:
	FTimerHandle GRITimer;
	FTimerHandle WarmupTimer;
	FTimerHandle MatchTimer;
};



