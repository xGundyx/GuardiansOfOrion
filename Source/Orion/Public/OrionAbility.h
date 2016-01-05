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

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ShowTarget"))
		void EventShowTarget();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "HideTarget"))
		void EventHideTarget();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "TriggerTarget"))
		void EventTriggerTarget();

	void TriggerTarget(AOrionCharacter *Pawn);

	UFUNCTION(Reliable, server, WithValidation)
		void ServerTriggerTarget(AOrionPlayerController *PC, FVector pos, FRotator rot);
		bool ServerTriggerTarget_Validate(AOrionPlayerController *PC, FVector pos, FRotator rot) { return true; }
		void ServerTriggerTarget_Implementation(AOrionPlayerController *PC, FVector pos, FRotator rot);

	UPROPERTY(EditDefaultsOnly, Category = Skill)
		TSubclassOf<class AOrionPlaceableItem>PlaceableItemClass;

	UFUNCTION(BlueprintCallable, Category = Skill)
		bool IsJetpacking() { return Cast<AOrionCharacter>(GetOwner()) && Cast<AOrionCharacter>(GetOwner())->Health > 0.0f && bIsJetpacking; }

	UFUNCTION(BlueprintCallable, Category = Skill)
		bool IsCloaking() { return Cast<AOrionCharacter>(GetOwner()) && Cast<AOrionCharacter>(GetOwner())->Health > 0.0f && bIsCloaking; }

	UFUNCTION(BlueprintCallable, Category = Skill)
		bool IsOvercharging() { return Cast<AOrionCharacter>(GetOwner()) && Cast<AOrionCharacter>(GetOwner())->Health > 0.0f && bIsOvercharging; }

	UFUNCTION(BlueprintCallable, Category = Skill)
		bool IsTurreting() { return Cast<AOrionCharacter>(GetOwner()) && Cast<AOrionCharacter>(GetOwner())->Health > 0.0f && bIsTurreting; }

	UFUNCTION(BlueprintCallable, Category = Skill)
		void SetJetpacking(bool bActive) { bIsJetpacking = bActive; }

	UFUNCTION(BlueprintCallable, Category = Skill)
		void SetCloaking(bool bActive) { bIsCloaking = bActive; }

	UFUNCTION(BlueprintCallable, Category = Skill)
		void SetOvercharging(bool bActive) { bIsOvercharging = bActive; }

	UFUNCTION(BlueprintCallable, Category = Skill)
		void SetTurreting(bool bActive) { bIsTurreting = bActive; }

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Skill)
		bool bTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Skill)
		bool bTargetting;

	void SpawnItem(AOrionPlayerController *PC, FVector pos, FRotator rot);

	void DepleteEnergy() { Energy = 0.0f; }

	void SetEnergy(float NewEnergy) { Energy = NewEnergy; }
	float GetEnergy() { return Energy; }

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

	//active when we have the turret placer visible
	bool bIsTurreting;
};