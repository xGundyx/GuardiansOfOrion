

#pragma once

#include "AIController.h"
#include "OrionAIController.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API AOrionAIController : public AAIController
{
	GENERATED_UCLASS_BODY()

	
	virtual void Tick(float DeltaTime) override;

	/** Update direction AI is looking based on FocalPoint */
	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn = true) override;
};
