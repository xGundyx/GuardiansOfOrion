// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "OrionAOEVolume.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class ORION_API AOrionAOEVolume : public AActor
{
	GENERATED_BODY()
	
public:
	AOrionAOEVolume(const FObjectInitializer& ObjectInitializer);

	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;

	//sphere component to use for a volume
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AOE)
		class USphereComponent* SphereMesh;

	//particle effect to play inside
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AOE)
		UParticleSystem *AOEEffect;

	//pointer to the actual component
	UParticleSystemComponent *AOEPSC;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AOE)
		float Radius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AOE)
		float DamagePerTick;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AOE)
		bool bCauseSlow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AOE)
		float AOEScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AOE)
		float Duration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AOE)
		TSubclassOf<class AOrionBuff> BuffClass;

	UFUNCTION()
		void OnEnter(AActor *Other, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &Hit);

	UFUNCTION()
		void OnLeave(AActor *Other, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex);

	TArray<AOrionCharacter*> Players;
	void ReTickPlayers();
	void DestroyAOE();

	UPROPERTY(BlueprintReadWrite, Category = AOE)
		int32 TeamIndex;

};
