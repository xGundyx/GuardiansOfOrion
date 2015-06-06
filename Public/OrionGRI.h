

#pragma once

#include "OrionWeather.h"
#include "OrionPRI.h"
#include "GameFramework/GameState.h"
#include "OrionGRI.generated.h"

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

	UPROPERTY(Replicated, BlueprintReadOnly)
		bool bTeamGame;

	UPROPERTY(Replicated, BlueprintReadOnly)
		TArray<FTeamInfo> Teams;

	bool AddPlayerToTeam(AOrionPlayerController *PC, int32 TeamIndex);
	void RemovePlayerFromTeam(AOrionPlayerController *PC, int32 Index);

	void InitTeams();
};
