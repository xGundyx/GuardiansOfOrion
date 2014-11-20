

#include "Orion.h"
#include "OrionDroidController.h"

AOrionDroidController::AOrionDroidController(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}

AOrionPlayerController *AOrionDroidController::GetBuddy()
{
	return Buddy;
}

void AOrionDroidController::SetBuddy(AOrionPlayerController *newBuddy)
{
	Buddy = newBuddy;
	newBuddy->DroidBuddy = Cast<AOrionDroidPawn>(GetPawn());
}

bool AOrionDroidController::IsCloseToBuddy(float Radius)
{
	if (!Buddy || !Buddy->GetPawn() || !GetPawn())
		return false;

	return (GetPawn()->GetActorLocation() - Buddy->GetPawn()->GetActorLocation()).Size() < Radius;
}