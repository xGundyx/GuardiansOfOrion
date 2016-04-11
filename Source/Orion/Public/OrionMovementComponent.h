// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "OrionMovementComponent.generated.h"

/**
*
*/
UCLASS()
class ORION_API UOrionMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
	UOrionMovementComponent(const FObjectInitializer& ObjectInitializer);

	virtual float GetMaxSpeed() const override;
	////virtual bool IsCrouching() const override;
	virtual void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration);
	virtual void PerformMovement(float DeltaSeconds) override;

	////virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	////virtual void UpdateFromCompressedFlags(uint8 Flags) override;

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

/*class ORION_API FOrionNetworkPredictionData_Client_Character : public FNetworkPredictionData_Client_Character
{
public:
	FOrionNetworkPredictionData_Client_Character(const UCharacterMovementComponent& ClientMovement);
	virtual ~FOrionNetworkPredictionData_Client_Character(){}

	virtual FSavedMovePtr AllocateNewMove() override;
};

class ORION_API FOrionSavedMove_Character : public FSavedMove_Character
{
public:
	FOrionSavedMove_Character(){}
	virtual ~FOrionSavedMove_Character(){}

	virtual uint8 GetCompressedFlags() const override;

	virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData) override;

	virtual void Clear() override;

	bool bExitShip;
};*/