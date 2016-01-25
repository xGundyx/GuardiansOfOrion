// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "OrionPickupOrb.generated.h"

UCLASS()
class ORION_API AOrionPickupOrb : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOrionPickupOrb(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class USphereComponent* SphereCollision;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintCallable, Category = Orb)
		void Init();

	UFUNCTION()
		void OnRep_OrbType();
	
	UPROPERTY(ReplicatedUsing = OnRep_OrbType, BlueprintReadOnly, Category = Spawn)
		TEnumAsByte<EOrbType> OrbType;

	UFUNCTION(BlueprintImplementableEvent, Category = Orb)
		void EventInitOrb();

	//for coins
	UPROPERTY(BlueprintReadOnly, Category = Mesh)
		int32 CoinAmount;
};
