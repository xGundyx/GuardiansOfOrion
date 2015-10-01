#include "Orion.h"
#include "OrionPRI.h"

#define BASEXP 10000
#define XPINCREASE 2500

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

void AOrionPRI::AddXP(int32 Value)
{
	if (Value > 0)
	{
		int32 OldLevel = CalculateLevel(CharacterXP);

		CharacterXP += Value;

		//check for level up
		if (OldLevel < CalculateLevel(CharacterXP))
		{
			DoLevelUp();
		}
	}
}

int32 AOrionPRI::CalculateLevel(int32 XP)
{
	int32 XPRemaining = XP - BASEXP;
	int32 Level = 1;

	while (XPRemaining >= 0)
	{
		Level++;
		XPRemaining -= XPINCREASE;
	}

	return Level;
}

void AOrionPRI::DoLevelUp()
{
	//spawn some effects
	TArray<AActor*> Controllers;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrionPlayerController::StaticClass(), Controllers);

	for (int32 i = 0; i < Controllers.Num(); i++)
	{
		AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controllers[i]);
		if (PC)
		{
			if (PC->PlayerState == this)
				PC->PlayLevelUpEffect(); //display message to the player who leveled up
			else
				PC->ShowLevelUpMessage(); //text message to show others that this player has leveled up
		}
	}
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
	DOREPLIFETIME(AOrionPRI, CharacterXP);
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
