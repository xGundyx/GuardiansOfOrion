#pragma once

#include "GameFramework/Actor.h"
#include "OrionAbility.generated.h"

UCLASS()
class ORION_API AOrionAbility : public AActor
{
	GENERATED_BODY()

public:
	AOrionAbility(const FObjectInitializer& ObjectInitializer);

	void Tick(float DeltaSeconds) override;
	void BeginPlay() override;

	bool ActivateSkill();
	void DeactivateSkill();

	UFUNCTION(blueprintCallable, Category = Skill)
		bool IsSkillActive() { return bIsSkillActive; }

	UFUNCTION(BlueprintCallable, Category = Skill)
		float GetEnergyRemaining() { return Energy; }

	//override this to do ability specific stufff
	void DoSkillAbility(float DeltaSeconds);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "TickAbility"))
		void EventTickAbility(float DeltaSeconds);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "CheckMaterials"))
		void EventCheckMaterials();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "StartEffects"))
		void EventPlayStartEffects();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "StopEffects"))
		void EventPlayStopEffects();

	UFUNCTION(BlueprintCallable, Category = Skill)
		bool IsJetpacking() { return bIsJetpacking; }

	UFUNCTION(BlueprintCallable, Category = Skill)
		bool IsCloaking() { return bIsCloaking; }

	UFUNCTION(BlueprintCallable, Category = Skill)
		bool IsOvercharging() { return bIsOvercharging; }

	UFUNCTION(BlueprintCallable, Category = Skill)
		void SetJetpacking(bool bActive) { bIsJetpacking = bActive; }

	UFUNCTION(BlueprintCallable, Category = Skill)
		void SetCloaking(bool bActive) { bIsCloaking = bActive; }

	UFUNCTION(BlueprintCallable, Category = Skill)
		void SetOvercharging(bool bActive) { bIsOvercharging = bActive; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Skill)
		bool bOneShotAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Skill)
		float OneShotEnergyCost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Skill)
		float RechargeRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Skill)
		float EnergyRate;

	float TimeSinceLastActive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Skill)
		float RechargeDelay;

private:
	void DoActivateEffects();
	void DoDeactivateEffects();

	UFUNCTION()
		void OnRep_SkillActive();

	UPROPERTY(ReplicatedUsing = OnRep_SkillActive)
		bool bIsSkillActive;

	UPROPERTY(Replicated)
		float Energy;

	UPROPERTY(Replicated)
		bool bIsJetpacking;
	UPROPERTY(Replicated)
		bool bIsCloaking;
	UPROPERTY(Replicated)
		bool bIsOvercharging;
};