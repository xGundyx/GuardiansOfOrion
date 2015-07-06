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

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "StartEffects"))
		void EventPlayStartEffects();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "StopEffects"))
		void EventPlayStopEffects();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Skill)
		bool bOneShotAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Skill)
		float OneShotEnergyCost;

private:
	void DoActivateEffects();
	void DoDeactivateEffects();

	UFUNCTION()
		void OnRep_SkillActive();

	UPROPERTY(ReplicatedUsing = OnRep_SkillActive)
		bool bIsSkillActive;

	UPROPERTY(Replicated)
		float Energy;

	float RechargeRate;
	float EnergyRate;
	float TimeSinceLastActive;
	float RechargeDelay;
};