#pragma once

#include "GameFramework/Actor.h"
#include "OrionCharacter.h"
#include "OrionGrenade.generated.h"

UCLASS()
class ORION_API AOrionGrenade : public AActor
{
	GENERATED_BODY()
public:
	AOrionGrenade(const FObjectInitializer& ObjectInitializer);

	void Init(FVector dir);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Grenade)
		UParticleSystem* ExplosionFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Grenade)
		UParticleSystem* TrailFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Grenade)
		UStaticMeshComponent* GrenadeMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Grenade)
		USoundCue* ExplosionSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Grenade)
		float Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Grenade)
		float DamageRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Grenade)
		float LifeTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Grenade)
		float ExplosionScale;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
		class UProjectileMovementComponent* ProjectileMovement;

	virtual void Tick(float DeltaSeconds) override;
	virtual void Destroyed() override;
	void GoBoom();

	UFUNCTION()
		void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Explode"))
		void EventExplode();

	void Explode();
private:

};