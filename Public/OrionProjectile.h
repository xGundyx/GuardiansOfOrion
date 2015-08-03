// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Actor.h"
#include "OrionProjectile.generated.h"

UCLASS(config=Game)
class ORION_API AOrionProjectile : public AActor
{
	GENERATED_BODY()
public:
	AOrionProjectile(const FObjectInitializer& ObjectInitializer);

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	class USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement)
	class UProjectileMovementComponent* ProjectileMovement;

	virtual void Tick(float DeltaSeconds) override;

	/** called when projectile hits something */
	UFUNCTION()
		virtual void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	//UPROPERTY(EditDefaultsOnly, Category = Effects)
	//	UParticleSystem* TracerFX;

	virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintCallable, Category = Projectile)
		void Init(UParticleSystem* PS, FVector StartPoint, FVector EndPoint);

	virtual void ValidatePosition();


	UPROPERTY()
		UParticleSystemComponent* TracerPSC;

	UPROPERTY()
		FVector TargetPos;

	UPROPERTY()
		FVector StartPos;

	UPROPERTY()
		FVector TargetDir;
};

