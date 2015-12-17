// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "Orion.h"
#include "OrionCharacter.h"
#include "OrionDinoPawn.h"
#include "OrionMovementComponent.h"


UOrionMovementComponent::UOrionMovementComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	//bReplicates = true;
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

	AOrionCharacter* OrionCharacterOwner = Cast<AOrionCharacter>(PawnOwner);

	if (OrionCharacterOwner)
	{
		AOrionPlayerController *PC = Cast<AOrionPlayerController>(OrionCharacterOwner->Controller);
		if (PC)
		{
			OrionCharacterOwner->SprintRate = 1.0f + float(PC->GetSkillValue(SKILL_SPRINTSPEED)) / 100.0f;
			OrionCharacterOwner->AimRate = 1.0f + float(PC->GetSkillValue(SKILL_AIMSPEED)) / 100.0f;

			if (OrionCharacterOwner->CurrentSkill && OrionCharacterOwner->CurrentSkill->IsCloaking())
				OrionCharacterOwner->CloakRate = 1.0f + float(PC->GetSkillValue(SKILL_CLOAKSPEED)) / 100.0f;
		}

		if (OrionCharacterOwner->CurrentSkill && OrionCharacterOwner->CurrentSkill->IsCloaking())
			SpeedMod *= OrionCharacterOwner->CloakRate;

		if (OrionCharacterOwner->HasOrbEffect(ORB_SPEED))
			SpeedMod *= 1.5f;
	}

	if (OrionCharacterOwner)
	{
		if (Cast<AOrionDinoPawn>(PawnOwner))
		{
			SpeedMod *= 1.0f;
		}
		else if (Cast<AOrionDroidPawn>(PawnOwner))
		{
			if (OrionCharacterOwner->bFly)
				SpeedMod *= OrionCharacterOwner->bLanding ? 5.0 : 12.0;
			else if (OrionCharacterOwner->bDuck)
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
				SpeedMod *= 0.75f;
		}
		else
		{
			if (OrionCharacterOwner->bDowned)
				SpeedMod *= 0.2f;
			else if (OrionCharacterOwner->bFly)
				SpeedMod *= OrionCharacterOwner->bLanding ? 5.0 :12.0;
			else if (OrionCharacterOwner->bDuck)
			{
				SpeedMod *= 0.4f;
				if (OrionCharacterOwner->bAim)
					SpeedMod *= 0.625 * OrionCharacterOwner->AimRate;
			}
			else if (OrionCharacterOwner->bRun && IsMovingOnGround())
				SpeedMod *= 1.15f * OrionCharacterOwner->SprintRate;
			else if (OrionCharacterOwner->bAim)
				SpeedMod *= 0.4 * OrionCharacterOwner->AimRate;
			else
				SpeedMod *= 0.75f;// 0.55f;

			//move faster if overcharging
			////if (OrionCharacterOwner->IsOvercharging())
			////	SpeedMod *= 1.25f;
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
	AOrionCharacter* OrionCharacterOwner = Cast<AOrionCharacter>(PawnOwner);
	if (OrionCharacterOwner && (OrionCharacterOwner->PlayerState == nullptr || OrionCharacterOwner->PlayerState->bIsABot == false))
	{
		if (OrionCharacterOwner->IsRolling())
		{
			//Velocity.Normalize();
			//Velocity = Velocity*0.0f;
			Super::CalcVelocity(DeltaTime, Friction, bFluid, BrakingDeceleration);
			return;
		}
		else if ((OrionCharacterOwner->ThirdPersonCameraComponent || !OrionCharacterOwner->IsTopDown()) && OrionCharacterOwner->IsSprinting() && IsMovingOnGround())
		{
			Velocity = OrionCharacterOwner->GetActorRotation().Vector()*GetMaxSpeed();
			return;
		}
	}

	if (OrionCharacterOwner)
	{
		AOrionAIController *C = Cast<AOrionAIController>(OrionCharacterOwner->Controller);

		if (C && C->bIsStuck)
		{

		}
	}

	Super::CalcVelocity(DeltaTime, Friction, bFluid, BrakingDeceleration);

	//rare crash
	if (Velocity.ContainsNaN())
		Velocity = FVector(0.0f);
}

//hack to fix nan code
void UOrionMovementComponent::PerformMovement(float DeltaSeconds)
{
	//SCOPE_CYCLE_COUNTER(STAT_CharacterMovementAuthority);

	if (!HasValidData())
	{
		return;
	}

	// no movement if we can't move, or if currently doing physical simulation on UpdatedComponent
	if (MovementMode == MOVE_None || UpdatedComponent->Mobility != EComponentMobility::Movable || UpdatedComponent->IsSimulatingPhysics())
	{
		return;
	}

	// Force floor update if we've moved outside of CharacterMovement since last update.
	bForceNextFloorCheck |= (IsMovingOnGround() && UpdatedComponent->GetComponentLocation() != LastUpdateLocation);

	FVector OldVelocity;
	FVector OldLocation;

	// Scoped updates can improve performance of multiple MoveComponent calls.
	{
		FScopedMovementUpdate ScopedMovementUpdate(UpdatedComponent, bEnableScopedMovementUpdates ? EScopedUpdate::DeferredUpdates : EScopedUpdate::ImmediateUpdates);

		MaybeUpdateBasedMovement(DeltaSeconds);

		OldVelocity = Velocity;
		OldLocation = UpdatedComponent->GetComponentLocation();

		ApplyAccumulatedForces(DeltaSeconds);

		// Check for a change in crouch state. Players toggle crouch by changing bWantsToCrouch.
		const bool bAllowedToCrouch = CanCrouchInCurrentState();
		if ((!bAllowedToCrouch || !bWantsToCrouch) && IsCrouching())
		{
			UnCrouch(false);
		}
		else if (bWantsToCrouch && bAllowedToCrouch && !IsCrouching())
		{
			Crouch(false);
		}

		if (MovementMode == MOVE_NavWalking && bWantsToLeaveNavWalking)
		{
			TryToLeaveNavWalking();
		}

		// Character::LaunchCharacter() has been deferred until now.
		HandlePendingLaunch();

		// If using RootMotion, tick animations before running physics.
		if (!CharacterOwner->bClientUpdating && !CharacterOwner->bServerMoveIgnoreRootMotion && CharacterOwner->IsPlayingRootMotion() && CharacterOwner->GetMesh())
		{
			TickCharacterPose(DeltaSeconds);

			// Make sure animation didn't trigger an event that destroyed us
			if (!HasValidData())
			{
				return;
			}

			// For local human clients, save off root motion data so it can be used by movement networking code.
			if (CharacterOwner->IsLocallyControlled() && (CharacterOwner->Role == ROLE_AutonomousProxy) && CharacterOwner->IsPlayingNetworkedRootMotionMontage())
			{
				CharacterOwner->ClientRootMotionParams = RootMotionParams;
			}
		}

		// if we're about to use root motion, convert it to world space first.
		if (HasRootMotion())
		{
			USkeletalMeshComponent * SkelMeshComp = CharacterOwner->GetMesh();
			if (SkelMeshComp)
			{
				// Convert Local Space Root Motion to world space. Do it right before used by physics to make sure we use up to date transforms, as translation is relative to rotation.
				RootMotionParams.Set(SkelMeshComp->ConvertLocalRootMotionToWorld(RootMotionParams.RootMotionTransform));

				// Then turn root motion to velocity to be used by various physics modes.
				if (DeltaSeconds > 0.f)
				{
					Velocity = CalcRootMotionVelocity(RootMotionParams.RootMotionTransform.GetTranslation(), DeltaSeconds, Velocity);
				}

				UE_LOG(LogRootMotion, Log, TEXT("PerformMovement WorldSpaceRootMotion Translation: %s, Rotation: %s, Actor Facing: %s, Velocity: %s")
					, *RootMotionParams.RootMotionTransform.GetTranslation().ToCompactString()
					, *RootMotionParams.RootMotionTransform.GetRotation().Rotator().ToCompactString()
					, *CharacterOwner->GetActorForwardVector().ToCompactString()
					, *Velocity.ToCompactString()
					);
			}
		}

		// NaN tracking
		//checkf(!Velocity.ContainsNaN(), TEXT("UCharacterMovementComponent::PerformMovement: Velocity contains NaN (%s: %s)\n%s"), *GetPathNameSafe(this), *GetPathNameSafe(GetOuter()), *Velocity.ToString());
		if (Velocity.ContainsNaN())
			Velocity = FVector(0.0f);

		// Clear jump input now, to allow movement events to trigger it for next update.
		CharacterOwner->ClearJumpInput();

		// change position
		StartNewPhysics(DeltaSeconds, 0);

		if (!HasValidData())
		{
			return;
		}

		// uncrouch if no longer allowed to be crouched
		if (IsCrouching() && !CanCrouchInCurrentState())
		{
			UnCrouch(false);
		}

		if (!HasRootMotion() && !CharacterOwner->IsMatineeControlled())
		{
			PhysicsRotation(DeltaSeconds);
		}

		// Apply Root Motion rotation after movement is complete.
		if (HasRootMotion())
		{
			const FQuat OldActorRotationQuat = UpdatedComponent->GetComponentQuat();
			const FQuat RootMotionRotationQuat = RootMotionParams.RootMotionTransform.GetRotation();
			if (!RootMotionRotationQuat.Equals(FQuat::Identity))
			{
				const FQuat NewActorRotationQuat = RootMotionRotationQuat * OldActorRotationQuat;
				MoveUpdatedComponent(FVector::ZeroVector, NewActorRotationQuat, true);
			}

			// debug
			if (false)
			{
				const FRotator OldActorRotation = OldActorRotationQuat.Rotator();
				const FVector ResultingLocation = UpdatedComponent->GetComponentLocation();
				const FRotator ResultingRotation = UpdatedComponent->GetComponentRotation();

				// Show current position
				DrawDebugCoordinateSystem(GetWorld(), CharacterOwner->GetMesh()->GetComponentLocation() + FVector(0, 0, 1), ResultingRotation, 50.f, false);

				// Show resulting delta move.
				DrawDebugLine(GetWorld(), OldLocation, ResultingLocation, FColor::Red, true, 10.f);

				// Log details.
				UE_LOG(LogRootMotion, Warning, TEXT("PerformMovement Resulting DeltaMove Translation: %s, Rotation: %s, MovementBase: %s"),
					*(ResultingLocation - OldLocation).ToCompactString(), *(ResultingRotation - OldActorRotation).GetNormalized().ToCompactString(), *GetNameSafe(CharacterOwner->GetMovementBase()));

				const FVector RMTranslation = RootMotionParams.RootMotionTransform.GetTranslation();
				const FRotator RMRotation = RootMotionParams.RootMotionTransform.GetRotation().Rotator();
				UE_LOG(LogRootMotion, Warning, TEXT("PerformMovement Resulting DeltaError Translation: %s, Rotation: %s"),
					*(ResultingLocation - OldLocation - RMTranslation).ToCompactString(), *(ResultingRotation - OldActorRotation - RMRotation).GetNormalized().ToCompactString());
			}

			// Root Motion has been used, clear
			RootMotionParams.Clear();
		}

		// consume path following requested velocity
		bHasRequestedVelocity = false;

		OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
	} // End scoped movement update

	// Call external post-movement events. These happen after the scoped movement completes in case the events want to use the current state of overlaps etc.
	CallMovementUpdateDelegate(DeltaSeconds, OldLocation, OldVelocity);

	SaveBaseLocation();
	UpdateComponentVelocity();

	LastUpdateLocation = UpdatedComponent ? UpdatedComponent->GetComponentLocation() : FVector::ZeroVector;
}