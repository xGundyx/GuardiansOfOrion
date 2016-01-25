// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "OrionWeapon.h"
#include "OrionWeaponLink.generated.h"

USTRUCT()
struct FChainLinkTarget
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		UParticleSystemComponent* BeamPSC;

	UPROPERTY()
		class AOrionCharacter *LinkTarget;

	FChainLinkTarget()
	{
		LinkTarget = nullptr;
		BeamPSC = nullptr;
	}

	bool operator==(const AOrionCharacter *Other) const { return Other == LinkTarget; }
	bool operator==(const FChainLinkTarget Other) const { return Other.LinkTarget == LinkTarget; }
};

/**
 * 
 */
UCLASS()
class ORION_API AOrionWeaponLink : public AOrionWeapon
{
	GENERATED_BODY()
	
public:
	virtual void Destroyed() override;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
		UParticleSystem* BeamFX;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
		UParticleSystem* SelfBeamFX;

	UPROPERTY(Transient)
		UParticleSystemComponent* BeamPSC;

	UFUNCTION()
		void OnRep_ChainLinks();

	void HandleChainTargets(float DeltaSeconds);
	void UpdateChainBeamEffects();
	void ActivateChainBeam(FChainLinkTarget Target);
	void StopAllChainBeams();
	void HandleLinkTargets(AOrionCharacter *Target, float DeltaSeconds);

	float LastChainTime;

	UPROPERTY(ReplicatedUsing = OnRep_ChainLinks)
		TArray<AOrionCharacter*> ChainLinkTargets;

	TArray<FChainLinkTarget> ChainBeamPSC;

	UFUNCTION()
		void OnRep_LinkTarget();

	UPROPERTY(ReplicatedUsing = OnRep_LinkTarget, BlueprintReadWrite, Category = Link)
		class ACharacter *LinkTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
		TSubclassOf<class AOrionBuff> FireBuff;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
		TSubclassOf<class AOrionBuff> IceBuff;

	void ApplyDamageDebuff(AOrionCharacter *DamagedPawn);

	virtual void StartFire() override;
	virtual void StopFire() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void StartAiming(bool bPlaySound = false);
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
