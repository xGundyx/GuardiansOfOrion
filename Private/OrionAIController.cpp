

#include "Orion.h"
#include "OrionWeapon.h"
#include "OrionSquad.h"
#include "OrionAIController.h"
#include "OrionAbility.h"
#include "OrionDinoPawn.h"
#include "OrionDroidPawn.h"
#include "OrionPathFollowingComponent.h"
#include "OrionGameMode.h"

AOrionAIController::AOrionAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UOrionPathFollowingComponent>(TEXT("PathFollowingComponent")))

{
	MoveType = AIMOVE_WALKING;
	bFinishedPath = true;
	bWantsPlayerState = true;

	TimesStuck = 0;
	bCanBeStuck = true;
}

void AOrionAIController::Possess(APawn* aPawn)
{
	Super::Possess(aPawn);

	bFinishedPath = true;

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

	//check if we haven't moved since our last checkup
	if ((LastStuckPos - GetPawn()->GetActorLocation()).Size() < 1.0f)
		StuckCounter++;
	else
		StuckCounter = 0;

	LastStuckPos = GetPawn()->GetActorLocation();

	if (StuckCounter >= 5.0f)
	{
		bIsStuck = true;
		GetWorldTimerManager().SetTimer(StuckTimer, this, &AOrionAIController::ResetStuck, 3.0f, false);
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

		if (TimesStuck >= 5)
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

	if (!FocalPoint.IsZero() && Pawn && (Pawn->GetVelocity().SizeSquared2D() > 10.0f || Pawn->bAlwaysRotate || (Dino && Dino->bChargingAttack)))
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
	//EventSetBlackboardEnemy(pEnemy);
	UBlackboardComponent *BlackBoard = GetBlackboard();

	if (BlackBoard)
	{
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
		if (pPawn->bFinishingMove)
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
		else if (pEnemy->bIsHiddenFromView || pPawn->bIsHiddenFromView)
			bRemoveEnemy = true;
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

void AOrionAIController::OnSeePawn(APawn *SeenPawn)
{
	//for damage taken bugs
	if (SeenPawn == nullptr || !SeenPawn->IsValidLowLevel())
		return;

	//if we see our current enemy
	if (SeenPawn == GetEnemy())
		return;

	//if we already have an enemy, just ignore for now
	if (GetEnemy() && GetWorld()->GetTimeSeconds() - LastEnemyTime < 5.0f)
		return;

	//if we are capable of attacking the generator and this is a generator, attack it!
	AOrionCharacter *P = Cast<AOrionCharacter>(GetPawn());
	AOrionCharacter *pPawn = Cast<AOrionCharacter>(SeenPawn);
	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GetGameState());

	//if we're inside a blocking volume like smoke, ignore things we see
	if (P && P->bIsHiddenFromView)
		return;

	//ignore stuff inside smoke
	if (pPawn && pPawn->bIsHiddenFromView)
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
	if (myEnemy && myEnemy->IsValidLowLevel())
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

		if (pPawn->bIsHiddenFromView)
			return;

		if (!pPawn->IsOnShip() && pPawn->Health > 0)
			SetEnemy(SeenPawn);
	}
}