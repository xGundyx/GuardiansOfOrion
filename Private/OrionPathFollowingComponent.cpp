// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionDinoPawn.h"
#include "OrionNavArea_Jump.h"
#include "OrionPathFollowingComponent.h"

void UOrionPathFollowingComponent::SetMoveSegment(int32 SegmentStartIndex)
{
	//custom flying pathing
	/*if (Controller && Controller->GetPawn())
	{
		AOrionCharacter *Pawn = Cast<AOrionCharacter>(Controller->GetPawn());
		if (Pawn && Pawn->IsFlying() && !Controller->bFinishedPath)
		{
			//if we are close to our next target, advance us!
			if ((Pawn->GetActorLocation() - GetCurrentTargetFlyingLocation()).Size() < 200.0f)
			{
				Controller->FlightIndex++;
				if (Controller->FlightIndex >= Controller->FlightPath.Num())
					OnPathFinished(EPathFollowingResult::Success);
			}

			return;
		}
	}*/

	Super::SetMoveSegment(SegmentStartIndex);

	if (CharacterMoveComp != NULL)
	{
		const FNavPathPoint& SegmentStart = Path->GetPathPoints()[MoveSegmentStartIndex];

		if (FNavAreaHelper::HasJumpFlag(SegmentStart))
		{
			// jump! well... fly-in-straight-line!
			//CharacterMoveComp->SetMovementMode(MOVE_Flying);
			if (Controller && Controller->GetPawn())
			{
				AOrionCharacter *Pawn = Cast<AOrionCharacter>(Controller->GetPawn());
				if (Pawn)
				{
					Pawn->Jump();
				}
			}
		}
		else
		{
			// regular move
			//CharacterMoveComp->SetMovementMode(MOVE_Walking);
		}
	}
}

void UOrionPathFollowingComponent::SetStatus(EPathFollowingStatus::Type newStatus)
{
	Status = newStatus;
}

void UOrionPathFollowingComponent::SetMovementComponent(UNavMovementComponent* MoveComp)
{
	Super::SetMovementComponent(MoveComp);

	CharacterMoveComp = Cast<UCharacterMovementComponent>(MoveComp);
}

FVector UOrionPathFollowingComponent::GetCurrentTargetFlyingLocation()
{
	//special landing code
	if (Controller && Controller->bLanding)
	{
		return Controller->LandingLocation;
	}


	if (Controller && Controller->FlightPath.Num() > Controller->FlightIndex)
	{
		return Controller->FlightPath[Controller->FlightIndex];
	}

	if (MovementComp)
		return MovementComp->GetActorFeetLocation();

	return FVector(0.0f);
}

void UOrionPathFollowingComponent::FollowPathSegment(float DeltaTime)
{
	if (Controller && Controller->GetPawn())
	{
		AOrionCharacter *Pawn = Cast<AOrionCharacter>(Controller->GetPawn());
		if (Pawn && Pawn->IsFlying())
		{
			if (MovementComp == NULL)//|| !Path.IsValid())
			{
				return;
			}

			if ((Pawn->GetActorLocation() - GetCurrentTargetFlyingLocation()).Size() < (Pawn->bLanding ? Pawn->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 10.0f : Pawn->FlyingOffset))
			{
				if (Pawn->bLanding)
				{
					Controller->bFinishedLanding = true;
					OnPathFinished(EPathFollowingResult::Success);
				}
				else
				{
					Controller->FlightIndex++;
					if (Controller->FlightIndex >= Controller->FlightPath.Num())
					{
						Controller->bFinishedPath = true;
						OnPathFinished(EPathFollowingResult::Success);
					}
				}
			}

			CharacterMoveComp->SetMovementMode(MOVE_Flying);

			//const FVector CurrentLocation = MovementComp->IsMovingOnGround() ? MovementComp->GetActorFeetLocation() : MovementComp->GetActorLocation();
			const FVector CurrentLocation = MovementComp->GetActorFeetLocation();
			const FVector CurrentTarget = GetCurrentTargetFlyingLocation();
			FVector MoveVelocity = (CurrentTarget - CurrentLocation) / DeltaTime;

			////const int32 LastSegmentStartIndex = Path->GetPathPoints().Num() - 2;
			////const bool bNotFollowingLastSegment = (MoveSegmentStartIndex < LastSegmentStartIndex);

			PostProcessMove.ExecuteIfBound(this, MoveVelocity);
			MovementComp->RequestDirectMove(MoveVelocity, false);// true);// bNotFollowingLastSegment);
			return;
		}
	}

	if (MovementComp == NULL || !Path.IsValid())
		return;

	//if we need to play a rotate animation before we actually move, do that now
	if (HandleRotation())
		return;

	/*if (Path->IsPartial())
	{
		if (Controller && Controller->GetPawn())
		{
			AOrionCharacter *Pawn = Cast<AOrionCharacter>(Controller->GetPawn());
			if (Pawn)
			{
				Pawn->Jump();
			}
		}

		return;
	}*/

	CharacterMoveComp->SetMovementMode(MOVE_Walking);

	Super::FollowPathSegment(DeltaTime);
}

bool UOrionPathFollowingComponent::HandleRotation()
{
	return false;

	if (Controller && Controller->GetPawn())
	{
		AOrionDinoPawn *Pawn = Cast<AOrionDinoPawn>(Controller->GetPawn());
		if (Pawn && Pawn->TurnLeft90Animation)
		{
			const FVector CurrentLocation = MovementComp->GetActorFeetLocation();
			const FVector CurrentTarget = GetCurrentTargetLocation();

			const FVector CurrentDirection = Pawn->GetActorRotation().Vector();

			float DotResult = FVector::DotProduct((CurrentTarget - CurrentLocation).GetSafeNormal(), CurrentDirection.GetSafeNormal());

			if (DotResult < 0.707)
			{
				MovementComp->Velocity = FVector(0, 0, 0);
				if (Pawn->GetCurrentMontage() == nullptr)
				{
					FWeaponAnim Anim;
					Anim.Pawn3P = Pawn->TurnLeft90Animation;
					float Len = Pawn->OrionPlayAnimMontage(Anim);

					//if (!Pawn->GetWorld()->GetTimerManager().IsTimerActive(RotationTimer))
					//	Pawn->GetWorld()->GetTimerManager().SetTimer(RotationTimer, this, &UOrionPathFollowingComponent::DoRotation, Len*0.9f, false);
				}
				return true;
			}
		}
	}

	return false;
}

void UOrionPathFollowingComponent::DoRotation()
{
	if (Controller && Controller->GetPawn())
	{
		AOrionDinoPawn *Pawn = Cast<AOrionDinoPawn>(Controller->GetPawn());
		if (Pawn)
		{
			Pawn->SetActorRotation(Pawn->GetActorRotation() + FRotator(0, -90.0f, 0.0f));
		}
	}
}