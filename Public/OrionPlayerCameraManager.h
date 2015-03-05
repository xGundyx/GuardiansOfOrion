

#pragma once

#include "Camera/PlayerCameraManager.h"
#include "OrionPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API AOrionPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
public:
	AOrionPlayerCameraManager(const FObjectInitializer& ObjectInitializer);

	/** normal FOV */
	float NormalFOV;

	/** targeting FOV */
	float TargetingFOV;

	/** running FOV */
	float RunningFOV;

	/** After updating camera, inform pawn to update 1p mesh to match camera's location&rotation */
	virtual void UpdateCamera(float DeltaTime) override;

	virtual void ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) override;

protected:
	FRotator OldRotation;
};
