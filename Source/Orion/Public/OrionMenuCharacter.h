// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "OrionMenuCharacter.generated.h"

UCLASS()
class ORION_API AOrionMenuCharacter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOrionMenuCharacter(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = Spawn)
		int32 PlayerIndex;

	UFUNCTION(BlueprintCallable, Category = Placer)
		void UpdateData(FCharacterData Data);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "SetArmor"))
		void EventSetArmor(EItemType Slot, UOrionInventoryItem *Item);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "SetWeapon"))
		void EventSetWeapon(UOrionInventoryItem *Item);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class USkeletalMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class USkeletalMeshComponent* ArmsMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class USkeletalMeshComponent* HelmetMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class USkeletalMeshComponent* ChestMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class USkeletalMeshComponent* LegsMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class USkeletalMeshComponent* WeaponMesh;
};
