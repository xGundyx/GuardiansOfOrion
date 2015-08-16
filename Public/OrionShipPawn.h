// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Orion.h"
#include "OrionShipPawn.generated.h"

UCLASS()
class ORION_API AOrionShipPawn : public AOrionCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AOrionShipPawn(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	//view when spawning
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		TArray<class UCameraComponent*> SpawnCameraComponent;

	UFUNCTION(BlueprintCallable, Category = Pawn)
		void UnloadPlayers();

	void AddPassenger(AOrionPlayerController *PC, AOrionCharacter *Pawn);

	UPROPERTY(BlueprintReadOnly, Category = Spawn)
		int32 TeamIndex;

	UPROPERTY(BlueprintReadOnly, Category = Spawn)
		bool Coop;

	UPROPERTY(BlueprintReadOnly, Category = Spawn)
		bool PVP;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Spawned"))
		void EventSpawnShip(int32 Team);

	int32 GetNumPassengers() { return Passengers.Num(); }

	UPROPERTY(BlueprintReadWrite, Category = Camera)
		FVector CameraLocation;

	UPROPERTY(BlueprintReadWrite, Category = Camera)
		FRotator CameraRotation;

private:
	TArray<AOrionCharacter*> Passengers;
};
