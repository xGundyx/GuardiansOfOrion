

#include "Orion.h"
#include "OrionDamageNotify.h"


UOrionDamageNotify::UOrionDamageNotify(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UOrionDamageNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
}


