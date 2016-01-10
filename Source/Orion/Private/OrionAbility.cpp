#include "Orion.h"
#include "OrionAbility.h"

class ALandscape;

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
				float Rate = EnergyRate;

				if (IsCloaking())
				{
					AOrionCharacter *P = Cast<AOrionCharacter>(GetOwner());
					if (P)
					{
						AOrionPlayerController *PC = Cast<AOrionPlayerController>(P->Controller);
						if (PC)
							Rate = 100.0f / ((100.0f / EnergyRate) + float(PC->GetSkillValue(SKILL_CLOAKDURATION)));
					}
				}

				Energy = FMath::Max(0.0f, Energy - Rate * DeltaSeconds);

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

//try to spawn our ability, only spawnable on landscape
//called on client, and then sent to server
void AOrionAbility::TriggerTarget(AOrionCharacter *Pawn)
{
	if (!Pawn)
		return;

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Pawn->Controller);

	if (PC)
	{
		FHitResult Hit;

		PC->GetHitResultUnderCursor(ECC_Visibility, false, Hit);

		if (Cast<ALandscape>(Hit.GetActor()))
		{
			if((GetActorLocation() - Pawn->GetActorLocation()).Size2D() > 700.0f)
				return;

			//make sure we're on the landscape
			FCollisionQueryParams TraceParams(FName(TEXT("ExtraBlinkTrace")), true, this);

			TraceParams.AddIgnoredActor(this);
			TraceParams.bTraceAsyncScene = true;
			TraceParams.bReturnPhysicalMaterial = true;

			TArray<FHitResult> Results;

			bool bSuccess = false;

			if (GWorld->LineTraceMultiByChannel(Results, GetActorLocation() + FVector(0, 0, 25.0f), GetActorLocation() - FVector(0, 0, 25.0f), ECollisionChannel::ECC_Visibility, TraceParams))
			{
				for (int32 j = 0; j < Results.Num(); j++)
				{
					if (Cast<ALandscape>(Results[j].GetActor()))
						bSuccess = true;
				}
			}

			if (!bSuccess)
				return;

			EventTriggerTarget();

			EventHideTarget();

			bTargetting = false;

			//FVector dir = (Hit.Location - Pawn->GetActorLocation()).GetSafeNormal2D();

			if (Role < ROLE_Authority)
				ServerTriggerTarget(PC, GetActorLocation(), GetActorRotation());// Hit.Location, dir.Rotation());
			else
				SpawnItem(PC, GetActorLocation(), GetActorRotation());//Hit.Location, dir.Rotation());
		}
	}
}

void AOrionAbility::ServerTriggerTarget_Implementation(AOrionPlayerController *PC, FVector pos, FRotator rot)
{
	SpawnItem(PC, pos, rot);
}

void AOrionAbility::SpawnItem(AOrionPlayerController *PC, FVector pos, FRotator rot)
{
	Energy = 0.0f;
	//TimeSinceLastActive = GetWorld()->TimeSeconds;

	if (!PlaceableItemClass)
		return;

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.Owner = PC;

	GetWorld()->SpawnActor<AOrionPlaceableItem>(PlaceableItemClass, pos, rot, SpawnInfo);
}

bool AOrionAbility::ActivateSkill()
{
	if (!bIsSkillActive && Energy > 5.0f)
	{
		if (bOneShotAbility)
		{
			if (Energy >= OneShotEnergyCost)
			{
				if (bTarget)
				{
					if (bTargetting)
						EventHideTarget();
					else
						EventShowTarget();

					bTargetting = !bTargetting;

					return true;
				}

				Energy -= OneShotEnergyCost;
				//bIsSkillActive = true;
				TimeSinceLastActive = GetWorld()->TimeSeconds;

				DoActivateEffects();
			}
			else
				return false;
		}
		else
		{
			DoActivateEffects();
			bIsSkillActive = true;
		}
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