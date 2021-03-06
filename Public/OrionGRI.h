

#pragma once

#include "OrionWeather.h"
#include "OrionPRI.h"
#include "GameFramework/GameState.h"
#include "OrionPlayFabInventoryMapper.h"
#include "OrionGRI.generated.h"

USTRUCT(BlueprintType)
struct FMissionInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Mission)
		FString Title;

	UPROPERTY(BlueprintReadOnly, Category = Mission)
		FString Desc;
};

USTRUCT(BlueprintType)
struct FTeamInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		TArray<AOrionPRI*> TeamMembers;

	UPROPERTY()
		int32 TeamIndex;
};

/**
 * 
 */
UCLASS()
class ORION_API AOrionGRI : public AGameState
{
	GENERATED_BODY()
public:
	AOrionGRI(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, Replicated)
		AOrionWeather *Weather;

	UPROPERTY(Transient, Replicated)
		FTimeOfDay WorldTime;

	UPROPERTY(Replicated, BlueprintReadOnly)
		bool bAlwaysShowCursor;

	void SetWeather(AOrionWeather *theWeather);

	void Tick(float DeltaSeconds) override;
	void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = Weather)
		FTimeOfDay GetWorldTime() const;

	UFUNCTION(BlueprintCallable, Category = Game)
		bool IsTopDownGame();

	UPROPERTY(Replicated, BlueprintReadOnly)
		bool bTopDown;

	UFUNCTION(BlueprintCallable, Category = Game)
		bool OnSameTeam(AOrionPRI *Actor1, AOrionPRI *Actor2);

	UFUNCTION(BlueprintCallable, Category = Game)
		FMissionInfo GetMission(int32 Index);

	UPROPERTY(Replicated, BlueprintReadOnly)
		bool bTeamGame;

	UPROPERTY(Replicated, BlueprintReadOnly)
		int32 WarmupTimeRemaining;

	UPROPERTY(Replicated, BlueprintReadOnly)
		int32 MatchTimeRemaining;

	UPROPERTY(Replicated, BlueprintReadOnly)
		TArray<FTeamInfo> Teams;

	UPROPERTY(Replicated, BlueprintReadOnly)
		FString PhotonGUID;

	UPROPERTY(Replicated, BlueprintReadOnly)
		int32 DinosAliveInWave;

	UPROPERTY(Replicated, BlueprintReadOnly)
		bool bNightTime;

	//harvester stats
	UPROPERTY(Replicated, BlueprintReadWrite)
		int32 HarvKills;

	bool HarvHasMostKills();

	UFUNCTION()
		void OnRep_WaveNum();

	UPROPERTY(ReplicatedUsing = OnRep_WaveNum, BlueprintReadOnly)
		int32 WaveNum;

	UPROPERTY(Replicated, BlueprintReadOnly)
		int32 TotalWaves;

	UPROPERTY(Replicated, BlueprintReadOnly)
		FString MissionObjective;

	UPROPERTY(Replicated, BlueprintReadOnly)
		bool bSideMission;

	UPROPERTY(Replicated, BlueprintReadOnly)
		bool bBossMission;

	UPROPERTY(Replicated, BlueprintReadOnly)
		int32 SecondsTillNextSpawn;

	UPROPERTY(Replicated, BlueprintReadOnly)
		int32 SecondsTillNextWave;

	UFUNCTION()
		void HandleVictoryDefeat();

	UPROPERTY(ReplicatedUsing = HandleVictoryDefeat, BlueprintReadOnly)
		FString GameOverMessage;

	UPROPERTY(Replicated, BlueprintReadOnly)
		FVector MissionLocation;

	//list of connected players for easy access
	UPROPERTY(Replicated, BlueprintReadOnly)
		TArray<AOrionPRI*> PlayerList;

	UPROPERTY()
		class AOrionMusicManager *MusicManager;

	UFUNCTION()
		void OnRep_GlobalMessage();

	UPROPERTY(Replicated, BlueprintReadWrite, Category = Game)
		bool bIsLobby;

	UPROPERTY(ReplicatedUsing = OnRep_GlobalMessage)
		FString GlobalMessage;

	void SetGlobalMessage(FString msg);

	UFUNCTION()
		void OnRep_HeaderMessage();

	UPROPERTY(ReplicatedUsing = OnRep_HeaderMessage)
		FMissionInfo HeaderMessage;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Generator)
		int32 GameOverCountDown;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Generator)
		int32 TimesHarvDowned;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Generator)
		int32 TotalLimbsBlownOff;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Generator)
		FString ServerLocation;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Generator)
		FString MapName;

	UFUNCTION(BlueprintCallable, Category = Scores)
		FString GetMapName();

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Generator)
		bool bStatsEnabled;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Generator)
		FString OrionGameMode;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Generator)
		TEnumAsByte<EGameDifficulty> Difficulty;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Generator)
		int32 ItemLevel;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = GRI)
		bool bReadyingUp;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = GRI)
		int32 BonusTime;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = GRI)
		bool bBonusWon;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = GRI)
		bool bMatchIsOver;

	void StartVote(EVoteType Type);

	TArray<AOrionPlayerController*> Voters;
	FTimerHandle VoteTimer;
	EVoteType VoteType;
	FString NextMapVote;

	void VoteEnded();

	FString Player1;
	FString Player2;
	FString Player3;
	FString Player4;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = GRI)
		bool bIsLocalCoop;

	bool bInitialized;

	UPROPERTY(BlueprintReadOnly, Category = MiniMap)
		FVector CameraCenter;

	FVector CoopCameraLocation;
	FRotator CoopCameraRotation;

	void SetHeaderMessage(FString Title, FString Desc);

	bool AddPlayerToTeam(AOrionPlayerController *PC, int32 TeamIndex);
	void RemovePlayerFromTeam(AOrionPlayerController *PC, int32 Index);

	void InitTeams();

	UPROPERTY()
		TSubclassOf<class AOrionPlayFabInventoryMapper> DefaultMapperClass;

	UPROPERTY(BlueprintReadWrite, Category = Inventory)
		AOrionPlayFabInventoryMapper *Mapper;

	//keep track of active ragdolls and remove the oldest ones as we overflow
	TArray<AOrionCharacter*> ActiveRagdolls;

	void AddRagdoll(AOrionCharacter *Ragdoll);
	void RemoveRagdoll(AOrionCharacter *Ragdoll);
	void ValidateRagdollArray();

	UPROPERTY(BlueprintReadWrite, Category = Music)
		bool bPlayingIdleMovie;
};
