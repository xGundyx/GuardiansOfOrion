

#pragma once

#include "OrionAIController.h"
#include "OrionPlayerController.h"
#include "OrionDroidController.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API AOrionDroidController : public AOrionAIController
{
	GENERATED_BODY()
public:
	AOrionDroidController(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = Droid)
		AOrionPlayerController *GetBuddy();

	UFUNCTION(BlueprintCallable, Category = Droid)
		void SetBuddy(AOrionPlayerController *newBuddy);

	UFUNCTION(BlueprintCallable, Category = Target)
		bool IsCloseToBuddy(float Radius);

private:
	AOrionPlayerController *Buddy;
};
