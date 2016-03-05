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
	UFUNCTION(BlueprintCallable, Category = Pickup)
		bool Init(UClass *LootTable, int32 Level, EItemRarity ForceQuality = RARITY_COMMON);

	UPROPERTY(BlueprintReadOnly, Category = Pickup)
		bool bGlobalItem;

	FString EncodedValue;
	FDecodeItemInfo Decoder;

	UFUNCTION()
		void OnRep_GearType();

	UPROPERTY(ReplicatedUsing = OnRep_GearType, BlueprintReadOnly, Category = Spawn)
		TEnumAsByte<EItemRarity> GearType;

	//called from the client or host to grab the item and let the server know it's been grabbed
	UFUNCTION(BlueprintCallable, Category = Item)
		void ClientGrabItem(AOrionPlayerController *PC);

	void GrabItem(AOrionPlayerController *PC);

	UFUNCTION(reliable, server, WithValidation)
		void ServerGrabItem(AOrionPlayerController *PC);
		bool ServerGrabItem_Validate(AOrionPlayerController *PC);
		void ServerGrabItem_Implementation(AOrionPlayerController *PC);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "SetColor"))
		void EventSetColor(EItemRarity Rarity);
};
