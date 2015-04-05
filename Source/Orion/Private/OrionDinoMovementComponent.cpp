// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionDinoPawn.h"
#include "OrionDinoMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


UOrionDinoMovementComponent::UOrionDinoMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bUseRVOAvoidance = true;
}

void UOrionDinoMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UOrionDinoMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UOrionDinoMovementComponent::CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration)
{
	// Do not update velocity when using root motion
	if (!HasValidData() || HasRootMotion() || DeltaTime < MIN_TICK_TIME)
	{
		return;
	}

	Friction = FMath::Max(0.f, Friction);
	const float MaxAccel = GetMaxAcceleration();
	float MaxSpeed = GetMaxSpeed();

	// Check if path following requested movement
	bool bZeroRequestedAcceleration = true;
	FVector RequestedAcceleration = FVector::ZeroVector;
	float RequestedSpeed = 0.0f;
	if (ApplyRequestedMove(DeltaTime, MaxAccel, MaxSpeed, Friction, BrakingDeceleration, RequestedAcceleration, RequestedSpeed))
	{
		RequestedAcceleration.GetClampedToMaxSize(MaxAccel);
		bZeroRequestedAcceleration = false;
	}

	if (bForceMaxAccel)
	{
		// Force acceleration at full speed.
		// In consideration order for direction: Acceleration, then Velocity, then Pawn's rotation.
		if (Acceleration.SizeSquared() > SMALL_NUMBER)
		{
			Acceleration = (Acceleration.GetSafeNormal()) * MaxAccel;
		}
		else
		{
			Acceleration = MaxAccel * (Velocity.SizeSquared() < SMALL_NUMBER ? CharacterOwner->GetActorRotation().Vector() : (Velocity.GetSafeNormal()));
		}

		AnalogInputModifier = 1.f;
	}

	// Path following above didn't care about the analog modifier, but we do for everything else below, so get the fully modified value.
	// Use max of requested speed and max speed if we modified the speed in ApplyRequestedMove above.
	MaxSpeed = FMath::Max(RequestedSpeed, MaxSpeed * AnalogInputModifier);

	// Apply braking or deceleration
	const bool bZeroAcceleration = Acceleration.IsZero();
	const bool bVelocityOverMax = IsExceedingMaxSpeed(MaxSpeed);

	// Only apply braking if there is no acceleration, or we are over our max speed and need to slow down to it.
	if ((bZeroAcceleration && bZeroRequestedAcceleration) || bVelocityOverMax)
	{
		const FVector OldVelocity = Velocity;
		ApplyVelocityBraking(DeltaTime, Friction, BrakingDeceleration);

		// Don't allow braking to lower us below max speed if we started above it.
		if (bVelocityOverMax && Velocity.SizeSquared() < FMath::Square(MaxSpeed) && FVector::DotProduct(Acceleration, OldVelocity) > 0.0f)
		{
			Velocity = (OldVelocity.GetSafeNormal()) * MaxSpeed;
		}
	}
	else if (!bZeroAcceleration)
	{
		// Friction affects our ability to change direction. This is only done for input acceleration, not path following.
		const FVector AccelDir = (Acceleration.GetSafeNormal());
		const float VelSize = Velocity.Size();
		Velocity = Velocity - (Velocity - AccelDir * VelSize) * FMath::Min(DeltaTime * Friction, 1.f);
	}

	// Apply fluid friction
	if (bFluid)
	{
		Velocity = Velocity * (1.f - FMath::Min(Friction * DeltaTime, 1.f));
	}

	// Apply acceleration
	const float NewMaxSpeed = (IsExceedingMaxSpeed(MaxSpeed)) ? Velocity.Size() : MaxSpeed;
	Velocity += Acceleration * DeltaTime;
	Velocity += RequestedAcceleration * DeltaTime;
	Velocity = Velocity.GetClampedToMaxSize(NewMaxSpeed);

	if (bUseRVOAvoidance)
	{
		CalcAvoidanceVelocity(DeltaTime);
	}
}

float UOrionDinoMovementComponent::GetMaxSpeed() const
{
	float SpeedMod = Super::GetMaxSpeed();

	const AOrionDinoPawn* OrionCharacterOwner = Cast<AOrionDinoPawn>(PawnOwner);
	if (OrionCharacterOwner)
	{
		if (OrionCharacterOwner->bRun && IsMovingOnGround())
			SpeedMod *= OrionCharacterOwner->RunningModifier;
		else
			SpeedMod *= 1.0f;
	}

	return SpeedMod;
}

void UOrionDinoMovementComponent::PerformMovement(float DeltaSeconds)
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
		OldLocation = CharacterOwner->GetActorLocation();

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

		// Character::LaunchCharacter() has been deferred until now.
		HandlePendingLaunch();

		// If using RootMotion, tick animations before running physics.
		if (!CharacterOwner->bClientUpdating && CharacterOwner->IsPlayingRootMotion() && CharacterOwner->GetMesh())
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
				/*FRotator rot = RootMotionParams.RootMotionTransform.GetRotation().Rotator();
				FRotator trot;

				trot.Yaw = rot.Yaw;
				trot.Roll = rot.Pitch;
				trot.Pitch = -rot.Roll;

				RootMotionParams.RootMotionTransform.SetRotation(trot.Quaternion());

				FVector pos = RootMotionParams.RootMotionTransform.GetTranslation();
				FVector tpos;

				tpos.X = 0.0f;//pos.X;
				tpos.Y = 0.0f;//pos.Y;
				tpos.Z = 0.0f;//pos.Z;

				RootMotionParams.RootMotionTransform.SetTranslation(tpos);*/

				// Convert Local Space Root Motion to world space. Do it right before used by physics to make sure we use up to date transforms, as translation is relative to rotation.
				RootMotionParams.Set(SkelMeshComp->ConvertLocalRootMotionToWorld(RootMotionParams.RootMotionTransform));
				UE_LOG(LogRootMotion, Log, TEXT("PerformMovement WorldSpaceRootMotion Translation: %s, Rotation: %s, Actor Facing: %s"),
					*RootMotionParams.RootMotionTransform.GetTranslation().ToCompactString(), *RootMotionParams.RootMotionTransform.GetRotation().Rotator().ToCompactString(), *CharacterOwner->GetActorRotation().Vector().ToCompactString());
			}

			// Then turn root motion to velocity to be used by various physics modes.
			if (DeltaSeconds > 0.f)
			{
				const FVector RootMotionVelocity = RootMotionParams.RootMotionTransform.GetTranslation() / DeltaSeconds;
				// Do not override Velocity.Z if in falling physics, we want to keep the effect of gravity.
				Velocity = FVector(RootMotionVelocity.X, RootMotionVelocity.Y, (MovementMode == MOVE_Falling ? Velocity.Z : RootMotionVelocity.Z));
			}
		}

		// NaN tracking
		checkf(!Velocity.ContainsNaN(), TEXT("UCharacterMovementComponent::PerformMovement: Velocity contains NaN (%s: %s)\n%s"), *GetPathNameSafe(this), *GetPathNameSafe(GetOuter()), *Velocity.ToString());

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
			const FRotator OldActorRotation = CharacterOwner->GetActorRotation();
			const FRotator RootMotionRotation = RootMotionParams.RootMotionTransform.GetRotation().Rotator();

			if (!RootMotionRotation.IsNearlyZero())
			{
				const FRotator NewActorRotation = (OldActorRotation + RootMotionRotation).GetNormalized();
				MoveUpdatedComponent(FVector::ZeroVector, NewActorRotation, true);
			}

			// debug
			if (false)
			{
				const FVector ResultingLocation = CharacterOwner->GetActorLocation();
				const FRotator ResultingRotation = CharacterOwner->GetActorRotation();

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

void UOrionDinoMovementComponent::SimulateRootMotion(float DeltaSeconds, const FTransform& LocalRootMotionTransform)
{
	if (CharacterOwner && CharacterOwner->GetMesh() && (DeltaSeconds > 0.f))
	{
		// Convert Local Space Root Motion to world space. Do it right before used by physics to make sure we use up to date transforms, as translation is relative to rotation.
		const FTransform WorldSpaceRootMotionTransform = CharacterOwner->GetMesh()->ConvertLocalRootMotionToWorld(LocalRootMotionTransform);

		// Compute root motion velocity to be used by physics
		const FVector RootMotionVelocity = WorldSpaceRootMotionTransform.GetTranslation() / DeltaSeconds;
		// Do not override Velocity.Z if falling.
		Velocity = FVector(RootMotionVelocity.X, RootMotionVelocity.Y, (MovementMode == MOVE_Falling ? Velocity.Z : RootMotionVelocity.Z));

		// Update replicated movement mode.
		if (bNetworkMovementModeChanged)
		{
			bNetworkMovementModeChanged = false;
			ApplyNetworkMovementMode(CharacterOwner->GetReplicatedMovementMode());
		}

		StartNewPhysics(DeltaSeconds, 0);
		// fixme laurent - simulate movement seems to have step up issues? investigate as that would be cheaper to use.
		// 		SimulateMovement(DeltaSeconds);

		// Apply Root Motion rotation after movement is complete.
		FRotator rot = WorldSpaceRootMotionTransform.GetRotation().Rotator();
		FRotator trot;

		trot.Yaw = rot.Yaw;
		trot.Roll = rot.Pitch;
		trot.Pitch = rot.Roll;

		const FRotator RootMotionRotation = trot;// WorldSpaceRootMotionTransform.GetRotation().Rotator();
		if (!RootMotionRotation.IsNearlyZero())
		{
			const FRotator NewActorRotation = (CharacterOwner->GetActorRotation() + RootMotionRotation).GetNormalized();
			MoveUpdatedComponent(FVector::ZeroVector, NewActorRotation, true);
		}
	}
}