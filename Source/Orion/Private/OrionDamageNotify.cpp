

#include "Orion.h"
#include "OrionDamageNotify.h"


UOrionDamageNotify::UOrionDamageNotify(const FObjectInitializer& ObejctInitializer)
	: Super(ObejctInitializer)
{

}

void UOrionDamageNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
}


