

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
	UOrionDamageType(const FObjectInitializer& ObejctInitializer);

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
};