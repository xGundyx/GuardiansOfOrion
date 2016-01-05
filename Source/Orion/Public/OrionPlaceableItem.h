// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "OrionPlaceableItem.generated.h"

UCLASS()
class ORION_API AOrionPlaceableItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOrionPlaceableItem(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class USkeletalMeshComponent* ItemMesh;
	
	UFUNCTION(BlueprintCallable, Category = Item)
		FVector2D GetAim(FVector WorldAim);
};
