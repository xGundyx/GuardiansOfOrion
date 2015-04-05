// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionDinoPawn.h"
#include "OrionNavArea_Jump.h"
#include "OrionPathFollowingComponent.h"

void UOrionPathFollowingComponent::SetMoveSegment(int32 SegmentStartIndex)
{
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

void UOrionPathFollowingComponent::SetMovementComponent(UNavMovementComponent* MoveComp)
{
	Super::SetMovementComponent(MoveComp);

	CharacterMoveComp = Cast<UCharacterMovementComponent>(MoveComp);
}

void UOrionPathFollowingComponent::FollowPathSegment(float DeltaTime)
{
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