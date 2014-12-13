

#include "Orion.h"
#include "OrionDinoPawn.h"
#include "OrionDinoMovementComponent.h"
#include "OrionAIController.h"

//dinos have special movement code since some of them can be rather large
AOrionDinoPawn::AOrionDinoPawn(const FObjectInitializer& ObejctInitializer)
	: Super(ObejctInitializer.SetDefaultSubobjectClass<UOrionDinoMovementComponent>(ACharacter::CharacterMovementComponentName))
{

}

FVector2D AOrionDinoPawn::GetAim(float DeltaTime)
{
	FVector pos;
	FRotator rot;
	APawn *Target;

	if (!Controller)
		return FVector2D(0.0f, 0.0f);

	Target = Cast<AOrionAIController>(Controller)->GetEnemy();

	if (Target == NULL)
		return FVector2D(0.0f, 0.0f);

	GetPawnMesh()->GetSocketWorldLocationAndRotation(FName("Aim"), pos, rot);

	//make sure the target isn't too far away
	if ((Target->GetActorLocation() - pos).Size2D() > 5000.0)
		return FVector2D(0.0f, 0.0f);

	FVector Right = FVector::CrossProduct(FVector(0.0f, 0.0f, 1.0f), GetActorRotation().Vector());
	FVector Forward = FVector::CrossProduct(Right, GroundNormal);
	FRotator rTarget = Forward.Rotation();

	FVector AimDirWS = Cast<AOrionAIController>(Controller)->GetEnemy()->GetActorLocation() + AimAjustment - pos + FVector(0.0f,0.0f,75.0f);
	AimDirWS.Normalize();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirWS.Rotation() - rTarget;// AimDirLS.Rotation();

	//we need to offset the aim a bit based on player third person camera offsets
	const FVector2D start = CurrentAim2D;
	const FVector2D end = FVector2D((0.0 + AimRotLS.Yaw) / 90.0, AimRotLS.Pitch / 90.0);
	CurrentAim2D = FMath::Vector2DInterpTo(start, end, DeltaTime, 3.0);
	return CurrentAim2D;
}

void AOrionDinoPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Health > 0)
	{
		OrientToGround(DeltaTime);
		HandleFootPlacement(DeltaTime);
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
		 GetWorld()->LineTraceSingle(Hit, vStart, vEnd, COLLISION_WEAPON, TraceParams);
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