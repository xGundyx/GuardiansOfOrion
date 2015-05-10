// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "OrionPickup.generated.h"

UCLASS()
class ORION_API AOrionPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOrionPickup(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class UStaticMeshComponent* PickupMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class UBoxComponent* BoxCollision;

	//search the current loot table for some drop to assign us
	bool Init(UClass *LootTable, int32 Level);

	FString EncodedValue;
	FDecodeItemInfo Decoder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item)
		AOrionInventory *Inventory;
};
