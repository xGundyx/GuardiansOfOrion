

#pragma once

#include "GameFramework/DamageType.h"
#include "OrionDamageType.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API UOrionDamageType : public UDamageType
{
	GENERATED_UCLASS_BODY()

	/** force feedback effect to play on a player hit by this damage type */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	UForceFeedbackEffect *HitForceFeedback;

	/** force feedback effect to play on a player killed by this damage type */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	UForceFeedbackEffect *KilledForceFeedback;
};
