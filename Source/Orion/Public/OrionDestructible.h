// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "OrionDestructible.generated.h"

class UDestructibleComponent;

UCLASS()
class ORION_API AOrionDestructible : public ADestructibleActor //AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOrionDestructible(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual void PostInitializeComponents() override;

	//virtual void EditorReplacedActor(AActor * OldActor) override;

	//USkeletalMesh *Skel;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Destruction)
	//	UDestructibleComponent* DestructibleComponent;

	//UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "CopyProperties"))
	//	void EventCopyProperties(AActor *Other);
};
