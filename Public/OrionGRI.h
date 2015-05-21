

#pragma once

#include "OrionWeather.h"
#include "GameFramework/GameState.h"
#include "OrionGRI.generated.h"

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
};
