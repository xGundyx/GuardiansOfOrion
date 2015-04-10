

#include "Orion.h"
#include "OrionWeapon.h"
#include "OrionSquad.h"
#include "OrionAIController.h"
#include "OrionPathFollowingComponent.h"


AOrionAIController::AOrionAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UOrionPathFollowingComponent>(TEXT("PathFollowingComponent")))

{
	
}

void AOrionAIController::Possess(APawn* aPawn)
{
	Super::Possess(aPawn);

	bFinishedPath = true;

	if (Cast<UOrionPathFollowingComponent>(GetPathFollowingComponent()))
		Cast<UOrionPathFollowingComponent>(GetPathFollowingComponent())->Controller = this;
}

void AOrionAIController::FindFlightPath(FVector Destination)
{
	if (!FlyArea)
		GetFlyableNavMesh();

	if (FlyArea && GetPawn())
	{
		FlightPath = FlyArea->FindPath(GetPawn()->GetActorLocation(), Destination);
		FlightIndex = 0;

		if (FlightPath.Num() > 0)
		{
			bFinishedPath = false;
			MoveFlyToLocation(FlightPath[FlightPath.Num() - 1], 100.0f, true, false, false, true, UNavigationQueryFilter::StaticClass());
		}
	}
}

FVector AOrionAIController::GetRandomFlightPoint()
{
	if (!FlyArea)
		GetFlyableNavMesh();

	if (FlyArea)
		return FlyArea->GetRandomPoint();

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

	if (FlyArea)
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

	if (GetPawn() && GetPawn()->GetVelocity().Size2D()>10.0f)
		SetFocalPoint(GetPawn()->GetActorLocation() + GetPawn()->GetVelocity().GetSafeNormal() * 500.0f);
}

void AOrionAIController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
	// Look toward focus
	FVector FocalPoint = GetFocalPoint();
	if (!FocalPoint.IsZero() && GetPawn())
	{
		FVector Direction = FocalPoint - GetPawn()->GetActorLocation();
		
		FRotator NewControlRotation = Direction.Rotation();

		NewControlRotation.Yaw = FRotator::ClampAxis(NewControlRotation.Yaw);
		if (Cast<AOrionCharacter>(GetPawn())->IsFlying())
		{
			NewControlRotation.Pitch = FRotator::ClampAxis(NewControlRotation.Pitch);

		}

		SetControlRotation(NewControlRotation);

		APawn* const P = GetPawn();
		if (P && bUpdatePawn)
		{
			P->FaceRotation(NewControlRotation, DeltaTime);
		}
	}
}

void AOrionAIController::SetEnemy(APawn *pEnemy)
{
	//for now we'll force the ai to enter a run state when we have an enemy
	if (Cast<AOrionCharacter>(GetPawn()))
		Cast<AOrionCharacter>(GetPawn())->bRun = true;

	myEnemy = pEnemy;
	EventSetBlackboardEnemy(pEnemy);

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
	if (myEnemy == nullptr)
		return;

	//ignore dead players
	if (Cast<AOrionCharacter>(myEnemy) && Cast<AOrionCharacter>(myEnemy)->Health <= 0)
		RemoveEnemy();
}

void AOrionAIController::RemoveEnemy()
{
	if (Cast<AOrionCharacter>(GetPawn()))
		Cast<AOrionCharacter>(GetPawn())->bRun = false;

	myEnemy = nullptr;
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