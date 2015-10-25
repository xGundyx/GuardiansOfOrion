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

//update visual effects and timers on client machines
void AOrionPRI::OnRep_OrbEffects()
{
}

void AOrionPRI::UpdateOrbEffects()
{
	for (int32 i = 0; i < OrbEffects.Num(); i++)
	{
		float TimeLeft = GetWorld()->GetTimeSeconds() - OrbEffects[i].TimeStarted;
		if (TimeLeft >= OrbEffects[i].Duration)
		{
			OrbEffects.RemoveAt(i);
			i = -1;
		}
		else
		{
			OrbEffects[i].TimeLeft = TimeLeft;
		}
	}
}

void AOrionPRI::AddOrbEffect(EOrbType Type, float Duration)
{
	if (Role != ROLE_Authority)
		return;

	bool bFound = false;

	//make sure it doesn't already exist
	for (int32 i = 0; i < OrbEffects.Num(); i++)
	{
		if (OrbEffects[i].Type == Type)
		{
			OrbEffects[i].Duration = Duration;
			OrbEffects[i].TimeLeft = Duration;
			OrbEffects[i].TimeStarted = GetWorld()->GetTimeSeconds();
			bFound = true;
			break;
		}
	}

	if (!bFound)
	{
		FOrbHelper Orb;
		Orb.Duration = Duration;
		Orb.TimeLeft = Duration;
		Orb.TimeStarted = GetWorld()->GetTimeSeconds();
		Orb.Type = Type;

		OrbEffects.Add(Orb);
	}
}

int32 AOrionPRI::GetCharacterLevel(ECharacterClass CharacterType)
{
	int XP = 0;

	if (CharacterType == ECharacterClass::CLASS_SUPPORT)
		XP = SupportXP;
	else if (CharacterType == ECharacterClass::CLASS_ASSAULT)
		XP = AssaultXP;
	else if (CharacterType == ECharacterClass::CLASS_RECON)
		XP = ReconXP;
	else
		return 1;

	return CalculateLevel(XP);
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
	DOREPLIFETIME(AOrionPRI, UnlockedSkills);

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

	//ooooooorbs
	DOREPLIFETIME(AOrionPRI, OrbEffects);
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
