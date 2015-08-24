// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionSpectatorPawn.h"

void AOrionSpectatorPawn::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	//for now, just view through the playercontroller's camera
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (PC && PC->OverviewCamera)
		PC->OverviewCamera->GetCameraView(DeltaTime, OutResult);
	else if (PC && PC->Ragdoll && PC->Ragdoll->IsValidLowLevel())
		PC->Ragdoll->CalcCamera(DeltaTime, OutResult);
	else
		Super::CalcCamera(DeltaTime, OutResult);

	if (PC && PC->bDaveyCam)
	{
		if (Controller)
			OutResult.Rotation = Controller->GetControlRotation();

		FVector X = OutResult.Rotation.Vector();
		FVector Z = FVector(0.0f, 0.0f, 1.0f);
		FVector Y = FVector::CrossProduct(Z, X).GetSafeNormal();

		SpecCameraLocation += X * CameraOffset.X + Y * CameraOffset.Y;
		OutResult.Location = SpecCameraLocation;

		CameraOffset = FVector(0.0f);
	}
}

void AOrionSpectatorPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAction("Say", IE_Pressed, this, &AOrionSpectatorPawn::Say);
	InputComponent->BindAction("TeamSay", IE_Pressed, this, &AOrionSpectatorPawn::TeamSay);

	InputComponent->BindAxis("MoveForward", this, &AOrionSpectatorPawn::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AOrionSpectatorPawn::MoveRight);

	InputComponent->BindAxis("Gamepad_MoveForward", this, &AOrionSpectatorPawn::MoveForward);
	InputComponent->BindAxis("Gamepad_MoveRight", this, &AOrionSpectatorPawn::MoveRight);

	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AOrionSpectatorPawn::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AOrionSpectatorPawn::LookUpAtRate);
}

void AOrionSpectatorPawn::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AOrionSpectatorPawn::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AOrionSpectatorPawn::MoveForward(float Value)
{
	CameraOffset.X += Value * GetWorld()->DeltaTimeSeconds * 500.0f;
}

void AOrionSpectatorPawn::MoveRight(float Value)
{
	CameraOffset.Y += Value * GetWorld()->DeltaTimeSeconds * 500.0f;
}

void AOrionSpectatorPawn::Say()
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (PC)
		PC->EventSay();
}

void AOrionSpectatorPawn::TeamSay()
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (PC)
		PC->EventTeamSay();
}


