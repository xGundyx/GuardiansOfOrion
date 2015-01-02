// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionHoverVehicle.h"

AOrionHoverVehicle::AOrionHoverVehicle(const FObjectInitializer& ObejctInitializer)
	: Super(ObejctInitializer)
{
	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	CollisionMesh = ObejctInitializer.CreateOptionalDefaultSubobject<UStaticMeshComponent>(this, TEXT("CollisionMesh"));
	CollisionMesh->SetVisibility(false);
	CollisionMesh->SetEnableGravity(false);
	CollisionMesh->bGenerateOverlapEvents = true;
	CollisionMesh->SetCollisionProfileName(FName("PhysicsActor"));
	CollisionMesh->SetAngularDamping(1.0f);
	CollisionMesh->SetLinearDamping(1.0f);

	RootComponent = CollisionMesh;

	VehicleMesh = ObejctInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("VehicleMesh"));
	VehicleMesh->AlwaysLoadOnClient = true;
	VehicleMesh->AlwaysLoadOnServer = true;
	VehicleMesh->bOwnerNoSee = false;
	VehicleMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPose;
	VehicleMesh->bCastDynamicShadow = true;
	VehicleMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	VehicleMesh->bChartDistanceFactor = true;
	VehicleMesh->AttachTo(CollisionMesh);

	// Create a CameraComponent	
	VehicleCameraComponent = ObejctInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("VehicleCamera"));
	VehicleCameraComponent->AttachParent = CollisionMesh;
	VehicleCameraComponent->RelativeLocation = FVector(0, 0, 250.f);

	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AOrionHoverVehicle::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult)
{
	VehicleCameraComponent->GetCameraView(DeltaTime, OutResult);

	if (Controller)
		OutResult.Rotation = Controller->GetControlRotation();

	OutResult.Location = GetActorLocation() + FVector(0, 0, 250) - (OutResult.Rotation.Vector() * CameraDistance);
}

void AOrionHoverVehicle::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAction("Run", IE_Pressed, this, &AOrionHoverVehicle::Boost);
	//InputComponent->BindAction("Run", IE_Released, this, &AOrionHoverVehicle::StopSprint);

	InputComponent->BindAxis("MoveForward", this, &AOrionHoverVehicle::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AOrionHoverVehicle::MoveRight);

	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AOrionHoverVehicle::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AOrionHoverVehicle::LookUpAtRate);
}

void AOrionHoverVehicle::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// find out which way is right
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Get right vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement in that direction
		AccelRight = Direction * Value;
	}
	else
		AccelRight = FVector::ZeroVector;
}

void AOrionHoverVehicle::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AOrionHoverVehicle::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AOrionHoverVehicle::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// find out which way is forward
		const FRotator Rotation = GetActorRotation();// GetControlRotation();
		FRotator YawRotation(0, Rotation.Yaw, 0);

		// Get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(Direction, Value);
		// add movement in that direction
		//AccelForwards = Direction*Value;
	}
	//else
		//AccelForwards = FVector::ZeroVector;
}

void AOrionHoverVehicle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//all movement is processed by the server ::todo
	/*FVector TotalAccel = AccelForwards + AccelRight;

	if (TotalAccel.Size() <= 0.01)
	{
		//apply a brake force
		FVector dir = AccumulatedVelocity;
		dir.Normalize();

		AccumulatedVelocity -= dir * FMath::Min(AccumulatedVelocity.Size(), 2500.0f * DeltaSeconds);
	}
	else
	{
		TotalAccel.Normalize();
		AccumulatedVelocity += TotalAccel * 4500.0f * DeltaSeconds;
	}

	//clamp to our max speed?

	//add in some gravity/hover force
	AccumulatedVelocity += AccelUp * 1500.0f * DeltaSeconds;

	//clamp velocity
	AccumulatedVelocity.Z = FMath::Clamp(AccumulatedVelocity.Z, -500.0f, 500.0f);

	AccumulatedVelocity.ClampMaxSize(1200.0f);

	VehicleMesh->SetAllPhysicsLinearVelocity(AccumulatedVelocity, false);*/

	//try to keep us oriented to the ground
	//try to keep us upright
	////HandleRotationAndHeight(DeltaSeconds);
}

void AOrionHoverVehicle::HandleRotationAndHeight(float DeltaSeconds)
{
	FVector frontpos, backpos;
	FVector fronthit, backhit;
	FVector frontnorm, backnorm;
	FRotator rot;

	VehicleMesh->GetSocketWorldLocationAndRotation(FName("FrontThruster"), frontpos, rot);
	VehicleMesh->GetSocketWorldLocationAndRotation(FName("BackThruster"), backpos, rot);

	FHitResult Hit;

	FCollisionQueryParams TraceParams(FName(TEXT("GroundTrace")), true, this);

	TraceParams.AddIgnoredActor(this);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;

	FVector vStart = frontpos;
	FVector vEnd = vStart - FVector(0, 0, 250.0f);

	if (GWorld->SweepSingle(Hit, vStart, vEnd, FQuat::Identity, ECollisionChannel::ECC_Pawn, FCollisionShape::MakeSphere(5), TraceParams))
	{
		fronthit = Hit.Location;
		frontnorm = Hit.Normal;
	}
	else
	{
		fronthit = vEnd;// frontpos;
		frontnorm = FVector(0.0f, 0.0f, 1.0f);
	}

	vStart = backpos;
	vEnd = vStart - FVector(0, 0, 250.0f);

	if (GWorld->SweepSingle(Hit, vStart, vEnd, FQuat::Identity, ECollisionChannel::ECC_Pawn, FCollisionShape::MakeSphere(5), TraceParams))
	{
		backhit = Hit.Location;
		backnorm = Hit.Normal;
	}
	else
	{
		backhit = vEnd;// backpos;
		backnorm = FVector(0.0f, 0.0f, 1.0f);
	}

	//try to hover at a height between the two hitpos
	FVector TargetHeight = GetActorLocation();
	TargetHeight.Z = ((backhit + fronthit) / 2.0f).Z + 50.0f;
	
	AccelUp = FVector(0.0f, 0.0f, (TargetHeight.Z - GetActorLocation().Z)/100.0f);
	AccelUp.Z = FMath::Clamp(AccelUp.Z, -1.0f, 1.0f);

	//try to rotate us to the right orientation
	FVector up = (frontnorm + backnorm) / 2.0f;
	up.Normalize();

	//grab our camera's yaw
	float yaw = GetControlRotation().Yaw;

	FRotator newRot = FRotator(0.0f/*90.0f - up.Rotation().Pitch*/, yaw, 0.0f);

	SetActorRotation(FMath::Lerp(GetActorRotation(), newRot, DeltaSeconds*5.0f));
}

void AOrionHoverVehicle::Boost()
{

}