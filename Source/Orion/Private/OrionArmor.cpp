

#include "Orion.h"
#include "OrionArmor.h"


AOrionArmor::AOrionArmor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	USceneComponent *SceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("WeaponComponent"));
	RootComponent = SceneComponent;

	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	//bReplicateInstigator = true;
	bNetUseOwnerRelevancy = true;
}

void AOrionArmor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOrionArmor, Mesh);
	DOREPLIFETIME(AOrionArmor, Mesh1P);
}


