// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "OrionMovementComponent.generated.h"

/**
*
*/
UCLASS()
class UOrionMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
	UOrionMovementComponent(const FObjectInitializer& ObjectInitializer);

	virtual float GetMaxSpeed() const override;
	virtual bool IsCrouching() const override;
	virtual void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration);

	//virtual void ServerMove_Implementation(float TimeStamp, FVector_NetQuantize10 InAccel, FVector_NetQuantize100 ClientLoc, uint8 CompressedMoveFlags, uint8 ClientRoll, uint32 View, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode);
	/*virtual void ServerMove_Implementation(
		float TimeStamp,
		FVector_NetQuantize100 InAccel,
		FVector_NetQuantize100 ClientLoc,
		uint8 MoveFlags,
		uint8 ClientRoll,
		uint32 View,
		UPrimitiveComponent* ClientMovementBase,
		FName ClientBaseBoneName,
		uint8 ClientMovementMode) override;*/
};
