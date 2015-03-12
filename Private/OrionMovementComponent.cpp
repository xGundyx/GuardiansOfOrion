// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "Orion.h"
#include "OrionCharacter.h"
#include "OrionDinoPawn.h"
#include "OrionMovementComponent.h"


UOrionMovementComponent::UOrionMovementComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	
}

/*void UOrionMovementComponent::ServerMove_Implementation(
	float TimeStamp,
	FVector_NetQuantize100 InAccel,
	FVector_NetQuantize100 ClientLoc,
	uint8 MoveFlags,
	uint8 ClientRoll,
	uint32 View,
	UPrimitiveComponent* ClientMovementBase,
	FName ClientBaseBoneName,
	uint8 ClientMovementMode)
{
	if (!HasValidData() || !IsComponentTickEnabled())
	{
		return;
	}

	FNetworkPredictionData_Server_Character* ServerData = GetPredictionData_Server_Character();
	check(ServerData);

	if (!VerifyClientTimeStamp(TimeStamp, *ServerData))
	{
		return;
	}

	bool bServerReadyForClient = true;
	APlayerController* PC = Cast<APlayerController>(CharacterOwner->GetController());
	if (PC)
	{
		bServerReadyForClient = PC->NotifyServerReceivedClientData(CharacterOwner, TimeStamp);
		if (!bServerReadyForClient)
		{
			InAccel = FVector::ZeroVector;
		}
	}

	// View components
	const uint16 ViewPitch = (View & 65535);
	const uint16 ViewYaw = (View >> 16);

	const FVector Accel = InAccel;
	// Save move parameters.
	const float DeltaTime = ServerData->GetServerMoveDeltaTime(TimeStamp) * CharacterOwner->CustomTimeDilation;

	ServerData->CurrentClientTimeStamp = TimeStamp;
	ServerData->ServerTimeStamp = GetWorld()->TimeSeconds;
	FRotator ViewRot;
	ViewRot.Pitch = FRotator::DecompressAxisFromShort(ViewPitch);
	ViewRot.Yaw = FRotator::DecompressAxisFromShort(ViewYaw);
	ViewRot.Roll = FRotator::DecompressAxisFromByte(ClientRoll);

	if (PC)
	{
		PC->SetControlRotation(ViewRot);
	}

	if (!bServerReadyForClient)
	{
		return;
	}

	// Perform actual movement
	if ((CharacterOwner->GetWorldSettings()->Pauser == NULL) && (DeltaTime > 0.f))
	{
		if (PC)
		{
			PC->UpdateRotation(DeltaTime);
		}

		MoveAutonomous(TimeStamp, DeltaTime, MoveFlags, Accel);
	}

	UE_LOG(LogNetPlayerMovement, Verbose, TEXT("ServerMove Time %f Acceleration %s Position %s DeltaTime %f"),
		TimeStamp, *Accel.ToString(), *CharacterOwner->GetActorLocation().ToString(), DeltaTime);

	ServerMoveHandleClientError(TimeStamp, DeltaTime, Accel, ClientLoc, ClientMovementBase, ClientBaseBoneName, ClientMovementMode);
}*/

float UOrionMovementComponent::GetMaxSpeed() const
{
	float SpeedMod = Super::GetMaxSpeed();

	const AOrionCharacter* OrionCharacterOwner = Cast<AOrionCharacter>(PawnOwner);
	if (OrionCharacterOwner)
	{
		if (Cast<AOrionDinoPawn>(PawnOwner))
		{
			SpeedMod *= 1.0f;
		}
		else
		{
			if (OrionCharacterOwner->bDuck)
			{
				SpeedMod *= 0.4f;
				if (OrionCharacterOwner->bAim)
					SpeedMod *= 0.625;
			}
			else if (OrionCharacterOwner->bRun && IsMovingOnGround())
				SpeedMod *= 1.15f;
			else if (OrionCharacterOwner->bAim)
				SpeedMod *= 0.4;
			else
				SpeedMod *= 0.75f;// 0.55f;
		}
	}

	return SpeedMod;
}

bool UOrionMovementComponent::IsCrouching() const
{
	const AOrionCharacter* OrionCharacterOwner = Cast<AOrionCharacter>(PawnOwner);
	if (OrionCharacterOwner)
	{
		return OrionCharacterOwner->IsDucking();
	}

	return false;
}

void UOrionMovementComponent::CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration)
{
	const AOrionCharacter* OrionCharacterOwner = Cast<AOrionCharacter>(PawnOwner);
	if (OrionCharacterOwner)
	{
		/*if (OrionCharacterOwner->IsRolling())
		{
			Velocity.Normalize();
			Velocity = Velocity*800.0f;
			return;
		}
		else */if (OrionCharacterOwner->IsSprinting() && IsMovingOnGround())
		{
			Velocity = OrionCharacterOwner->GetActorRotation().Vector()*GetMaxSpeed();
			return;
		}
	}

	Super::CalcVelocity(DeltaTime, Friction, bFluid, BrakingDeceleration);
}