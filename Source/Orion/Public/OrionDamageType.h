

#pragma once

#include "GameFramework/DamageType.h"
#include "OrionDamageType.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API UOrionDamageType : public UDamageType
{
	GENERATED_BODY()
public:
	UOrionDamageType(const FObjectInitializer& ObjectInitializer);

	/** force feedback effect to play on a player hit by this damage type */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
		UForceFeedbackEffect *HitForceFeedback;

	/** force feedback effect to play on a player killed by this damage type */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
		UForceFeedbackEffect *KilledForceFeedback;

	UPROPERTY(EditDefaultsOnly, Category = Orion)
		bool bKnockBack;

	UPROPERTY(EditDefaultsOnly, Category = Orion)
		float KnockbackForce;

	UPROPERTY(EditDefaultsOnly, Category = Orion)
		FName KnockbackSocket;

	UPROPERTY(EditDefaultsOnly, Category = Orion)
		float KnockbackUpForce;

	UPROPERTY(EditDefaultsOnly, Category = Orion)
		bool bIgnoreModify;

	UPROPERTY(EditDefaultsOnly, Category = Orion)
		bool bGibAll;

	UPROPERTY(EditDefaultsOnly, Category = Orion)
		bool bIsKnife;

	UPROPERTY(EditDefaultsOnly, Category = Orion)
		bool bIsBullet;

	UPROPERTY(EditDefaultsOnly, Category = Orion)
		FString WeaponName;

	UPROPERTY(EditDefaultsOnly, Category = Orion)
		TEnumAsByte<EDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, Category = Orion)
		int32 WeaponSlot;

	UPROPERTY(EditDefaultsOnly, Category = Orion)
		TEnumAsByte<EWeaponType> WeaponType;
};
