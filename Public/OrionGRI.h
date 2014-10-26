

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
	GENERATED_UCLASS_BODY()

	UPROPERTY(Transient, Replicated)
		AOrionWeather *Weather;

	UPROPERTY(Transient, Replicated)
		FTimeOfDay WorldTime;

	void SetWeather(AOrionWeather *theWeather);

	void Tick(float DeltaSeconds) override;
	void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = Weather)
		FTimeOfDay GetWorldTime() const;
};
