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
	UpdateOrbFX();
}

void AOrionPRI::UpdateOrbFX()
{
	if (!ControlledPawn)
		return;

	//check for new fx to start
	for (int32 i = 0; i < OrbEffects.Num(); i++)
	{
		bool bFound = false;
		for (int32 j = 0; j < OrbPSC.Num(); j++)
		{
			if (OrbEffects[i].Type == OrbPSC[j].Type)
			{
				bFound = true;
			}
		}

		if (!bFound)
		{
			FOrbEffectHelper FX;
			FX.PSC = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ControlledPawn->OrbFX, ControlledPawn->GetActorLocation() + FVector(0.0f, 0.0f, 10.0f));

			FString Message = "";

			if (FX.PSC)
			{
				switch (OrbEffects[i].Type)
				{
				case ORB_HEALTH:
					FX.PSC->SetVectorParameter("OrbColor", FVector(0.0f, 30.0f, 0.0f));
					FX.PSC->SetVectorParameter("OrbTrailColor", FVector(0.5f, 30.0f, 0.5f));
					Message = "HEALTH REGEN";
					break;
				case ORB_STOPPING:
					FX.PSC->SetVectorParameter("OrbColor", FVector(30.0f, 0.0f, 0.0f));
					FX.PSC->SetVectorParameter("OrbTrailColor", FVector(30.0f, 0.5f, 0.5f));
					Message = "INCREASED DAMAGE";
					break;
				case ORB_EXP:
					FX.PSC->SetVectorParameter("OrbColor", FVector(30.0f, 30.0f, 0.0f));
					FX.PSC->SetVectorParameter("OrbTrailColor", FVector(30.0f, 30.0f, 0.5f));
					Message = "DOUBLE XP";
					break;
				case ORB_ROF:
					FX.PSC->SetVectorParameter("OrbColor", FVector(0.0f, 0.0f, 30.0f));
					FX.PSC->SetVectorParameter("OrbTrailColor", FVector(0.5f, 0.5f, 30.0f));
					Message = "FIRING SPEED";
					break;
				case ORB_SPEED:
					FX.PSC->SetVectorParameter("OrbColor", FVector(30.0f, 0.0f, 30.0f));
					FX.PSC->SetVectorParameter("OrbTrailColor", FVector(30.0f, 0.5f, 30.0f));
					Message = "SPEED BOOST";
					break;
				case ORB_STRENGTH:
					FX.PSC->SetVectorParameter("OrbColor", FVector(30.0f, 1.0f, 0.0f));
					FX.PSC->SetVectorParameter("OrbTrailColor", FVector(30.0f, 1.0f, 0.5f));
					Message = "DAMAGE REDUCTION";
					break;
				};
			}

			FX.Type = OrbEffects[i].Type;

			OrbPSC.Add(FX);

			AOrionPlayerController *PC = Cast<AOrionPlayerController>(ControlledPawn->Controller);
			if (PC && PC->IsLocalPlayerController())
				PC->EventSetOrbMessage(Message, FX.Type);

			ControlledPawn->EventStartTrailFX();
		}
	}

	//check for fx to end
	for (int32 i = 0; i < OrbPSC.Num(); i++)
	{
		bool bFound = false;
		for (int32 j = 0; j < OrbEffects.Num(); j++)
		{
			if (OrbPSC[i].Type == OrbEffects[j].Type)
			{
				bFound = true;
				break;
			}
		}

		if (!bFound)
		{
			/*if (OrbPSC[i].PSC)
			{
				OrbPSC[i].PSC->DeactivateSystem();
				OrbPSC[i].PSC = nullptr;
			}*/
			OrbPSC.RemoveAt(i);
			i = -1;
		}
	}

	if (OrbEffects.Num() <= 0)
		ControlledPawn->EventStopTrailFX();
}

void AOrionPRI::UpdateOrbEffects()
{
	bool bUpdate = false;
	for (int32 i = 0; i < OrbEffects.Num(); i++)
	{
		float TimeLeft = GetWorld()->GetTimeSeconds() - OrbEffects[i].TimeStarted;
		if (TimeLeft >= OrbEffects[i].Duration)
		{
			OrbEffects.RemoveAt(i);
			i = -1;
			bUpdate = true;
		}
		else
		{
			OrbEffects[i].TimeLeft = TimeLeft;
		}
	}

	if (bUpdate)
		UpdateOrbFX();
}

void AOrionPRI::AddOrbEffect(EOrbType Type, float Duration)
{
	if (Role != ROLE_Authority)
		return;

	//dead players don't get buffs!
	if (!ControlledPawn || ControlledPawn->Health <= 0.0f)
		return;

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(ControlledPawn->Controller);

	if (PC && PC->GetStats())
	{
		switch (Type)
		{
		case ORB_STOPPING:
			PC->GetStats()->AddStatValue(STAT_STOPPINGORBS, 1);
			break;
		case ORB_HEALTH:
			PC->GetStats()->AddStatValue(STAT_HEALTHORBS, 1);
			break;
		case ORB_ROF:
			PC->GetStats()->AddStatValue(STAT_ROFORBS, 1);
			break;
		case ORB_SPEED:
			PC->GetStats()->AddStatValue(STAT_SPEEDORBS, 1);
			break;
		case ORB_EXP:
			PC->GetStats()->AddStatValue(STAT_EXPORBS, 1);
			break;
		case ORB_STRENGTH:
			PC->GetStats()->AddStatValue(STAT_REDUCTIONORBS, 1);
			break;
		}

		if (PC->GetAchievements())
			PC->GetAchievements()->CheckOrbs();
	}

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

		switch (Type)
		{
		case ORB_HEALTH:
			Orb.Color = FVector(0.0f, 30.0f, 0.0f);
			break;
		case ORB_STOPPING:
			Orb.Color = FVector(30.0f, 0.0f, 0.0f);
			break;
		case ORB_EXP:
			Orb.Color = FVector(30.0f, 30.0f, 0.0f);
			break;
		case ORB_ROF:
			Orb.Color = FVector(0.0f, 0.0f, 30.0f);
			break;
		case ORB_SPEED:
			Orb.Color = FVector(30.0f, 0.0f, 30.0f);
			break;
		case ORB_STRENGTH:
			Orb.Color = FVector(30.0f, 15.0f, 0.0f);
			break;
		}

		OrbEffects.Add(Orb);

		if (OrbEffects.Num() >= 6)
		{
			if (PC && PC->GetAchievements())
				PC->GetAchievements()->UnlockAchievement(ACH_6ORBS, PC);
		}

		UpdateOrbFX();
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
