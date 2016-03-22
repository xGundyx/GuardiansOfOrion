// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "OrionCharacter.h"
#include "OrionLobbyPawn.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API AOrionLobbyPawn : public AOrionCharacter
{
	GENERATED_BODY()
	
public:
	AOrionLobbyPawn(const FObjectInitializer& ObjectInitializer);

	bool IsFirstPerson() const override;
	virtual void BeginPlay() override;
	virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult) override;
	
	void Say();
	void TeamSay();

	virtual void StartAiming() override;
	virtual void StopAiming() override;

	void Use();

protected:
	/*void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void AddControllerYawInput(float Rate);
	void AddControllerPitchInput(float Rate);*/

	//virtual void FaceRotation(FRotator NewControlRotation, float DeltaTime = 0.f) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
};
