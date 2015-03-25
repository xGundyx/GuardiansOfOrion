

#include "Orion.h"
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