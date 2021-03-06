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
	if (!HasValidData() || HasAnimRootMotion() || DeltaTime < MIN_TICK_TIME)
	{
		return;
	}

	AOrionDinoPawn* Dino = Cast<AOrionDinoPawn>(PawnOwner);
	if (Dino && Dino->bChargeAttack)
	{
		//if we're charging, just move forwards in a straight line until we collide with something or hit our goal, does no pathfinding
		Velocity = Velocity + Dino->GetActorRotation().Vector() * MaxAcceleration * 3.0f * DeltaTime;
		Velocity = Velocity.GetClampedToMaxSize(1500.0f);

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
		if (OrionCharacterOwner->bChargingAttack)
			SpeedMod = 0.0f;
		else if (OrionCharacterOwner->DinoName == "TRex" && OrionCharacterOwner->bFinishingMove)
			SpeedMod = 0.0f;
		else if (OrionCharacterOwner->bFly)
			SpeedMod *= OrionCharacterOwner->bLanding ? 5.0 : 12.0;
		else if (OrionCharacterOwner->bRun && IsMovingOnGround())
			SpeedMod *= (OrionCharacterOwner->bChargeAttack ? 4.0f : 1.0f) * OrionCharacterOwner->RunningModifier;
		else
			SpeedMod *= (OrionCharacterOwner->bChargeAttack ? 4.0f : 1.0f) * OrionCharacterOwner->RunningModifier;
	}

	if (SpeedMod == 0.0f)
		SpeedMod = 0.001f;

	return SpeedMod;
}