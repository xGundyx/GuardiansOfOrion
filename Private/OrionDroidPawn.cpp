

#include "Orion.h"
#include "OrionAIController.h"
#include "OrionDroidPawn.h"


AOrionDroidPawn::AOrionDroidPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

float AOrionDroidPawn::GetFinRotation(float DeltaTime)
{
	FinsRotation += DeltaTime * 90.0f;

	if (FinsRotation > 360.0f)
		FinsRotation -= 360.0f;
	else if (FinsRotation < 0.0f)
		FinsRotation += 360.0f;

	return FinsRotation;
}

//fire weapon from weapon socket
void AOrionDroidPawn::HandleSpecialWeaponFire(FName SocketName)
{
	if (GetMesh() && CurrentWeapon)
	{
		FVector pos;
		FRotator rot;
		GetMesh()->GetSocketWorldLocationAndRotation(SocketName, pos, rot);
		CurrentWeapon->FireSpecial(SocketName, rot.Vector());
	}
}

float AOrionDroidPawn::GetFrontRotation(float DeltaTime)
{
	float v = FVector::DotProduct(GetVelocity().GetSafeNormal(), GetActorRotation().Vector());

	FrontRotation += DeltaTime * 180.0f * v;

	if (FrontRotation > 360.0f)
		FrontRotation -= 360.0f;
	else if (FrontRotation < 0.0f)
		FrontRotation += 360.0f;

	return FrontRotation;
}

float AOrionDroidPawn::GetBackRotation(float DeltaTime)
{
	float v = FVector::DotProduct(GetVelocity().GetSafeNormal(), GetActorRotation().Vector());

	BackRotation -= DeltaTime * 180.0f * v;

	if (BackRotation > 360.0f)
		BackRotation -= 360.0f;
	else if (BackRotation < 0.0f)
		BackRotation += 360.0f;

	return BackRotation;
}

FVector2D AOrionDroidPawn::GetAim(float DeltaTime)
{
	FVector pos;
	FRotator rot;
	APawn *Target;

	if (!Controller)
		return FVector2D(AimYaw, AimPitch);

	Target = Cast<AOrionAIController>(Controller)->GetEnemy();

	if (Target == NULL || !Target->IsValidLowLevel())
	{
		AimYaw = 0.0f;
		AimPitch = 0.0f;
		return FVector2D(AimYaw, AimPitch);
	}

	GetPawnMesh()->GetSocketWorldLocationAndRotation(FName("Aim"), pos, rot);

	//make sure the target isn't too far away
	if ((Target->GetActorLocation() - pos).Size2D() > 5000.0)
	{
		AimYaw = 0.0f;
		AimPitch = 0.0f;
		return FVector2D(AimYaw, AimPitch);
	}

	FVector Right = FVector::CrossProduct(FVector(0.0f, 0.0f, 1.0f), GetActorRotation().Vector());

	FVector AimDirWS = Cast<AOrionAIController>(Controller)->GetEnemy()->GetActorLocation() + AimAjustment - pos + FVector(0.0f, 0.0f, 75.0f);
	AimDirWS.Normalize();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	//we need to offset the aim a bit based on player third person camera offsets
	FVector2D start = CurrentAim2D;
	const FVector2D end = FVector2D((0.0 + AimRotLS.Yaw) / 90.0, AimRotLS.Pitch / 90.0);

	if (start.X > 1.0 && end.X < -1.0)
		start.X = -2.0 - (2.0 - start.X);
	else if (start.X < -1.0 && end.X > 1.0)
		start.X = 2.0 + (start.X + 2.0);

	CurrentAim2D = FMath::Vector2DInterpTo(start, end, DeltaTime, 3.0);

	AimYaw = CurrentAim2D.X;
	AimPitch = CurrentAim2D.Y;

	return CurrentAim2D;
}