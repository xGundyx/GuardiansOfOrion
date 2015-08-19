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
}


