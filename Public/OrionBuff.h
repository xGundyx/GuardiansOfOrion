#pragma once

#include "GameFramework/Actor.h"
#include "OrionBuff.generated.h"

UCLASS(BlueprintType)
class ORION_API AOrionBuff : public AActor
{
	GENERATED_BODY()

public:
	AOrionBuff(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Buff)
		class UTexture2D *Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Buff)
		FString BuffName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Buff)
		float Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Buff)
		float HealPercent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Buff)
		float DamageReduction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Buff)
		bool bAffectsEnemies;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Buff)
		bool bAffectsTeammates;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Buff)
		float SpeedMultiplier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Buff)
		float Duration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Buff)
		float TickInterval;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = Buff)
		class UParticleSystem *Effect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Buff)
		bool bStackable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Buff)
		int32 StackLimit;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Buff)
		TSubclassOf<UDamageType> DamageType;

	UPROPERTY(BlueprintReadOnly, Category = Buff)
		int32 NumStacks;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Buff)
		bool bIsBuff;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Buff)
		bool bBlockSight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Buff)
		int32 UpgradeLevel;

	UPROPERTY(BlueprintReadOnly, Category = Buff)
		float LastRefreshedTime;

	UPROPERTY(BlueprintReadOnly, Category = Buff)
		float TickTimer;

	UPROPERTY(BlueprintReadOnly, Category = Buff)
		AController *ControllerOwner;

	UPROPERTY(BlueprintReadWrite, Category = Buff)
		int32 TeamIndex;
};