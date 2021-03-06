

#include "Orion.h"
#include "OrionWeapon.h"
#include "OrionSquad.h"
#include "OrionAIController.h"
#include "OrionAbility.h"
#include "OrionDinoPawn.h"
#include "OrionDroidPawn.h"
#include "OrionPathFollowingComponent.h"
#include "OrionMovementComponent.h"
#include "OrionGameMode.h"

AOrionAIController::AOrionAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UOrionPathFollowingComponent>(TEXT("PathFollowingComponent")))

{
	MoveType = AIMOVE_WALKING;
	bFinishedPath = true;
	bWantsPlayerState = true;

	TimesStuck = 0;
	bCanBeStuck = true;

	bInitialSpot = false;
}

void AOrionAIController::Possess(APawn* aPawn)
{
	Super::Possess(aPawn);

	bFinishedPath = true;

	FatalitySenseTimer = 0.0f;

	if (Cast<UOrionPathFollowingComponent>(GetPathFollowingComponent()))
		Cast<UOrionPathFollowingComponent>(GetPathFollowingComponent())->Controller = this;

	AOrionDinoPawn *DinoPawn = Cast<AOrionDinoPawn>(aPawn);
	AOrionDroidPawn *DroidPawn = Cast<AOrionDroidPawn>(aPawn);

	if (DinoPawn)
	{
		bIsElite = DinoPawn->bIsElite;
		AIName = DinoPawn->DinoName;
	}
	else if (DroidPawn)
	{
		bIsElite = DroidPawn->bIsElite;
		AIName = DroidPawn->DroidName;
	}

	if (Cast<AOrionCharacter>(aPawn))
		Cast<AOrionCharacter>(aPawn)->bRun = true;

	LastStuckPos = aPawn->GetActorLocation();
	StuckCounter = 0;
	GetWorldTimerManager().SetTimer(StuckTimer, this, &AOrionAIController::HandleStuck, 1.0f, true);
}

void AOrionAIController::HandleStuck()
{
	AOrionGameMode *Game = Cast<AOrionGameMode>(GetWorld()->GetAuthGameMode());
	AOrionCharacter *P = Cast<AOrionCharacter>(GetPawn());
	if (Game && P && P->GetActorLocation().Z < Game->DinoKillZ)
	{
		FDamageEvent dEvent = FDamageEvent::FDamageEvent();
		dEvent.DamageTypeClass = UOrionDamageType::StaticClass();

		P->Die(10000000.0f, dEvent, nullptr, P);

		return;
	}

	if (!GetPawn() || !bCanBeStuck)
		return;

	//if we're a flier and have somehow flown way outside of the playable area, just respawn us
	if (P->IsFlying())
	{
		if (Game && Game->OOBVolume)
		{
			if (!Game->OOBVolume->EncompassesPoint(P->GetActorLocation(), 2500.0f))
			{
				P->OutOfBoundsCounter++;

				if (P->OutOfBoundsCounter >= 10)
				{
					Game->AddSpawn(P);

					FDamageEvent dEvent = FDamageEvent::FDamageEvent();
					dEvent.DamageTypeClass = UOrionDamageType::StaticClass();

					P->Die(10000000.0f, dEvent, nullptr, P);
					return;
				}
			}
			else
			{
				P->OutOfBoundsCounter = -1;
			}
		}
	}

	//check if we haven't moved since our last checkup
	if ((LastStuckPos - GetPawn()->GetActorLocation()).Size() < 1.0f)
	{
		StuckCounter++;

		/*if (StuckCounter >= 4)
		{
			//give them a little nudge
			UOrionMovementComponent *Comp = Cast<UOrionMovementComponent>(GetPawn()->GetMovementComponent());
			if (Comp)
			{
				FVector force = FMath::VRand();
				force.Z = 0.0f;
				force.Normalize();

				Comp->AddImpulse(force * 500.0f + FVector(0.0f, 0.0f, 250.0f), true);
			}
		}*/
	}
	else
		StuckCounter = 0;

	LastStuckPos = GetPawn()->GetActorLocation();

	if (StuckCounter >= 10)
	{
		bIsStuck = true;
		//GetWorldTimerManager().SetTimer(StuckTimer, this, &AOrionAIController::ResetStuck, 3.0f, false);
		ResetStuck();
	}
	else
		bIsStuck = false;
}

void AOrionAIController::ResetStuck()
{
	if (GetPawn())
	{
		bIsStuck = false;
		LastStuckPos = GetPawn()->GetActorLocation();
		StuckCounter = 0;

		TimesStuck++;

		AOrionCharacter *P = Cast<AOrionCharacter>(GetPawn());

		if (TimesStuck >= 5 || (P && P->IsFlying()))
		{
			//destroy and re-add to the spawn queue
			AOrionGameMode *Game = Cast<AOrionGameMode>(GetWorld()->GetAuthGameMode());
			AOrionCharacter *P = Cast<AOrionCharacter>(GetPawn());
			if (Game && P)
			{
				Game->AddSpawn(P);
				FDamageEvent dEvent = FDamageEvent::FDamageEvent();
				dEvent.DamageTypeClass = UOrionDamageType::StaticClass();

				P->Die(10000000.0f, dEvent, nullptr, P);
			}
		}
		else
		{
			//give them a little nudge
			UOrionMovementComponent *Comp = Cast<UOrionMovementComponent>(GetPawn()->GetMovementComponent());
			if (Comp)
			{
				FVector force = FMath::VRand();
				force.Z = 0.0f;
				force.Normalize();

				Comp->AddImpulse(force * 500.0f + FVector(0.0f, 0.0f, 250.0f), true);
			}
		}
	}

	GetWorldTimerManager().SetTimer(StuckTimer, this, &AOrionAIController::HandleStuck, 1.0f, true);
}

void AOrionAIController::FindFlightPath(FVector Destination)
{
	if (!FlyArea)
		GetFlyableNavMesh();

	if (FlyArea && GetPawn())
	{
		FlightPath = FlyArea->FindPath(GetPawn()->GetActorLocation(), Destination);
		FlightIndex = 0;

		bFinishedLanding = false;

		if (FlightPath.Num() > 0)
		{
			bFinishedPath = false;
			MoveFlyToLocation(FlightPath[FlightPath.Num() - 1], 100.0f, true, false, false, true, UNavigationQueryFilter::StaticClass());
		}
	}
}

void AOrionAIController::FlyToPoint(FVector Destination)
{
	if(GetPawn())
	{
		bFinishedPath = false;

		FlightPath.Empty();
		FlightPath.Add(Destination);
		FlightIndex = 0;

		MoveDirectFlyToLocation(Destination, 100.0f, UNavigationQueryFilter::StaticClass());
	}
}

FVector AOrionAIController::GetRandomFlightPoint(bool bIsLanding)
{
	if (bIsLanding)
	{
		FNavLocation pos;

		if (GetWorld() && GetWorld()->GetNavigationSystem() && GetPawn())
		{
			GetWorld()->GetNavigationSystem()->GetRandomReachablePointInRadius(GetPawn()->GetActorLocation(), 25000.0f, pos);
			return pos.Location;
		}

		return FVector(0.0f);
	}

	if (!FlyArea)
		GetFlyableNavMesh();

	if (FlyArea)
		return FlyArea->GetRandomPoint();

	return FVector(0.0f);
}

FVector AOrionAIController::GetNextDirectFlyLocation()
{
	if (!GetPawn())
		return FVector(0.0f);

	//if we have an enemy, make sure we can see them
	if (myEnemy)
	{
		bool bCanSeeEnemy = false;

		FHitResult Hit;
		FCollisionQueryParams TraceParams(FName(TEXT("FlyTrace")), true, this);

		TraceParams.AddIgnoredActor(this);
		TraceParams.AddIgnoredActor(GetPawn());
		TraceParams.bTraceAsyncScene = true;
		TraceParams.bReturnPhysicalMaterial = true;

		FVector vStart = GetPawn()->GetActorLocation();
		FVector vEnd = myEnemy->GetActorLocation() + FVector(0.0f, 0.0f, 45.0f);

		if (GetWorld()->LineTraceSingleByChannel(Hit, vStart, vEnd, ECC_Pawn, TraceParams))
		//if (GetWorld()->LineTraceSingleByObjectType(Hit, vStart, vEnd, FCollisionObjectQueryParams::DefaultObjectQueryParam, TraceParams))
		{
			if (Hit.GetActor() != myEnemy)
				bCanSeeEnemy = false;
			else
				bCanSeeEnemy = true;
		}
		else
			bCanSeeEnemy = true;

		if (bCanSeeEnemy)
			return vEnd - FVector(0.0f, 0.0f, 75.0f);
		
		//if we have an enemy, but we can't see him, try to fly up a bit and and try again
		FVector dir = (vEnd + FVector(0.0f, 0.0f, 500.0f) - vStart).GetSafeNormal();

		return vStart + dir * 750.0f;
	}

	return FVector(0.0f);
}

AOrionFlyableArea *AOrionAIController::GetFlyableNavMesh()
{
	if (FlyArea)
		return FlyArea;

	TArray<AActor*> FlyableVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrionFlyableArea::StaticClass(), FlyableVolumes);

	if (FlyableVolumes.Num() > 0)
		FlyArea = Cast<AOrionFlyableArea>(FlyableVolumes[0]);
	else
		FlyArea = nullptr;

	if (FlyArea && !FlyArea->IsInit())
		FlyArea->BuildFlightPaths();

	return FlyArea;
}

EPathFollowingRequestResult::Type AOrionAIController::MoveFlyToLocation(const FVector& Dest, float AcceptanceRadius, bool bStopOnOverlap, bool bUsePathfinding, bool bProjectDestinationToNavigation, bool bCanStrafe, TSubclassOf<UNavigationQueryFilter> FilterClass)
{
	//SCOPE_CYCLE_COUNTER(STAT_MoveToLocation);

	EPathFollowingRequestResult::Type Result = EPathFollowingRequestResult::Failed;
	bool bCanRequestMove = true;

	//UE_VLOG(this, LogAINavigation, Log, TEXT("MoveToLocation: Goal(%s) AcceptRadius(%.1f%s) bUsePathfinding(%d) bCanStrafe(%d) Filter(%s)")
	//	, TEXT_AI_LOCATION(Dest), AcceptanceRadius, bStopOnOverlap ? TEXT(" + agent") : TEXT(""), bUsePathfinding, bCanStrafe, *GetNameSafe(FilterClass));

	// Check input is valid
	if (Dest.ContainsNaN())
	{
		//UE_VLOG(this, LogAINavigation, Error, TEXT("AAIController::MoveToLocation: Destination is not valid! Goal(%s) AcceptRadius(%.1f%s) bUsePathfinding(%d) bCanStrafe(%d)")
		//	, TEXT_AI_LOCATION(Dest), AcceptanceRadius, bStopOnOverlap ? TEXT(" + agent") : TEXT(""), bUsePathfinding, bCanStrafe);

		ensure(!Dest.ContainsNaN());
		bCanRequestMove = false;
	}

	FVector GoalLocation = Dest;

	// fail if projection to navigation is required but it failed
	/*if (bCanRequestMove && bProjectDestinationToNavigation)
	{
	UNavigationSystem* NavSys = UNavigationSystem::GetCurrent(GetWorld());
	const FNavAgentProperties& AgentProps = GetNavAgentPropertiesRef();
	FNavLocation ProjectedLocation;

	if (NavSys && !NavSys->ProjectPointToNavigation(Dest, ProjectedLocation, AgentProps.GetExtent(), &AgentProps))
	{
	UE_VLOG_LOCATION(this, LogAINavigation, Error, Dest, 30.f, FLinearColor::Red, TEXT("AAIController::MoveToLocation failed to project destination location to navmesh"));
	bCanRequestMove = false;
	}

	GoalLocation = ProjectedLocation.Location;
	}*/

	if (bCanRequestMove && GetPathFollowingComponent() && GetPathFollowingComponent()->HasReached(GoalLocation, AcceptanceRadius, !bStopOnOverlap))
	{
		//UE_VLOG(this, LogAINavigation, Log, TEXT("MoveToLocation: already at goal!"));

		// make sure previous move request gets aborted
		GetPathFollowingComponent()->AbortMove(TEXT("Aborting move due to new move request finishing with AlreadyAtGoal"), FAIRequestID::AnyRequest);

		GetPathFollowingComponent()->SetLastMoveAtGoal(true);

		OnMoveCompleted(FAIRequestID::CurrentRequest, EPathFollowingResult::Success);
		Result = EPathFollowingRequestResult::AlreadyAtGoal;
		bCanRequestMove = false;
	}

	if (bCanRequestMove)
	{
		if (Cast<UOrionPathFollowingComponent>(GetPathFollowingComponent()))
		{
			Result = EPathFollowingRequestResult::RequestSuccessful;
			Cast<UOrionPathFollowingComponent>(GetPathFollowingComponent())->SetStatus(EPathFollowingStatus::Moving);
		}
	}

	if (Result == EPathFollowingRequestResult::Failed)
	{
		if (GetPathFollowingComponent())
		{
			GetPathFollowingComponent()->SetLastMoveAtGoal(false);
		}

		OnMoveCompleted(FAIRequestID::InvalidRequest, EPathFollowingResult::Invalid);
	}

	return Result;
}

EPathFollowingRequestResult::Type AOrionAIController::MoveDirectFlyToLocation(const FVector& Dest, float AcceptanceRadius, TSubclassOf<UNavigationQueryFilter> FilterClass)
{
	EPathFollowingRequestResult::Type Result = EPathFollowingRequestResult::Failed;
	bool bCanRequestMove = true;

	// Check input is valid
	if (Dest.ContainsNaN())
	{
		ensure(!Dest.ContainsNaN());
		bCanRequestMove = false;
	}

	FVector GoalLocation = Dest;

	if (bCanRequestMove && GetPathFollowingComponent() && GetPathFollowingComponent()->HasReached(GoalLocation, AcceptanceRadius, false))
	{
		//UE_VLOG(this, LogAINavigation, Log, TEXT("MoveToLocation: already at goal!"));

		// make sure previous move request gets aborted
		GetPathFollowingComponent()->AbortMove(TEXT("Aborting move due to new move request finishing with AlreadyAtGoal"), FAIRequestID::AnyRequest);

		GetPathFollowingComponent()->SetLastMoveAtGoal(true);

		OnMoveCompleted(FAIRequestID::CurrentRequest, EPathFollowingResult::Success);
		Result = EPathFollowingRequestResult::AlreadyAtGoal;
		bCanRequestMove = false;
	}

	if (bCanRequestMove)
	{
		if (Cast<UOrionPathFollowingComponent>(GetPathFollowingComponent()))
		{
			Result = EPathFollowingRequestResult::RequestSuccessful;
			Cast<UOrionPathFollowingComponent>(GetPathFollowingComponent())->SetStatus(EPathFollowingStatus::Moving);
		}
	}

	if (Result == EPathFollowingRequestResult::Failed)
	{
		if (GetPathFollowingComponent())
		{
			GetPathFollowingComponent()->SetLastMoveAtGoal(false);
		}

		OnMoveCompleted(FAIRequestID::InvalidRequest, EPathFollowingResult::Invalid);
	}

	return Result;
}

void AOrionAIController::SetLanding(bool IsLanding)
{
	bLanding = IsLanding;

	AOrionCharacter *pPawn = Cast<AOrionCharacter>(GetPawn());
	if (pPawn)
		pPawn->bLanding = IsLanding;
}

void AOrionAIController::PawnPendingDestroy(APawn* inPawn)
{
	if (IsInState(NAME_Inactive))
	{
		UE_LOG(LogPath, Log, TEXT("PawnPendingDestroy while inactive %s"), *GetName());
	}

	if (inPawn != GetPawn())
	{
		return;
	}

	UnPossess();
	ChangeState(NAME_Inactive);

	if (Squad)
		Squad->RemoveSquadMember(this);

	//always destroy a controller after its pawn dies
	//if (PlayerState == NULL)
	//{
		bAutoDestroyWhenFinished = true;
		//Destroy();
	//}
}

void AOrionAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetPawn() && GetPawn()->GetVelocity().Size2D() > 10.0f)
		SetFocalPoint(GetPawn()->GetActorLocation() + GetPawn()->GetVelocity().GetSafeNormal() * 500.0f);
	else if (myEnemy && myEnemy->IsValidLowLevel())
		SetFocalPoint(myEnemy->GetActorLocation());

	CheckEnemyStatus();
}

void AOrionAIController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
	// Look toward focus
	FVector FocalPoint = GetFocalPoint();

	AOrionCharacter *Pawn = Cast<AOrionCharacter>(GetPawn());
	AOrionDinoPawn *Dino = Cast<AOrionDinoPawn>(GetPawn());

	if (!FocalPoint.IsZero() && Pawn && !Pawn->bStunned && (Pawn->GetVelocity().SizeSquared2D() > 10.0f || Pawn->bAlwaysRotate || Pawn->bTempAlwaysRotate || (Dino && Dino->bChargingAttack)))
	{
		FVector Direction = FocalPoint - GetPawn()->GetActorLocation();
		
		FRotator NewControlRotation = Direction.Rotation();

		NewControlRotation.Yaw = FRotator::ClampAxis(NewControlRotation.Yaw);

		if (Dino && Dino->bChargeAttack)
			return;
		//else if (Dino->bChargingAttack)
		//
		else if (Pawn->IsFlying() && !Pawn->bLanding)
		{
			NewControlRotation.Pitch = FRotator::ClampAxis(NewControlRotation.Pitch);
		}
		else if (Pawn->bLanding)
		{
			NewControlRotation.Pitch = 0.0f;
		}

		SetControlRotation(NewControlRotation);

		if (bUpdatePawn)
		{
			Pawn->FaceRotation(NewControlRotation, DeltaTime);
		}
	}
}

UBlackboardComponent *AOrionAIController::GetBlackboard()
{
	UBlackboardComponent* BlackboardComp = nullptr;

	BlackboardComp = FindComponentByClass<UBlackboardComponent>();

	return BlackboardComp;
}

void AOrionAIController::SetEnemy(APawn *pEnemy)
{
	AOrionGameMode *Game = Cast<AOrionGameMode>(GetWorld()->GetAuthGameMode());

	if (Game && !bInitialSpot)
	{
		AOrionCharacter *Pawn = Cast<AOrionCharacter>(GetPawn());
		AOrionDinoPawn *Dino = Cast<AOrionDinoPawn>(GetPawn());
		AOrionDroidPawn *Droid = Cast<AOrionDroidPawn>(GetPawn());

		FName PawnName;

		if (Dino)
			PawnName = Dino->DinoName;
		else if (Droid)
			PawnName = Droid->DroidName;

		if (Dino && Dino->bIsBigDino && FMath::RandRange(0, 5) == 2)
			Game->PlayRandomVoiceFromPlayer(VOICE_WHATISTHAT);
		else if (PawnName == "TREX")
			Game->PlayRandomVoiceFromPlayer(VOICE_TREX);
		else if (PawnName == "NAMOR")
			Game->PlayRandomVoiceFromPlayer(VOICE_NAMOR);
		else if (PawnName == "KRUGER")
			Game->PlayRandomVoiceFromPlayer(VOICE_KRUGER);
		else if (PawnName == "ANKY")
			Game->PlayRandomVoiceFromPlayer(VOICE_ANKY);
		else if (PawnName == "TRIKE")
			Game->PlayRandomVoiceFromPlayer(VOICE_TRIKE);
		else if (PawnName == "JECKYL")
			Game->PlayRandomVoiceFromPlayer(VOICE_JECKYL);
		else if (PawnName == "ORB")
			Game->PlayRandomVoiceFromPlayer(VOICE_ORB);
		else if (PawnName == "GRUMPS")
			Game->PlayRandomVoiceFromPlayer(VOICE_GRUMPS);
		else if (PawnName == "BONES")
			Game->PlayRandomVoiceFromPlayer(VOICE_BONES);
		else if (Pawn)
			Pawn->PlayVoice(VOICE_INCOMING);

	}

	bInitialSpot = true;

	//EventSetBlackboardEnemy(pEnemy);
	UBlackboardComponent *BlackBoard = GetBlackboard();

	if (BlackBoard)
	{
		//stop us from doing our current movement so we can move to our new target?
		if (myEnemy != pEnemy)
			StopMovement();

		myEnemy = pEnemy;
		BlackBoard->SetValueAsObject(TEXT("Enemy"), myEnemy);
	}
	else
	{
		return;
	}

	LastEnemyTime = GetWorld()->GetTimeSeconds();

	//for now we'll force the ai to enter a run state when we have an enemy
	if (Cast<AOrionCharacter>(GetPawn()))
		Cast<AOrionCharacter>(GetPawn())->bRun = true;

	//if we're in a squad, and we're the leader, tell our followers what to do about it
	if (Squad && Squad->Leader == this)
	{
		Squad->SetEnemy(pEnemy);
	}
}

APawn *AOrionAIController::GetEnemy()
{
	return myEnemy;
}

//if we have an enemy, make sure he is still valid
void AOrionAIController::CheckEnemyStatus()
{
	if (myEnemy == nullptr || !myEnemy->IsValidLowLevel())
	{
		RemoveEnemy();
		return;
	}

	bool bRemoveEnemy = false;

	AOrionCharacter *pEnemy = Cast<AOrionCharacter>(myEnemy);
	AOrionCharacter *pPawn = Cast<AOrionCharacter>(GetPawn());

	if (pEnemy && pPawn)
	{
		//don't lose our enemy if we're in the middle of finishing them off
		if (pPawn->bFinishingMove || pPawn->GetCurrentMontage())
			return;

		//ignored downed generators
		if (pEnemy->bIsHealableMachine && !pEnemy->bPowered)
			bRemoveEnemy = true;
		//ignore pawns getting fatalitied
		else if (pEnemy->bFatality)
			bRemoveEnemy = true;
		//can't see cloaked assholes
		if (pEnemy->CurrentSkill && pEnemy->CurrentSkill->IsCloaking())
			bRemoveEnemy = true;
		if (pEnemy->CurrentSecondarySkill && pEnemy->CurrentSecondarySkill->IsCloaking())
			bRemoveEnemy = true;
		//ignore dead players
		else if (pEnemy->Health <= 0)
			bRemoveEnemy = true;
		//can't see players inside smoke
		else if (pEnemy->HiddenFromViewLevel == 2)// || pPawn->bIsHiddenFromView)
			bRemoveEnemy = true;
		else if (pEnemy->HiddenFromViewLevel == 1 && !pEnemy->bIsHealableMachine && !pPawn->bIsBigDino)// || pPawn->bIsHiddenFromView)
			bRemoveEnemy = true;
		//ignored downed
		else if (pEnemy->bDowned)
			bRemoveEnemy = true;
		else if (pEnemy->CurrentSkill && pEnemy->CurrentSkill->IsJetpacking() && !pPawn->bCanAttackJetpackers)
			bRemoveEnemy = true;
		else if (!pPawn->bCanAttackJetpackers && !IsValidTarget(pEnemy))
			return;
	}

	if (bRemoveEnemy)
		RemoveEnemy();
}

void AOrionAIController::RemoveEnemy()
{
	AOrionCharacter *P = Cast<AOrionCharacter>(GetPawn());
	if (P)
	{
		P->bRun = true;// false;
		P->bFinishingMove = false;
	}

	AOrionDinoPawn *D = Cast<AOrionDinoPawn>(GetPawn());

	if (D)
	{
		D->bTongueAttack = false;
	}

	myEnemy = nullptr;

	StopFiringWeapon();

	UBlackboardComponent *BlackBoard = GetBlackboard();

	if (BlackBoard)
	{
		BlackBoard->SetValueAsObject(TEXT("Enemy"), nullptr);
	}
}

void AOrionAIController::GetPlayerViewPoint(FVector& OutCamLoc, FRotator& OutCamRot) const
{
	FRotator rot;
	APawn *Target;

	Target = myEnemy;

	if (Target == NULL || !Target->IsValidLowLevel())
	{
		return Super::GetPlayerViewPoint(OutCamLoc, OutCamRot);
	}

	AOrionCharacter *P = Cast<AOrionCharacter>(GetPawn());

	if (!P)
		return Super::GetPlayerViewPoint(OutCamLoc, OutCamRot);

	P->GetPawnMesh()->GetSocketWorldLocationAndRotation(FName("Aim"), OutCamLoc, rot);

	//make sure the target isn't too far away
	if ((Target->GetActorLocation() - OutCamLoc).Size2D() > 3000.0)
	{
		return Super::GetPlayerViewPoint(OutCamLoc, OutCamRot);
	}

	FVector AimDirWS = Target->GetActorLocation() - OutCamLoc + FVector(0.0f, 0.0f, 75.0f);
	AimDirWS.Normalize();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	//OutCamRot = AimRotLS;
	OutCamRot = AimDirWS.Rotation();
}

void AOrionAIController::StartFiringWeapon(FName SocketName, FVector Direction)
{
	AOrionCharacter *pPawn = Cast<AOrionCharacter>(GetPawn());
	if (pPawn && pPawn->GetWeapon())
	{
		pPawn->GetWeapon()->StartFire();
	}
}

void AOrionAIController::StopFiringWeapon()
{
	AOrionCharacter *pPawn = Cast<AOrionCharacter>(GetPawn());
	if (pPawn && pPawn->GetWeapon())
	{
		pPawn->GetWeapon()->StopFire();
	}
}

void AOrionAIController::OnHearNoise(APawn *HeardPawn, const FVector &Location, float Volume)
{

}

bool AOrionAIController::IsValidTarget(AOrionCharacter *pTarget)
{
	if (!pTarget || !pTarget->IsValidLowLevel())
		return false;

	if (pTarget->bIsHealableMachine)
		return true;

	AOrionCharacter *P = Cast<AOrionCharacter>(GetPawn());

	if (!P)
		return false;

	FSharedConstNavQueryFilter
	/*TSharedPtr<const FNavigationQueryFilter>*/ QueryFilter = GetWorld()->GetNavigationSystem()->MainNavData->GetQueryFilter(P->DefaultFilterClass);// UNavigationQueryFilter::GetQueryFilter(GetWorld()->GetNavigationSystem()->MainNavData, P->DefaultFilterClass);
	if (!GetWorld()->GetNavigationSystem()->TestPathSync(FPathFindingQuery(nullptr, *GetWorld()->GetNavigationSystem()->MainNavData, 
		P->GetActorLocation() - 0.9f * P->GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), pTarget->GetActorLocation() - 0.9f * pTarget->GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), QueryFilter)))
	{
		// Point is not on NavMesh or not reachable for some other reason
		return false;
	}

	return true;
}

void AOrionAIController::OnSeePawn(APawn *SeenPawn)
{
	//don't change target when getting ready to bite someone
	if (FatalitySenseTimer > 0.0f)
		return;

	//for damage taken bugs
	if (SeenPawn == nullptr || !SeenPawn->IsValidLowLevel())
		return;

	//if we see our current enemy
	if (SeenPawn == GetEnemy())
		return;

	//if we already have an enemy, just ignore for now
	if (GetEnemy() && GetWorld()->GetTimeSeconds() - LastEnemyTime < 10.0f)
		return;

	//if we are capable of attacking the generator and this is a generator, attack it!
	AOrionCharacter *P = Cast<AOrionCharacter>(GetPawn());
	AOrionCharacter *pPawn = Cast<AOrionCharacter>(SeenPawn);
	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GetGameState());
	AOrionPlayerController *SeenPC = Cast<AOrionPlayerController>(SeenPawn->Controller);
	//AOrionCharacter *E = Cast<AOrionCharacter>(GetEnemy());

	//if we're inside a blocking volume like smoke, ignore things we see
	//if (P && P->bIsHiddenFromView)
	//	return;

	if (SeenPC && SeenPC->DropPod)
		return;

	//no changing enemies during attacks/fatalities
	if (P && P->GetCurrentMontage())
		return;

	if (!pPawn || !P)
		return;

	if (P && P->bFinishingMove)
		return;

	//ignore jetpackers if we must
	if (pPawn->CurrentSkill && pPawn->CurrentSkill->IsJetpacking() && (GetEnemy() || !P->bCanAttackJetpackers))
		return;

	if ((!P->bCanAttackJetpackers || GetEnemy()) && !IsValidTarget(pPawn))
		return;

	//ignore stuff inside smoke
	if (pPawn && pPawn->HiddenFromViewLevel == 2)
		return;
	else if (pPawn && pPawn->HiddenFromViewLevel == 1 && !pPawn->bIsHealableMachine && !pPawn->bIsBigDino)
		return;

	if (pPawn && P && pPawn->bIsHealableMachine && GRI)
	{
		if (GRI->bSideMission)
			return;

		if (GetEnemy())
			return;

		if (!P->IsValidLowLevel() || !P->bCanAttackGenerator)
			return;

		if (!pPawn->bPowered)
			return;

		SetEnemy(SeenPawn);
	}

	//for now just ignore other bots and go straight for humans
	if (SeenPawn && SeenPawn->PlayerState && SeenPawn->PlayerState->bIsABot)
		return;

	//don't care about spawning ships
	if (Cast<AOrionShipPawn>(SeenPawn))
		return;

	//if we have an enemy already, only attack if this target is a better choice
	if (myEnemy && myEnemy->IsValidLowLevel() && !IsValidTarget(Cast<AOrionCharacter>(myEnemy)))
	{
		float dist1 = (SeenPawn->GetActorLocation() - GetPawn()->GetActorLocation()).SizeSquared2D();
		float dist2 = (myEnemy->GetActorLocation() - GetPawn()->GetActorLocation()).SizeSquared2D();

		if (dist2 < dist1)
			return;
	}

	if (pPawn)
	{
		if (pPawn->bFatality || pPawn->bFinishingMove)
			return;

		if (pPawn->CurrentSkill && pPawn->CurrentSkill->IsCloaking())
			return;

		if (pPawn->CurrentSecondarySkill && pPawn->CurrentSecondarySkill->IsCloaking())
			return;

		if (pPawn->HiddenFromViewLevel > 0)
			return;

		//ignore players waiting for revives
		if (pPawn->bDowned)
			return;

		if (!pPawn->IsOnShip() && pPawn->Health > 0)
			SetEnemy(SeenPawn);
	}
}