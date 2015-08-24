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

	//override this so we can make bullets and grenades to a little more damage to them
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	//virtual void EditorReplacedActor(AActor * OldActor) override;

	//USkeletalMesh *Skel;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Destruction)
	//	UDestructibleComponent* DestructibleComponent;

	//UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "CopyProperties"))
	//	void EventCopyProperties(AActor *Other);
};
