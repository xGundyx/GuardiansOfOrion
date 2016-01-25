// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionHoverVehicle.h"

AOrionHoverVehicle::AOrionHoverVehicle(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	//this is just a dummy for us to use
	////BaseSphere = ObjectInitializer.CreateOptionalDefaultSubobject<USphereComponent>(this, TEXT("BaseSphere"));

	CollisionMesh = ObjectInitializer.CreateOptionalDefaultSubobject<UStaticMeshComponent>(this, TEXT("CollisionMesh"));
	CollisionMesh->SetVisibility(false);
	CollisionMesh->SetEnableGravity(false);
	CollisionMesh->bGenerateOverlapEvents = true;
	CollisionMesh->SetCollisionProfileName(FName("PhysicsActor"));
	CollisionMesh->SetAngularDamping(1.0f);
	CollisionMesh->SetLinearDamping(1.0f);
	////CollisionMesh->AttachTo(BaseSphere);

	VehicleMesh = ObjectInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("VehicleMesh"));
	VehicleMesh->AlwaysLoadOnClient = true;
	VehicleMesh->AlwaysLoadOnServer = true;
	VehicleMesh->bOwnerNoSee = false;
	VehicleMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPose;
	VehicleMesh->bCastDynamicShadow = true;
	VehicleMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	//VehicleMesh->bChartDistanceFactor = true;
	VehicleMesh->AttachTo(CollisionMesh);

	// Create a CameraComponent	
	VehicleCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("VehicleCamera"));
	VehicleCameraComponent->AttachParent = CollisionMesh;
	VehicleCameraComponent->RelativeLocation = FVector(0, 0, 250.f);

	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = true;
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CollisionMesh;
}

void AOrionHoverVehicle::TryToGetIn(AOrionCharacter *newDriver)
{
	//if we already have a driver, then not allowed!
	if (Driver)
		return;

	//can't enter a vehicle that is upside down, dead, or locked
	if (Health <= 0)
		return;

	Driver = newDriver;
	Driver->SetDrivenVehicle(this);
}

void AOrionHoverVehicle::DriverLeave()
{
	if (Driver)
	{
		Driver->ExitVehicle();
		Driver = nullptr;
	}
}

AOrionCharacter *AOrionHoverVehicle::GetDriver()
{
	return Driver;
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

	InputComponent->BindAction("Use", IE_Pressed, this, &AOrionHoverVehicle::Use);
}

bool AOrionHoverVehicle::ServerUse_Validate()
{
	return true;
}

void AOrionHoverVehicle::ServerUse_Implementation()
{
	DriverLeave();
}

void AOrionHoverVehicle::Use()
{
	ServerUse();
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

void AOrionHoverVehicle::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// everyone
	DOREPLIFETIME(AOrionHoverVehicle, HealthMax);
	DOREPLIFETIME(AOrionHoverVehicle, Health);

	DOREPLIFETIME(AOrionHoverVehicle, Driver);
}

void AOrionHoverVehicle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AOrionHoverVehicle::Boost()
{

}