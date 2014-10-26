

#include "Orion.h"
#include "OrionDamageNotify.h"


UOrionDamageNotify::UOrionDamageNotify(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}

void UOrionDamageNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
}


