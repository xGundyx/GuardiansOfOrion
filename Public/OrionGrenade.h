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

	UFUNCTION(BlueprintCallable, Category = Grenade)
		void Init(FVector dir);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Grenade)
		TEnumAsByte<EVoiceType> VoiceType;

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
		float FXTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Grenade)
		float ExplosionScale;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = Grenade)
		bool bIsMiniGrenade;

	UPROPERTY(Replicated)
		float GrenadeLife;

	UPROPERTY(Replicated)
		float GrenadeScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Grenade)
		FString GrenadeName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
		class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(BlueprintReadWrite, Category = Grenade)
		TEnumAsByte<EGrenadeType> SecondaryGrenadeType;

	UPROPERTY(BlueprintReadWrite, Category = Grenade)
		bool bMultiGrenade;

	virtual void Tick(float DeltaSeconds) override;
	virtual void Destroyed() override;
	void GoBoom();

	bool bCanSpawnExplosionGrenade;

	UFUNCTION(BlueprintCallable, Category = Grenade)
		void SetFuseTime(float FuseTime);

	UFUNCTION()
		void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Explode"))
		void EventExplode();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "SecondaryExplode"))
		void EventSpawnExplosionGrenade();

	void Explode();
private:

};