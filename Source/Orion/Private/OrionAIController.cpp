

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

	if (Cast<UOrionPathFollowingComponent>(GetPathFollowingComponent()))
		Cast<UOrionPathFollowingComponent>(GetPathFollowingComponent())->Controller = this;
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
		Destroy();
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