// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "OrionCharacter.h"
#include "OrionHoverVehicle.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API AOrionHoverVehicle : public APawn
{
	GENERATED_BODY()
	
public:
	AOrionHoverVehicle(const FObjectInitializer& ObjectInitializer);

	void MoveForward(float Val);
	void MoveRight(float Val);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void Boost();

	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;
	virtual void Tick(float DeltaSeconds) override;

	void DriverLeave();
	void Use();
	UFUNCTION(reliable, server, WithValidation)
		void ServerUse();
		bool ServerUse_Validate();
		void ServerUse_Implementation();

protected:
	void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class USphereComponent* BaseSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class UStaticMeshComponent* CollisionMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class USkeletalMeshComponent* VehicleMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class UCameraComponent* VehicleCameraComponent;

	UPROPERTY(EditAnywhere, Category = Camera)
		float BaseTurnRate;

	UPROPERTY(EditAnywhere, Category = Camera)
		float BaseLookUpRate;

	UPROPERTY(EditAnywhere, Category = Camera)
		float CameraDistance;

	void TryToGetIn(AOrionCharacter *newDriver);
		AOrionCharacter *GetDriver();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Health)
		float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Health)
		float HealthMax;

private:
	FVector AccelForwards;
	FVector AccelRight;
	FVector AccelUp;
	FVector AccumulatedVelocity;

	UPROPERTY(Replicated)
		AOrionCharacter *Driver;
};
