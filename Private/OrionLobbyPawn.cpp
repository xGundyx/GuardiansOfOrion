// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionSkeletalMeshComponent.h"
#include "OrionLobbyPawn.h"

AOrionLobbyPawn::AOrionLobbyPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	/*GetMesh()->bOwnerNoSee = true;
	HelmetMesh->bOwnerNoSee = true;
	BodyMesh->bOwnerNoSee = true;
	ArmsMesh->bOwnerNoSee = true;
	LegsMesh->bOwnerNoSee = true;
	Flight1Mesh->bOwnerNoSee = true;
	Flight2Mesh->bOwnerNoSee = true;*/

	Arms1PArmorMesh->SetHiddenInGame(true);
	Arms1PMesh->SetHiddenInGame(true);
}

void AOrionLobbyPawn::BeginPlay()
{
	Super::BeginPlay();
	bShoulderCamera = false;
}

void AOrionLobbyPawn::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	FVector CamStart;

	if (IsFirstPerson())
	{
		FirstPersonCameraComponent->GetCameraView(DeltaTime, OutResult);
		if (Controller)
			OutResult.Rotation = Controller->GetControlRotation();
	}
	else
	{
		ThirdPersonCameraComponent->GetCameraView(DeltaTime, OutResult);

		//for spectating
		AOrionPlayerController *PC = Cast<AOrionPlayerController>(GetWorld()->GetFirstPlayerController());
		if (PC)
			OutResult.Rotation = PC->GetControlRotation();

		FVector X, Y, Z;

		X = OutResult.Rotation.Vector();
		Z = FVector(0.0f, 0.0f, 1.0f);
		Y = FVector::CrossProduct(Z, X);
		Y.Normalize();
		Z = FVector::CrossProduct(X, Y);
		Z.Normalize();

		CamStart = GetActorLocation() + FVector(0.0f, 0.0f, 25.0f);// GetMesh()->GetBoneLocation(FName("Chr01_Spine02"));

		FVector Offset = CamStart - GetActorLocation();

		if (bBlinking)
		{
			float TeleportTime = FMath::Max(0.0f, 0.2f - (GetWorld()->GetTimeSeconds() - LastTeleportTime));

			CamStart = BlinkPos.Start + (1.0f - (TeleportTime / 0.2f)) * (BlinkPos.End - BlinkPos.Start);
			CamStart += Offset;
		}

		ThirdPersonAdjust = FMath::Lerp(ThirdPersonAdjust, bFatality ? 4.5f : (bAim ? 1.0f : 2.5f), DeltaTime * 6.0f);

		OutResult.Location = CamStart - X*CameraOffset.X * ThirdPersonAdjust + CameraOffset.Y*Y + CameraOffset.Z*Z;

		CameraLocation = OutResult.Location;
	}

	//add some weapon bob and view sway
	//default eye height is 0,0,64 for firstperson
	float OldBobTime = BobTime;
	FRotator rot = GetActorRotation();
	FVector X = rot.Vector();
	FVector Z = FVector(0, 0, 1);
	FVector Y = FVector::CrossProduct(Z, X);

	float Speed2D = GetVelocity().Size2D();
	if (GetMovementComponent() && GetMovementComponent()->IsFalling())
		Speed2D = 0.0f;

	if (IsRolling() || IsSprinting() || (CurrentSkill && CurrentSkill->IsJetpacking()))
		Speed2D = 0.0f;

	if (Speed2D < 10)
		BobTime += 0.2 * FMath::Min(0.1f, DeltaTime);
	else
		BobTime += FMath::Min(0.1f, DeltaTime) * FMath::Min(1.0f, 0.3f + 0.7f * Speed2D / GetMovementComponent()->GetMaxSpeed());

	//Bob = Lerp(Bob, (bIsWalking ? default.Bob : default.Bob*BobScaleWhenRunning), DeltaSeconds * 10);

	float TimeDilation = 1.0f;

	if (IsAiming())
		TimeDilation = 0.4f;

	WalkBob = Y * 0.01 * Speed2D * FMath::Sin(8.0f * BobTime * TimeDilation);
	WalkBob.Z = 0.01 * Speed2D * FMath::Sin(16.0f * BobTime * TimeDilation);

	//FirstPersonCameraComponent->SetRelativeLocation(FMath::Lerp(FirstPersonCameraComponent->RelativeLocation, FVector(0, 0, 64.0) + WalkBob, FMath::Min(0.5f,DeltaTime*10.0f)));
	OutResult.Location += WalkBob * (IsAiming() ? 0.02f : 0.05f);

	//camera collision
	FHitResult Hit;

	FCollisionQueryParams TraceParams(FName(TEXT("CameraTrace")), true, this);

	TraceParams.AddIgnoredActor(this);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;

	FVector vStart = CamStart;
	FVector vEnd = OutResult.Location;

	//if (GWorld->LineTraceSingle(Hit, vStart, vEnd, COLLISION_WEAPON,TraceParams))
	if (GWorld->SweepSingleByChannel(Hit, vStart, vEnd, FQuat::Identity, ECollisionChannel::ECC_Camera, FCollisionShape::MakeSphere(15), TraceParams))
	{
		if (Hit.bBlockingHit)
		{
			OutResult.Location = Hit.Location;
			CameraLocation = OutResult.Location;
		}
	}

	//add some aim kick
	/*if (IsFirstPerson())
	FirstPersonCameraComponent->SetRelativeRotation(AimKick);
	else
	ThirdPersonCameraComponent->SetRelativeRotation(AimKick);
	*/
	//OutResult.Rotation += AimKick;
}

bool AOrionLobbyPawn::IsFirstPerson() const
{
	return false;// IsAlive() && Controller && Controller->IsLocalPlayerController();
}

void AOrionLobbyPawn::Use()
{
	if (ShouldIgnoreControls())
		return;

	//stuff that only happens client side
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);
	if (PC && IsLocallyControlled())
	{
		PC->EventCheckUse();
	}

	/*if (Role < ROLE_Authority)
	{
		ServerUse();
		return;
	}*/
}

void AOrionLobbyPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAxis("Turn", this, &AOrionCharacter::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AOrionCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &AOrionCharacter::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AOrionCharacter::LookUpAtRate);

	InputComponent->BindAxis("MoveForward", this, &AOrionCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AOrionCharacter::MoveRight);

	////InputComponent->BindAction("Reload", IE_Pressed, this, &AOrionCharacter::Reload);
	////InputComponent->BindAction("Reload", IE_Released, this, &AOrionTopDownPawn::ReleaseReload);

	////InputComponent->BindAction("Melee", IE_Pressed, this, &AOrionCharacter::DoMelee);
	////InputComponent->BindAction("Melee", IE_Released, this, &AOrionCharacter::EndMelee);

	////InputComponent->BindAction("Roll", IE_Pressed, this, &AOrionTopDownPawn::PressRoll);
	////InputComponent->BindAction("Blink", IE_Pressed, this, &AOrionTopDownPawn::CheckForBlink);

	InputComponent->BindAction("Run", IE_Pressed, this, &AOrionCharacter::Sprint);
	InputComponent->BindAction("Run", IE_Released, this, &AOrionCharacter::StopSprint);

	////InputComponent->BindAction("Fire", IE_Pressed, this, &AOrionCharacter::OnFire);
	////InputComponent->BindAction("Fire", IE_Released, this, &AOrionCharacter::OnStopFire);
	////InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AOrionCharacter::TouchStarted);

	InputComponent->BindAction("Aim", IE_Pressed, this, &AOrionLobbyPawn::StartAiming);
	InputComponent->BindAction("Aim", IE_Released, this, &AOrionLobbyPawn::StopAiming);

	InputComponent->BindAction("Use", IE_Pressed, this, &AOrionLobbyPawn::Use);

	////InputComponent->BindAction("BehindView", IE_Pressed, this, &AOrionCharacter::BehindView);

	////InputComponent->BindAction("NextWeapon", IE_Pressed, this, &AOrionCharacter::OnNextWeapon);
	////InputComponent->BindAction("PrevWeapon", IE_Pressed, this, &AOrionCharacter::OnPrevWeapon);
	////InputComponent->BindAction("LastWeapon", IE_Pressed, this, &AOrionCharacter::OnNextWeapon);

	////InputComponent->BindAction("WeaponSlot1", IE_Pressed, this, &AOrionTopDownPawn::OnPrimaryWeapon);
	////InputComponent->BindAction("WeaponSlot2", IE_Pressed, this, &AOrionTopDownPawn::OnSecondaryWeapon);
	////InputComponent->BindAction("WeaponSlot3", IE_Pressed, this, &AOrionTopDownPawn::OnGadget);

	////InputComponent->BindAction("Gamepad_WeaponSlot3", IE_Pressed, this, &AOrionTopDownPawn::OnGadget);

	////InputComponent->BindAction("ActivateSkill", IE_Pressed, this, &AOrionCharacter::TryToActivateSkill);
	////InputComponent->BindAction("ThrowGrenade", IE_Pressed, this, &AOrionCharacter::TossGrenade);

	//chat is only for the controller impaired
	InputComponent->BindAction("Say", IE_Pressed, this, &AOrionLobbyPawn::Say);
	InputComponent->BindAction("TeamSay", IE_Pressed, this, &AOrionLobbyPawn::TeamSay);

	//controller inputs
	InputComponent->BindAxis("Gamepad_MoveForward", this, &AOrionCharacter::MoveForward);
	InputComponent->BindAxis("Gamepad_MoveRight", this, &AOrionCharacter::MoveRight);

	////InputComponent->BindAction("Gamepad_Reload", IE_Pressed, this, &AOrionCharacter::Reload);
	////InputComponent->BindAction("Gamepad_Reload", IE_Released, this, &AOrionTopDownPawn::ReleaseReload);

	////InputComponent->BindAction("Gamepad_Melee", IE_Pressed, this, &AOrionCharacter::DoMelee);
	////InputComponent->BindAction("Gamepad_Melee", IE_Released, this, &AOrionCharacter::EndMelee);

	////InputComponent->BindAction("Gamepad_Roll", IE_Pressed, this, &AOrionTopDownPawn::PressRoll);
	////InputComponent->BindAction("Gamepad_Blink", IE_Pressed, this, &AOrionTopDownPawn::CheckForBlink);

	InputComponent->BindAction("Gamepad_Run", IE_Pressed, this, &AOrionCharacter::GamepadSprint);

	////InputComponent->BindAction("Gamepad_Roll", IE_Pressed, this, &AOrionLobbyPawn::Use);
	////InputComponent->BindAction("Gamepad_Fire", IE_Released, this, &AOrionCharacter::OnStopFire);
	////InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AOrionCharacter::TouchStarted);

	InputComponent->BindAction("Gamepad_Aim", IE_Pressed, this, &AOrionLobbyPawn::StartAiming);
	InputComponent->BindAction("Gamepad_Aim", IE_Released, this, &AOrionLobbyPawn::StopAiming);

	////InputComponent->BindAction("Gamepad_Use", IE_Pressed, this, &AOrionCharacter::Use);

	////InputComponent->BindAction("Gamepad_LastWeapon", IE_Pressed, this, &AOrionCharacter::OnNextWeapon);
	//InputComponent->BindAction("Gamepad_PrevWeapon", IE_Pressed, this, &AOrionCharacter::OnPrevWeapon);

	////InputComponent->BindAction("Gamepad_ActivateSkill", IE_Pressed, this, &AOrionCharacter::TryToActivateSkill);
	////InputComponent->BindAction("Gamepad_ThrowGrenade", IE_Pressed, this, &AOrionCharacter::TossGrenade);
}

void AOrionLobbyPawn::StartAiming()
{
	if (ShouldIgnoreControls())
		return;

	if (Role < ROLE_Authority)
		ServerAim(true);

	StopSprint();

	bAim = true;
}

void AOrionLobbyPawn::StopAiming()
{
	if (Role < ROLE_Authority)
		ServerAim(false);

	bAim = false;
}

void AOrionLobbyPawn::Say()
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (PC && !PC->bMenuOpen)
		PC->EventSay();
}

void AOrionLobbyPawn::TeamSay()
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (PC && !PC->bMenuOpen)
		PC->EventTeamSay();
}
