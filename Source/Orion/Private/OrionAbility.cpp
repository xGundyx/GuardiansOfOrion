#include "Orion.h"
#include "OrionAbility.h"

AOrionAbility::AOrionAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	//energy/sec
	RechargeRate = 25.0f;

	//energy/sec
	EnergyRate = 10.0f;

	//time between last energy use and recharge
	RechargeDelay = 5.0f;

	//fully charged on spawn
	Energy = 100.0f;

	bIsSkillActive = false;
	bOneShotAbility = false;
	OneShotEnergyCost = 25.0f;

	bIsJetpacking = false;
	bIsCloaking = false;
	bIsOvercharging = false;

	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	//bReplicateInstigator = true;
	bNetUseOwnerRelevancy = true;
}

void AOrionAbility::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Role == ROLE_Authority)
	{
		if (bIsSkillActive)
		{
			if (!bOneShotAbility)
			{
				Energy = FMath::Max(0.0f, Energy - EnergyRate * DeltaSeconds);

				if (Energy <= 0.0f)
				{
					Energy = 0.0f;
					DeactivateSkill();
				}
			}
		}
		else if (Energy < 100.0f)
		{
			if (GetWorld()->TimeSeconds - TimeSinceLastActive > RechargeDelay)
			{
				Energy = FMath::Min(100.0f, Energy + RechargeRate * DeltaSeconds);
			}
		}
	}

	DoSkillAbility(DeltaSeconds);
}

void AOrionAbility::BeginPlay()
{
	Super::BeginPlay();
}

bool AOrionAbility::ActivateSkill()
{
	if (!bIsSkillActive && Energy > 5.0f)
	{
		DoActivateEffects();
		if (bOneShotAbility)
		{
			if (Energy >= OneShotEnergyCost)
			{
				Energy -= OneShotEnergyCost;
				//bIsSkillActive = true;
				TimeSinceLastActive = GetWorld()->TimeSeconds;
			}
		}
		else
			bIsSkillActive = true;
		return true;
	}

	return false;
}

void AOrionAbility::DeactivateSkill()
{
	if (bIsSkillActive)
	{
		DoDeactivateEffects();
		TimeSinceLastActive = GetWorld()->TimeSeconds;
		bIsSkillActive = false;
	}
}

void AOrionAbility::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// everyone
	DOREPLIFETIME(AOrionAbility, bIsSkillActive);
	DOREPLIFETIME(AOrionAbility, Energy);
	DOREPLIFETIME(AOrionAbility, bIsJetpacking);
	DOREPLIFETIME(AOrionAbility, bIsCloaking);
	DOREPLIFETIME(AOrionAbility, bIsOvercharging);
}

void AOrionAbility::DoSkillAbility(float DeltaSeconds)
{
	EventTickAbility(DeltaSeconds);
}

void AOrionAbility::OnRep_SkillActive()
{
	if (bIsSkillActive)
		DoActivateEffects();
	else
		DoDeactivateEffects();
}

void AOrionAbility::DoActivateEffects()
{
	EventPlayStartEffects();
}

void AOrionAbility::DoDeactivateEffects()
{
	EventPlayStopEffects();
}