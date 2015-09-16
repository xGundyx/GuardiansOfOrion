

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
		int32 DinosAliveInWave;

	UPROPERTY(Replicated, BlueprintReadOnly)
		int32 WaveNum;

	UPROPERTY(Replicated, BlueprintReadOnly)
		int32 TotalWaves;

	UPROPERTY(Replicated, BlueprintReadOnly)
		FString MissionObjective;

	UPROPERTY(Replicated, BlueprintReadOnly)
		bool bSideMission;

	UPROPERTY(Replicated, BlueprintReadOnly)
		bool bBossMission;

	UFUNCTION()
		void HandleVictoryDefeat();

	UPROPERTY(ReplicatedUsing = HandleVictoryDefeat, BlueprintReadOnly)
		bool bVictory;

	UPROPERTY(ReplicatedUsing = HandleVictoryDefeat, BlueprintReadOnly)
		bool bDefeat;

	UPROPERTY(Replicated, BlueprintReadOnly)
		FVector MissionLocation;

	//list of connected players for easy access
	UPROPERTY(Replicated, BlueprintReadOnly)
		TArray<AOrionPRI*> PlayerList;

	UPROPERTY()
		class AOrionMusicManager *MusicManager;

	UFUNCTION()
		void OnRep_GlobalMessage();

	UPROPERTY(ReplicatedUsing = OnRep_GlobalMessage)
		FString GlobalMessage;

	void SetGlobalMessage(FString msg);

	bool AddPlayerToTeam(AOrionPlayerController *PC, int32 TeamIndex);
	void RemovePlayerFromTeam(AOrionPlayerController *PC, int32 Index);

	void InitTeams();

	UPROPERTY()
		TSubclassOf<class AOrionPlayFabInventoryMapper> DefaultMapperClass;

	UPROPERTY(BlueprintReadWrite, Category = Inventory)
		AOrionPlayFabInventoryMapper *Mapper;
};
