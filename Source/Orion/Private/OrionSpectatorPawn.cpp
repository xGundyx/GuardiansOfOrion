// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionSpectatorPawn.h"

AOrionSpectatorPawn::AOrionSpectatorPawn()
{
	CameraFOV = 90.0f;
	CameraSpeed = 750.0f;
}

void AOrionSpectatorPawn::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	//for now, just view through the playercontroller's camera
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (PC && PC->OverviewCamera)
		PC->OverviewCamera->GetCameraView(DeltaTime, OutResult);
	else if (SpecViewTarget && SpecViewTarget->ControlledPawn)
		return SpecViewTarget->ControlledPawn->CalcCamera(DeltaTime, OutResult);
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

		SpecCameraLocation += X * CameraOffset.X + Y * CameraOffset.Y += FVector(0.0f, 0.0f, 1.0f) * CameraOffset.Z;
		OutResult.Location = SpecCameraLocation;

		CameraOffset = FVector(0.0f);

		OutResult.FOV = CameraFOV;
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

	InputComponent->BindAction("Fire", IE_Pressed, this, &AOrionSpectatorPawn::OnFire);
	InputComponent->BindAction("Aim", IE_Pressed, this, &AOrionSpectatorPawn::OnAim);
	InputComponent->BindAction("Reload", IE_Pressed, this, &AOrionSpectatorPawn::OnReload);
	InputComponent->BindAction("ActivateSkill", IE_Pressed, this, &AOrionSpectatorPawn::OnAbility);

	InputComponent->BindAction("Gamepad_Fire", IE_Pressed, this, &AOrionSpectatorPawn::OnFire);
	InputComponent->BindAction("Gamepad_Aim", IE_Pressed, this, &AOrionSpectatorPawn::OnAim);
	InputComponent->BindAction("Gamepad_Reload", IE_Pressed, this, &AOrionSpectatorPawn::OnReload);
	InputComponent->BindAction("Gamepad_ActivateSkill", IE_Pressed, this, &AOrionSpectatorPawn::OnAbility);

	InputComponent->BindAction("Gamepad_Roll", IE_Pressed, this, &AOrionSpectatorPawn::CameraUp);
	InputComponent->BindAction("Gamepad_Blink", IE_Pressed, this, &AOrionSpectatorPawn::CameraDown);
}

void AOrionSpectatorPawn::OnFire()
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (PC)
	{
		if (PC->bDaveyCam)
		{
			//increase camera speed
			CameraSpeed = FMath::Min(3000.0f, CameraSpeed += 250.0f);
		}
		//spec next player
		else
			GetNextSpecTarget(true);

	}
}

void AOrionSpectatorPawn::OnAim()
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (PC)
	{
		if (PC->bDaveyCam)
		{
			//decrease camera speed
			CameraSpeed = FMath::Max(250.0f, CameraSpeed -= 250.0f);
		}
		//spec prev player
		else
			GetNextSpecTarget(false);
	}
}

void AOrionSpectatorPawn::GetNextSpecTarget(bool bForwards)
{
	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);

	if (GRI)
	{
		int32 CurrentIndex = 0;

		for (int32 i = 0; i < GRI->PlayerList.Num(); i++)
		{
			if (GRI->PlayerList[i] == PlayerState)
				continue;

			if (SpecViewTarget == GRI->PlayerList[i])
			{
				CurrentIndex = i;
				break;
			}
		}

		for (int32 i = CurrentIndex + (bForwards ? 1 : -1); i < GRI->PlayerList.Num() && i >= 0; (bForwards ? i++ : i--))
		{
			if (GRI->PlayerList[i] && GRI->PlayerList[i]->IsValidLowLevel() && GRI->PlayerList[i]->ControlledPawn)
			{
				SpecViewTarget = GRI->PlayerList[i];
				return;
			}
		}

		for (int32 i = (bForwards ? 0 : GRI->PlayerList.Num() - 1); i < CurrentIndex; (bForwards ? i++ : i--))
		{
			if (GRI->PlayerList[i] && GRI->PlayerList[i]->IsValidLowLevel() && GRI->PlayerList[i]->ControlledPawn)
			{
				SpecViewTarget = GRI->PlayerList[i];
				return;
			}
		}
	}
}

void AOrionSpectatorPawn::OnReload()
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (PC)
	{
		if (PC->bDaveyCam)
		{
			//zoom in
			CameraFOV = FMath::Min(100.0f, CameraFOV + 5.0f);
		}
	}
}

void AOrionSpectatorPawn::OnAbility()
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (PC)
	{
		if (PC->bDaveyCam)
		{
			//zoom out
			CameraFOV = FMath::Max(20.0f, CameraFOV - 5.0f);
		}
	}
}

void AOrionSpectatorPawn::TurnAtRate(float Rate)
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (PC)
	{
		if (PC->bDaveyCam)
		{
			// calculate delta for this frame from the rate information
			AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
		}
	}
}

void AOrionSpectatorPawn::LookUpAtRate(float Rate)
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (PC)
	{
		if (PC->bDaveyCam)
		{
			// calculate delta for this frame from the rate information
			AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
		}
	}
}

void AOrionSpectatorPawn::MoveForward(float Value)
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (PC)
	{
		if (PC->bDaveyCam)
		{
			CameraOffset.X += Value * GetWorld()->DeltaTimeSeconds * CameraSpeed;
		}
	}
}

void AOrionSpectatorPawn::MoveRight(float Value)
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (PC)
	{
		if (PC->bDaveyCam)
		{
			CameraOffset.Y += Value * GetWorld()->DeltaTimeSeconds * CameraSpeed;
		}
	}
}

void AOrionSpectatorPawn::CameraUp()
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (PC)
	{
		if (PC->bDaveyCam)
		{
			CameraOffset.Z += GetWorld()->DeltaTimeSeconds * CameraSpeed;
		}
	}
}

void AOrionSpectatorPawn::CameraDown()
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (PC)
	{
		if (PC->bDaveyCam)
		{
			CameraOffset.Z -= GetWorld()->DeltaTimeSeconds * CameraSpeed;
		}
	}
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


