// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "OrionWeapon.h"
#include "OrionWeaponLink.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API AOrionWeaponLink : public AOrionWeapon
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = Effects)
		UParticleSystem* BeamFX;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
		UParticleSystem* SelfBeamFX;

	UPROPERTY(Transient)
		UParticleSystemComponent* BeamPSC;

	UFUNCTION()
		void OnRep_LinkTarget();

	UPROPERTY(ReplicatedUsing = OnRep_LinkTarget, BlueprintReadWrite, Category = Link)
		class ACharacter *LinkTarget;

	virtual void StartFire() override;
	virtual void StopFire() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void StartAiming();
	virtual void StopAiming();

	void StartBeamEffects();
	void StopBeamEffects();

	void ValidateLinkTarget(float DeltaSeconds);
	bool CanLink(ACharacter *Target);
	void HandleTarget(float DeltaSeconds);

	virtual float OnUnEquip() override;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
		USoundCue* StartSound;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
		USoundCue* LoopSound;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
		USoundCue* EndSound;

	UAudioComponent *LoopAudio;
};
