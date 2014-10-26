

#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "OrionDamageNotify.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API UOrionDamageNotify : public UAnimNotify
{
	GENERATED_UCLASS_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
