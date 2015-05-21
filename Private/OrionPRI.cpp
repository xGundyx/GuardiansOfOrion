

#include "Orion.h"
#include "OrionPRI.h"


AOrionPRI::AOrionPRI(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void AOrionPRI::SeamlessTravelTo(APlayerState * NewPlayerState)
{
	Super::SeamlessTravelTo(NewPlayerState);

	//we want to copy all playfab data over so we don't have to redownload it

}

void AOrionPRI::CopyProperties(class APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	if (Role == ROLE_Authority)
	{
		AOrionPRI *PRI = Cast<AOrionPRI>(PlayerState);
		//PRI->InventoryManager = InventoryManager;
		PRI->PlayFabID = PlayFabID;
		PRI->SessionTicket = SessionTicket;
		PRI->CharacterID = CharacterID;
	}
}

void AOrionPRI::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AOrionPRI, InventoryManager, COND_OwnerOnly);
}

void AOrionPRI::OnRep_InventoryManager()
{
	//if (InventoryManager)
	//	InventoryManager->Init();
}
