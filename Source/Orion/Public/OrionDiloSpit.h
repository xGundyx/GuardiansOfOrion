// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "OrionProjectile.h"
#include "OrionDiloSpit.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API AOrionDiloSpit : public AOrionProjectile
{
	GENERATED_BODY()
	
public:
	void ValidatePosition() override;

	UFUNCTION()
		void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	
	UPROPERTY(BlueprintReadWrite, Category = Buff)
		int32 TeamIndex;
};
