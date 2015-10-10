#include "Orion.h"
#include "OrionPRI.h"

AOrionPRI::AOrionPRI(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TeamIndex = -1;

	Kills = 0;
	Assists = 0;
	Deaths = 0;
}

void AOrionPRI::SeamlessTravelTo(APlayerState * NewPlayerState)
{
	Super::SeamlessTravelTo(NewPlayerState);

	//we want to copy all playfab data over so we don't have to redownload it

}

int32 AOrionPRI::GetXPIntoLevel()
{
	int XP = 0;

	if (ClassType == "SUPPORT")
		XP = SupportXP;
	else if (ClassType == "ASSAULT")
		XP = AssaultXP;
	else if (ClassType == "RECON")
		XP = ReconXP;
	else
		return 0;

	int32 Ret = XP;
	int32 XPRemaining = XP - BASEXP;
	int32 Level = 1;

	while (XPRemaining >= 0)
	{
		Level++;
		Ret = XPRemaining;
		XPRemaining -= (BASEXP + (XPINCREASE * (Level - 1)));
	}

	return Ret;
}

int32 AOrionPRI::GetXPToLevel()
{
	int XP = 0;

	if (ClassType == "SUPPORT")
		XP = SupportXP;
	else if (ClassType == "ASSAULT")
		XP = AssaultXP;
	else if (ClassType == "RECON")
		XP = ReconXP;
	else
		return 1;

	int32 Level = CalculateLevel(XP);

	return BASEXP + (XPINCREASE * (Level - 1));
}

bool AOrionPRI::IsOnShip()
{
	return bOnShip;
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
		PRI->PlayFabName = PlayFabName;
	}
}

void AOrionPRI::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AOrionPRI, InventoryManager, COND_OwnerOnly);
	DOREPLIFETIME(AOrionPRI, bOnShip);
	DOREPLIFETIME(AOrionPRI, TeamIndex);
	DOREPLIFETIME(AOrionPRI, ControlledPawn);

	//playfab ids and stuff
	DOREPLIFETIME(AOrionPRI, PlayFabID);
	DOREPLIFETIME(AOrionPRI, SessionTicket);
	DOREPLIFETIME(AOrionPRI, CharacterID);
	DOREPLIFETIME(AOrionPRI, PlayFabName);

	//scores
	DOREPLIFETIME(AOrionPRI, Kills);
	DOREPLIFETIME(AOrionPRI, Assists);
	DOREPLIFETIME(AOrionPRI, Deaths);
	DOREPLIFETIME(AOrionPRI, ClassType);

	//exp
	DOREPLIFETIME(AOrionPRI, AssaultXP);
	DOREPLIFETIME(AOrionPRI, SupportXP);
	DOREPLIFETIME(AOrionPRI, ReconXP);

	//photon
	DOREPLIFETIME(AOrionPRI, ServerInfo);
}

void AOrionPRI::OnRep_InventoryManager()
{
	//if (InventoryManager)
	//	InventoryManager->Init();
}

void AOrionPRI::SetTeamIndex(int32 index)
{
	TeamIndex = index;
}

int32 AOrionPRI::GetTeamIndex()
{
	return TeamIndex;
}
