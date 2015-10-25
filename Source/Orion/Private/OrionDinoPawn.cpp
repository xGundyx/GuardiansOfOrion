

#include "Orion.h"
#include "OrionDinoPawn.h"
#include "OrionDinoMovementComponent.h"
#include "OrionAIController.h"

//dinos have special movement code since some of them can be rather large
AOrionDinoPawn::AOrionDinoPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UOrionDinoMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	RunningModifier = 1.0f;
	RotationRate = 1.5f;

	Shield = 0.0f;
	ShieldMax = 0.0f;
}

FVector2D AOrionDinoPawn::GetAim(float DeltaTime)
{
	FVector pos;
	FRotator rot;
	APawn *Target;

	if (bChargeAttack)
		return FVector2D(0.0f, -0.5f);

	if (bFinishingMove)
		return FVector2D(0.0f, 0.0f);

	if (!Controller)
		return FVector2D(AimYaw, AimPitch);

	Target = Cast<AOrionAIController>(Controller)->GetEnemy();

	if (Target == NULL || !Target->IsValidLowLevel())
	{
		if (Target)
			Cast<AOrionAIController>(Controller)->RemoveEnemy();

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
	FVector Forward = FVector::CrossProduct(Right, GroundNormal);
	FRotator rTarget = Forward.Rotation();

	FVector AimDirWS = Cast<AOrionAIController>(Controller)->GetEnemy()->GetActorLocation() + AimAjustment - pos + FVector(0.0f,0.0f,75.0f);
	AimDirWS.Normalize();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = /*AimDirWS.Rotation() - rTarget;*/AimDirLS.Rotation();

	//we need to offset the aim a bit based on player third person camera offsets
	FVector2D start = CurrentAim2D;
	const FVector2D end = FVector2D((0.0 + AimRotLS.Yaw) / 90.0, AimRotLS.Pitch / 90.0);

	if (start.X > 1.0 && end.X < -1.0)
		start.X = -2.0 - (2.0 - start.X);
	else if (start.X < -1.0 && end.X > 1.0)
		start.X = 2.0 + (start.X + 2.0);

	CurrentAim2D = FMath::Vector2DInterpTo(start, end, DeltaTime, 6.0);

	AimYaw = CurrentAim2D.X;
	AimPitch = CurrentAim2D.Y;

	return CurrentAim2D;
}

void AOrionDinoPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Health > 0)
	{
		////OrientToGround(DeltaTime);
		////HandleFootPlacement(DeltaTime);
	}

	if (IsPlayingRootMotion())
	{
		//FRotator rot = GetActorRotation();
		//rot.Roll = rot.Pitch;
		//rot.Pitch = GetActorRotation().Roll;
		//SetActorRotation(rot);// FMath::RInterpTo(GetActorRotation(), rot, DeltaTime, 5.0f));
	}
	else
	{
		FRotator rot = GetActorRotation();
		rot.Roll = 0.0f;
		if (!IsFlying())
			rot.Pitch = 0.0f;
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), rot, DeltaTime, 1.0f));
	}

	//check to see if we should stop charging
	if (bChargeAttack)
	{
		FVector dir = GetActorRotation().Vector();
		FVector targetdir = (ChargeTarget - GetActorLocation()).GetSafeNormal();

		if (FVector::DotProduct(dir, targetdir) < 0.5f)
		{
			bShouldStopMovement = true;
			bChargeAttack = false;
			bChargingAttack = false;

			if (Controller)
				Controller->StopMovement();
		}
	}
}

//rotate our mesh so the feet are on the ground, this also adjusts the height offset of the mesh
void AOrionDinoPawn::OrientToGround(float DeltaTime)
{
	//do a trace from actor location down to the ground and find the normal of the ground we are on
	FVector vStart = GetActorLocation();
	FVector vEnd = vStart - FVector(0.0f, 0.0f, 1.0f)*GetCapsuleComponent()->GetScaledCapsuleHalfHeight()*2.0f;

	static FName BodyTag = FName(TEXT("BodyTrace"));

	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(BodyTag, false, this);
	TraceParams.bTraceAsyncScene = false;
	TraceParams.bReturnPhysicalMaterial = false;

	FHitResult Hit(ForceInit);
	if (GetWorld())
	{
		GetWorld()->SweepTestByChannel(vStart, vEnd, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(10.0f), TraceParams);
		//GetWorld()->LineTraceSingleByChannel(Hit, vStart, vEnd, ECC_Visibility, TraceParams);
		if (Hit.bBlockingHit)
		{
			GroundNormal = Hit.ImpactNormal;
		}
		else
		{
			GroundNormal = FVector(0.0f, 0.0f, 1.0f);
		}

		//interp our mesh normal to the ground normal
		FVector Right = FVector::CrossProduct(FVector(0.0f, 0.0f, 1.0f), GetActorRotation().Vector());
		FVector Forward = FVector::CrossProduct(Right, GroundNormal);
		FRotator rTarget = Forward.Rotation();
		//Mesh->SetRelativeRotation(FRotator(FMath::FInterpTo(Mesh->RelativeRotation.Pitch, rTarget, DeltaTime, 3.0f), Mesh->RelativeRotation.Yaw, Mesh->RelativeRotation.Roll));
		GetMesh()->SetRelativeRotation(FRotator(rTarget.Roll, GetMesh()->RelativeRotation.Yaw, -rTarget.Pitch));
	}
}

void AOrionDinoPawn::HandleFootPlacement(float DeltaTime)
{

}